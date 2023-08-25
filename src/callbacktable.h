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
#ifndef BS_CALLBACK_TABLE_H
#define BS_CALLBACK_TABLE_H

#include <glib.h>
#include "callbacklist.h"

typedef struct {
    GHashTable *table;
    BSCallbackList *list;
} BSCallbackTable;

BSCallbackTable *bs_callback_table_new();
void bs_callback_table_free(BSCallbackTable *table);
gint bs_callback_table_add(BSCallbackTable *table, const gchar *key, 
                           void (*cb)(void *), void *data);
void bs_callback_table_call(BSCallbackTable *table, const gchar *key);
void bs_callback_table_remove(BSCallbackTable *table, gint id);

#endif /* BS_CALLBACK_TABLE_H*/
