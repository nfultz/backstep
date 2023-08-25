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
/*
 * Session management support.
 * This code was borrowed/adapted from the ROX filer source code.
 */

#include <stdlib.h>
#include <fcntl.h> 
#include <gtk/gtk.h>
#include <X11/Xlib.h>
#include <X11/SM/SMlib.h>
#include <string.h>
#include "sessionclient.h"


typedef struct {
	SmProp prop;
	gboolean set;
} SessionProperty;


static SmPropValue *
make_list_of_array_value(const gchar *vals[], gint nvals)
{
    SmPropValue *values = g_new(SmPropValue, nvals);
    gint i;
    
    for(i = 0; i < nvals; i++) {
        values[i].value = g_strdup(vals[i]);
        values[i].length = strlen(vals[i]);
    }
    return values;
}


static SmPropValue *
make_array_value(const gchar *val)
{
    SmPropValue *value = g_new(SmPropValue, 1);
    
    value->value = g_strdup(val);
    value->length = strlen(val);

    return value;
}


static SmPropValue *
make_card_value(gchar val)
{
    SmPropValue *value = g_new(SmPropValue, 1);
    
    value->value = g_memdup(&val, sizeof(gchar));
    value->length = 1;
    
    return value;
}


static SessionProperty *
find_property(BSSessionClient *client, const gchar *name)
{
    GSList *list = client->props;
    SessionProperty *prop;

    for (; list; list = list->next) {
        prop = (SessionProperty *)list->data;
        if (strcmp(prop->prop.name, name) == 0)
            return prop;
    }
    return NULL;        
}


static SessionProperty *
new_property(BSSessionClient *client, const gchar *name, const gchar *type)
{
    SessionProperty *prop;

    prop = g_new(SessionProperty, 1);
    prop->prop.name = g_strdup(name);
    prop->prop.type = g_strdup(type);
    prop->prop.vals = NULL;
    prop->prop.num_vals = 0;
    client->props = g_slist_append(client->props, prop);

    return prop;
}


static gint 
close_connection(gpointer data)
{
    BSSessionClient *client = (BSSessionClient *)data;
    
    SmcCloseConnection(client->conn, 0, NULL);
    
    return 0;
}


static void 
poll_ice_messages(gpointer data, gint source, GdkInputCondition condition)
{
    BSSessionClient *client = (BSSessionClient *)data;
    Bool ret;

    if (IceProcessMessages(client->ice_conn, NULL, &ret) ==
            IceProcessMessagesIOError)
        SmcCloseConnection(client->conn, 0, NULL);
    return;
}


static void 
ice_watch_fn(IceConn conn, IcePointer client_data,
             Bool opening, IcePointer *watch_data)
{
    BSSessionClient *client = (BSSessionClient *)client_data;
    
    if (opening) {
        client->fd = IceConnectionNumber(conn);
        fcntl(client->fd, F_SETFD, FD_CLOEXEC);
        client->input_tag = gdk_input_add(client->fd, GDK_INPUT_READ,
                                          &poll_ice_messages, client);
    } else if (IceConnectionNumber(conn) == client->fd) {
        gdk_input_remove(client->input_tag);
        bs_session_client_free(client);
    }
}


static void 
save_yourself(SmcConn conn, SmPointer client_data, int save_type,
                 Bool shutdown, int interact_style, Bool fast)
{
    BSSessionClient *client = (BSSessionClient *)client_data;
    gboolean success = TRUE;
    if (client->save_yourself_fn)
        success = client->save_yourself_fn(client);
    if (success)
        bs_session_client_register_properties(client);
    SmcSaveYourselfDone(client->conn, success);
}


static void 
shutdown_cancelled(SmcConn conn, SmPointer client_data)
{
    BSSessionClient *client = (BSSessionClient *)client_data;
    if (client->shutdown_cancelled_fn)
        client->shutdown_cancelled_fn(client);
}


static void 
save_complete(SmcConn conn, SmPointer client_data)
{
    BSSessionClient *client = (BSSessionClient *)client_data;
    if (client->save_complete_fn)
        client->save_complete_fn(client);
}


static void die
(SmcConn conn, SmPointer client_data)
{
    BSSessionClient *client = (BSSessionClient *)client_data;
    if (client->die_fn)
        client->die_fn(client);
}


BSSessionClient *
bs_session_client_new(const gchar *client_id)
{
    BSSessionClient *client;
    
    client = g_new(BSSessionClient, 1);
    client->id = g_strdup(client_id);
    client->props = NULL;
    client->conn = NULL;
    client->ice_conn = NULL;
    client->fd = -1;
    
    return client;
}


void 
bs_session_client_free(BSSessionClient *client)
{
    GSList *list = client->props;
    SessionProperty *prop;
    
    for (; list; list = list->next) {
        prop = (SessionProperty *)list->data;
        g_free(prop->prop.vals->value);
        g_free(prop->prop.vals);
        g_free(prop->prop.name);
        g_free(prop->prop.type);
        g_free(prop);
    }
    g_slist_free(client->props);
    g_free(client->id);
    g_free(client);
}


void 
bs_session_client_set_list_of_array_prop(BSSessionClient *client, 
                                         const gchar *name, const char *vals[],
                                         gint nvals)
{
    SessionProperty *prop = find_property(client, name);
    SmPropValue *value = make_list_of_array_value(vals, nvals);
        
    if (prop == NULL)
        prop = new_property(client, name, SmLISTofARRAY8);
    
    else
        g_free(prop->prop.vals);
    
    prop->prop.vals = value;
    prop->prop.num_vals = nvals;
    prop->set = TRUE;
}


void 
bs_session_client_set_array_prop(BSSessionClient *client, const gchar *name,
                                 const gchar *vals)
{
    SessionProperty *prop = find_property(client, name);
    SmPropValue *value = make_array_value(vals);
        
    if (prop == NULL)
        prop = new_property(client, name, SmARRAY8);
    
    else
        g_free(prop->prop.vals);
    
    prop->prop.vals = value;
    prop->prop.num_vals = 1;
    prop->set = TRUE;
}


void 
bs_session_client_set_card_prop(BSSessionClient *client, const gchar *name, 
                                const gchar val)
{
    SessionProperty *prop = find_property(client, name);
    SmPropValue *value = make_card_value(val);
        
    if (prop == NULL)
        prop = new_property(client, name, SmCARD8);
    
    else
        g_free(prop->prop.vals);
    
    prop->prop.vals = value;
    prop->prop.num_vals = 1;
    prop->set = TRUE;
}


void 
bs_session_client_get_prop_value(BSSessionClient *client, const gchar *name,
                                 SmPropValue **val_ret, gint *nvals_ret)
{
    SessionProperty *prop = find_property(client, name);
    
    if (!prop) {
        *val_ret = NULL;
        *nvals_ret = 0;
    } else {
        *val_ret = prop->prop.vals;
        *nvals_ret = prop->prop.num_vals;
    }
}

    
void 
bs_session_client_register_properties(BSSessionClient *client)
{
    GPtrArray *set_props= g_ptr_array_new();
    GSList *list = client->props;
    SessionProperty *prop;
    
    for (; list; list = list->next) {
        prop = (SessionProperty *)list->data;
        if (prop->set == TRUE) {
            g_ptr_array_add(set_props, &prop->prop);
            prop->set = FALSE;
        }
    }
    if (set_props->len > 0)
        SmcSetProperties(client->conn, set_props->len, 
                         (SmProp **)set_props->pdata);
    
    g_ptr_array_free(set_props, TRUE);
}


gboolean 
bs_session_client_connect(BSSessionClient *client)
{
    gchar *client_id_ret = NULL;
    SmcConn conn = NULL;
    SmcCallbacks callbacks;

    callbacks.save_yourself.callback = save_yourself;
    callbacks.save_yourself.client_data = (SmPointer)client;
    callbacks.die.callback = die;
    callbacks.die.client_data = (SmPointer)client;
    callbacks.save_complete.callback = save_complete;
    callbacks.save_complete.client_data = (SmPointer)client;
    callbacks.shutdown_cancelled.callback = shutdown_cancelled;
    callbacks.shutdown_cancelled.client_data = (SmPointer)client;
    
    if (IceAddConnectionWatch(&ice_watch_fn, client) == 0) {
        g_printerr("Session Manager: IceAddConnectionWatch failed\n");
        return FALSE;
    }

    if ((conn = SmcOpenConnection(NULL, NULL, 1, 0, 
                                  SmcSaveYourselfProcMask |
                                  SmcSaveCompleteProcMask |
                                  SmcShutdownCancelledProcMask |
                                  SmcDieProcMask,
                                  &callbacks, client->id, &client_id_ret,
                                  0, NULL)) == NULL) {
        g_warning("Session Manager: Init error\n");
        return FALSE;
    }
        
    client->id = g_strdup(client_id_ret);
    client->conn = conn;
    client->ice_conn = SmcGetIceConnection(conn);
    gdk_set_sm_client_id(client->id);
    XFree(client_id_ret);
    
    gtk_quit_add(0, &close_connection, client);
    
    return TRUE;
}
