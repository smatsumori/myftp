#ifndef __HEADER_MYFTPC__
#define __HEADER_MYFTPC__

/*** INCLUDES ***/
#include "../utils/utils.h"
#include "../utils/packet.h"
#include <time.h>

#define CMD_LENGTH 20
#define ERR_EXECVP 10		// TODO: move below lines to utils.h

/*** PROTOTYPES ***/
struct myftpchead;
struct typetable;
struct proctable;
int setcmd(struct myftpchead *hpr, char cmd[CMD_LENGTH]);
int chdirw(char *dir);
void send_quit(struct myftpchead *hpr);
void send_pwd(struct myftpchead *hpr);
void send_dir(struct myftpchead *hpr);
void tcpc_send(struct myftpchead *hpr);
void tcpc_send_data(struct myftpchead *hpr);
void tcpc_recv(struct myftpchead *hpr);
void send_data(struct myftpchead *);
void recv_data(struct myftpchead *);
void send_retr(struct myftpchead *);
int packet_checker(struct myftpchead *);

struct myftpchead {
	int mysockd;		/* socket descriptor for this client */
	int argc;
	uint8_t type;
	uint8_t code;
	const char *hostname;
	char argv[CMD_LENGTH][MAX_CMD];
	char pwd[DIR_LEN];
	char *data_to_send;		
	FILE * pFile;
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
		for (struct typetable *ptr = ttab; ; ptr++) {
			if (strcmp(ptr->cmd, "__SENTINEL__") == 0) {		// if client command
				return 1;
			} else if (strcmp(hpr->argv[0], ptr->cmd) == 0) {		// if ftp command
				hpr->type = ptr->type;		// set type
				return 0;		// ftp command
			}
		}
	}
	return -1;
}


void
exec_cmd(struct myftpchead *hpr)
{	
	/* execute client command */
	/* no need to send msg in this cmd */
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


void
send_quit(struct myftpchead *hpr)
{
	static struct myftp_packh pkh = {
		.type = FTP_QUIT, .code = CODE_OK,
		.length = 0
	};
	hpr->packet_to_send = pkh;
	tcpc_send(hpr);
	return;
}

void
send_pwd(struct myftpchead *hpr)
{
	static struct myftp_packh pkh = {
		.type = FTP_PWD, .code = CODE_OK,
		.length = 0
	};
	if (1 < hpr->argc) {		// options
		hpr->data_to_send = hpr->argv[1];
	} else {
		hpr->data_to_send = NULL;
	}
	hpr->packet_to_send = pkh;
	tcpc_send(hpr);
	return;
}

void
send_dir(struct myftpchead *hpr)
{
	static struct myftp_packh pkh = {
		.type = FTP_LIST, .code = CODE_OK,
		.length = 0
	};
	if (1 < hpr->argc) {		// options
		hpr->data_to_send = hpr->argv[1];
	} else {
		hpr->data_to_send = NULL;
	}
	
	hpr->packet_to_send = pkh;
	tcpc_send(hpr);
	return;
}

void
send_cwd(struct myftpchead *hpr)
{
	static struct myftp_packh pkh = {
		.type = FTP_CWD, .code = CODE_OK,
		.length = 0
	};
	if (1 < hpr->argc) {		// options
		hpr->data_to_send = hpr->argv[1];
	} else {
		fprintf(stderr, "Invalid\n");
		hpr->data_to_send = NULL;
	}
	
	hpr->packet_to_send = pkh;
	tcpc_send(hpr);
	return;
}

void
send_stor(struct myftpchead *hpr)
{
	static struct myftp_packh pkh = {
		.type = FTP_STOR, .code = CODE_OK,
		.length = 0
	};

	// TODO: handle read error
	fprintf(stderr, "argv%s\n", hpr->argv[1]);
	hpr->pFile	= fopen(hpr->argv[1], "r");
	strcpy(hpr->argv[1], hpr->argv[2]);
  tcpc_send(hpr);
	return;
}

void
send_retr(struct myftpchead *hpr)
{
	static struct myftp_packh pkh = {
		.type = FTP_RETR, .code = CODE_OK,
		.length = 0
	};
	
	time_t t = time(NULL);
	struct tm tm = *localtime(&t);
	char filename[100];
	FILE *fp;
	if (hpr->argv[2] != NULL) {
		fprintf(stderr, "filename:%s, save dir:%s\n", 
				hpr->argv[1], hpr->argv[2]);
		sprintf(filename, "./%s/newfile-%d-%d-%d-%d-%d-%d", hpr->argv[2],
				tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour,
				tm.tm_min, tm.tm_sec);
	} else {
		fprintf(stderr, "filename:%s, save dir:./\n", 
				hpr->argv[1]);
		sprintf(filename, "./newfile-%d-%d-%d-%d-%d-%d",
				tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour,
				tm.tm_min, tm.tm_sec);
	}
	fp = fopen(filename, "w");
	hpr->pFile = fp;
  tcpc_send(hpr);
	return;
}

void
send_data(struct myftpchead *hpr)
{
	// TODO: implement store first
  char line[1000];
	static char buf[FTP_MAX_RECVSIZE];		// TODO: remove magic
	while (fgets(line, 1000, hpr->pFile) != NULL) {
		strcat(buf, line);
	}
	hpr->data_to_send = buf;
	tcpc_send_data(hpr);
	fclose(hpr->pFile);
	return;
}

void 
recv_data(struct myftpchead *hpr)
{
	// TODO: implement
	
	tcpc_recv(hpr);
	if (hpr->pFile != NULL) {
		fputs(hpr->data_to_send, hpr->pFile);
	} else {
		fprintf(stderr, "ERROR: Cannot open file\n");
	}
	fclose(hpr->pFile);
	fprintf(stderr, "file saved\n");
	hpr->data_to_send = NULL;
	return;
}

int
packet_checker(struct myftpchead *hpr)
{
	int type = hpr->packet_recieved.type;
	int code = hpr->packet_recieved.code;
	print_packeth(&hpr->packet_recieved);

	switch (type) {
		case FTP_QUIT:
		case FTP_PWD:
		case FTP_LIST:
		case FTP_RETR:
		case FTP_CWD:
		case FTP_STOR:
		case 0x20:
			return 0;

		case 0x10:
			switch (code) {
				case 0x00:
				case 0x01:
				case 0x02:
				return 1;
			}

		case 0x11:		// CMd
			switch (code) {
				case 0x01:
					fprintf(stderr, "CMD-ERR: Syntax\n");
					break;
				case 0x02:
					fprintf(stderr, "CMD-ERR: Undefined\n");
					break;
				case 0x03:
					fprintf(stderr, "CMD-ERR: Protocol\n");
					break;
				default:
					return -1;
			}
		
		case 0x12:		// FILE
			switch (code) {
				case 0x00:
					fprintf(stderr, "FILE-ERR: Directory not found.\n");
					break;
				case 0x01:
					fprintf(stderr, "FILE-ERR: Permission denied\n");
					break;
				default:
					return -2;
			}

		case 0x13:	// Undefined
			switch (code) {
				case 0x05:
					fprintf(stderr, "UNKWN-ERR: Undefined\n");
				default:
					return -3;
			}

		default:
			return -4;
	}
	return -1;
}
#endif	// __HEADER_MYFTPC__
