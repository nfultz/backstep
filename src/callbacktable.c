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
#include "callbacktable.h"

static void free_table_value(gpointer key, gpointer value, gpointer data);
static void remove_cb_from_value(gpointer key, gpointer value, gpointer data);


BSCallbackTable *
bs_callback_table_new()
{
    BSCallbackTable *table;

    table = (BSCallbackTable *)g_malloc(sizeof(BSCallbackTable));
    table->table = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, NULL);
    table->list = bs_callback_list_new();
    return table;
}


void
bs_callback_table_free(BSCallbackTable *table)
{
    bs_callback_list_free(table->list);
    g_hash_table_foreach(table->table, free_table_value, NULL);
    g_hash_table_destroy(table->table);
    g_free(table);
}


gint
bs_callback_table_add(BSCallbackTable *table, const gchar *key,
                      void (*cb)(void *), void *data)
{
    BSCallbackList *list;

    if (key == NULL)
        list = table->list;
    else {
        list = g_hash_table_lookup(table->table, key);
        if (list == NULL) {
            list = bs_callback_list_new();
            g_hash_table_insert(table->table, g_strdup(key), list);
        }
    }

    return bs_callback_list_append(list, cb, data);
}


void
bs_callback_table_call(BSCallbackTable *table, const gchar *key)
{
    BSCallbackList *list;

    if (key == NULL)
        list = table->list;
    else {
        list = g_hash_table_lookup(table->table, key);
        if (list == NULL)
            return;
    }
    
    bs_callback_list_call(list);
}


void
bs_callback_table_remove(BSCallbackTable *table, gint id)
{
    if (bs_callback_list_contains_callback(table->list, id)) {
        bs_callback_list_remove(table->list, id);
        return;
    }

    g_hash_table_foreach(table->table, remove_cb_from_value, &id);
}


static void 
free_table_value(gpointer key, gpointer value, gpointer data)
{
    BSCallbackList *list = (BSCallbackList *)value;
    bs_callback_list_free(list);
}


static void 
remove_cb_from_value(gpointer key, gpointer value, gpointer data)
{
    BSCallbackList *list = (BSCallbackList *)value;
    gint id = *((gint *)data);
    bs_callback_list_remove(list, id);
}
