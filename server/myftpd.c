#include "./mydhcpd.h"

/*** FSM ***/
enum globalevent_Flags {
	GLOBAL_EV_RECVCONNREQ, GLOBAL_EV_CHILD_EXIT,
};

/*** PROTOTYPES ***/
int global_event_dispatcher(struct myftpdhead *hpr);
int global_client_selector(struct myftpdhead *hpr);

/*** SIGNAL HANDLER ***/

/*** FUNCTION ***/
int 
global_event_dispatcher(struct myftpdhead *hpr)
{
	// TODO: implement
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
	
	return 0;
}
