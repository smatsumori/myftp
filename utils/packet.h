#ifndef __MYFTP_PACKET__
#define __MYFTP_PACKET__

/*** DEFINES ***/
#define FTP_DATASIZE 1024
#define FTP_MAX_RECVSIZE 8000
#define FTP_QUIT 0x01
#define FTP_PWD 0x02
#define FTP_CWD 0x03
#define FTP_LIST 0x04
#define FTP_RETR 0x05
#define FTP_STOR 0x06

#define CODE_OK 0x00
#define CODE_OK_DATA_FOLLOW_S
#define CODE_OK_DATA_FOLLOW_C
#define CODE_CMD_ERR_SYNTAX
#define CODE_CMD_ERR_UNDEFINED
#define CODE_CMD_ERR_PROTOCOL
#define CODE_FILE_ERR_NO_SUCH_FILE
#define CODE_FILE_ERR_PERMISSION_DENIED
#define CODE_UNKWN_ERR

/*** INCLUDES ***/
#include <stdint.h>

/*** FTPPACKET ***/
struct myftp_packh {
	uint8_t type;
	uint8_t code;
	uint16_t length;
};

void
print_packeth(struct myftp_packh *fpr)
{
	printf("Type: 0x%x, Code: 0x%x, Size: %d\n", fpr->type, fpr->code, fpr->length);
	return;
}

#endif	// __MYFTP_PACKET__
