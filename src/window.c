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
#include <gdk/gdkx.h>
#include "window.h"
#include "error.h"
#include "atoms.h"
#include "xutil.h"
#include "callback.h"
#include "trace.h"

BSWindow *
bs_window_new(Window xid, GError **error)
{
    BSWindow *bswin;
    gint z;

    bswin = (BSWindow *)g_malloc(sizeof(BSWindow));
    bswin->window = xid;
    bswin->gdkwindow = gdk_window_foreign_new(xid);
    bswin->states = (BSNetWMState *)g_malloc(sizeof(BSNetWMState) * 
                                             BS_NET_WM_STATE_MAX);
    bswin->types = (BSNetWMWindowType *)g_malloc(sizeof(BSNetWMWindowType) * 
                                                 BS_NET_WM_WINDOW_TYPE_MAX);
    bswin->states_callbacks = (GList **)g_malloc(sizeof(GList *) *
                                                 BS_NET_WM_STATE_MAX);
    bswin->types_callbacks = (GList **)g_malloc(sizeof(GList *) *
                                                BS_NET_WM_WINDOW_TYPE_MAX);

    for (z = 0 ; z < BS_NET_WM_STATE_MAX; z++) {
        bswin->states[z] = FALSE;
        bswin->states_callbacks[z] = NULL;
    }
    for (z = 0 ; z < BS_NET_WM_WINDOW_TYPE_MAX; z++) {
        bswin->types[z] = FALSE;
        bswin->types_callbacks[z] = NULL;
    }
    bswin->desktop = -1;
    bswin->desktop_callbacks = NULL;

    if (bs_window_update_states(bswin, error) != 0
     || bs_window_update_types(bswin, error) != 0
     || bs_window_update_desktop(bswin, error) != 0) {
        bs_window_free(bswin);
        return NULL;
    }
    return bswin;
}


void 
bs_window_free(BSWindow *bswin)
{
    gint z;
    for (z = 0 ; z < BS_NET_WM_STATE_MAX; z++) {
        GList *list = bswin->states_callbacks[z];
        while (list) {
            bs_callback_free((BSCallback *)list->data);
            list = g_list_remove_link(list, list);
        }
    }
    g_free(bswin->states_callbacks);
    for (z = 0 ; z < BS_NET_WM_WINDOW_TYPE_MAX; z++) {
        GList *list = bswin->types_callbacks[z];
        while (list) {
            bs_callback_free((BSCallback *)list->data);
            list = g_list_remove_link(list, list);
        }
    }
    g_free(bswin->types_callbacks);
    while (bswin->desktop_callbacks) {
        bs_callback_free((BSCallback *)bswin->desktop_callbacks->data);
        bswin->desktop_callbacks = g_list_remove_link(bswin->desktop_callbacks,
                                                      bswin->desktop_callbacks);
    }
    g_free(bswin->states);
    g_free(bswin->types);
    g_object_unref(bswin->gdkwindow);
    g_free(bswin);
}

gint 
bs_window_update_states(BSWindow *bswin, GError **error)
{
    Atom *states = NULL;
    gulong nitems, z;
    BSNetWMState old_states[BS_NET_WM_STATE_MAX];
    
    g_assert(bswin != NULL);
    g_assert(bswin->window != None);

    if (bs_xutil_get_window_property(bswin->window, 
                                     NET_WM_STATE, XA_ATOM, 32, &nitems,
                                     (guchar **)&states, error) != 0)
        return -1; 

    for (z = 0; z < BS_NET_WM_STATE_MAX; z++) {
        old_states[z] = bswin->states[z];
        bswin->states[z] = FALSE;
    }

    for (z = 0; z < nitems; z++) {
        Atom state = *(states + z);
        if (state == NET_WM_STATE_MODAL)
            bswin->states[BS_NET_WM_STATE_MODAL] = TRUE;
        else if (state == NET_WM_STATE_MODAL)
            bswin->states[BS_NET_WM_STATE_MODAL] = TRUE;
        else if (state == NET_WM_STATE_STICKY)
            bswin->states[BS_NET_WM_STATE_STICKY] = TRUE;
        else if (state == NET_WM_STATE_MAXIMIZED_VERT)
            bswin->states[BS_NET_WM_STATE_MAXIMIZED_VERT] = TRUE;
        else if (state == NET_WM_STATE_MAXIMIZED_HORZ)
            bswin->states[BS_NET_WM_STATE_MAXIMIZED_HORZ] = TRUE;
        else if (state == NET_WM_STATE_SHADED)
            bswin->states[BS_NET_WM_STATE_SHADED] = TRUE;
        else if (state == NET_WM_STATE_SKIP_TASKBAR)
            bswin->states[BS_NET_WM_STATE_SKIP_TASKBAR] = TRUE;
        else if (state == NET_WM_STATE_SKIP_PAGER)
            bswin->states[BS_NET_WM_STATE_SKIP_PAGER] = TRUE;
        else if (state == NET_WM_STATE_HIDDEN)
            bswin->states[BS_NET_WM_STATE_HIDDEN] = TRUE;
        else if (state == NET_WM_STATE_FULLSCREEN)
            bswin->states[BS_NET_WM_STATE_FULLSCREEN] = TRUE;
        else if (state == NET_WM_STATE_ABOVE)
            bswin->states[BS_NET_WM_STATE_ABOVE] = TRUE;
        else if (state == NET_WM_STATE_BELOW)
            bswin->states[BS_NET_WM_STATE_BELOW] = TRUE;
        else if (state == NET_WM_STATE_DEMANDS_ATTENTION)
            bswin->states[BS_NET_WM_STATE_DEMANDS_ATTENTION] = TRUE;
    }
    XFree(states);

    for (z = 0; z < BS_NET_WM_STATE_MAX; z++)
        if (old_states[z] != bswin->states[z]) {
            GList *tmp = bswin->states_callbacks[z];
            for (;tmp;tmp = tmp->next) {
                g_assert(tmp->data != NULL);
                bs_callback_call((BSCallback *)tmp->data);
            }
        }

    return 0;
}
    
    
gint 
bs_window_update_types(BSWindow *bswin, GError **error)
{
    Atom *types = NULL;
    gulong nitems, z;
    BSNetWMWindowType old_types[BS_NET_WM_WINDOW_TYPE_MAX];
    
    for (z = 0; z < BS_NET_WM_WINDOW_TYPE_MAX; z++) {
        old_types[z] = bswin->types[z];
        bswin->types[z] = FALSE;
    }

    if (bs_xutil_get_window_property(bswin->window, 
                                     NET_WM_WINDOW_TYPE, XA_ATOM, 32, &nitems,
                                     (guchar **)&types, NULL) != 0) {
        return 0; 
    }

    for (z = 0; z < nitems; z++) {
        Atom type = *(types + z);
        if (type == NET_WM_WINDOW_TYPE_DESKTOP)
            bswin->types[BS_NET_WM_WINDOW_TYPE_DESKTOP] = TRUE;
        else if (type == NET_WM_WINDOW_TYPE_DOCK)
            bswin->types[BS_NET_WM_WINDOW_TYPE_DOCK] = TRUE;
        else if (type == NET_WM_WINDOW_TYPE_TOOLBAR)
            bswin->types[BS_NET_WM_WINDOW_TYPE_TOOLBAR] = TRUE;
        else if (type == NET_WM_WINDOW_TYPE_MENU)
            bswin->types[BS_NET_WM_WINDOW_TYPE_MENU] = TRUE;
        else if (type == NET_WM_WINDOW_TYPE_UTILITY)
            bswin->types[BS_NET_WM_WINDOW_TYPE_UTILITY] = TRUE;
        else if (type == NET_WM_WINDOW_TYPE_SPLASH)
            bswin->types[BS_NET_WM_WINDOW_TYPE_SPLASH] = TRUE;
        else if (type == NET_WM_WINDOW_TYPE_DIALOG)
            bswin->types[BS_NET_WM_WINDOW_TYPE_DIALOG] = TRUE;
        else if (type == NET_WM_WINDOW_TYPE_NORMAL)
            bswin->types[BS_NET_WM_WINDOW_TYPE_NORMAL] = TRUE;
    }
    XFree(types);

    for (z = 0; z < BS_NET_WM_WINDOW_TYPE_MAX; z++)
        if (old_types[z] != bswin->types[z]) {
            GList *tmp = bswin->types_callbacks[z];
            for (;tmp;tmp = tmp->next) {
                g_assert(tmp->data != NULL);
                bs_callback_call((BSCallback *)tmp->data);
            }
        }
    return 0;
}


gint
bs_window_update_desktop(BSWindow *bswin, GError **error)
{
    guchar *data = NULL;
    gulong nitems = 0;
    gint old_desktop;

    g_assert(bswin != NULL);
    g_assert(error == NULL || *error == NULL);

    old_desktop = bswin->desktop;

    if (bs_xutil_get_window_property(bswin->window,
                                     NET_WM_DESKTOP, XA_CARDINAL, 32, &nitems,
                                     &data, error) != 0)
        return -1;

    bswin->desktop = *((gint *)data); 
    XFree(data);

    if (bswin->desktop != old_desktop) {
        GList *tmp = bswin->desktop_callbacks;
        for (;tmp; tmp = tmp->next)
            bs_callback_call((BSCallback *)tmp->data);
    }
    return 0;
}


gboolean 
bs_window_is_state(BSWindow *bswin, BSNetWMState state)
{
    return bswin->states[state];
}


gboolean
bs_window_is_type(BSWindow *bswin, BSNetWMWindowType type)
{
    return bswin->types[type];
}


gint
bs_window_get_desktop(BSWindow *bswin)
{
    return bswin->desktop;
}


void
bs_window_add_states_callback(BSWindow *bswin, BSNetWMState state,
                              void(*callback)(gpointer), gpointer data)
{
    g_assert(bswin != NULL);
    g_assert(callback != NULL);
    bswin->states_callbacks[state] = 
        g_list_append(bswin->states_callbacks[state],
                      bs_callback_new(callback, data));
}


void
bs_window_add_types_callback(BSWindow *bswin, BSNetWMWindowType type,
                             void(*callback)(gpointer), gpointer data)
{
    bswin->types_callbacks[type] = 
        g_list_append(bswin->types_callbacks[type],
                      bs_callback_new(callback, data));
}


void
bs_window_add_desktop_callback(BSWindow *bswin, void(*callback)(gpointer),
                               gpointer data)
{
    bswin->desktop_callbacks = g_list_append(bswin->desktop_callbacks, 
                                             bs_callback_new(callback, data));
}


gchar *
bs_window_get_net_wm_icon_name(BSWindow *bswin, GError **error)
{
    gchar *data = NULL, *name;
    gulong nitems;
    if (bs_xutil_get_window_property(bswin->window, 
                                     NET_WM_ICON_NAME, AnyPropertyType, 8,
                                     &nitems, (guchar **)&data, error) != 0)
        return NULL;

    name = g_strdup(data);
    XFree(data);
    return name;
}


gchar *
bs_window_get_wm_icon_name(BSWindow *bswin, GError **error)
{
    gchar *data = NULL, *name;
    gulong nitems;
    if (bs_xutil_get_window_property(bswin->window, 
                                     WM_ICON_NAME, AnyPropertyType, 8, &nitems,
                                     (guchar **)&data, error) != 0)
        return NULL;

    name = g_strdup(data);
    XFree(data);
    return name;
}


void
bs_window_change_desktop(BSWindow *bswin, gint desktop)
{
    XEvent xev;

    xev.xclient.type = ClientMessage;
    xev.xclient.serial = 0;
    xev.xclient.send_event = True;
    xev.xclient.display = GDK_DISPLAY();
    xev.xclient.window = bswin->window;
    xev.xclient.message_type = NET_WM_DESKTOP;
    xev.xclient.format = 32;
    xev.xclient.data.l[0] = desktop;
    xev.xclient.data.l[1] = 0;
    xev.xclient.data.l[2] = 0;

    XSendEvent(GDK_DISPLAY(), DefaultRootWindow(GDK_DISPLAY()), False,
               SubstructureRedirectMask | SubstructureNotifyMask, &xev);

}


void 
bs_window_get_geometry(BSWindow *bswin, gint *x, gint *y, 
                       gint *width, gint *height)
{
    gdk_window_get_geometry(bswin->gdkwindow, NULL, NULL, width, height, NULL);
    gdk_window_get_root_origin(bswin->gdkwindow, x, y);
}

