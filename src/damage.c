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
#include <gdk/gdkx.h>

#ifdef HAVE_RENDER
#include <X11/extensions/Xdamage.h>
#endif

#include "damage.h"

static gint event_base;

void
bs_damage_init()
{
#ifdef HAVE_RENDER
    gint error_base;
    XDamageQueryExtension(GDK_DISPLAY(), &event_base, &error_base);
    gdk_x11_register_standard_event_type(gdk_display_get_default(), 
                                         event_base, 1);
#else
    event_base = -1;
#endif
}


gint
bs_damage_event_base()
{
    return event_base;
}
