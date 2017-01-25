#include "./myftpd.h"
#include "./tcpsmodule.h"
#include "./client.h"

/*** FSM ***/
enum globalevent_Flags {
	GLOBAL_EV_RECVCONNREQ, GLOBAL_EV_CHILD_EXIT,
	GLOBAL_EV_EXIT,
};

/*** PROTOTYPES ***/
int global_event_dispatcher(struct myftpdhead *hpr);
int global_client_handler(struct myftpdhead *hpr, int event);
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
global_client_handler(struct myftpdhead *hpr, int event)
{
	int pid, stat;
	switch (event) {
		case GLOBAL_EV_RECVCONNREQ:
			fprintf(stderr, "Dispatching client process\n");
			// TODO: handle signals
			switch ((pid = fork())) {
				case -1:
					report_error_and_exit(ERR_FORK, "global_client_handler");
					break;
				case 0:		/* child */
					// TODO: execute cmd
					client_handler(hpr);
					break;
				default:		/* parent */
					if (waitpid(pid, &stat, 0) < 0) 
						report_error_and_exit(ERR_WAIT, "global_client_handler");
					break;
			}
	}
	return 0;
}

/*** MAIN ***/
int main(int argc, char const* argv[])
{
	#ifndef DEBUG
		if (1 < argc) {
			 if (chdirw((char *)argv[1]) < 0) {
					report_error_and_exit(ERR_CHDIR, "Invalid path");
			 }
			 fprintf(stderr, "Server Default PWD: %s\n", getenv("PWD"));
		}
	#endif
	int event;
	struct myftpdhead *hpr = &myftpdh;
	tcpd_init(hpr);
	while (1) {
		event = global_event_dispatcher(hpr);
		global_client_handler(hpr, event);
	}
	return 0;
}
