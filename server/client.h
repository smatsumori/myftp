#ifndef __FTP_CLIENT_HEADER__
#define __FTP_CLIENT_HEADER__

/* client status */

struct client {
	struct client *fp;
	struct client *bp;
	int id;

	/* client network status (network byte order) */
	struct in_addr id_addr;
	struct in_addr cli_addr;
	struct in_addr netmask;		// do we need this?
	in_port_t port;
};

#endif	// End of __FTP_CLIENT_HEADER__
