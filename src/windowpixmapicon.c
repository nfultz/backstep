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
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <gdk/gdkx.h>
#include "windowpixmapicon.h"
#include "xutil.h"
#include "atoms.h"
#include "settings.h"
#include "configuration.h"
#include "trace.h"

static char *default_icon_xpm_data[] = {
    "48 48 41 1",
    " 	c None",
    ".	c #1A88DC",
    "+	c #062135",
    "@	c #000000",
    "#	c #FFFFFF",
    "$	c #F1C6C6",
    "%	c #E79999",
    "&	c #E07A7A",
    "*	c #DC6B6B",
    "=	c #F5FDF5",
    "-	c #CFF5D3",
    ";	c #B2F0B8",
    ">	c #9DECA5",
    ",	c #90E998",
    "'	c #89E892",
    ")	c #DC6868",
    "!	c #D6706A",
    "~	c #ABBB87",
    "{	c #88E891",
    "]	c #B8F1BD",
    "^	c #F7FDF7",
    "/	c #EAA8A8",
    "(	c #CE7D6E",
    "_	c #93D68B",
    ":	c #97EB9F",
    "<	c #EBFBEC",
    "[	c #9EC686",
    "}	c #A7EEAD",
    "|	c #8AE48F",
    "1	c #8BE894",
    "2	c #A9B480",
    "3	c #D5726B",
    "4	c #BA9B78",
    "5	c #A6B982",
    "6	c #97D089",
    "7	c #8EE08F",
    "8	c #E07969",
    "9	c #FFF370",
    "0	c #E7956A",
    "a	c #F1BF6D",
    "b	c #EAA36B",
    "                                                ",
    "                                                ",
    "                                                ",
    "                                                ",
    "                                                ",
    "  ............................................  ",
    "  .++..@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@..++.  ",
    "  .++......................................++.  ",
    "  ............................................  ",
    "  ..########################################..  ",
    "  ..########################################..  ",
    "  ..########################################..  ",
    "  ..########################################..  ",
    "  ..########################################..  ",
    "  ..########################################..  ",
    "  ..########################################..  ",
    "  ..##########$%&**&%$#=-;>,'',>;-=#########..  ",
    "  ..########$&)))))))!~{{{{{{{{{{{{]^#######..  ",
    "  ..#######/))))))))(_{{{{{{{{{{{{{{:<######..  ",
    "  ..######/)))))))))[{{{{{{{{{{{{{{{{}######..  ",
    "  ..#####$))))))))))|{{{{{{{{{{{{{{{{1######..  ",
    "  ..#####&))))))))))[{{{{{{{{{{{{{{{{}######..  ",
    "  ..####$)))))))))))(_{{{{{{{{{{{{{{:<######..  ",
    "  ..####%))))))))))))!2{{{{{{{{{{{{]^#######..  ",
    "  ..####&))))))))))))))34567'',>;-=#########..  ",
    "  ..####*))))))))))))))))))*################..  ",
    "  ..####*))))))))))))))))))*################..  ",
    "  ..####&))))))))))))))))))899999999########..  ",
    "  ..####%))))))))))))))))))099999999########..  ",
    "  ..####$))))))))))))))))))a99999999########..  ",
    "  ..#####&))))))))))))))))8999999999########..  ",
    "  ..#####$))))))))))))))))a999999999########..  ",
    "  ..######/))))))))))))))b9999999999########..  ",
    "  ..#######/))))))))))))/99999999999########..  ",
    "  ..########$&))))))))&$#99999999999########..  ",
    "  ..##########$%&**&%$###99999999999########..  ",
    "  ..#####################99999999999########..  ",
    "  ..########################################..  ",
    "  ..########################################..  ",
    "  ..########################################..  ",
    "  ..########################################..  ",
    "  ............................................  ",
    "  ............................................  ",
    "                                                ",
    "                                                ",
    "                                                ",
    "                                                ",
    "                                                "
};


static void get_application_pixmap(GdkWindow *window, GdkPixmap **pixmap, 
                                   GdkBitmap **mask);
static void get_default_pixmap(GdkPixmap **pixmap, GdkBitmap **mask);
static void get_net_wm_icon(GdkWindow *window, GdkPixmap **pixmap,
                            GdkBitmap **mask);
static void argbdata_to_pixdata(gulong *argb_data, int len, guchar **pixdata);
static void free_pixels(guchar *pixels, gpointer data);
static void get_wm_icon(GdkWindow *window, GdkPixmap **pixmap,
                        GdkBitmap **mask);
static gulong find_best_size(gulong *data, gulong size);
static GdkPixbuf *scale_pixbuf(GdkPixbuf *pixbuf, gint w, gint h);
static void scale_pixmap(GdkPixmap **pixmapp, GdkBitmap **maskp, 
                         gint width, gint height);

BSWindowPixmapIcon *
bs_window_pixmap_icon_new(GdkWindow *window)
{
    BSWindowPixmapIcon *bspmi;

    bspmi = (BSWindowPixmapIcon *)g_malloc(sizeof(BSWindowPixmapIcon));
    bspmi->pixmap = NULL;
    bspmi->mask = NULL;
    get_application_pixmap(window, &bspmi->pixmap, &bspmi->mask);
    if (!bspmi->pixmap)
        get_default_pixmap(&bspmi->pixmap, &bspmi->mask);
    return bspmi;
}


void
bs_window_pixmap_icon_free(BSWindowPixmapIcon *bspmi)
{
    if (bspmi->pixmap)
        g_object_unref(bspmi->pixmap);
    if (bspmi->mask)
        g_object_unref(bspmi->mask);
    free(bspmi);
}


static void
get_default_pixmap(GdkPixmap **pixmap, GdkBitmap **mask)
{
    *pixmap = gdk_pixmap_create_from_xpm_d(gdk_get_default_root_window(),
                                           mask, NULL, default_icon_xpm_data);

    if (!*pixmap)
        g_critical("Could not create default pixmap from data");
    if (bs_configuration_get_boolean_value(BS_ICONS_SAME_SIZE)) {
        gint size;
        size = bs_configuration_get_int_value(BS_ICONS_SAME_SIZE_SIZE);
        if (size != 48)
            scale_pixmap(pixmap, mask, size, size);
    }
}


static void
get_application_pixmap(GdkWindow *window, GdkPixmap **pixmap, GdkBitmap **mask)
{
    get_net_wm_icon(window, pixmap, mask);
    if (!(*pixmap))
        get_wm_icon(window, pixmap, mask);
}


static void
free_pixels(guchar *pixels, gpointer data)
{
    g_free(pixels);
}


static void
get_net_wm_icon(GdkWindow *window, GdkPixmap **pixmap, GdkBitmap **mask)
{
    gulong *list = NULL, nitems, z = 0l;

    if (bs_xutil_get_window_property(GDK_WINDOW_XID(window),
                                     NET_WM_ICON,
                                     XA_CARDINAL,
                                     32,
                                     &nitems,
                                     (guchar **)&list,
                                     NULL) != 0) {
        trace("Could not get NET_WM_ICON property")
        return;
    }

    if (nitems > 0) {
        gint index;
        gulong width , height;
        guchar *pix_data = NULL;
        GdkPixbuf *pixbuf = NULL;

        index = find_best_size(list, nitems);

        if (index == -1) {
            g_warning("bad _NET_WM_ICON data");
            XFree(list);
            return;
        }

        width = *(list + index);
        height = *(list + index + 1);

        argbdata_to_pixdata(list + index + 2, width * height, &pix_data);
        pixbuf = gdk_pixbuf_new_from_data(pix_data, GDK_COLORSPACE_RGB, TRUE, 8,
                                          width, height, width * 4, free_pixels,
                                          NULL);

        if (bs_configuration_get_boolean_value(BS_ICONS_SAME_SIZE)) {
            gint size = bs_configuration_get_int_value(BS_ICONS_SAME_SIZE_SIZE);
            if (width != size && height && size)
                pixbuf = scale_pixbuf(pixbuf, size, size);

        }
        gdk_pixbuf_render_pixmap_and_mask(pixbuf,
                                          pixmap,
                                          mask,
                                          255);
        g_object_unref(pixbuf);

    }

    XFree(list);
}


static void
argbdata_to_pixdata(gulong *argb_data, gint len, guchar **pixdata)
{
    guchar *p;
    gint i;

    *pixdata = g_new(guchar, len * 4);
    p = *pixdata;

    i = 0;
    while (i < len)
    {
        guint argb;
        guint rgba;

        argb = argb_data[i++];
        rgba = (argb << 8) | (argb >> 24);

        *(p++) = rgba >> 24;
        *(p++) = (rgba >> 16) & 0xff;
        *(p++) = (rgba >> 8) & 0xff;
        *(p++) = rgba & 0xff;
    }
}


static void 
get_wm_icon(GdkWindow *window, GdkPixmap **pixmap, GdkBitmap **mask)
{
    XWMHints *hints;

    gdk_error_trap_push ();
    hints = XGetWMHints(GDK_DISPLAY(), GDK_WINDOW_XID(window));
    gdk_flush ();
    if (gdk_error_trap_pop()) {
        trace("X error for window %ld", GDK_WINDOW_XID(window));
    }
    if (!hints) {
        trace("WMHints not set")
        return;
    } else {
        trace("Got WMHints")
    }

    if (hints->icon_pixmap != None) {
        /* some windows (e.g., gnu emacs) will return hints where pixmap is *
         * bad but not None - so we need to recover from an x bad id error  */
        gdk_error_trap_push();
        *pixmap = gdk_pixmap_foreign_new(hints->icon_pixmap);
        gdk_flush();
        if (gdk_error_trap_pop())
            *pixmap = NULL;
    } else {
        trace("No pixmap set")
    }
    if (hints->icon_mask != None) {
        gdk_error_trap_push();
        *mask = gdk_pixmap_foreign_new(hints->icon_mask);
        gdk_flush();
        if (gdk_error_trap_pop())
            *mask = NULL;
    }
    if (*pixmap) {
        if (bs_configuration_get_boolean_value(BS_ICONS_SAME_SIZE)) {
            gint w, h, size;
            gdk_drawable_get_size(*pixmap, &w, &h);
            size = bs_configuration_get_int_value(BS_ICONS_SAME_SIZE_SIZE);
            if (w != size && h != size)
                scale_pixmap(pixmap, mask, size, size);
        }
    }
}


static gulong
find_best_size(gulong *data, gulong size)
{
    gulong index = 0, best_diff = -1, best_index = -1;

    while (index < size) {
        gint width, height, diff;
        
        width = *(data + index);
        height = *(data + index + 1);

        diff = ABS(48 - width);
        if (best_diff == -1 || diff < best_diff) {
            best_diff = diff;
            best_index = index;
        }
        
        index += ((width * height) + 2);
    }
    return best_index;
}


static GdkPixbuf *
scale_pixbuf(GdkPixbuf *pixbuf, gint width, gint height)
{
    return pixbuf;
    /* do nothing for now
    GdkPixbuf *scaled_pixbuf;


    scaled_pixbuf = gdk_pixbuf_scale_simple(pixbuf, width, height,
                                            GDK_INTERP_HYPER);
    g_object_unref(pixbuf);
    return scaled_pixbuf;*/
}


static void 
scale_pixmap(GdkPixmap **pixmapp, GdkBitmap **maskp, gint width, gint height)
{
    /* We're not scaling anything yet.  Some rather thin-looking icons look 
     * really bad or almost disappear when scaled.  Also we need to reverse 
     * the white and black pixels when the pixmap has depth 1
    GdkPixmap *pixmap = *pixmapp;
    GdkBitmap *mask = *maskp;
    GdkPixbuf *pixbuf, *maskbuf;
    GdkColormap *cm;
    gint w, h;

    gdk_drawable_get_size(pixmap, &w, &h);
    pixbuf = gdk_pixbuf_new(GDK_COLORSPACE_RGB, TRUE, 8, w, h);
    if (maskp && mask)
        maskbuf = gdk_pixbuf_new(GDK_COLORSPACE_RGB, TRUE, 8, w, h);
    cm = gdk_drawable_get_colormap(pixmap);
    if (!cm && gdk_drawable_get_depth(pixmap) != 1)
        cm = gdk_colormap_get_system();
    gdk_pixbuf_get_from_drawable(pixbuf, pixmap, cm, 0, 0, 0, 0, w, h);
    pixbuf = scale_pixbuf(pixbuf, width, height);
    g_object_unref(pixmap);
    *pixmapp = NULL; 
    gdk_pixbuf_render_pixmap_and_mask(pixbuf, pixmapp, NULL, 255);
    g_object_unref(pixbuf);
    if (maskp && mask) {
        gdk_pixbuf_get_from_drawable(maskbuf, mask, NULL, 0, 0, 0, 0, w, h);
        maskbuf = scale_pixbuf(maskbuf, width, height);
        g_object_unref(mask);
        *maskp = NULL; 
        gdk_pixbuf_render_pixmap_and_mask(maskbuf, NULL, maskp, 255);
        g_object_unref(maskbuf);
    }*/
}
