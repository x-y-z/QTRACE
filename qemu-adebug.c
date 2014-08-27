/*
 *  QTRACE asynchronous debug tool 
 *
 *  Copyright (c) 2003-2005 Fabrice Bellard
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, see <http://www.gnu.org/licenses/>.
 */

#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include "qemu-adebug.h"

/// --------------------------------------------
/* connect/disconnect the shared memory */
/// --------------------------------------------
static inline DebugChannel* connect(void) 
{
	int shared_id = shmget(SHARED_MEM_KEY, 
         	               sizeof(DebugChannel), 
                               S_IRUSR | S_IWUSR);

  	return (DebugChannel*)shmat(shared_id, NULL, 0);
}

static inline void disconnect(DebugChannel *channel) { shmdt(channel); }

/// --------------------------------------------
/* handle commands */ 
/// --------------------------------------------
static const char* flushcc 		= 	"--flushcc";
static const char* client_reset 	= 	"--client-reset";
static const char* client_print 	= 	"--client-print";

#define QTRACE_HANDLE_CONNECTION(cmd)          				\
static inline void handle_connection_##cmd(DebugChannel *channel)   	\
{ 									\
   	channel->_##cmd##_ = 1;   					\
   	QTRACE_WAIT_COMMAND_HANDLED(channel->_##cmd##_);		\
	printf("Command Successful\n");					\
} 

QTRACE_HANDLE_CONNECTION(flushcc);
QTRACE_HANDLE_CONNECTION(client_reset);
QTRACE_HANDLE_CONNECTION(client_print);
#undef QTRACE_HANDLE_CONNECTION


static inline void handle_connection_success(int argc, char** argv, DebugChannel *channel) 
{
  	int i;
  	for (i=0;i<argc;++i)
  	{
     		if (!strcmp(argv[i], flushcc)) handle_connection_flushcc(channel);
     		if (!strcmp(argv[i], client_reset)) handle_connection_client_reset(channel);
     		if (!strcmp(argv[i], client_print)) handle_connection_client_print(channel);
  	}

	return;
}

static void handle_connection_error(void) 
{
	QTRACE_ERROR("unable to connect to shm %d\n", SHARED_MEM_KEY);
  	QTRACE_EXIT(-1);
}

int main(int argc, char **argv)
{
   	/* connect to the shared memory */
   	DebugChannel *channel=connect();

   	if (!channel) handle_connection_error();
   	else handle_connection_success(argc, argv, channel); 

   	/* disconnect */
   	disconnect(channel);

	/* done */
   	return 0;
}

