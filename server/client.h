#ifndef __FTP_CLIENT_HEADER__
#define __FTP_CLIENT_HEADER__

#include <stdio.h>
#include <time.h>

#define CMD_BUFLEN 256

/*** PROTOTYPE ***/
int client_handler(struct myftpdhead *hpr);
int client_recv(int sockd, char *msg[CMD_BUFLEN]);
int client_send_cwd(int sockd, char *msg);
int client_send_pwd(int sockd, char *msg);
int client_send_list(int sockd, char *msg);
int client_recv_data(int sockd, char *msg);
int client_recv_store(int sockd, char *);
int client_send_retr(int, char *);
int client_send_retr_ack(int, char *);
/*** FUNCTION ***/
int
client_handler(struct myftpdhead *hpr)
{
	int clisockd = hpr->clisockd;
	char *msg;
	fprintf(stderr, "Client forked.\n");
	while (1) {
		switch (client_recv(clisockd, &msg)) {
			case FTP_CWD:
				client_send_cwd(clisockd, msg);
				break;
			case FTP_PWD:
				client_send_pwd(clisockd, msg);
				break;
			case FTP_LIST:
				client_send_list(clisockd, msg);
				break;
			case FTP_STOR:
				if (client_send_store_ack(clisockd, msg) < 0) {
					fprintf(stderr, "Error store cmd\n");
					break;
				}
				/* if valid recv data */
				client_recv_store(clisockd, msg);
				break;
			
			case FTP_RETR:
				if (client_send_retr_ack(clisockd, msg) < 0) {
					fprintf(stderr, "Error retr cmd\n");
					break;
				}
				/* if valid (file exists) */
				client_send_retr(clisockd, msg);
				break;

			case FTP_QUIT:
				close(clisockd);
				return 0;
		}
	}
	return 0;
}

int 
client_recv(int sockd, char *msg[CMD_BUFLEN])		// has side effects on msg
{
	*msg = NULL;
	int size;
  char obuf[FTP_DATASIZE] = "";
	struct myftp_packh myftphp;

	while (1) {
		/* recv ftp packet header */
		if ((size = recv(sockd, &myftphp, sizeof(myftphp), 0)) < 0)
			report_error_and_exit(ERR_RECV, "client_recv");
		fprintf(stderr, "Packet Recived: %d, ", size);
		print_packeth(&myftphp);

		/* recv cmd data */
		if (0 < myftphp.length) {
			if ((size = recv(sockd, &obuf, FTP_DATASIZE, 0)) < 0)
				report_error_and_exit(ERR_RECV, "client_recv");
			fprintf(stderr, "arg: %s\n", obuf);
			*msg = obuf;
			break;
		} else {
			*msg = NULL;
			return myftphp.type;
		}
	}
	return myftphp.type;
}

int
client_recv_data(int sockd, char *msg)
{
	/* data structure */
	/* [1] PACKET HEAD (Data follows) */
	/* [2] DATA */
	/* [3] PACKET HEAD (Data follows) */
	/* [4] DATA */
	/*	... */
	/* [n-1] DATA */
	/* [n] PACKET HEAD (Data end) */

	int size;
	char buf[FTP_MAX_RECVSIZE];
	char obuf[FTP_DATASIZE];
	struct myftp_packh myftphp;
	while (1) {
		/* recv data packet */
		if ((size = recv(sockd, &myftphp, sizeof(myftphp), 0)) < 0)
			report_error_and_exit(ERR_RECV, "client_recv");

		fprintf(stderr, "Packet Recived: %d, ", size);
		print_packeth(&myftphp);

		if (myftphp.type == 0x20) {
			if (myftphp.code == 0x00) {
				/* recv EOF */
				fprintf(stderr, "EOF\n");
				strcpy(msg, buf);
				return 1;
			} else if (myftphp.code == 0x01) {
				/* recv ftp data */
				if ((size = recv(sockd, &obuf, FTP_DATASIZE, 0)) < 0)
					report_error_and_exit(ERR_RECV, "client_recv");
				strcat(buf, obuf);
				continue;
			}
		} else {
			return -1;
		}
	}
	return -1;	/* error */
}

int
client_send_data(int sockd, char *msg)
{	
	/* this is only for RETR */
	struct myftp_packh myftphp = {
		.type = 0x20, .code = 0x01, .length = sizeof(char) * strlen(msg)
	};
	int dataleft = sizeof(char) * strlen(msg);
	fprintf(stderr, "Sending Data Packet: [type: 0x%x, code: 0x%x, size: %d]\n", 
			myftphp.type, myftphp.code, myftphp.length);
	fprintf(stderr, "DATA [%s]\n", msg);
	while (1) {
		print_packeth(&myftphp);
		if (send(sockd, &myftphp, sizeof(myftphp), 0) < 0)
			report_error_and_exit(ERR_SEND, "client_send_data");
		if (send(sockd, msg, FTP_DATASIZE, 0) < 0)
			report_error_and_exit(ERR_SEND, "client_send_data");
		if (0 < (dataleft -= 1024)) {
			fprintf(stderr, "Data remains: %d\n", dataleft);
		} else {
			break;
		}
	}
	/* send last packet */
	myftphp.code = 0x00;
	if (send(sockd, &myftphp, FTP_DATASIZE, 0) < 0)
		report_error_and_exit(ERR_SEND, "client_send_data");
	return 0;
}

int
client_send_code(int sockd, int type, int code, char *codedata)
{
	/* send code and code data */
	struct myftp_packh myftphp = {
		.type =  type, .code = code, .length = 0
	};

	if (codedata != NULL) {
		/* code data follows */
		myftphp.length = strlen(codedata);
	}
	fprintf(stderr, "Sending CMD packet: ");
	print_packeth(&myftphp);

	if (send(sockd, &myftphp, sizeof(myftphp), 0) < 0) 
		report_error_and_exit(ERR_SEND, "client_send_code");

	if (codedata != NULL) {
		fprintf(stderr, "Sending CMD data: [%s]\n", codedata);
		if (send(sockd, codedata, strlen(codedata), 0) < 0) 
			report_error_and_exit(ERR_SEND, "client_send_code");
	}

	return 0;
}

int
client_send_pwd(int sockd, char *msg)
{
	char cbuf[256];
	FILE *fp = popen("pwd", "r");

	char buf[CMD_BUFLEN];
	while (fgets(buf, sizeof(buf), fp) != 0) {
	}
	int length = strlen(buf) * sizeof(char);
	buf[length - 1] = '\0';
	fprintf(stderr, "buf %s\n", buf);
	client_send_code(sockd, 0x10, 0x00, buf);	// HARDCODED
	pclose(fp);
	return 0;
}

int
client_send_list(int sockd, char *msg)
{
	char cmd[30];
	if (msg != NULL) {
		sprintf(cmd, "ls ./%s", msg);
		DIR *dirp = opendir(msg);
		if (dirp == NULL) {
			switch (errno) {
				case EACCES:
					fprintf(stderr, "Permission denied\n");
					client_send_code(sockd, 0x12, 0x01, NULL);
					break;
				case ENOENT:
					fprintf(stderr, "No such file or directory\n");
					client_send_code(sockd, 0x12, 0x00, NULL);
					break;
				default:
					fprintf(stderr, "Undefined Error\n");
					client_send_code(sockd, 0x13, 0x05, NULL);
					break;
			}
			return -1;
		}
	} else {
		sprintf(cmd, "ls", msg);
	}
	fprintf(stderr, "executing: %s\n", cmd);
	FILE *fp = popen(cmd, "r");


	char obuf[1000];
	char buf[CMD_BUFLEN] = "";	
	while (fgets(obuf, sizeof(obuf), fp) != NULL) {
		strcat(buf, obuf);
	}
	int length = strlen(buf) * sizeof(char);
	buf[length - 1] = '\0';
	client_send_code(sockd, 0x10, 0x01, buf);	// HARDCODED
	pclose(fp);
	return 0;
}

int
client_send_cwd(int sockd, char *msg)
{
	if (chdirw(msg) == -1) {
		// TODO: handle error
		fprintf(stderr, "No such directory: %s ERRNO:%d\n", msg, errno);
		client_send_code(sockd, 0x12, 0x00, NULL);
	} else {
		fprintf(stderr, "CD success\n");
		client_send_code(sockd, 0x10, 0x00, NULL);
	}
	return 0;
}

int
client_send_store_ack(int sockd, char *msg)
{
	/* returns -1 if not valid */
	/* TODO: this does not check if 
	 * the current directory has a write 
	 * permission 
	 */
	if (msg != NULL) {
		/* check directory exists */
		DIR *dirp = opendir(msg);
		if (dirp) {
			fprintf(stderr, "Valid directory\n");
			closedir(dirp);
		} else if (ENOENT == errno) {
			fprintf(stderr, "Directory does not exist. [%s]\n", *msg);
			client_send_code(sockd, 0x12, 0x00, NULL);
			return -1;
		} else if (EACCES == errno) {
			fprintf(stderr, "Permission denied\n");
			client_send_code(sockd, 0x12, 0x01, NULL);
			return -2;
		} else {
			fprintf(stderr, "Undefined error\n");
			client_send_code(sockd, 0x13, 0x05, NULL);
			return -3;
		}
	}
	client_send_code(sockd, 0x10, 0x02, NULL);
	return 0;
}

int
client_recv_store(int sockd, char *dir)
{
	char recvfile[FTP_MAX_RECVSIZE];
	client_recv_data(sockd, recvfile);
	fprintf(stderr, "---------- DATA ------------\n");
	fprintf(stderr, "%s", recvfile);
	fprintf(stderr, "--------------------------\n");
	time_t t = time(NULL);
	struct tm tm = *localtime(&t);
	FILE *pFile;
	char filename[100];
	/* TODO: write to directory */
	if (dir == NULL) {
		sprintf(filename, "newfile-%d-%d-%d-%d-%d-%d", 
				tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour,
				tm.tm_min, tm.tm_sec);
	} else {
		sprintf(filename, "./%s/newfile-%d-%d-%d-%d-%d-%d", dir,
				tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour,
				tm.tm_min, tm.tm_sec);
	}
	pFile = fopen(filename, "w");
	if (pFile != NULL) {
		fputs (recvfile, pFile);
		fclose(pFile);
	}
	fprintf(stderr, "file saved: %s\n", filename);
	return 0;
}


int
client_send_retr_ack(int sockd, char *filename)
{
	/* returns -1 if not valid */
	/* TODO: this does not check if 
	 * the current directory has a write 
	 * permission 
	 */
	FILE *pFile = fopen(filename, "r");
	if (pFile) {
		fprintf(stderr, "Valid file name: %s\n", filename);
		fclose(pFile);
	} else if (ENOENT == errno) {
		fprintf(stderr, "File does not exist. [%s]\n", filename);
		client_send_code(sockd, 0x12, 0x00, NULL);
		return -1;
	} else if (EACCES == errno) {
		fprintf(stderr, "Permission denied\n");
		client_send_code(sockd, 0x12, 0x01, NULL);
		return -2;
	} else {
		fprintf(stderr, "Undefined error\n");
		client_send_code(sockd, 0x13, 0x05, NULL);
		return -3;
	}
	client_send_code(sockd, 0x10, 0x01, NULL);
	return 0;
}

int
client_send_retr(int sockd, char *filename)
{
	fprintf(stderr, "Sending RETR data\n");
	char line[1000];
	static char buf[6000];
	FILE *pFile = fopen(filename, "r");

	while (fgets(line, 6000, pFile) != NULL) {
		strcat(buf, line);
	}
	client_send_data(sockd, buf);
	fclose(pFile);
	return 0;
}
#endif	// End of __FTP_CLIENT_HEADER__
