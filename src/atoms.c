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
#include <unistd.h>
#include <glib.h>
#include <gdk/gdk.h>
#include <gdk/gdkx.h>
#include "atoms.h"

Atom NET_CLIENT_LIST;
Atom NET_WM_WINDOW_TYPE;
Atom NET_WM_WINDOW_TYPE_DESKTOP;
Atom NET_WM_WINDOW_TYPE_DOCK;
Atom NET_WM_WINDOW_TYPE_TOOLBAR;
Atom NET_WM_WINDOW_TYPE_MENU;
Atom NET_WM_WINDOW_TYPE_UTILITY;
Atom NET_WM_WINDOW_TYPE_SPLASH;
Atom NET_WM_WINDOW_TYPE_DIALOG;
Atom NET_WM_WINDOW_TYPE_NORMAL;
Atom NET_WM_STATE;
Atom NET_WM_STATE_MODAL;
Atom NET_WM_STATE_STICKY;
Atom NET_WM_STATE_MAXIMIZED_VERT;
Atom NET_WM_STATE_MAXIMIZED_HORZ;
Atom NET_WM_STATE_SHADED;
Atom NET_WM_STATE_SKIP_TASKBAR;
Atom NET_WM_STATE_SKIP_PAGER;
Atom NET_WM_STATE_HIDDEN;
Atom NET_WM_STATE_FULLSCREEN;
Atom NET_WM_STATE_ABOVE;
Atom NET_WM_STATE_BELOW;
Atom NET_WM_STATE_DEMANDS_ATTENTION;
Atom NET_WM_DESKTOP;
Atom NET_CURRENT_DESKTOP;
Atom NET_WM_ICON;
Atom NET_WORKAREA;
Atom NET_WM_ICON_NAME;
Atom NET_WM_ICON_GEOMETRY;
Atom NET_WM_HANDLED_ICONS;
Atom WM_ICON_NAME;

void
bs_atoms_init()
{
    int count = 0;

    NET_CLIENT_LIST = XInternAtom(GDK_DISPLAY(), "_NET_CLIENT_LIST", True);

    /* wait for ewmh-compliant window manager */
    while (NET_CLIENT_LIST == None && count++ < 60) {
        g_message("_NET_CLIENT_LIST not set: sleeping for 1 second...");
        sleep(1);
        NET_CLIENT_LIST = XInternAtom(GDK_DISPLAY(), "_NET_CLIENT_LIST", True);
    }
    if (NET_CLIENT_LIST == None) {
        g_critical(
                "Cannot get window list: _NET_CLIENT_LIST Atom does not exist");
    }

    NET_WM_WINDOW_TYPE          = XInternAtom(GDK_DISPLAY(),
                                              "_NET_WM_WINDOW_TYPE", 
                                              False);
    NET_WM_WINDOW_TYPE_DESKTOP  = XInternAtom(GDK_DISPLAY(), 
                                              "_NET_WM_WINDOW_TYPE_DESKTOP",
                                              False);
    NET_WM_WINDOW_TYPE_DOCK     = XInternAtom(GDK_DISPLAY(),
                                              "_NET_WM_WINDOW_TYPE_DOCK",
                                              False);
    NET_WM_WINDOW_TYPE_TOOLBAR  = XInternAtom(GDK_DISPLAY(),
                                              "_NET_WM_WINDOW_TYPE_TOOLBAR",
                                              False);
    NET_WM_WINDOW_TYPE_MENU     = XInternAtom(GDK_DISPLAY(),
                                              "_NET_WM_WINDOW_TYPE_MENU",
                                              False);
    NET_WM_WINDOW_TYPE_UTILITY  = XInternAtom(GDK_DISPLAY(),
                                              "_NET_WM_WINDOW_TYPE_UTILITY", 
                                              False);
    NET_WM_WINDOW_TYPE_SPLASH   = XInternAtom(GDK_DISPLAY(),
                                              "_NET_WM_WINDOW_TYPE_SPLASH",
                                              False);
    NET_WM_WINDOW_TYPE_DIALOG   = XInternAtom(GDK_DISPLAY(),
                                              "_NET_WM_WINDOW_TYPE_DIALOG",
                                              False);
    NET_WM_WINDOW_TYPE_NORMAL   = XInternAtom(GDK_DISPLAY(),
                                              "_NET_WM_WINDOW_TYPE_NORMAL",
                                              False);
    NET_WM_STATE                = XInternAtom(GDK_DISPLAY(), 
                                              "_NET_WM_STATE",
                                              False);
    NET_WM_STATE_MODAL          = XInternAtom(GDK_DISPLAY(),
                                              "_NET_WM_STATE_MODAL",
                                              False);
    NET_WM_STATE_STICKY         = XInternAtom(GDK_DISPLAY(),
                                              "_NET_WM_STATE_STICKY",
                                              False);
    NET_WM_STATE_MAXIMIZED_VERT = XInternAtom(GDK_DISPLAY(),
                                              "_NET_WM_STATE_MAXIMIZED_VERT",
                                              False);
    NET_WM_STATE_MAXIMIZED_HORZ = XInternAtom(GDK_DISPLAY(),
                                              "_NET_WM_STATE_MAXIMIZED_HORZ",
                                              False);
    NET_WM_STATE_SHADED         = XInternAtom(GDK_DISPLAY(),
                                              "_NET_WM_STATE_SHADED",
                                              False);
    NET_WM_STATE_SKIP_TASKBAR   = XInternAtom(GDK_DISPLAY(),
                                              "_NET_WM_STATE_SKIP_TASKBAR",
                                              False);
    NET_WM_STATE_SKIP_PAGER     = XInternAtom(GDK_DISPLAY(),
                                              "_NET_WM_STATE_SKIP_PAGER",
                                              False);
    NET_WM_STATE_HIDDEN         = XInternAtom(GDK_DISPLAY(),
                                              "_NET_WM_STATE_HIDDEN",
                                              False);
    NET_WM_STATE_FULLSCREEN     = XInternAtom(GDK_DISPLAY(),
                                              "_NET_WM_STATE_FULLSCREEN",
                                              False);
    NET_WM_STATE_ABOVE          = XInternAtom(GDK_DISPLAY(),
                                              "_NET_WM_STATE_ABOVE",
                                              False);
    NET_WM_STATE_BELOW          = XInternAtom(GDK_DISPLAY(),
                                              "_NET_WM_STATE_BELOW",
                                              False);
    NET_WM_STATE_DEMANDS_ATTENTION = XInternAtom(GDK_DISPLAY(),
                                              "_NET_WM_STATE_DEMANDS_ATTENTION",
                                              False);
    NET_WM_DESKTOP             = XInternAtom(GDK_DISPLAY(),
                                             "_NET_WM_DESKTOP",
                                             False);
    NET_CURRENT_DESKTOP        = XInternAtom(GDK_DISPLAY(),
                                             "_NET_CURRENT_DESKTOP",
                                             False);
    NET_WM_ICON                = XInternAtom(GDK_DISPLAY(),
                                             "_NET_WM_ICON",
                                             False);
    NET_WORKAREA               = XInternAtom(GDK_DISPLAY(),
                                             "_NET_WORKAREA",
                                             False);
    NET_WM_ICON_NAME           = XInternAtom(GDK_DISPLAY(),
                                             "_NET_WM_ICON_NAME",
                                             False);
    NET_WM_ICON_GEOMETRY       = XInternAtom(GDK_DISPLAY(),
                                             "_NET_WM_ICON_GEOMETRY",
                                             False);
    NET_WM_HANDLED_ICONS       = XInternAtom(GDK_DISPLAY(),
                                             "_NET_WM_HANDLED_ICONS",
                                             False);
    WM_ICON_NAME               = XInternAtom(GDK_DISPLAY(),
                                             "WM_ICON_NAME",
                                             False);
}
