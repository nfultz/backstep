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
#ifndef BS_WINDOW_PIXMAP_RENDER_H
#define BS_WINDOW_PIXMAP_RENDER_H

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif
#ifdef HAVE_RENDER

#include <glib.h>
#include <gdk/gdk.h>
#include <X11/extensions/Xrender.h>
#include <X11/extensions/Xdamage.h>
#include "callback.h"

typedef struct {
    GdkPixmap *pixmap;
    GdkWindow *window;
    gint pm_width;
    gint pm_height;
    gint win_width;
    gint win_height;
    gint win_depth;
    Picture src_picture;
    Picture dst_picture;
    Damage damage;
    gboolean has_alpha;
    BSCallback *changed_callback;
} BSWindowPixmapRender;

BSWindowPixmapRender *bs_window_pixmap_render_new(GdkWindow *window);
void bs_window_pixmap_render_free(BSWindowPixmapRender *bspmr);
gboolean bs_window_pixmap_render_is_ready(BSWindowPixmapRender *bspmr);
void bs_window_pixmap_render_set_changed_callback(BSWindowPixmapRender *bspmr,
                                                 void(*callback)(gpointer data),
                                                 gpointer data);
void bs_window_pixmap_render_get_size(BSWindowPixmapRender *bspmr,
                                      gint *width,
                                      gint *height);

#endif
#endif /* BS_WINDOW_PIXMAP_RENDER_H */
