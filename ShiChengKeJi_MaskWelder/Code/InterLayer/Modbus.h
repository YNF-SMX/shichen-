#ifndef _MODBUS_H_
#define	_MODBUS_H_
#include "bsplib.h"
#include "interlayer_config.h"

//#define	ModbusLocalID	1


//´óÐ¡¶Ë×ª»»
#define	Endian_Transform_16(A)		(((u16)(A)&0xff00)>>8)|(((u16)(A)&0x00ff)<<8)
#define	Endian_Transform_32(A)		(((u32)(A)&0xff000000)>>24)|(((u32)(A)&0x00ff0000)>>8)|(((u32)(A)&0x0000ff00)<<8)|(((u32)(A)&0x000000ff)<<24)

#define	NoError					0
#define WrongDeviceAddr			1
#define WrongFunctionCode		2
#define WrongReceiveLength		3
#define WrongCRC				4
#define RegisterAddrTooLarge	5
#define	WrongWriteAddr			6

#define ETH						0
#define uart_485				1
#define uart_232				2


#endif
