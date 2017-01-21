#ifndef __FTP_CLIENT_HEADER__
#define __FTP_CLIENT_HEADER__


/*** PROTOTYPE ***/
int client_recv(struct myftpdhead *hpr);

/*** FUNCTION ***/
int
client_handler(struct myftpdhead *hpr)
{
	fprintf(stderr, "Client forked.\n");
	client_recv(hpr);
	return 0;
}

int 
client_recv(struct myftpdhead *hpr)
{


}
#endif	// End of __FTP_CLIENT_HEADER__
