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
#include "xutil.h"
#include "error.h"

gint 
bs_xutil_get_window_property(Window id, Atom property, Atom type, gint format,
                             gulong *nitems, guchar **data, GError **error)
{
    gulong unused;
    Atom type_ret;
    gint format_ret;
    gint status, ec;

    g_assert(error == NULL || *error == NULL);

    gdk_error_trap_push();
    status = XGetWindowProperty(GDK_DISPLAY(), id, property, 0, G_MAXLONG,
                                False, type, &type_ret, &format_ret, nitems,
                                &unused, data);
    if ((ec = gdk_error_trap_pop())) {
        if (error)
            *error = g_error_new(BACKSTEP_ERROR, BS_ERROR_X_ERROR, "Received "
                                 "error code %d from XGetWindowProperty", ec);
        return -1;
    }

    if (status != Success) {
        if (error)
            *error = g_error_new(BACKSTEP_ERROR, BS_ERROR_WIN_PROP_NOEXIST,
                                 "Property %s not found on window %ld",
                                 XGetAtomName(GDK_DISPLAY(), property), id);
        return -1;
    }

    if (type_ret != type && type != AnyPropertyType) {
        if (error)
            *error = g_error_new(BACKSTEP_ERROR, BS_ERROR_WIN_PROP_WRONG_TYPE,
                                 "The %s property for window %ld is not the "
                                 "correct type", XGetAtomName(GDK_DISPLAY(),
                                 property), id);
        XFree(*data);
        return -1;
    }

    if (format_ret != format) {
        if (error)
            *error = g_error_new(BACKSTEP_ERROR, BS_ERROR_WIN_PROP_WRONG_FMT,
                                 "The %s property for window %ld is not the "
                                 "correct format", XGetAtomName(GDK_DISPLAY(),
                                 property), id);
        XFree(*data);
        return -1;
    }

    return 0;
}
                        
