#ifndef __HEADER_TCPC_MODULE__
#define __HEADER_TCPC_MODULE__
/*** INCLUDES ***/
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "./myftpc.h"

// TODO: move following defines to utils
#define ERR_CONNECT 10
#define ERR_ATON 11
#define ERR_CLOSE 12
#define FTP_SERV_PORT 50021
#define FTP_SERV_ADDR "131.113.108.53"

/*** PROTOTYPES ***/
void dummy(struct myftpchead *hpr);
void tcpc_init(struct myftpchead *hpr);
void tcpc_connreq(struct myftpchead *hpr);
void tcpc_close(struct myftpchead *hpr);
void tcpc_send(struct myftpchead *hpr);
void tcpc_recv(struct myftpchead *hpr);
void tcpc_quick_establish(struct myftpchead *hpr);

/*** FUNCTIONS ***/

void 
dummy(struct myftpchead *hpr)
{
	return;
}

void
tcpc_init(struct myftpchead *hpr) {
	fprintf(stderr, "TCP initialization... ");
	/*** CLIENT ***/
	if ((hpr->mysockd = socket(PF_INET, SOCK_STREAM, 0)) < 0)
		report_error_and_exit(ERR_SOCKET, "tpcp_init");

	/*** SERVER ***/
	bzero(&(hpr->servsockaddr), sizeof hpr->servsockaddr);
	hpr->servsockaddr.sin_family = AF_INET;
	hpr->servsockaddr.sin_port = htons(FTP_SERV_PORT);
	#ifdef DEBUG
	if (inet_aton(FTP_SERV_ADDR, &(hpr->servsockaddr.sin_addr)) == 0)
		report_error_and_exit(ERR_ATON, "tcpc_init");
	#endif
	#ifdef ANYADDR
	hpr->servsockaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	#endif
	fprintf(stderr, "Compleate!\n");
	return;
}

void
tcpc_connreq(struct myftpchead *hpr) {
	
	/* send connection request to server */
	fprintf(stderr, "Establishing TCP connection with: %s\n",
			inet_ntoa(hpr->servsockaddr.sin_addr));
	socklen_t namelen = sizeof(hpr->servsockaddr);
	if ((connect(hpr->mysockd, (struct sockaddr *)&(hpr->servsockaddr), namelen)) < 0)
		report_error_and_exit(ERR_CONNECT, "tcpc_connreq");
		
	fprintf(stderr, "Established!\n");
	return;
}

void 
tcpc_close(struct myftpchead *hpr) {
	/* close connection with server */
	fprintf(stderr, "Closing TCP connection with server: %s ... ", inet_ntoa(hpr->servsockaddr.sin_addr));

	if(close(hpr->mysockd) < 0)
		report_error_and_exit(ERR_CLOSE, "tcpc_close");

	fprintf(stderr, "Closed!\n");
	return;
}

void
tcpc_send(struct myftpchead *hpr)
{
	int hsize = 0;
	int dsize = 0;
	/* send packet */
	fprintf(stderr, "Sending packet...");

	/*  set header information */
	hpr->packet_to_send.type  = hpr->type;
	hpr->packet_to_send.code = hpr->code;
	if (hpr->data_to_send == NULL) {	// no data to send
		hpr->packet_to_send.length = 0;
	} else {
		hpr->packet_to_send.length = sizeof *(hpr->data_to_send);
	}

	if ((hsize = sendto(hpr->mysockd, &hpr->packet_to_send, sizeof hpr->packet_to_send, 0,
					(struct sockaddr *)&(hpr->servsockaddr), sizeof (hpr->servsockaddr))) < 0) {
		report_error_and_exit(ERR_SENDTO, "sendto");
	}
	if (hpr->data_to_send != NULL) {		/* if there is a data to send */
		if ((dsize = sendto(hpr->mysockd, hpr->data_to_send, sizeof *hpr->data_to_send, 0,	//TODO: check if this works
					(struct sockaddr *)&(hpr->servsockaddr), sizeof (hpr->servsockaddr))) < 0) {
			report_error_and_exit(ERR_SENDTO, "sendto");
		}
	}
	/* send data */
	fprintf(stderr, "Complete\n");
	fprintf(stderr, "Data size: (HEAD)%d + (DATA)%d\n", hsize, dsize);
	hpr->data_to_send = NULL;
	return;
}

void
tcpc_recv(struct myftpchead *hpr)
{
	// TODO; implement
	/* common for server and client */
	fprintf(stderr, "Recieving packet...");
	return;
}

void
tcpc_quick_establish(struct myftpchead *hpr)
{
	/* establish connection with no error packet handling */
	tcpc_init(hpr);
	tcpc_connreq(hpr);
	return;
}
#endif // __HEADER_TCPC_MODULE__
