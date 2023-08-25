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
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <stdio.h>
#include <errno.h>
#include <glib.h>
#include "daemon.h"

static void do_fork();
void signal_handler(int sig);

void
bs_daemon_daemonize()
{
    pid_t pid;
    gint i;

    do_fork();
    setsid();
    do_fork();

    if (chdir("/") == -1) {
        g_critical("Could not chdir: %s", strerror(errno));
        return;
    }

    umask(0);
    close(0);
    close(1);
    close(2);

    i = open("/dev/null", O_RDWR);
    dup(i);
    dup(i);

    signal(SIGHUP, signal_handler);
    signal(SIGTERM, signal_handler);
    signal(SIGCHLD, signal_handler);
}

static void
do_fork()
{
    switch (fork()) {
        case -1:
            g_critical("Could not fork: %s", strerror(errno));
            break;
        case 0:
            break;
        default:
            exit(EXIT_SUCCESS);
    }
}


void 
signal_handler(int sig)
{
    switch (sig) {
        case SIGTERM:
            exit(EXIT_SUCCESS);
            break;
    }
}
