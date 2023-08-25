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
#include <gdk/gdk.h>
#include <gdk/gdkx.h>
#include "workspaces.h"
#include "atoms.h"
#include "xutil.h"
#include "callback.h"

static gint current_desktop = -1;
static GList *callbacks = NULL;

static void update_desktop();
static GdkFilterReturn root_filter(GdkXEvent *xevent, GdkEvent *gdkevent,
                                   void *data);
static void make_callbacks();


void
bs_workspaces_init()
{
    gdk_window_add_filter(gdk_get_default_root_window(), root_filter, NULL);
    gdk_window_set_events(gdk_get_default_root_window(), 
                          gdk_window_get_events(gdk_get_default_root_window()) |                          GDK_PROPERTY_CHANGE_MASK);
    update_desktop();
}


static void
update_desktop()
{
    guchar *data = NULL;
    GError *error = NULL;
    gulong nitems;

    if (bs_xutil_get_window_property(GDK_ROOT_WINDOW(),
                                     NET_CURRENT_DESKTOP,
                                     XA_CARDINAL,
                                     32,
                                     &nitems,
                                     &data,
                                     &error) != 0) {
        current_desktop = -1;
        g_warning("Could not obtain current desktop: %s\n", error->message);
        g_error_free(error);
        return;
    }
    current_desktop = *((gint *)data);
}


gint
bs_workspaces_get_current()
{
    return current_desktop;
}


static GdkFilterReturn 
root_filter(GdkXEvent *xevent, GdkEvent *gdkevent, void *data)
{
    XEvent *event;
    XPropertyEvent *prop_event;
    
    event = (XEvent *)xevent;

    switch (event->type) {
        case PropertyNotify:
            prop_event = (XPropertyEvent *)event;
            if (prop_event->atom == NET_CURRENT_DESKTOP) {
                update_desktop();
                make_callbacks();
            }
            break;
    }
    return GDK_FILTER_CONTINUE;
}


gint
bs_workspaces_add_workspace_changed_callback(void(*cb)(gpointer data),
                                             gpointer data)
{
    BSCallback *bscb;

    bscb = bs_callback_new(cb, data);
    callbacks = g_list_append(callbacks, bscb);
    return bs_callback_id(bscb);
}


static void
make_callbacks()
{
    GList *tmp;

    for (tmp = callbacks; tmp; tmp = tmp->next) {
        BSCallback *cb = (BSCallback *)tmp->data;
        bs_callback_call(cb);
    }
}


void
bs_workspaces_remove_workspace_changed_callback(gint id)
{
    GList *tmp = callbacks;
    while (tmp) {
        BSCallback *cb = (BSCallback *)tmp->data;
        if (bs_callback_id(cb) == id) {
            bs_callback_free(cb);
            callbacks = g_list_delete_link(callbacks, tmp);
            return;
        }
        tmp = tmp->next;
    }
}
