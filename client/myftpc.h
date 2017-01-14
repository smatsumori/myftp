#ifndef __HEADER_MYFTPC__
#define __HEADER_MYFTPC__

/*** PROTOTYPES ***/

/*** INCLUDES ***/
#include "../utils/utils.h"
#include "../utils/packet.h"
#include <netinet/in.h>

struct myftpchead {
	int mysockd;		/* socket descriptor for this client */
	struct sockaddr_in servsockaddr;
	struct myftp_packh packet_to_send;
	struct myftp_packh packet_recieved;
};

struct proctable {
	int status;
	int event;	/* input */
	void (*func)(struct dhcphead *hpr);
	int nextstatus;
};

#endif	// __HEADER_MYFTPC__
