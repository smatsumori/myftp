#include "./myftpc.h"
#include "./tcpmodule.h"

enum event_Flags {
	EV_INIT, EV_TCP_CONNECTED, EV_TCP_DISCONNECT
};

enum status_Flags {
	ST_INIT,
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
