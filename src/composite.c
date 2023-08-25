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
#include <glib.h>
#include <gdk/gdkx.h>

#ifdef HAVE_RENDER
#include <X11/extensions/Xcomposite.h>
#endif

#include "composite.h"

static gboolean has_name_pixmap;

void
bs_composite_init()
{
#ifdef HAVE_RENDER
    gint event_base, error_base;
    gint major = 0, minor = 2;
    
    if (!XCompositeQueryExtension(GDK_DISPLAY(), &event_base, &error_base))
        g_critical("Composite extension not enabled in X server");

    XCompositeQueryVersion(GDK_DISPLAY(), &major, &minor);

    has_name_pixmap = (major > 0 || minor >= 2) ? TRUE : FALSE;

    XCompositeRedirectSubwindows(GDK_DISPLAY(),
                                 GDK_ROOT_WINDOW(),
                                 CompositeRedirectAutomatic);
#else
    has_name_pixmap = FALSE;
#endif
}


gboolean
bs_composite_has_name_pixmap()
{
    return has_name_pixmap;
}
