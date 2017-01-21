#include "./myftpc.h"
#include "./tcpmodule.h"

/*** PROTOTYPE ***/
int wait_event(struct myftpchead *hpr, int status);


enum event_Flags {
	EV_SENTINEL, EV_INIT_CMPL, EV_STDIN, EV_STDIN_INVALID, EV_RECV_PACKET,
	EV_FTPCMD_PWD, EV_FTPCMD_CD, EV_FTPCMD_DIR,		// 2-steps-commands	TODO:
	EV_FTPCMD__GET, EV_FTPCMD__PUT,															// 3-steps-commands TODO:
	EV_RECV_CMDERR, EV_RECV_FILEERR, EV_RECV_UNKWNERR,
	EV_TIMEOUT, EV_EXIT, EV_INVALID, EV_STDIN_CLICMD,
};

enum status_Flags {
	ST_SENTINEL, ST_INIT, ST_ESTABLISHED, ST_WAIT_PACKET, ST_WAIT_PACKET_RE,
	ST_WAIT_PWD, ST_WAIT_CWD, ST_WAIT_LIST,
	ST_WAIT__RETR_OK, ST_WAIT__STOR_OK,
	ST_EXIT,
};

struct myftpchead ftpched = {
	.mysockd = -1, .data_to_send = NULL
};

struct eventtable etab[] = {
	{EV_INIT_CMPL, "EV_INIT_CMPL", ""},
	{EV_STDIN, "EV_STDIN", ""},
	{EV_STDIN_INVALID, "EV_STDIN_INVALID", "Invalid input."},
	{EV_RECV_PACKET, "EV_RECV_PACKET", "Packet recived from server."},
	{EV_TIMEOUT, "EV_TIMEOUT", ""},
	{EV_EXIT, "EV_EXIT", "Exiting client."},
	{EV_INVALID, "EV_INVALID", "Invalid Event."},
	{EV_STDIN_CLICMD, "EV_STDIN_CLICMD", "Client command."},
	{EV_RECV_CMDERR, "EV_RECV_CMDERR", "Command Error."},
	{EV_RECV_FILEERR, "EV_RECV_FILEERR", "File Error."},
	{EV_RECV_UNKWNERR, "EV_RECV_UNKWNERR", "Unknown Error."},
	{EV_SENTINEL, "EV_SENTINEL", "Sentinel."}
};

struct eventtable stab[] = {
	{ST_INIT, "ST_INIT", ""},
	{ST_ESTABLISHED, "ST_ESTABLISHED", ""},
	{ST_WAIT_PACKET, "ST_WAIT_PACKET", ""},
	{ST_WAIT_PACKET_RE, "ST_WAIT_PACKET_RE", ""},
	{ST_EXIT, "ST_EXIT", ""},
	{ST_WAIT_PWD, "ST_WAIT_PWD", ""},
	{ST_WAIT_CWD, "ST_WAIT_CWD", ""},
	{ST_WAIT_LIST, "ST_WAIT_LIST", ""},
	{ST_WAIT__RETR_OK, "ST_WAIT__RETR_OK", ""},
	{ST_WAIT__STOR_OK, "ST_WAIT__STOR_OK", ""},
	{ST_SENTINEL, "ST_SENTINEL", "Sentinel"}
};

struct proctable ptab[] = {
	{ST_INIT, EV_INIT_CMPL, tcpc_quick_establish, ST_ESTABLISHED},
	{ST_ESTABLISHED, EV_STDIN, tcpc_send, ST_WAIT_PACKET},
	{ST_ESTABLISHED, EV_STDIN_INVALID, dummy, ST_ESTABLISHED},
	{ST_ESTABLISHED, EV_STDIN_CLICMD, exec_cmd, ST_ESTABLISHED},
	{ST_ESTABLISHED, EV_FTPCMD_PWD, dummy, ST_WAIT_PWD},	// TODO: send msg
	{ST_ESTABLISHED, EV_FTPCMD_CD, dummy, ST_WAIT_CWD},
	{ST_ESTABLISHED, EV_FTPCMD_DIR, dummy, ST_WAIT_LIST},
	{ST_ESTABLISHED, EV_FTPCMD__GET, dummy, ST_WAIT__RETR_OK},
	{ST_ESTABLISHED, EV_FTPCMD__PUT, dummy, ST_WAIT__STOR_OK},
	{ST_WAIT_PWD, EV_RECV_PACKET, dummy, ST_ESTABLISHED},			//TODO: show msg
	{ST_WAIT_CWD, EV_RECV_PACKET, dummy, ST_ESTABLISHED},
	{ST_WAIT_LIST, EV_RECV_PACKET, dummy, ST_ESTABLISHED},
	{ST_WAIT__RETR_OK, EV_RECV_PACKET, dummy, ST_ESTABLISHED},		//TODO: send packet
	{ST_WAIT__RETR_OK, EV_RECV_FILEERR, dummy, ST_ESTABLISHED}, // TODO: show err msg
	{ST_WAIT__STOR_OK, EV_RECV_PACKET, dummy, ST_ESTABLISHED},
	{ST_WAIT__STOR_OK, EV_RECV_FILEERR, dummy, ST_ESTABLISHED},
	{ST_WAIT_PACKET, EV_TIMEOUT, tcpc_send, ST_WAIT_PACKET_RE},
	{ST_WAIT_PACKET, EV_RECV_PACKET, dummy, ST_ESTABLISHED},		// TODO: remove dummy and show pack
	{ST_WAIT_PACKET_RE, EV_TIMEOUT, tcpc_close, ST_EXIT},
	{ST_SENTINEL, EV_SENTINEL, NULL, ST_SENTINEL}
};

struct command_table {	// TODO: ADD command here
	char *cmd;
	void (*func)(int, char *[]);
} cmd_tab[] = {
	{NULL, NULL}
};


/*** MAIN ***/
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
		if ((event = wait_event(hpr, status)) == EV_INVALID)
			report_error_and_exit(ERROR_EVENT, "main:event");
		print_event(event, etab);
		
		for (ptptr = ptab; ptptr -> status; ptptr++) {
				if (ptptr -> status == status && ptptr -> event == event) {
					(*ptptr -> func)(hpr);
					status = ptptr->nextstatus;
					fprintf(stderr, "moving to status: %2d\n\n", status);
					print_status(status, stab);
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
	//TODO: implement packet handler
	char cmd[CMD_LENGTH];	
	switch (status) {
		case ST_INIT:
			return EV_INIT_CMPL;

		case ST_WAIT_PACKET_RE:
		case ST_WAIT_PACKET:
			return EV_RECV_PACKET;	// TODO: implement

		case ST_ESTABLISHED:		// TODO: implement
			fprintf(stderr, "$ftp ");
			fgets(cmd, CMD_LENGTH, stdin);		//TODO: error handling
			switch (setcmd(hpr, cmd)) {
				case 0:		// VALID (ftp command)
					return EV_STDIN;	// TODO:IMPLEMENT
				case 1:		// VALID (client command)
					return EV_STDIN_CLICMD;
				case -1:		// INVALID
					return EV_STDIN_INVALID;
			}
	}
	return EV_INVALID;
}
