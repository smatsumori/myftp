#ifndef __TCPSMODULE_HEADER__
#define __TCPSMODULE_HEADER__
// TODO: merge this file with tcpdmodule if it's possible

/*** INCLUDES ***/
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "./myftpd.h"

// TODO: move the following lines to utils
#define ERR_CONNECT 10
#define ERR_ATON 11
#define ERR_CLOSE 12
#define FTP_SERV_PORT 50021
#define FTP_SERV_ADDR "131.113.108.53"

/*** PROTOTYPES ***/
void dummy(struct myftpdhead *hpr);
void tcpd_init(struct myftpdhead *hpr);
void tcpd_connreq(struct myftpdhead *hpr);
void tcpd_close(struct myftpdhead *hpr);
void tcpd_send(struct myftpdhead *hpr);
void tcpd_recv(struct myftpdhead *hpr);
void tcpd_quick_establish(struct myftpdhead *hpr);

/*** FUNCTIONS ***/
void 
dummy(struct myftpdhead *hpr)
{
	#ifdef DEBUG
		fprintf(stderr, "DUMMY FUNCTION\n");
	#endif
	return;
}

void 
tcpd_init(struct myftpdhead *hpr)
{
	fprintf(stderr, "TCP initialization... ");
	/*** SERVER ***/
	if ((hpr->mysockd = socket(PF_INET, SOCK_STREAM, 0)) < 0)
		report_error_and_exit(ERR_SOCKET, "tpcp_init");
	// TODO: bind socket
	return;
}

#endif	// __TCPSMODULE_HEADER__
