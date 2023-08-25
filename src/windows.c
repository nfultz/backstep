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
#include <X11/Xlib.h>
#include "windows.h"
#include "xutil.h"
#include "atoms.h"
#include "window.h"

static void free_bswin_list_elem(gpointer data, gpointer user_data);

GList *
bs_windows_get_clients(GError **error)
{
    Window *windows = NULL;
    gulong nitems, z;
    GList *list = NULL;

    if (bs_xutil_get_window_property(DefaultRootWindow(GDK_DISPLAY()),
                                     NET_CLIENT_LIST,
                                     XA_WINDOW,
                                     32,
                                     &nitems,
                                     (guchar **)&windows,
                                     error) != 0)
        return NULL;

    for (z = 0; z < nitems; z++) {
        Window xwin;
        BSWindow *bswin;
        
        xwin = *(windows + z);
        if (!(bswin = bs_window_new(xwin, error))) {
            g_list_foreach(list, free_bswin_list_elem, NULL);
            g_list_free(list);
            return NULL;
        }
        list = g_list_append(list, bswin);
    }
    return list;
}


void
bs_windows_free_list(GList *list)
{
    g_list_foreach(list, free_bswin_list_elem, NULL);
    g_list_free(list);
}


static void 
free_bswin_list_elem(gpointer data, gpointer user_data)
{
    BSWindow *bswin = (BSWindow *)data;
    bs_window_free(bswin);
}
