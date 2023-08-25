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
#ifndef BS_CONFIGURATION_H
#define BS_CONFIGURATION_H

#include <glib.h>

typedef struct {
    gchar *name;
    gchar *default_value;
    gchar *allowed[];
} BSStringConfigItem;

typedef struct {
    gchar *name;
    gint default_value;
    gint min;
    gint max;
} BSIntegerConfigItem;

typedef struct {
    gchar *name;
    gboolean default_value;
} BSBooleanConfigItem;

void bs_configuration_init();

gboolean bs_configuration_get_boolean_value(BSBooleanConfigItem *ci);

gint bs_configuration_get_int_value(BSIntegerConfigItem *ci);

gchar *bs_configuration_get_string_value(BSStringConfigItem *ci);

void bs_configuration_set_value(const gchar *key, const gchar *value);

gint bs_configuration_add_callback(const gchar *key, void(*callback)(void *),
                                   void *data);

void bs_configuration_remove_callback(gint id);

#endif /* BS_CONFIGURATION_H */
