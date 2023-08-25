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
#include "groups.h"

static gint max_group = 0;
static GHashTable *table;

static void set_max_group(gint start);

void
bs_groups_init()
{
    table = g_hash_table_new_full(g_int_hash, g_int_equal, g_free, NULL);
}


gint
bs_groups_next_new_group()
{
    return max_group + 1;
}


void
bs_groups_assign(BSManagedWindow *win, gint group)
{
    GList *list;
    gint *key;

    g_assert(group > 0);
    /*g_assert(group <= (max_group + 1));*/

    key = (gint *)g_malloc(sizeof(gint));
    *key = group;

    list = g_hash_table_lookup(table, key);
    list = g_list_append(list, win);
    g_hash_table_insert(table, key, list);

    if (group > max_group)
        max_group = group;
}


void
bs_groups_unassign(BSManagedWindow *win, gint group)
{
    GList *list;
    gint *key;

    g_assert(group > 0);
    g_assert(group <= max_group);

    key = (gint *)g_malloc(sizeof(int));
    *key = group;
    list = g_hash_table_lookup(table, key);

    if (list) {
        list = g_list_remove(list, win);
        if (list)
            g_hash_table_insert(table, key, list);
        else {
            g_hash_table_remove(table, key);
            g_free(key);
            if (group == max_group)
                set_max_group(group - 1);
        }
    } else
        g_free(key);
}


void
bs_groups_client_shown(BSManagedWindow *win, gint group)
{
    GList *list;

    list = g_hash_table_lookup(table, &group);
    while (list) {
        BSManagedWindow *next = (BSManagedWindow *)list->data;
        if (next != win) {
            bs_managed_window_show_client(next);
        }
        list = list->next;
    }
}


void
bs_groups_client_hidden(BSManagedWindow *win, gint group)
{
    GList *list;

    list = g_hash_table_lookup(table, &group);
    while (list) {
        BSManagedWindow *next = (BSManagedWindow *)list->data;
        if (next != win) {
            bs_managed_window_hide_client(next);
        }
        list = list->next;
    }
}


static void set_max_group(gint start)
{
    g_assert(start >= 0);
    while (start > 0) {
        if (g_hash_table_lookup(table, &start) != NULL) {
            max_group = start;
            return;
        }
        start--;
    }
    max_group = 0;
}
