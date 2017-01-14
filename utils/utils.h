#ifndef __HEADER_UTILS__
#define __HEADER_UTILS__

/*** INCLUDES ***/
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <sys/select.h>
#include <unistd.h>
#include <sys/time.h>
#include <signal.h>
#include <strings.h>
#include <string.h>

/*** DEFINES ***/
#define MAX_NAME 25
#define MAX_DESCRIPTION 100

/*** ERRORS ***/
enum error_Flags {
	ERR_MALLOC, ERR_SOCKET, ERROR_EVENT, ERR_PROCESSING, ERR_SENDTO
};

/*** FSM ***/
struct eventtable {
	int id;
	char name[MAX_NAME];
	char description[MAX_DESCRIPTION];
};

/*** UTILS ***/
void report_error_and_exit(int errno, const char *msg)
{
	fprintf(stderr, "Runtime error\n");
	perror(msg);
	exit(errno);
}

void print_event(int id, struct eventtable *etabp)
{
	struct eventtable *evptr;
	for (evptr = etabp; evptr->id != 0; evptr++) {
		if (evptr->id == id) {
			fprintf(stderr, "## Event:%2d :: %2s :: %s ##\n",
				 	evptr->id, evptr->name, evptr->description);
			return;
		}
	}
	fprintf(stderr, "error: print_event\n");
	return;
}

void print_status(int id, struct eventtable *stabp)
{
	struct eventtable *stptr;
	for (stptr = stabp; stptr->id != 0; stptr++) {
		if (stptr->id == id) {
			fprintf(stderr, "--------- status %2d: %s ---------\n", stptr->id, stptr->name);
			return;
		}
	}
	fprintf(stderr, "error: print_status\n");
	return;
}

#endif 
