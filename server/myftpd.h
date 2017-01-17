#ifndef __HEADER_MYDHCPD__
#define __HEADER_MYDHCPD__
/*** INCLUDES ***/
#include "../utils/utils.h"
#include "../utils/packet.h"

struct myftpdhead {
	int mysockd;		/* socket descriptor for this client */
	int argc;
	int cmd;
	char argv[CMD_LENGTH][MAX_CMD];
	struct sockaddr_in servsockaddr;
	struct myftp_packh packet_to_send;
	struct myftp_packh packet_recieved;
};

#endif	//endof: __HEADER_MYDHCPD__
