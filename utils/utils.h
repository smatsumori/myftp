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
#include <netinet/in.h>
#include <errno.h>

/*** DEFINES ***/
#define MAX_NAME 25
#define MAX_DESCRIPTION 100
#define CMD_LENGTH 20
#define MAX_CMD 10
#define DIR_LEN 100
#define FTP_SERV_PORT 50020
#define FTP_SERV_ADDR "131.113.108.53"

/*** ERRORS ***/
enum error_Flags {
	ERR_MALLOC, ERR_SOCKET, ERROR_EVENT, ERR_PROCESSING, ERR_SENDTO,
	ERR_FORK, ERR_WAIT, ERR_RECV, ERR_SEND
};

/*** FSM ***/
struct eventtable {
	int id;
	char name[MAX_NAME];
	char description[MAX_DESCRIPTION];
};

/*** UTILS ***/
void report_error_and_exit(int erno, const char *msg)
{
	fprintf(stderr, "Runtime error: %d\n", erno);
	perror(msg);
	exit(erno);
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

int 
chdirw(char *dir) {
	char pwd[DIR_LEN];
	int stat = chdir(dir);
	char *newPWD = getcwd(pwd, sizeof(pwd));
	setenv("PWD", newPWD, 1);
	return stat;
}
#endif 
