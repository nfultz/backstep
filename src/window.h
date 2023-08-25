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
#ifndef BS_WINDOW_H
#define BS_WINDOW_H

#include <glib.h>
#include <gdk/gdk.h>
#include <X11/Xlib.h>

typedef enum {
    BS_NET_WM_STATE_MODAL,
    BS_NET_WM_STATE_STICKY,
    BS_NET_WM_STATE_MAXIMIZED_VERT,
    BS_NET_WM_STATE_MAXIMIZED_HORZ,
    BS_NET_WM_STATE_SHADED,
    BS_NET_WM_STATE_SKIP_TASKBAR,
    BS_NET_WM_STATE_SKIP_PAGER,
    BS_NET_WM_STATE_HIDDEN,
    BS_NET_WM_STATE_FULLSCREEN,
    BS_NET_WM_STATE_ABOVE,
    BS_NET_WM_STATE_BELOW,
    BS_NET_WM_STATE_DEMANDS_ATTENTION,
    BS_NET_WM_STATE_MAX
} BSNetWMState;

typedef enum {
    BS_NET_WM_WINDOW_TYPE_DESKTOP,
    BS_NET_WM_WINDOW_TYPE_DOCK,
    BS_NET_WM_WINDOW_TYPE_TOOLBAR,
    BS_NET_WM_WINDOW_TYPE_MENU,
    BS_NET_WM_WINDOW_TYPE_UTILITY,
    BS_NET_WM_WINDOW_TYPE_SPLASH,
    BS_NET_WM_WINDOW_TYPE_DIALOG,
    BS_NET_WM_WINDOW_TYPE_NORMAL,
    BS_NET_WM_WINDOW_TYPE_MAX
} BSNetWMWindowType;


/*
 * This is an opaque structure - don't mess with its internals directly.
 */
typedef struct {
    Window window;
    GdkWindow *gdkwindow;
    BSNetWMState *states;
    BSNetWMWindowType *types;
    gint desktop;
    GList **states_callbacks;
    GList **types_callbacks;
    GList *desktop_callbacks;
} BSWindow;

/*
 * Create a new BSWindow.  *error will be set if there is a problem updating the
 * window's states or types or desktop and NULL will be returned.
 */
BSWindow *bs_window_new(Window xid, GError **error);

/*
 * Deallocate the BSWindow.  Does not destroy the underlying Window.
 */
void bs_window_free(BSWindow *bswin);

/*
 * Force the window to update its current states. Returns non-zero if an error
 * occurs, in which case *error will be set.
 */
gint bs_window_update_states(BSWindow *bswin, GError **error);

/*
 * Force the window to update its current types. Returns non-zero if an error
 * occurs, in which case *error will be set.
 */
gint bs_window_update_types(BSWindow *bswin, GError **error);

/*
 * Force the window to update its current desktop. Returns non-zero if an error
 * occurs, in which case *error will be set.
 */
gint bs_window_update_desktop(BSWindow *bswin, GError **error);

/*
 * Returns whether the BSWindow has the specified state.  This does not update
 * the BSwindow's status.
 */
gboolean bs_window_is_state(BSWindow *bswin, BSNetWMState state);

/*
 * Returns whether the BSWindow is the specified type.  This does not update the
 * BSwindow's status.
 */
gboolean bs_window_is_type(BSWindow *bswin, BSNetWMWindowType type);

/*
 * Returns the current desktop of the BSWindow.  This does not update the
 * BSwindow's status.
 */
gint bs_window_get_desktop(BSWindow *bswin);

void bs_window_add_states_callback(BSWindow *bswin, BSNetWMState state,
                                   void(*callback)(gpointer), gpointer data);

void bs_window_add_types_callback(BSWindow *bswin, BSNetWMWindowType type,
                                  void(*callback)(gpointer), gpointer data);

void bs_window_add_desktop_callback(BSWindow *bswin, void(*callback)(gpointer),
                                    gpointer data);

gchar *bs_window_get_net_wm_icon_name(BSWindow *bswin, GError **error);

gchar *bs_window_get_wm_icon_name(BSWindow *bswin, GError **error);

void bs_window_change_desktop(BSWindow *bswin, gint desktop);

void bs_window_get_geometry(BSWindow *bswin, gint *x, gint *y, 
                            gint *width, gint *height);

#endif /* BS_WINDOW_H */
