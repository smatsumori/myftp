#ifndef __FTP_CLIENT_HEADER__
#define __FTP_CLIENT_HEADER__

#include <stdio.h>

/*** PROTOTYPE ***/
int client_handler(struct myftpdhead *hpr);
int client_recv(int sockd, char *msg);
int client_send_cwd(int sockd, char *msg);
int client_send_pwd(int sockd, char *msg);
int client_send_list(int sockd, char *msg);


/*** FUNCTION ***/
int
client_handler(struct myftpdhead *hpr)
{
	int clisockd = hpr->clisockd;
	char msg[256];	// TODO: remove this magic no
	fprintf(stderr, "Client forked.\n");
	while (1) {
		switch (client_recv(clisockd, msg)) {
			case FTP_CWD:
				client_send_cwd(clisockd, msg);
				break;
			case FTP_PWD:
				client_send_pwd(clisockd, msg);
				break;
			case FTP_LIST:
				client_send_list(clisockd, msg);
				break;
			case FTP_QUIT:
				close(clisockd);
				return 0;
		}
	}
	return 0;
}

int 
client_recv(int sockd, char *msg)		// has side effects on msg
{
	msg = NULL;
	int size;
	char buf[FTP_MAX_RECVSIZE];
	char obuf[FTP_DATASIZE];
	struct myftp_packh myftphp;

	while (1) {
		/* recv ftp packet header */
		if ((size = recv(sockd, &myftphp, sizeof(myftphp), 0)) < 0)
			report_error_and_exit(ERR_RECV, "client_recv");
		fprintf(stderr, "Packet Recived: %d, ", size);
		print_packeth(&myftphp);

		/* recv data */
		if (myftphp.type == 0x20) {
			if (myftphp.code == 0x00) {
				fprintf(stderr, "EOF\n");
				return 1;
			} else if (myftphp.code == 0x01) {
				/* recv ftp data */
				if ((size = recv(sockd, &obuf, FTP_DATASIZE, 0)) < 0)
					report_error_and_exit(ERR_RECV, "client_recv");
				fprintf(stderr, "DATA: %s", obuf);
				continue;
			}
		} else if (0 < myftphp.length) {
			if ((size = recv(sockd, &obuf, FTP_DATASIZE, 0)) < 0)
				report_error_and_exit(ERR_RECV, "client_recv");
			fprintf(stderr, "ARG: %s\n", obuf);
			break;
		} else {
			return myftphp.type;
		}
	}
	return myftphp.type;
}

int
client_send_data(int sockd, char *msg)
{	
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
client_send_code(int sockd, int type, int code)
{
	struct myftp_packh myftphp = {
		.type =  type, .code = code, .length = sizeof(struct myftp_packh)
	};
	
	fprintf(stderr, "Sending Command Packet: [type: 0x%d, code: 0x%d, size: %d]\n", 
			myftphp.type, myftphp.code, myftphp.length);
	if (send(sockd, &myftphp, sizeof(myftphp), 0) < 0) 
		report_error_and_exit(ERR_SEND, "client_send_code");
	return 0;
}

int
client_send_pwd(int sockd, char *msg)
{
	char cbuf[256];
	sprintf(cbuf, "pwd %s\0", msg);
	FILE *fp = popen(cbuf, "r");
	char buf[256];		// TODO: remove magic no
	while (fgets(buf, sizeof(buf), fp) != 0) {
	}
	int length = strlen(buf) * sizeof(char);
	buf[length - 1] = '\0';			// remove escape sequence
	client_send_code(sockd, 0x10, 0x00);	// HARDCODED
	client_send_data(sockd, buf);		
	pclose(fp);
	return 0;
}
int
client_send_list(int sockd, char *msg)
{
	FILE *fp = popen("ls", "r");
	char buf[256];		// TODO: remove magic no
	while (fgets(buf, sizeof(buf), fp) != 0) {
	}
	int length = strlen(buf) * sizeof(char);
	buf[length - 1] = '\0';
	client_send_code(sockd, 0x10, 0x01);	// HARDCODED
	client_send_data(sockd, buf);
	pclose(fp);
	return 0;
}

int
client_send_cwd(int sockd, char *msg)
{
	if (chdirw(msg) == -1) {
		fprintf(stderr, "No such directory: %s ERRNO:%d\n", msg, errno);
		client_send_code(sockd, 0x10, 0x00);
	} else {
		fprintf(stderr, "CD success\n");
		client_send_code(sockd, 0x10, 0x00);
	}
	return 0;
}
#endif	// End of __FTP_CLIENT_HEADER__
