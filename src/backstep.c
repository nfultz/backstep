/*
 * Copyright (C) 2005 Christopher Carroll <christopher@salbot.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */
#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif
#include <stdlib.h>
#include <unistd.h>
#include <glib.h>
#include <gtk/gtk.h>
#include <gdk/gdk.h>
#include <gdk/gdkx.h>
#include "program.h"
#include "daemon.h"
#include "xutil.h"
#include "damage.h"
#include "composite.h"
#include "atoms.h"
#include "workspaces.h"
#include "window.h"
#include "windows.h"
#include "managedwindow.h"
#include "configuration.h"
#include "settings.h"
#include "session.h"
#include "colors.h"
#include "groups.h"
#include "trace.h"


GList *managed_windows = NULL;
static GdkWindow *app_toplevel_win = NULL;

static void init(gint *argc, gchar ***argv);
static void windows_init();
static GdkFilterReturn event_filter(GdkXEvent *xevent, GdkEvent *gdkevent,
                                    gpointer data);
static void update_client_list();
static gpointer find_mwin_for_xid(Window xid);
static void free_win_list(GList *list);
static void init_toplevel_win();

int
main(int argc, char *argv[])
{
    init(&argc, &argv);
    gtk_main();
    exit(EXIT_SUCCESS);
}


static void
init(gint *argc, gchar ***argv)
{
    g_log_set_always_fatal(G_LOG_LEVEL_CRITICAL);
    bs_program_parse_args(argc, argv);
    if (!bs_program_no_daemon())
        bs_daemon_daemonize();
    gtk_init(argc, argv);
    bs_session_init(bs_program_name(), bs_program_client_id());
    bs_configuration_init();
    bs_settings_init();
    bs_atoms_init();
    bs_colors_init();
    if (bs_configuration_get_boolean_value(BS_ICONS_XRENDER)) {
        bs_damage_init();
        bs_composite_init();
    }
    bs_groups_init();
    bs_workspaces_init();
    bs_configurator_init();
    init_toplevel_win();
    windows_init();
}


static void 
windows_init()
{
    GdkWindow *root_win;

    root_win = gdk_get_default_root_window();
    gdk_window_set_events(root_win, gdk_window_get_events(root_win) | 
                                    GDK_PROPERTY_CHANGE_MASK);
    gdk_window_add_filter(root_win, event_filter, NULL);
    update_client_list();
}


static GdkFilterReturn 
event_filter(GdkXEvent *xevent, GdkEvent *gdkevent, gpointer data)
{
    XEvent *event;
    XPropertyEvent *prop_event;
    
    event = (XEvent *)xevent;

    switch (event->type) {
        case PropertyNotify:
            prop_event = (XPropertyEvent *)event;
            if (prop_event->atom == NET_CLIENT_LIST)
                update_client_list();
            break;
    }
    return GDK_FILTER_CONTINUE;
}


static void update_client_list()
{
    Window *windows, xid;
    gulong nitems, z;
    GError *error = NULL;
    BSManagedWindow *oldwin;
    GList *tmp = NULL;
    gint count = 0;

    while (bs_xutil_get_window_property(DefaultRootWindow(GDK_DISPLAY()),
                                        NET_CLIENT_LIST,
                                        XA_WINDOW,
                                        32,
                                        &nitems,
                                        (guchar **)&windows,
                                        &error) != 0) {
        if (++count == 60) {
            g_critical("Could not get clients list: %s", error->message);
            g_error_free(error);
            return;
        }
        g_error_free(error);
        error = NULL;
        XFree(windows);
        sleep(1);
    }

    for (z = 0; z < nitems; z++) {
        Window xid = *(windows + z);
        oldwin = find_mwin_for_xid(xid);
        if (oldwin) {
            managed_windows = g_list_remove(managed_windows, oldwin);
            tmp = g_list_append(tmp, oldwin);
        } else {
            BSWindow *bswin = bs_window_new(xid, &error);
            if (!bswin) {
                g_warning(error->message);
                g_error_free(error);
                error = NULL;
                continue;
            }
            if (bs_window_is_state(bswin, BS_NET_WM_STATE_SKIP_TASKBAR) ||
                bs_window_is_state(bswin, BS_NET_WM_STATE_SKIP_PAGER)   ||
                bs_window_is_type(bswin, BS_NET_WM_WINDOW_TYPE_DESKTOP) ||
                bs_window_is_type(bswin, BS_NET_WM_WINDOW_TYPE_DOCK)    ||
                bs_window_is_type(bswin, BS_NET_WM_WINDOW_TYPE_SPLASH))
                bs_window_free(bswin);
            else {
                trace("creating managed window %ld...", bswin->window);
                tmp = g_list_append(tmp, bs_managed_window_new(bswin));
                trace("ok.", bswin->window);
            }
        }
    }
    XFree(windows);

    /* what's left in managed_windows are destroyed windows */
    while (managed_windows) {
        BSManagedWindow *win = managed_windows->data;
        managed_windows = g_list_remove_link(managed_windows, managed_windows);
        bs_managed_window_free(win);
    }
    managed_windows = tmp;
}


static gpointer 
find_mwin_for_xid(Window xid)
{
    GList *list = managed_windows;
    for (; list; list = list->next) {
        BSManagedWindow *bsmwin = (BSManagedWindow *)list->data;
        if (bsmwin->window->window == xid)
            return list->data;
    }
    return NULL;
}


static void
free_win_list(GList *list)
{
    GList *tmp;

    while (list) {
        BSManagedWindow *win = list->data;
        list = g_list_remove_link(list, list);
        bs_managed_window_free(win);
    }
}


static void init_toplevel_win()
{
    GdkWindowAttr attrs;
    long handled = 1;

    attrs.title = "Backstep";
    attrs.event_mask = 0;
    attrs.wclass = GDK_INPUT_ONLY;
    attrs.window_type = GDK_WINDOW_TOPLEVEL;
    attrs.override_redirect = FALSE;

    app_toplevel_win = gdk_window_new(NULL, &attrs, GDK_WA_TITLE | 
                                                    GDK_WA_WMCLASS | 
                                                    GDK_WA_NOREDIR);
    XChangeProperty(GDK_DISPLAY(), GDK_WINDOW_XID(app_toplevel_win),
                    NET_WM_HANDLED_ICONS, XA_CARDINAL, 32, PropModeReplace,
                    (guchar *)&handled, 1);
}
