#ifndef __HEADER_MYDHCPD__
#define __HEADER_MYDHCPD__
/*** INCLUDES ***/
#include "../utils/utils.h"
#include "../utils/packet.h"
#include <netinet/in.h>

struct myftpdhead {
	// TODO: implement
	int mysockd;		/* socket descriptor for this server */
	int clisockd;		/* socket descriptor for client in command*/
	int argc;
	int cmd;
	char argv[CMD_LENGTH][MAX_CMD];
	struct sockaddr_in clisockaddr;
	struct myftp_packh packet_to_send;
	struct myftp_packh packet_recieved;
};

#endif	//endof: __HEADER_MYDHCPD__
