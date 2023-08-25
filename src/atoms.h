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
#ifndef BS_ATOMS_H
#define BS_ATOMS_H

#include <X11/Xlib.h>
#include <X11/Xatom.h>

extern Atom NET_CLIENT_LIST;
extern Atom NET_WM_WINDOW_TYPE;
extern Atom NET_WM_WINDOW_TYPE_DESKTOP;
extern Atom NET_WM_WINDOW_TYPE_DOCK;
extern Atom NET_WM_WINDOW_TYPE_TOOLBAR;
extern Atom NET_WM_WINDOW_TYPE_MENU;
extern Atom NET_WM_WINDOW_TYPE_UTILITY;
extern Atom NET_WM_WINDOW_TYPE_SPLASH;
extern Atom NET_WM_WINDOW_TYPE_DIALOG;
extern Atom NET_WM_WINDOW_TYPE_NORMAL;
extern Atom NET_WM_STATE;
extern Atom NET_WM_STATE_MODAL;
extern Atom NET_WM_STATE_STICKY;
extern Atom NET_WM_STATE_MAXIMIZED_VERT;
extern Atom NET_WM_STATE_MAXIMIZED_HORZ;
extern Atom NET_WM_STATE_SHADED;
extern Atom NET_WM_STATE_SKIP_TASKBAR;
extern Atom NET_WM_STATE_SKIP_PAGER;
extern Atom NET_WM_STATE_HIDDEN;
extern Atom NET_WM_STATE_FULLSCREEN;
extern Atom NET_WM_STATE_ABOVE;
extern Atom NET_WM_STATE_BELOW;
extern Atom NET_WM_STATE_DEMANDS_ATTENTION;
extern Atom NET_WM_DESKTOP;
extern Atom NET_CURRENT_DESKTOP;
extern Atom NET_WM_ICON;
extern Atom NET_WORKAREA;
extern Atom NET_WM_ICON_NAME;
extern Atom NET_WM_ICON_GEOMETRY;
extern Atom NET_WM_HANDLED_ICONS;
extern Atom WM_ICON_NAME;

void bs_atoms_init();

#endif /* BS_ATOMS_H */
