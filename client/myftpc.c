#include "./myftpc.h"

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
