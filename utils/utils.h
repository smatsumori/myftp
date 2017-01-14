#ifndef __HEADER_UTILS__
#define __HEADER_UTILS__

/*** DEFINES ***/
#include <assert.h>
#include <sys/select.h>
#include <unistd.h>
#include <sys/time.h>
#include <signal.h>
#include <strings.h>
#include <string.h>

/*** UTILS ***/
void report_error_and_exit(int errno, const char *msg)
{
	fprintf(stderr, "Runtime error\n");
	perror(msg);
	exit(errno);
}

#endif
