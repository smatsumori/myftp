#ifndef __FTP_CLIENT_HEADER__
#define __FTP_CLIENT_HEADER__

#include <stdio.h>

/*** PROTOTYPE ***/
int client_recv(struct myftpdhead *hpr);
int client_handler(struct myftpdhead *hpr);
int client_send_pwd(int sockd);


/*** FUNCTION ***/
int
client_handler(struct myftpdhead *hpr)
{
	int clisockd = hpr->clisockd;
	fprintf(stderr, "Client forked.\n");
	while (1) {
		switch (client_recv(hpr)) {
			case FTP_PWD:
				client_send_pwd(clisockd);
				break;
			case FTP_QUIT:
				close(clisockd);
				return 0;
		}
	}
	return 0;
}

int 
client_recv(struct myftpdhead *hpr)
{
	int size;
	char buf[FTP_MAX_RECVSIZE];
	char obuf[FTP_DATASIZE];
	struct myftp_packh myftphp;

	while (1) {
		/* recv ftp packet header */
		if ((size = recv(hpr->clisockd, &myftphp, sizeof(myftphp), 0)) < 0)
			report_error_and_exit(ERR_RECV, "client_recv");
		fprintf(stderr, "Packet Recived: %d, ", size);
		print_packeth(&myftphp);

		if (myftphp.type == 0x20) {
			if (myftphp.code == 0x00) {
				fprintf(stderr, "EOF\n");
				return 1;
			} else if (myftphp.code == 0x01) {
				/* recv ftp data */
				if ((size = recv(hpr->clisockd, &obuf, FTP_DATASIZE, 0)) < 0)
					report_error_and_exit(ERR_RECV, "client_recv");
				fprintf(stderr, "%s", obuf);
				continue;
			}
		} else {
			return myftphp.type;
		}
	}
}

int
client_send_data(int sockd, char *msg)
{
	struct myftp_packh myftphp = {
		.type = 0x20, .code = 0x01, .length = sizeof(char) * strlen(msg)
	};
	int dataleft = sizeof(char) * strlen(msg);
	fprintf(stderr, "Sending Data: %d\n", dataleft);
	while (1) {
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
	return 0;
}

int
client_send_pwd(int sockd)
{
	FILE *fp = popen("pwd", "r");
	char buf[256];		// TODO: remove magic no
	while (fgets(buf, sizeof(buf), fp) != 0) {
		puts(buf);
	}
	int length = strlen(buf) * sizeof(char);
	buf[length - 1] = '\0';			// remove escape sequence
	client_send_data(sockd, buf);
	pclose(fp);
	return 0;
}
int
client_send_list(int sockd)
{
	FILE *fp = popen("ls", "r");
	char buf[256];		// TODO: remove magic no
	while (fgets(buf, sizeof(buf), fp) != 0) {
		puts(buf);
	}
	int length = strlen(buf) * sizeof(char);
	buf[length - 1] = '\0';
	client_send_data(sockd, buf);
	pclose(fp);
	return 0;
}
#endif	// End of __FTP_CLIENT_HEADER__
