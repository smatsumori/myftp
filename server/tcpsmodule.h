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
#define ERR_SOCKET 13
#define ERR_BIND 14
#define ERR_ACCEPT 15

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
	struct sockaddr_in myskt;
	myskt.sin_family = AF_INET;
	myskt.sin_port = htons(FTP_SERV_PORT);
	myskt.sin_addr.s_addr = htonl(INADDR_ANY);

	if (bind(hpr->mysockd, (struct sockaddr *)&myskt, sizeof myskt) < 0)
		report_error_and_exit(ERR_BIND, "tcpd_init");

	fprintf(stderr, "Compleate!\n");

	fprintf(stderr, "Port: %d\n", FTP_SERV_PORT);
	return;
}

void
tcpd_listen(struct myftpdhead *hpr)
{
	fprintf(stderr, "Listening...\n");
	if (listen(hpr->mysockd, 5) < 0)
		report_error_and_exit(ERR_CONNECT, "tcpd_listen");
	return;
}

int
tcp_accept(struct myftpdhead *hpr)
{
	fprintf(stderr, "Accepting...\n");
	socklen_t sktlen = sizeof (hpr->clisockaddr);
	if ((hpr->clisockd = accept(hpr->mysockd, 
					(struct sockaddr *)&hpr->clisockaddr, &sktlen)) < 0)
		report_error_and_exit(ERR_ACCEPT, "tcp_accept");
	fprintf(stderr, "Accepted!\n");
	fprintf(stderr, "Client: IP %s\n", inet_ntoa(hpr->clisockaddr.sin_addr));
	return 0;
}

#endif	// __TCPSMODULE_HEADER__
