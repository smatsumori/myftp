#ifndef __HEADER_MYFTPC__
#define __HEADER_MYFTPC__

/*** INCLUDES ***/
#include "../utils/utils.h"
#include "../utils/packet.h"
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#define CMD_LENGTH 20
#define DIR_LEN 100

#define ERR_EXECVP 10		// TODO: move below lines to utils.h

/*** PROTOTYPES ***/
struct myftpchead;
struct typetable;
struct proctable;
int setcmd(struct myftpchead *hpr, char cmd[CMD_LENGTH]);
int chdirw(char *dir);

struct myftpchead {
	int mysockd;		/* socket descriptor for this client */
	int argc;
	uint8_t type;
	uint8_t code;
	char argv[CMD_LENGTH][MAX_CMD];
	char pwd[DIR_LEN];
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
				// TODO: set code here
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
	char *homepath = getenv("HOME");
	char cmd[CMD_LENGTH];
	if (strcmp(hpr->argv[0], "exit") == 0) {
		// TODO: implement
		fprintf(stderr, "exit\n");
		exit(0);
	} else if (strcmp(hpr->argv[0], "lpwd") == 0) {
		strcpy(hpr->pwd, getenv("PWD"));
		printf("%s\n", hpr->pwd);
	} else if (strcmp(hpr->argv[0], "ldir") == 0) {
		if (hpr->argc == 1) {
			system("ls");
		} else {
			// TODO: check if this works
			sprintf(cmd, "ls %s", hpr->argv[1]);
			system(cmd);
		}
	} else if (strcmp(hpr->argv[0], "lcd") == 0) {
		if (hpr->argc == 1) {
			chdirw((char *)homepath);	
		} else if (1 < hpr->argc) {
			if(chdirw(hpr->argv[1]) == -1) {
				fprintf(stderr, "no such directory\n");
				return;
			}
		} else {
			return;
		}
	} else if (strcmp(hpr->argv[0], "help") == 0) {
		/* this code is crappy */
		fprintf(stderr, "[CMD] | [Argument] | [Description]\n");
		fprintf(stderr, "quit | -- | quit myftpc\n");
		fprintf(stderr, "pwd | -- | show current directory in server\n");
		fprintf(stderr, "cd | path | move current directory in server\n");
		fprintf(stderr, "dir | [path] | show files in current directory\n");
		fprintf(stderr, "lpwd | -- | show current directory in client\n");
		fprintf(stderr, "lcd | path | move current directory in client\n");
		fprintf(stderr, "ldir | [path] | show files in current directory\n");
		fprintf(stderr, "get | path1 [path2] | get flie from server\n");
		fprintf(stderr, "put | path1 [path2] | send file to server\n");
		fprintf(stderr, "help | -- | show help\n");
	}
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

#endif	// __HEADER_MYFTPC__
