#ifndef __HEADER_MYFTPC__
#define __HEADER_MYFTPC__

/*** INCLUDES ***/
#include "../utils/utils.h"
#include "../utils/packet.h"
#include <netinet/in.h>
#include <string.h>

#define MAX_CMD 2
#define CMD_LENGTH 20

/*** PROTOTYPES ***/
struct myftpchead;
int setcmd(struct myftpchead *hpr, char cmd[CMD_LENGTH]);

struct myftpchead {
	int mysockd;		/* socket descriptor for this client */
	int argc;
	int cmd;
	char argv[CMD_LENGTH][MAX_CMD];
	struct sockaddr_in servsockaddr;
	struct myftp_packh packet_to_send;
	struct myftp_packh packet_recieved;
};

struct proctable {
	int status;
	int event;	/* input */
	void (*func)(struct myftpchead *hpr);
	int nextstatus;
};

/*** FUNCTIONS ***/
int 
setcmd(struct myftpchead *hpr, char cmd[CMD_LENGTH])
{
	hpr->argc = 0;
	/* returns 0 if the command is valid, othewise -1 */
	const char *delim = " \t\n";
	char *ptr;
	if ((ptr = strtok(cmd, delim)) == NULL) {
		fprintf(stderr, "No input\n");
		return -1;
	} else {
	  /* TODO: extend MAX_CMD if you want to get more than 2 cmds */
		(hpr->argc)++;	// argc = 1
		strcpy(hpr->argv[(hpr->argc) - 1], ptr);
		fprintf(stderr, "CMD: %s\n", ptr);
		while ((ptr = strtok(NULL, delim)) != NULL) {
			fprintf(stderr, "CMD: %s\n", ptr);
			(hpr->argc)++;
			strcpy(hpr->argv[(hpr->argc) - 1], ptr);
		}
		return 0;
	}
	return -1;
}
#endif	// __HEADER_MYFTPC__
