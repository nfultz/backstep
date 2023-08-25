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
#include "window.h"
#include "windowpixmap.h"
#include "configuration.h"
#include "settings.h"
#include "trace.h"

#ifdef HAVE_RENDER
#define standard_asserts()                           \
{                                                    \
    g_assert(bspm);                                  \
    g_assert(bspm->pm_render || !bspm->use_xrender); \
    g_assert(bspm->pm_icon || bspm->use_xrender);    \
}
#else
#define standard_asserts()   \
{                            \
    g_assert(bspm);          \
    g_assert(bspm->pm_icon); \
}
#endif

BSWindowPixmap *
bs_window_pixmap_new(BSWindow *window)
{
    BSWindowPixmap *bspm;

    bspm = (BSWindowPixmap *)g_malloc(sizeof(BSWindowPixmap));
    bspm->pm_icon = NULL;
#ifdef HAVE_RENDER
    bspm->pm_render = NULL;
    bspm->use_xrender = bs_configuration_get_boolean_value(BS_ICONS_XRENDER);
    if (bspm->use_xrender)
        bspm->pm_render = bs_window_pixmap_render_new(window->gdkwindow);
    else
        bspm->pm_icon = bs_window_pixmap_icon_new(window->gdkwindow);
#else
    bspm->use_xrender = FALSE;
    bspm->pm_icon = bs_window_pixmap_icon_new(window->gdkwindow);
#endif
    return bspm;
}


void
bs_window_pixmap_free(BSWindowPixmap *bspm)
{
    g_assert(bspm);
#ifdef HAVE_RENDER
    if (bspm->pm_render)
        bs_window_pixmap_render_free(bspm->pm_render);
#endif
    if (bspm->pm_icon)
        bs_window_pixmap_icon_free(bspm->pm_icon);
    g_free(bspm);
}


GdkPixmap *
bs_window_pixmap_get_pixmap(BSWindowPixmap *bspm)
{
    standard_asserts();
#ifdef HAVE_RENDER
    return bspm->use_xrender ? bspm->pm_render->pixmap : bspm->pm_icon->pixmap;
#else
    return bspm->pm_icon->pixmap;
#endif
}


GdkBitmap *
bs_window_pixmap_get_mask(BSWindowPixmap *bspm)
{
    standard_asserts();
#ifdef HAVE_RENDER
    return bspm->pm_render ? NULL : bspm->pm_icon->mask;
#else
    return bspm->pm_icon->mask;
#endif
}


gboolean
bs_window_pixmap_is_ready(BSWindowPixmap *bspm)
{
    standard_asserts();
#ifdef HAVE_RENDER
    return bspm->use_xrender ? 
        bs_window_pixmap_render_is_ready(bspm->pm_render) : TRUE;
#else
    return TRUE;
#endif
}

void 
bs_window_pixmap_set_changed_callback(BSWindowPixmap *bspm,
                                      void(*callback)(gpointer data),
                                      gpointer data)
{
    standard_asserts();
#ifdef HAVE_RENDER
    if (bspm->pm_render)
        bs_window_pixmap_render_set_changed_callback(bspm->pm_render,
                                                     callback,
                                                     data);
#endif
}


void
bs_window_pixmap_get_size(BSWindowPixmap *bspm, gint *width, gint *height)
{
    standard_asserts();
#ifdef HAVE_RENDER
    if (bspm->pm_render)
        bs_window_pixmap_render_get_size(bspm->pm_render, width, height);
    else if (bspm->pm_icon) {
        gdk_drawable_get_size(bspm->pm_icon->pixmap, width, height);
    }
#else
    gdk_drawable_get_size(bspm->pm_icon->pixmap, width, height);
#endif
}


gboolean
bs_window_pixmap_use_common_frame_size(BSWindowPixmap *bspm)
{
    standard_asserts();
#ifdef HAVE_RENDER
    return bspm->pm_render == NULL;
#else
    return TRUE;
#endif
}
