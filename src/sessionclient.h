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
#ifndef BS_SESSION_CLIENT_H
#define BS_SESSION_CLIENT_H

#include <glib.h>
#include <X11/SM/SMlib.h>


typedef struct _BSSessionClient BSSessionClient;

struct _BSSessionClient {
	gchar *id;
	GSList *props;		
	gboolean (*save_yourself_fn)(BSSessionClient *client);
	void (*shutdown_cancelled_fn)(BSSessionClient *client);
	void (*save_complete_fn)(BSSessionClient *client);
	void (*die_fn)(BSSessionClient *client);
	SmcConn conn;
	IceConn ice_conn;
	gint fd;
	gint input_tag;
};


BSSessionClient *bs_session_client_new(const gchar *client_id);
void bs_session_client_free(BSSessionClient *client);
gboolean bs_session_client_connect(BSSessionClient *client);
void bs_session_client_get_prop_value(BSSessionClient *client, 
                                      const gchar *name,
                                      SmPropValue **val_ret, 
                                      gint *nvals_ret);
void bs_sessin_client_set_list_of_array_prop(BSSessionClient *client, 
                                             const gchar *name,
                                             const gchar *vals[],
                                             gint nvals);
void bs_session_client_set_array_prop(BSSessionClient *client, 
                                      const gchar *name, 
                                      const gchar *vals);
void bs_session_client_set_card_prop(BSSessionClient *client, 
                                     const gchar *name, gchar val);
void bs_session_client_register_properties(BSSessionClient *client);

#endif /* BS_SESSION_CLIENT_H */
