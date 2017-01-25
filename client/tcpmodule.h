#ifndef __HEADER_TCPC_MODULE__
#define __HEADER_TCPC_MODULE__
/*** INCLUDES ***/
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include "./myftpc.h"

// TODO: move following defines to utils
#define ERR_CONNECT 10
#define ERR_ATON 11
#define ERR_CLOSE 12

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

	#ifndef DEBUG
		struct addrinfo hints;
		struct addrinfo *res;
		fprintf(stderr, "resolving hostname: %s:%d\n", hpr->hostname, FTP_SERV_PORT);
		int err, sd;
		char port[10];
		sprintf(port, "%d", FTP_SERV_PORT);
		memset(&hints, 0, sizeof hints);
		hints.ai_socktype = SOCK_STREAM;
		if ((err = getaddrinfo((char *)hpr->hostname, port, &hints, &res)) < 0) {
			report_error_and_exit(ERR_GETADDR, "Cannot resolve hostname\n");
		}
		if ((connect(hpr->mysockd, res->ai_addr, res->ai_addrlen)) < 0)
			report_error_and_exit(ERR_CONNECT, "tcpc_connreq: getaddr");
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
	#ifdef DEBUG
	fprintf(stderr, "Establishing TCP connection with: %s\n",
			inet_ntoa(hpr->servsockaddr.sin_addr));
	socklen_t namelen = sizeof(hpr->servsockaddr);
	if ((connect(hpr->mysockd, (struct sockaddr *)&(hpr->servsockaddr), namelen)) < 0)
		report_error_and_exit(ERR_CONNECT, "tcpc_connreq");
	#endif
	#ifndef DEBUG

	#endif
	fprintf(stderr, "Established!\n");
	return;
}

void
tcpc_send_data(struct myftpchead *hpr)
{
	int hsize = 0;
	int dsize = 0;
	int dremains = 0;
	struct myftp_packh myftphp = {
		.type = 0x20, .code = 0x01, .length = strlen(hpr->data_to_send)
	};
	fprintf(stderr, "Sending data packet...\n", hpr->data_to_send);
	dremains = myftphp.length;
	
	/* send ftp packet header */
	if ((hsize = send(hpr->mysockd, &myftphp, sizeof myftphp, 0)) < 0) {
		report_error_and_exit(ERR_SENDTO, "send");
	}
	fprintf(stderr, "Sending Head Packet ");
	print_packeth(&myftphp);

	while (1) {

		/* send data packet or ftp EOF packet */
		if (hpr->data_to_send != NULL) {
			if (dremains <= 0) {		// EOF
				myftphp.type = 0x20;
				myftphp.code = 0x00;
				fprintf(stderr, "Sending EOF ");
				print_packeth(&myftphp);
				if ((hsize = send(hpr->mysockd, &myftphp, sizeof myftphp, 0)) < 0) {
					report_error_and_exit(ERR_SENDTO, "send");
				}
				break;
			} else {
				fprintf(stderr, "Dremains: %d\n", dremains);
				if ((dsize = send(hpr->mysockd, hpr->data_to_send, FTP_DATASIZE, 0)) < 0) {
					report_error_and_exit(ERR_SENDTO, "send");
				}
				fprintf(stderr, "Sending Data %d\n", dsize);
				dremains -= FTP_DATASIZE;
			}
		} else {
			break;
		}
	}
	fprintf(stderr, "Complete\n");
	fprintf(stderr, "Data size: (HEAD)%d + (DATA)%d\n", hsize, dsize);
	hpr->data_to_send = NULL;
	return;
}

void
tcpc_send(struct myftpchead *hpr)
{
	int hsize = 0;
	int dsize = 0;
	int dremains = 0;
	/* send packet */
	fprintf(stderr, "Sending code packet...");
	/*  set header information */
	hpr->packet_to_send.type  = hpr->type;
	hpr->packet_to_send.code = hpr->code;
	hpr->packet_to_send.length = 0;
	if (1 < hpr->argc) {
		hpr->packet_to_send.length = strlen(hpr->argv[1]);
	}
	/* send ftp code packet */
	if ((hsize = send(hpr->mysockd, &hpr->packet_to_send,
				 	sizeof hpr->packet_to_send, 0)) < 0)
		report_error_and_exit(ERR_SENDTO, "send");

	if (1 < hpr->argc) {
		fprintf(stderr, "ARG: %s\n", hpr->argv[1]);
		if ((dsize = send(hpr->mysockd, hpr->argv[1],strlen(hpr->argv[1]), 0)) < 0)
			report_error_and_exit(ERR_SEND, "send");
	}
	fprintf(stderr, "Complete! ");
	print_packeth(&hpr->packet_to_send);
	hpr->data_to_send = NULL;
	return;
}

void
tcpc_recv(struct myftpchead *hpr)
{
	/* common for server and client */
	fprintf(stderr, "Recieving packet...\n");
	int pc = 0;
	int size;
	char buf[FTP_MAX_RECVSIZE];
	char obuf[FTP_DATASIZE];
	static struct myftp_packh myftphp;
	while (1) {
		/* recv ftp packet header */
		if ((size = recv(hpr->mysockd, &myftphp, sizeof(myftphp), 0)) < 0)
			report_error_and_exit(ERR_RECV, "client_recv");
		fprintf(stderr, "[%d] Size: %d, ", ++pc, size);
		print_packeth(&myftphp);
		hpr->packet_recieved = myftphp;

		if (myftphp.type == 0x20) {		// IF: data
			if (myftphp.code == 0x00) {
				fprintf(stderr, "EOF\n");
				fprintf(stderr, "DATA: %s\n", buf);		// TODO: remove this
				hpr->data_to_send = buf;
				return;
			} else if (myftphp.code == 0x01) {
				/* recv ftp data */
				if ((size = recv(hpr->mysockd, &obuf, FTP_DATASIZE, 0)) < 0)
					report_error_and_exit(ERR_RECV, "client_recv");
				fprintf(stderr, "[%d] Data Size: %d, ", ++pc, size);
				print_packeth(&myftphp);
				strcat(buf, obuf);
				continue;
			}
		} else if (0 < myftphp.length) {		// IF: code data
			if ((size = recv(hpr->mysockd, &obuf, FTP_DATASIZE, 0)) < 0)
				report_error_and_exit(ERR_RECV, "client_recv");
			fprintf(stderr, "[%d] Code Data Size: %d\n, ", ++pc, size);
			//strcat(buf, obuf);
			obuf[size] = '\0';
			fprintf(stderr, "%s\n", obuf);
			return;
		} else {
			return;
		}	
	}
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
tcpc_quick_establish(struct myftpchead *hpr)
{
	/* establish connection with no error packet handling */
	tcpc_init(hpr);
	tcpc_connreq(hpr);
	return;
}
#endif // __HEADER_TCPC_MODULE__
