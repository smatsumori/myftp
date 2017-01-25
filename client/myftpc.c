#include "./tcpmodule.h"

/*** PROTOTYPE ***/
int wait_event(struct myftpchead *hpr, int status);

// TODO: remove EV_STDIN
enum event_Flags {
	EV_SENTINEL, EV_INIT_CMPL, EV_STDIN, EV_STDIN_INVALID, EV_RECV_PACKET,
	EV_FTPCMD_QUIT, EV_FTPCMD_PWD, EV_FTPCMD_CD, EV_FTPCMD_DIR,		// 2-steps-commands
	EV_FTPCMD__GET, EV_FTPCMD__PUT,															// 3-steps-commands
	EV_RECV_INVALID,
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
	{EV_RECV_INVALID, "EV_RECV_INVALID", "Error packet recieved."},
	{EV_FTPCMD_QUIT, "EV_FTPCMD_QUIT", "Close connection."},
	{EV_FTPCMD_PWD, "EV_FTPCMD_PWD", "Ftpcmd pwd"},
	{EV_FTPCMD_DIR, "EV_FTPCMD_DIR", "Ftpcmd dir"},
	{EV_FTPCMD_CD, "EV_FTPCMD_CD", "Ftpcmd cd"},
	{EV_FTPCMD__GET, "EV_FTPCMD__GET", "Ftpcmd get"},
	{EV_FTPCMD__PUT, "EV_FTPCMD__PUT", "Ftpcmd put"},
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
	{ST_ESTABLISHED, EV_FTPCMD_PWD, send_pwd, ST_WAIT_PWD},
	{ST_ESTABLISHED, EV_FTPCMD_CD, send_cwd, ST_WAIT_CWD},
	{ST_ESTABLISHED, EV_FTPCMD_DIR, send_dir, ST_WAIT_LIST},
	{ST_ESTABLISHED, EV_FTPCMD__GET, send_retr, ST_WAIT__RETR_OK},
	{ST_ESTABLISHED, EV_FTPCMD__PUT, send_stor, ST_WAIT__STOR_OK},
	{ST_ESTABLISHED, EV_FTPCMD_QUIT, send_quit, ST_EXIT},
	{ST_WAIT_PWD, EV_RECV_PACKET, dummy, ST_ESTABLISHED},
	{ST_WAIT_PWD, EV_RECV_INVALID, dummy, ST_ESTABLISHED},
	{ST_WAIT_CWD, EV_RECV_PACKET, dummy, ST_ESTABLISHED},
	{ST_WAIT_CWD, EV_RECV_INVALID, dummy, ST_ESTABLISHED},
	{ST_WAIT_LIST, EV_RECV_PACKET, dummy, ST_ESTABLISHED},
	{ST_WAIT_LIST, EV_RECV_INVALID, dummy, ST_ESTABLISHED},
	{ST_WAIT__RETR_OK, EV_RECV_PACKET, recv_data, ST_ESTABLISHED},
	{ST_WAIT__RETR_OK, EV_RECV_INVALID, dummy, ST_ESTABLISHED}, 
	{ST_WAIT__STOR_OK, EV_RECV_PACKET, send_data, ST_ESTABLISHED},
	{ST_WAIT__STOR_OK, EV_RECV_INVALID, dummy, ST_ESTABLISHED},
	{ST_WAIT_PACKET, EV_TIMEOUT, tcpc_send, ST_WAIT_PACKET_RE},
	{ST_WAIT_PACKET, EV_RECV_PACKET, dummy, ST_ESTABLISHED},
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
	struct proctable *ptptr;
	struct myftpchead *hpr = &ftpched;
	#ifdef DEBUG
		fprintf(stderr, "Running on DEBUG MODE\n");
	#endif
	#ifndef DEBUG
		if (1 < argc) {
			hpr->hostname = argv[1];
		} else {
			fprintf(stderr, "usage: ./myftpcout <hostname>\n");
			exit(0);
		}
	#endif
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
	// TODO: implement packet handler
	char cmd[CMD_LENGTH];	
	switch (status) {
		case ST_INIT:
			return EV_INIT_CMPL;

		case ST_WAIT_PACKET_RE:
		case ST_WAIT_PACKET:
			return EV_RECV_PACKET;	// TODO: implement

		case ST_WAIT__RETR_OK:
		case ST_WAIT__STOR_OK:
		case ST_WAIT_PWD:
		case ST_WAIT_CWD:
		case ST_WAIT_LIST:
			tcpc_recv(hpr);
			if (packet_checker(hpr) <= 0) {	// invalid
				return EV_RECV_INVALID;
			} else {
				return EV_RECV_PACKET;
			}
			break;

		case ST_ESTABLISHED:
			fprintf(stderr, "$ftp ");
			fgets(cmd, CMD_LENGTH, stdin);		//TODO: error handling
			switch (setcmd(hpr, cmd)) {
				case 0:		// VALID (ftp command)
					switch (hpr->type) {
						case FTP_QUIT:
							return EV_FTPCMD_QUIT;
						case FTP_PWD:
							return EV_FTPCMD_PWD;
						case FTP_CWD:
							return EV_FTPCMD_CD;
						case FTP_LIST:
							return EV_FTPCMD_DIR;
						case FTP_RETR:
							return  EV_FTPCMD__GET;
						case FTP_STOR:
							return EV_FTPCMD__PUT;
					}
					return EV_STDIN;
				case 1:		// VALID (client command)
					return EV_STDIN_CLICMD;
				case -1:		// INVALID
					return EV_STDIN_INVALID;
			}
		  break;		// end of ST_ESTABLISHED
	}
	return EV_INVALID;
}
