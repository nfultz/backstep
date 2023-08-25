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
#ifndef BS_WINDOW_PIXMAP_H
#define BS_WINDOW_PIXMAP_H

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <gdk/gdk.h>
#include "window.h"
#include "windowpixmaprender.h"
#include "windowpixmapicon.h"

typedef struct {
    gboolean use_xrender;
#ifdef HAVE_RENDER
    BSWindowPixmapRender *pm_render;
#endif
    BSWindowPixmapIcon *pm_icon;
} BSWindowPixmap;

BSWindowPixmap *bs_window_pixmap_new(BSWindow *bswin);
void bs_window_pixmap_free(BSWindowPixmap *bspm);

GdkPixmap *bs_window_pixmap_get_pixmap(BSWindowPixmap *bspm);
GdkBitmap *bs_window_pixmap_get_mask(BSWindowPixmap *bspm);

gboolean bs_window_pixmap_is_ready(BSWindowPixmap *bspm);

void bs_window_pixmap_set_changed_callback(BSWindowPixmap *bspm,
                                           void(*callback)(gpointer data),
                                           gpointer data);

void bs_window_pixmap_get_size(BSWindowPixmap *bspm, gint *width, gint *height);
gboolean bs_window_pixmap_use_common_frame_size(BSWindowPixmap *bspm);

#endif /* BS_WINDOW_PIXMAP_H */
