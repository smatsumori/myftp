#ifndef __HEADER_MYFTPC__
#define __HEADER_MYFTPC__

/*** INCLUDES ***/
#include "../utils/utils.h"
#include "../utils/packet.h"
#include <netinet/in.h>
#include <string.h>

#define CMD_LENGTH 20

/*** PROTOTYPES ***/
struct myftpchead;
struct typetable;
struct proctable;
int setcmd(struct myftpchead *hpr, char cmd[CMD_LENGTH]);

struct myftpchead {
	int mysockd;		/* socket descriptor for this client */
	int argc;
	uint8_t type;
	uint8_t code;
	char argv[CMD_LENGTH][MAX_CMD];
	char **data_to_send;		
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

struct typetable {
	char cmd[CMD_LENGTH];
	uint8_t type;
} static ttab[] = {
	{"quit", FTP_QUIT},
	{"pwd", FTP_PWD},
	{"cd", FTP_CWD},
	{"dir", FTP_LIST},
	{"get", FTP_RETR},
	{"put", FTP_STOR},
	{"__SENTINEL__", 0}
};

/*** FUNCTIONS ***/
int 
setcmd(struct myftpchead *hpr, char cmd[CMD_LENGTH])
{
	/* TODO: set a command to myftpchead cmd */
	hpr->argc = 0;
	/* returns 0, 1 if the command is valid, othewise -1 */
	/* 0 is ftpcmd, 1 is client cmd */
	const char *delim = " \t\n";
	char *ptr;
	if ((ptr = strtok(cmd, delim)) == NULL) {
		fprintf(stderr, "No input\n");
		return -1;
	} else {
	  /* TODO: extend MAX_CMD if you want to get more than 2 cmds */
		(hpr->argc)++;	// argc = 1
		strcpy(hpr->argv[(hpr->argc) - 1], ptr);
		fprintf(stderr, "CMD: %s", ptr);
		while ((ptr = strtok(NULL, delim)) != NULL) {
			fprintf(stderr, " %s", ptr);
			(hpr->argc)++;
			strcpy(hpr->argv[(hpr->argc) - 1], ptr);
		}
		fprintf(stderr, "\n");
		
		/* set a ftp command */
		for (struct typetable *ptr = &ttab; ; ptr++) {
			if (strcmp(ptr->cmd, "__SENTINEL__") == 0) {
				return 1;		// client command
			} else if (strcmp(hpr->argv[0], ptr->cmd) == 0) {
				hpr->type = ptr->type;
				return 0;		// ftp command
			}
		}
	}
	return -1;
}

void
exec_cmd(struct myftpchead *hpr)
{	
	// TODO: implement
	fprintf(stderr, "Execute here\n");
	return;
}
#endif	// __HEADER_MYFTPC__
