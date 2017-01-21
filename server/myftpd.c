#include "./myftpd.h"
#include "./tcpsmodule.h"

/*** FSM ***/
enum globalevent_Flags {
	GLOBAL_EV_RECVCONNREQ, GLOBAL_EV_CHILD_EXIT,
	GLOBAL_EV_EXIT,
};

/*** PROTOTYPES ***/
int global_event_dispatcher(struct myftpdhead *hpr);
int global_client_selector(struct myftpdhead *hpr);
struct myftpdhead myftpdh;

/*** SIGNAL HANDLER ***/

/*** FUNCTION ***/
int 
global_event_dispatcher(struct myftpdhead *hpr)
{
	// TODO: implement
	if (0) {
		// exit server
		return GLOBAL_EV_EXIT;
	}
	tcpd_listen(hpr);
	switch (tcp_accept(hpr)) {
		case 0:
		return GLOBAL_EV_RECVCONNREQ;
	}
	return 0;
}
int 
global_client_selector(struct myftpdhead *hpr)
{
	// TODO: implement
	return 0;
}

/*** MAIN ***/
int main(int argc, char const* argv[])
{
	struct myftpdhead *hpr = &myftpdh;
	tcpd_init(hpr);
	while (1) {
		global_event_dispatcher(hpr);
	}
	return 0;
}
