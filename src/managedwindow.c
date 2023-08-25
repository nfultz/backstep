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
#include <glib.h>
#include <gdk/gdkx.h>
#include <X11/Xlib.h>
#include "managedwindow.h"
#include "configuration.h"
#include "configurator.h"
#include "settings.h"
#include "atoms.h"
#include "window.h"
#include "workspaces.h"
#include "groups.h"
#include "trace.h"

static void hidden_state_changed(gpointer data);
static void sticky_state_changed(gpointer data);
static GdkFilterReturn event_filter(GdkXEvent *xevent, GdkEvent *gdkevent,
                                    gpointer data);
static void icon_window_activated(gpointer data);
static void workspace_changed(gpointer data);
static void compute_icon_position(BSManagedWindow *bsmwin, gint *x, gint *y);
static void icon_right_click(guint32 time, gpointer data);
static void update_icon_geom_prop(BSManagedWindow *bsmwin, gint x, gint y);
static void icon_window_moved(gpointer data);


BSManagedWindow *
bs_managed_window_new(BSWindow *bswin)
{
    BSManagedWindow *bsmwin;
    glong events;
    gint x, y;

    g_assert(bswin != NULL);
    trace("%ld", bswin->window);

    bsmwin = (BSManagedWindow *)g_malloc(sizeof(BSManagedWindow));
    bsmwin->window = bswin;
    bsmwin->group = 0;
    bsmwin->propagate_to_group = TRUE;

    bsmwin->pixmap = bs_window_pixmap_new(bsmwin->window);
    bsmwin->icon_window = bs_icon_window_new(bsmwin->pixmap);

    if (bs_window_is_state(bswin, BS_NET_WM_STATE_STICKY) ||
        bs_configuration_get_boolean_value(BS_ICONS_STICKY)) {

        bs_icon_window_set_sticky(bsmwin->icon_window, TRUE);
    }

    bs_icon_window_set_activated_callback(bsmwin->icon_window, 
                                          icon_window_activated, 
                                          bsmwin);
    bs_icon_window_set_rightclick_action(bsmwin->icon_window,
                                         icon_right_click,
                                         bsmwin);
    bs_icon_window_set_moved_callback(bsmwin->icon_window,
                                      icon_window_moved,
                                      bsmwin);
    events = gdk_window_get_events(bswin->gdkwindow);
    events |= (GDK_STRUCTURE_MASK | GDK_PROPERTY_CHANGE_MASK |
               GDK_STRUCTURE_MASK);
    bs_window_add_states_callback(bswin, BS_NET_WM_STATE_HIDDEN, 
                                  hidden_state_changed, bsmwin);
    bs_window_add_states_callback(bswin, BS_NET_WM_STATE_STICKY, 
                                  sticky_state_changed, bsmwin);
    gdk_window_set_events(bswin->gdkwindow, events);
    gdk_window_add_filter(bswin->gdkwindow, event_filter, bsmwin);

    bsmwin->workspace_cb_id = 
        bs_workspaces_add_workspace_changed_callback(workspace_changed, bsmwin);

    /* go ahead and set the icon_geom property, since if the wm uses it, it will
     * need it before we get the first hidden state changed event */
    compute_icon_position(bsmwin, &x, &y);
    update_icon_geom_prop(bsmwin, x, y);

    return bsmwin;
}


void
bs_managed_window_free(BSManagedWindow *bsmwin)
{
    trace("%ld", bsmwin->window->window);
    if (bsmwin->group > 0)
        bs_groups_unassign(bsmwin, bsmwin->group);
    gdk_window_remove_filter(bsmwin->window->gdkwindow, event_filter, bsmwin);
    bs_workspaces_remove_workspace_changed_callback(bsmwin->workspace_cb_id);
    bs_icon_window_free(bsmwin->icon_window);
    bs_window_pixmap_free(bsmwin->pixmap);
    bs_window_free(bsmwin->window);
    g_free(bsmwin);
}


static GdkFilterReturn 
event_filter(GdkXEvent *xevent, GdkEvent *gdkevent, gpointer data)
{
    XEvent *event;
    BSManagedWindow *bsmwin;
    
    event = (XEvent *)xevent;
    bsmwin = (BSManagedWindow *)data;
    GError *error = NULL;

    if (event->type == PropertyNotify) {
        g_assert(bsmwin != NULL);
        g_assert(bsmwin->window != NULL);
        XPropertyEvent *prop_event = (XPropertyEvent *)event;
        if (prop_event->atom == NET_WM_STATE)
            bs_window_update_states(bsmwin->window, &error);
        else if (prop_event->atom == NET_WM_DESKTOP)
            bs_window_update_desktop(bsmwin->window, &error);
        else if (prop_event->atom == NET_WM_WINDOW_TYPE)
            bs_window_update_types(bsmwin->window, &error);
    } else if (event->type == ConfigureNotify) {

        /* if the window moves or resizes but we haven't shown the icon yet,
         * we want to update the NET_WM_ICON_GEOMETRY prop so any wm animation
         * will look right the first time the window is hidden */
        if (!bs_icon_window_is_positioned(bsmwin->icon_window)) {
            gint x, y;
            compute_icon_position(bsmwin, &x, &y);
            update_icon_geom_prop(bsmwin, x, y);
        }
    }
    /*} else if (event->type == MapNotify) {
        if (bs_icon_window_is_showing(bswin->icon_window))
            bs_icon_window_hide(bswin->icon_window);
    }*/
    if (error) {
        trace(error->message)
        g_error_free(error);
    }
    return GDK_FILTER_CONTINUE;
}


static void 
hidden_state_changed(gpointer data)
{
    BSManagedWindow *bsmwin = (BSManagedWindow *)data;
    if (bs_window_is_state(bsmwin->window, BS_NET_WM_STATE_HIDDEN)) {
        trace("%ld: hidden", bsmwin->window->window);
        if (bs_icon_window_is_positioned(bsmwin->icon_window)) {
            bs_icon_window_show(bsmwin->icon_window,
                                bs_window_get_desktop(bsmwin->window));
        } else {
            gint x, y;
            compute_icon_position(bsmwin, &x, &y);
            update_icon_geom_prop(bsmwin, x, y);
            
            bs_icon_window_show_at(bsmwin->icon_window,
                                   bs_window_get_desktop(bsmwin->window),
                                   x, y);
        }
        if (bsmwin->group > 0 && bsmwin->propagate_to_group) {
            bs_groups_client_hidden(bsmwin, bsmwin->group);
        }
    } else {
        trace("%ld: unhidden", bsmwin->window->window);
        bs_icon_window_hide(bsmwin->icon_window);
        if (bsmwin->group > 0 && bsmwin->propagate_to_group) {
            bs_groups_client_shown(bsmwin, bsmwin->group);
        }
    }
}
    

static void 
sticky_state_changed(gpointer data)
{
    BSManagedWindow *bsmwin = (BSManagedWindow *)data;
    bs_icon_window_set_sticky(bsmwin->icon_window, 
                              bs_window_is_state(bsmwin->window, 
                                                 BS_NET_WM_STATE_STICKY));
}


static void 
icon_window_activated(gpointer data)
{
    BSManagedWindow *bsmwin = (BSManagedWindow *)data;
    if ((!bs_window_is_state(bsmwin->window, BS_NET_WM_STATE_STICKY)) &&
        bs_workspaces_get_current() != bs_window_get_desktop(bsmwin->window)) {

        bs_window_change_desktop(bsmwin->window, bs_workspaces_get_current());
    }
    XMapRaised(GDK_DISPLAY(), bsmwin->window->window);
}


static void 
workspace_changed(gpointer data)
{
    BSManagedWindow *bsmwin = (BSManagedWindow *)data;
    if (!bs_window_is_state(bsmwin->window, BS_NET_WM_STATE_HIDDEN))
        bs_icon_window_hide(bsmwin->icon_window);
}


static void
compute_icon_position(BSManagedWindow *bsmwin, gint *x, gint *y)
{
    gint w_x, w_y, w_w, w_h;
    gint i_w, i_h;
    bs_window_get_geometry(bsmwin->window, &w_x, &w_y, &w_w, &w_h);
    bs_icon_window_get_size(bsmwin->icon_window, &i_w, &i_h);
    
    *x = (w_x + (w_w / 2)) - (i_w / 2);
    *y = (w_y + (w_h / 2)) - (i_h / 2);
}


void
bs_managed_window_set_group(BSManagedWindow *bsmwin, gint group)
{
    if (bsmwin->group > 0)
        bs_groups_unassign(bsmwin, bsmwin->group);
    bsmwin->group = group;
    if (group > 0)
        bs_groups_assign(bsmwin, group);
}


void
bs_managed_window_show_client(BSManagedWindow *bsmwin)
{
    bsmwin->propagate_to_group = FALSE;
    icon_window_activated(bsmwin);
    bsmwin->propagate_to_group = TRUE;
}


void
bs_managed_window_hide_client(BSManagedWindow *bsmwin)
{
    /*XEvent e;*/

    bsmwin->propagate_to_group = FALSE;

    /*e.xclient.type = ClientMessage;
    e.xclient.message_type = NET_WM_STATE;
    e.xclient.display = GDK_DISPLAY();
    e.xclient.window = bsmwin->window->window;
    e.xclient.format = 32;
    e.xclient.data.l[0] = 1;
    e.xclient.data.l[1] = NET_WM_STATE_HIDDEN;
    e.xclient.data.l[2] = 0l;
    e.xclient.data.l[3] = 2;
    e.xclient.data.l[4] = 0;
    XSendEvent(GDK_DISPLAY(), GDK_ROOT_WINDOW(), False, SubstructureNotifyMask |
               SubstructureRedirectMask, &e);*/
    XIconifyWindow(GDK_DISPLAY(), bsmwin->window->window, 
                   DefaultScreen(GDK_DISPLAY()));
    bsmwin->propagate_to_group = TRUE;
}


static void 
icon_right_click(guint32 time, gpointer data)
{
    BSManagedWindow *bsmwin = (BSManagedWindow *)data;
    bs_configurator_show_menu(time, bsmwin);
}


static void update_icon_geom_prop(BSManagedWindow *bsmwin, gint x, gint y)
{
    gint w, h;
    gulong data[4];

    if (x == -1 || y == -1)
        bs_icon_window_get_gemoetry(bsmwin->icon_window, &x, &y, &w, &h);
    else 
        bs_icon_window_get_size(bsmwin->icon_window, &w, &h);

    data[0] = x;
    data[1] = y;
    data[2] = w;
    data[3] = h;

    XChangeProperty(GDK_DISPLAY(), bsmwin->window->window, NET_WM_ICON_GEOMETRY,
                    XA_CARDINAL, 32, PropModeReplace, (guchar *)&data, 4);
}


static void 
icon_window_moved(gpointer data)
{
    BSManagedWindow *bsmwin = (BSManagedWindow *)data;
    update_icon_geom_prop(bsmwin, -1, -1);
}
