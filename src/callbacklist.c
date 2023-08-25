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
#include "callbacklist.h"
#include "callback.h"

BSCallbackList *
bs_callback_list_new()
{
    BSCallbackList *list;

    list = (BSCallbackList *)g_malloc(sizeof(BSCallbackList));
    list->list = NULL;
    return list;
}


void
bs_callback_list_free(BSCallbackList *list)
{
    GList *tmp = list->list;
    while (tmp) {
        BSCallback *cb = (BSCallback *)tmp->data;
        bs_callback_free(cb);
        tmp = tmp->next;
    }
    g_list_free(list->list);
    g_free(list);
}


int
bs_callback_list_append(BSCallbackList *list, void (*cb)(void *), void *data)
{
    BSCallback *bscb = bs_callback_new(cb, data);
    list->list = g_list_append(list->list, bscb);
    return bs_callback_id(bscb);
}


void
bs_callback_list_call(BSCallbackList *list)
{
    GList *tmp = list->list;
    while (tmp) {
        BSCallback *cb = (BSCallback *)tmp->data;
        bs_callback_call(cb);
        tmp = tmp->next;
    }
}


gboolean
bs_callback_list_remove(BSCallbackList *list, gint id)
{
    GList *tmp = list->list;
    while (tmp) {
        BSCallback *cb = (BSCallback *)tmp->data;
        if (bs_callback_id(cb) == id) {
            bs_callback_free(cb);
            list->list = g_list_delete_link(list->list, tmp);
            return TRUE;
        }
        tmp = tmp->next;
    }
    return FALSE;
}


gboolean 
bs_callback_list_contains_callback(BSCallbackList *list, gint id)
{
    GList *tmp = list->list;
    while (tmp) {
        BSCallback *cb = (BSCallback *)tmp->data;
        if (bs_callback_id(cb) == id)
            return TRUE;
        tmp = tmp->next;
    }
    return FALSE;
}
