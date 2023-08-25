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
#ifndef BS_MANAGED_WINDOW
#define BS_MANAGED_WINDOW

#include "window.h"
#include "windowpixmap.h"
#include "iconwindow.h"

typedef struct {
    BSWindow *window;
    BSWindowPixmap *pixmap;
    BSIconWindow *icon_window;
    gint workspace_cb_id;
    gint group;
    gboolean propagate_to_group;
} BSManagedWindow;

BSManagedWindow *bs_managed_window_new(BSWindow *bswin);

void bs_managed_window_free(BSManagedWindow *bsmwin);
void bs_managed_window_set_group(BSManagedWindow *bsmwin, gint group);
void bs_managed_window_show_client(BSManagedWindow *bsmwin);
void bs_managed_window_hide_client(BSManagedWindow *bsmwin);

#endif /* BS_MANAGED_WINDOW */
