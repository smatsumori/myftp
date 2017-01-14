#include "./myftpc.h"
#include "./tcpmodule.h"

enum event_Flags {
	EV_SENTINEL, EV_INIT_CMPL, EV_STDIN, EV_STDIN_INVALID, EV_RECV_PACKET,
	EV_TIMEOUT, EV_EXIT, EV_INVALID
};

enum status_Flags {
	ST_SENTINEL, ST_INIT, ST_ESTABLISHED, ST_WAIT_PACKET, ST_WAIT_PACKET_RE,
	ST_EXIT,
};

struct myftpchead ftpched = {
	.mysockd = -1
};

struct proctable ptab[] = {
	{ST_INIT, EV_INIT_CMPL, tcpc_quick_establish, ST_ESTABLISHED},
	{ST_ESTABLISHED, EV_STDIN, tcpc_send, ST_WAIT_PACKET},
	{ST_ESTABLISHED, EV_STDIN_INVALID, dummy, ST_ESTABLISHED},
	{ST_WAIT_PACKET, EV_TIMEOUT, tcpc_send, ST_WAIT_PACKET_RE},
	{ST_WAIT_PACKET, EV_RECV_PACKET, dummy, ST_ESTABLISHED},		// TODO: remove dummy
	{ST_WAIT_PACKET_RE, EV_TIMEOUT, tcpc_close, ST_EXIT},
	{ST_SENTINEL, EV_SENTINEL, NULL, ST_SENTINEL}
};

struct command_table {	// TODO: ADD command here
	char *cmd;
	void (*func)(int, char *[]);
} cmd_tab[] = {
	{NULL, NULL}
};

int main(int argc, char const* argv[])
{
	#ifdef DEBUG
		fprintf(stderr, "Running on DEBUG MODE\n");
	#endif
	#ifndef DEBUG
		/* get server host name from stdin */
	#endif
	struct proctable *ptptr;
	struct myftpchead *hpr = &ftpched;
	int status = ST_INIT;
	int event = EV_INIT_CMPL;

	while(1) {
		if ((event = wait_event(hpr)) == EV_INVALID)
			report_error_and_exit(ERROR_EVENT, "main");
//		print_event(event, etab);
		
		for (ptptr = ptab; ptptr -> status; ptptr++) {
				if (ptptr -> status == status && ptptr -> event == event) {
					(*ptptr -> func)(hpr);
					status = ptptr->nextstatus;
					fprintf(stderr, "moving to status: %2d\n\n", status);
//					print_status(status, stab);
					break;
				}
		}
		if (ptptr -> status == ST_SENTINEL) 
			report_error_and_exit(ERR_PROCESSING, "Hit sentinel. Processing error in main()");
	
	}

	return 0;
}

int
wait_event(struct myftpchead *hpr, int status)
{
	switch (status) {
		case ST_INIT:
			return EV_INIT_CMPL;

		case ST_WAIT_PACKET_RE:
		case ST_WAIT_PACKET:
			return EV_RECV_PACKET;	// TODO: implement

		case ST_ESTABLISHED:		// TODO: implement
			return EV_STDIN;
			return EV_STDIN_INVALID;
	
	}
	return EV_INVALID;
}
