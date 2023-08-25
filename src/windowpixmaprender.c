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
#ifdef HAVE_RENDER
#include <gdk/gdkx.h>
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/Xutil.h>
#include <X11/extensions/Xrender.h>
#include <X11/extensions/Xcomposite.h>
#include <X11/extensions/Xdamage.h>
#include "windowpixmaprender.h"
#include "trace.h"

static double scale = .33;

static void window_first_showing(BSWindowPixmapRender *bspmr);

static GdkFilterReturn event_filter(GdkXEvent *xevent,
                                    GdkEvent *gdkevent, 
                                    gpointer data);


BSWindowPixmapRender *
bs_window_pixmap_render_new(GdkWindow *window)
{
    BSWindowPixmapRender *bspmr;

    bspmr = (BSWindowPixmapRender *)g_malloc(sizeof(BSWindowPixmapRender));
    bspmr->pixmap = NULL;
    bspmr->window = window;
    g_object_ref(window);
    bspmr->src_picture = None;
    bspmr->dst_picture = None;
    bspmr->damage = None;
    bspmr->changed_callback = NULL;
    bspmr->pm_width = -1;
    bspmr->pm_height = -1;

    if (gdk_window_is_visible(bspmr->window))
        window_first_showing(bspmr);
    else
        gdk_window_set_events(bspmr->window,
                              gdk_window_get_events(bspmr->window) | 
                              GDK_STRUCTURE_MASK);

    gdk_window_add_filter(bspmr->window, event_filter, bspmr);
    return bspmr;
}


void
bs_window_pixmap_render_free(BSWindowPixmapRender *bspmr)
{
    gdk_window_remove_filter(bspmr->window, event_filter, bspmr);

    /*
     * FIXME this produces an X Window System error
     * Need to figure out if/how to free the damage resource
     */
    /*if (bspmr->damage != None) {
        XDamageDestroy(GDK_DISPLAY(), bspmr->damage);
        bspmr->damage = None;
    }*/

    /*
     * FIXME this produces a RenderBadPicture error
     * Need to figure out if/how to free the src picture
     */
    /*if (bspmr->src_picture != None) {
        XRenderFreePicture(GDK_DISPLAY(), bspmr->src_picture);
        bspmr->src_picture = None;
    }*/

    if (bspmr->dst_picture != None) {
        XRenderFreePicture(GDK_DISPLAY(), bspmr->dst_picture);
        bspmr->dst_picture = None;
    }

    g_object_unref(bspmr->window);
    bspmr->window = NULL;

    if (bspmr->pixmap) {
        g_object_unref(bspmr->pixmap);
        bspmr->pixmap = NULL;
    }

    if (bspmr->changed_callback) {
        bs_callback_free(bspmr->changed_callback);
        bspmr->changed_callback = NULL;
    }

    free(bspmr);
}


gboolean
bs_window_pixmap_render_is_ready(BSWindowPixmapRender *bspmr)
{
    return bspmr->pixmap != NULL;
}


void
bs_window_pixmap_render_set_changed_callback(BSWindowPixmapRender *bspmr,
                                             void(*callback)(gpointer data),
                                             gpointer data)
{
    if (bspmr->changed_callback)
        bs_callback_free(bspmr->changed_callback);
    bspmr->changed_callback = bs_callback_new(callback, data);
}


static void 
window_first_showing(BSWindowPixmapRender *bspmr)
{
    XRenderPictureAttributes src_pa, dst_pa;
    XRenderPictFormat *format;
    XTransform xform = {{
        { XDoubleToFixed(1), XDoubleToFixed(0), XDoubleToFixed(0)     },
        { XDoubleToFixed(0), XDoubleToFixed(1), XDoubleToFixed(0)     },
        { XDoubleToFixed(0), XDoubleToFixed(0), XDoubleToFixed(scale) }
    }};

    gdk_window_get_geometry(bspmr->window, NULL, NULL, &(bspmr->win_width),
                            &(bspmr->win_height), &(bspmr->win_depth));

    bspmr->pm_width = (gint)floor(bspmr->win_width * scale);
    bspmr->pm_height = (gint)floor(bspmr->win_height * scale);

    src_pa.subwindow_mode = IncludeInferiors;
    dst_pa.subwindow_mode = IncludeInferiors;

    format = XRenderFindVisualFormat(GDK_DISPLAY(), 
                                     GDK_VISUAL_XVISUAL(
                                       gdk_drawable_get_visual(bspmr->window)));
    bspmr->has_alpha = (format->type == PictTypeDirect && 
                        format->direct.alphaMask);

    bspmr->src_picture = XRenderCreatePicture(GDK_DISPLAY(), 
                                              GDK_WINDOW_XID(bspmr->window),
                                              format,
                                              CPSubwindowMode,
                                              &src_pa);

    bspmr->pixmap = gdk_pixmap_new(bspmr->window, bspmr->pm_width,
                                   bspmr->pm_height, bspmr->win_depth);

    bspmr->dst_picture = XRenderCreatePicture(GDK_DISPLAY(), 
                                              GDK_PIXMAP_XID(bspmr->pixmap),
                                              format,
                                              CPSubwindowMode,
                                              &dst_pa);
    XRenderSetPictureTransform(GDK_DISPLAY(), bspmr->src_picture, &xform);
    XRenderComposite(GDK_DISPLAY(),
                     bspmr->has_alpha ? PictOpOver : PictOpSrc,
                     bspmr->src_picture,
                     None,
                     bspmr->dst_picture,
                     0,
                     0,
                     0,
                     0,
                     0,
                     0,
                     bspmr->pm_width,
                     bspmr->pm_height);

    bspmr->damage = XDamageCreate(GDK_DISPLAY(),
                                  GDK_WINDOW_XID(bspmr->window),
                                  XDamageReportNonEmpty);

    if (bspmr->changed_callback)
        bs_callback_call(bspmr->changed_callback);
}
    

static GdkFilterReturn
event_filter(GdkXEvent *xevent, GdkEvent *gdkevent, gpointer data)
{
    XEvent *event;
    BSWindowPixmapRender *bspmr;
    
    event = (XEvent *)xevent;
    bspmr = (BSWindowPixmapRender *)data;

    if (event->type == bs_damage_event_base() + XDamageNotify) {
        XDamageNotifyEvent *damage_event;
        
        damage_event = (XDamageNotifyEvent *)event;
        XDamageSubtract(GDK_DISPLAY(), damage_event->damage, None, None);
        if (!damage_event->more) {

            XRenderComposite(GDK_DISPLAY(),
                             bspmr->has_alpha ? PictOpOver : PictOpSrc,
                             bspmr->src_picture,
                             None,
                             bspmr->dst_picture,
                             damage_event->area.x,
                             damage_event->area.y,
                             0,
                             0,
                             damage_event->area.x,
                             damage_event->area.y,
                             damage_event->area.width,
                             damage_event->area.height);
        }
    } else if (event->type == MapNotify) {
        if (bspmr->pixmap == NULL)
            window_first_showing(bspmr);
    }
    return GDK_FILTER_CONTINUE;
}


void
bs_window_pixmap_render_get_size(BSWindowPixmapRender *bspmr, gint *w, gint *h)
{
    *w = bspmr->pm_width;
    *h = bspmr->pm_height;
}
#endif /* HAVE_RENDER */
