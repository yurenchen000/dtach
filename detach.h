/*
    dtach - A simple program that emulates the detach feature of screen.
    Copyright (C) 2001 Ned T. Crigler

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/
#ifndef detach_h
#define detach_h

#include "config.h"

#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef HAVE_PTY_H
#include <pty.h>
#endif

#ifdef HAVE_UTIL_H
#include <util.h>
#endif

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#ifdef HAVE_SYS_IOCTL_H
#include <sys/ioctl.h>
#endif

#ifdef HAVE_SYS_RESOURCE_H
#include <sys/resource.h>
#endif

#include <poll.h>
#include <termios.h>

#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/un.h>

extern char *progname, *sockname;
extern int detach_char, no_suspend;
extern struct termios orig_term;

enum
{
	MSG_PUSH,
	MSG_WINCH,
	MSG_REDRAW,
	MSG_ATTACH,
	MSG_DETACH
};

// The client to master protocol.
struct packet
{
	unsigned char type;
	unsigned char len;
	union
	{
		unsigned char buf[sizeof(struct winsize)];
		struct winsize ws;
	} u;
};

// The master sends a simple stream of text to the attaching clients, without
// any protocol. This might change back to the packet based protocol in the
// future. In the meantime, however, we minimize the amount of data sent back
// and forth between the client and the master. BUFSIZE is the size of the
// buffer used for the text stream.
#define BUFSIZE 4096

int attach_main(int noerror);
int master_main(char **argv);
#endif