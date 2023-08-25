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
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <gtk/gtk.h>
#include <X11/SM/SMlib.h>
#include "session.h"
#include "sessionclient.h"


static void 
save_state(BSSessionClient *client)
{
    SmPropValue *program;
    gint nvals;
    GPtrArray *restart_cmd = g_ptr_array_new();
    
    bs_session_client_get_prop_value(client, SmProgram, &program, &nvals);
    g_ptr_array_add(restart_cmd, program->value);
    g_ptr_array_add(restart_cmd, "-c");
    g_ptr_array_add(restart_cmd, client->id);
    
    bs_session_client_set_list_of_array_prop(client, SmRestartCommand,
                                             (const gchar **)restart_cmd->pdata,
                                             restart_cmd->len);

    g_ptr_array_free(restart_cmd, TRUE);
}


static gboolean 
save_yourself(BSSessionClient *client)
{
    save_state(client);
    return TRUE;
}


static void 
die(BSSessionClient *client)
{
    gtk_main_quit();
}


void
bs_session_init(const gchar *prog, const gchar *client_id)
{
    BSSessionClient *client;
    gchar *bin_path;

    if (getenv("SESSION_MANAGER") == NULL)
        return;

    client = bs_session_client_new(client_id);
    
    if (!bs_session_client_connect(client)) {
        bs_session_client_free(client);
        return;
    }
    
    bs_session_client_set_array_prop(client, SmProgram, prog);
    bs_session_client_set_card_prop(client, SmRestartStyleHint, 
                                    SmRestartIfRunning);
    
    client->save_yourself_fn = save_yourself;
    client->shutdown_cancelled_fn = NULL;
    client->save_complete_fn = NULL;
    client->die_fn = die;
}
