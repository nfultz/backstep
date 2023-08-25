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
#ifndef BS_ICON_WINDOW_H
#define BS_ICON_WINDOW_H

#include <glib.h>
#include <X11/Xlib.h>
#include "windowpixmap.h"
#include "callback.h"

typedef struct {
    Window window;
    GdkWindow *gdkwindow;
    Window dst_window;
    GdkWindow *dst_gdkwindow;
    BSWindowPixmap *src;
    gboolean has_position;
    BSCallback *activated_callback;
    void(*rightclick_action)(guint32 time, void *data);
    BSCallback *moved_callback;
    gpointer rightclick_data;
    gboolean sticky;
    gint border;
    gint mouse_state;
    gint frame_border_callback_id;
    gint frame_show_callback_id;
    gint frame_color_callback_id;
} BSIconWindow;

BSIconWindow *bs_icon_window_new(BSWindowPixmap *src);
void bs_icon_window_free(BSIconWindow *iw);
void bs_icon_window_show(BSIconWindow *iw, gint desktop);
void bs_icon_window_show_at(BSIconWindow *iw, gint desktop, gint x, gint y);
void bs_icon_window_hide(BSIconWindow *iw);
void bs_icon_window_set_activated_callback(BSIconWindow *iw,
                                           void(*callback)(gpointer data),
                                           gpointer data);
void bs_icon_window_set_rightclick_action(BSIconWindow *iw,
                                          void(*callback)(guint32 time,
                                                          gpointer data),
                                            gpointer data);
void bs_icon_window_set_sticky(BSIconWindow *iw, gboolean sticky);
gboolean bs_icon_window_is_positioned(BSIconWindow *iw);
void bs_icon_window_get_size(BSIconWindow *iw, gint *width, gint *height);
void bs_icon_window_get_gemoetry(BSIconWindow *iw, gint *x, gint *y, 
                                 gint *width, gint *height);
void bs_icon_window_set_moved_callback(BSIconWindow *iw,
                                       void(*callback)(gpointer data),
                                       gpointer data);

#endif /* BS_ICON_WINDOW_H */
