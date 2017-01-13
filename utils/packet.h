#ifndef __MYFTP_PACKET__
#define __MYFTP_PACKET__

/*** DEFINES ***/
#define DATASIZE 1024

/*** FTPPACKET ***/
struct myftp_packh {
	uint8_t type;
	uint8_t code;
	uint16_t length;
};
#endif	// __MYFTP_PACKET__
