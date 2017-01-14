#include "./myftpc.h"

enum event_Flags {
	EV_SENTINEL, EV_INIT_CMPL, EV_STDIN, EV_STDIN_INVALID, EV_RECV_PACKET,
	EV_TIMEOUT, EV_EXIT,
};

enum status_Flags {
	ST_SENTINEL, ST_INIT, ST_ESTABLISHED, ST_WAIT_PACKET, ST_WAIT_PACKET_RE,
	ST_EXIT,
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
	return 0;
}
