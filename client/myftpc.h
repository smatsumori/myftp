#ifndef __HEADER_MYFTPC__
#define __HEADER_MYFTPC__

struct myftpchead {
	int mysockd;		/* socket descriptor for this client */
	struct sockaddr_in servsockaddr;
	struct myftp_packh packet_to_send;
	struct myftp_packh packet_recieved;
};

#endif	// __HEADER_MYFTPC__
