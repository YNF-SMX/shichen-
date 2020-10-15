#include "Modbus.h"
#include "bsplib.h"
#include "crc16.h"
#include "string.h"
#include "IoUpdata.h"
#include "Tools.h"
#include "flashoperate.h"

extern u32* errocode;
extern u32* flahsave;
extern void* globaldata;
extern u32 sysdatalen;

extern u32 userwritelen;
extern u32 userreadlen;
extern u32 usersavelen;

u8 ModbusCheck(u8* Buf,u32 Len,u32 globaldatalen,u32 userreadlen,char ModbusLocalID)
{
    //Check ID
    if(Buf[0] != ModbusLocalID)
    {
        return WrongDeviceAddr;
    }
    //Check FunctionCode
    if(Buf[1] != 3 && Buf[1] != 6 && Buf[1] != 16 && Buf[1] != 0xA7)
    {
        return WrongFunctionCode;
    }
    //Check Received Length
    switch(Buf[1])
    {
    case 3:
    case 6:
        if(Len != 8)
        {
            return WrongReceiveLength;
        }
        break;

    case 16:
        if(Len != (Endian_Transform_16(*(unsigned short *)&Buf[4])) * 2 +9)
        {
            return WrongReceiveLength;
        }
        break;
		
	case 0xa7:
		if(Len != (Endian_Transform_16(*(unsigned short *)&Buf[8])) * 2 +12)
		{
            return WrongReceiveLength;
		}
		break;
    }
    //Check Addr Limit
    switch(Buf[1])
    {
    case 3:
    case 16:
        if(((Endian_Transform_16(*(unsigned short *)(&Buf[2]))) + (Endian_Transform_16(*(unsigned short *)(&Buf[4])))) >
                ((globaldatalen )/2))
        {
            return RegisterAddrTooLarge;
        }
        break;

    case 6:
        if((Endian_Transform_16(*(unsigned short *)&Buf[2])) > (globaldatalen)/2)
        {
            return RegisterAddrTooLarge;
        }
        break;
		
	case 0xa7:
        if
		(
			((Endian_Transform_16(*(unsigned short *)(&Buf[2]))) + (Endian_Transform_16(*(unsigned short *)(&Buf[4])))) > ((globaldatalen )/2) ||
			((Endian_Transform_16(*(unsigned short *)(&Buf[6]))) + (Endian_Transform_16(*(unsigned short *)(&Buf[8])))) > ((globaldatalen )/2)
		)
        {
            return RegisterAddrTooLarge;
        }
		break;

    }
    //Check write limit
    switch(Buf[1])
    {
    case 6:
        if((Endian_Transform_16(*(unsigned short *)&Buf[2])) < (SYSREADLEN+userreadlen)/2)
        {
            return WrongWriteAddr;
        }
        break;

    case 16:
        if((Endian_Transform_16(*(unsigned short *)&Buf[2])) + (Endian_Transform_16(*(unsigned short *)&Buf[4])) < (SYSREADLEN+userreadlen)/2)
        {
            return WrongWriteAddr;
        }
        break;
		
    case 0xa7:
        if((Endian_Transform_16(*(unsigned short *)&Buf[6])) + (Endian_Transform_16(*(unsigned short *)&Buf[8])) < (SYSREADLEN+userreadlen)/2)
        {
            return WrongWriteAddr;
        }
        break;
    }

    //Check CRC
    if((Endian_Transform_16(*(unsigned short *)&Buf[Len - 2])) != crc16(Buf,Len-2))
    {
        return WrongCRC;
    }

    return NoError;
}

void EndianChangeAndCopy_32(u8 *Target, u8 *Source, u32 length)
{
    u32 i;
    for(i=0; i<length/4; i++)
    {
        *(u32*)(Target+i*4) = (Endian_Transform_32(*(u32*)(Source+i*4)));
    }
}

void EndianChangeAndCopy_16(u8 *Target, u8 *Source, u32 length)
{
    u32 i;
    for(i=0; i<length/2; i++)
    {
        *(u16*)(Target+i*2) = (Endian_Transform_16(*(u16*)(Source+i*2)));
    }
}

void Modbus_Do(u32 RxLen,u8 *RxBuf,u32 *TxLen,u8 *TxBuf,void* globaldata,u32 sysdatalen,u32 userreadlen,char ModbusLocalID)
{
    u32 i;
    u8* RegArray_8 = globaldata;
    u16* RegArray_16 = globaldata;

    i = ModbusCheck(RxBuf,RxLen,sysdatalen,userreadlen,ModbusLocalID);
    if(i)
    {
        if(WrongDeviceAddr == i)
        {
            //*TxLen = 5;
            *TxLen = 0;
            return;
        }
        TxBuf[0] = ModbusLocalID;
        TxBuf[1] = RxBuf[1];
        TxBuf[2] = i;
        *(u16 *)&TxBuf[3] = Endian_Transform_16(crc16(TxBuf,3));
        *TxLen = 5;
    }
    else
    {
        switch(RxBuf[1])
        {
        case 3:
            if((Endian_Transform_16(*(u16*)&RxBuf[2])) <= INPUTENDADDR
                    && ((Endian_Transform_16(*(u16*)&RxBuf[2])) + (Endian_Transform_16(*(u16*)&RxBuf[4]))) >= INPUTSTARTADDR)
            {
                InputStatusUpdata();
            }

            if((Endian_Transform_16(*(u16*)&RxBuf[2])) <= OUTPUTENDADDR  && \
                    ((Endian_Transform_16(*(u16*)&RxBuf[2])) + (Endian_Transform_16(*(u16*)&RxBuf[4]))) >= OUTPUTSTARTADDR)
            {
                OutputStatusUpdata();
            }
            TxBuf[0] = ModbusLocalID;
            TxBuf[1] = 3;
            TxBuf[2] = (Endian_Transform_16(*(u16*)&RxBuf[4])) * 2;
            EndianChangeAndCopy_16(&TxBuf[3],&RegArray_8[(Endian_Transform_16(*(u16*)&RxBuf[2]))*2],TxBuf[2]);
            *(u16*)&TxBuf[3 + TxBuf[2]] = (Endian_Transform_16(crc16(TxBuf,TxBuf[2]+3)));
            *TxLen = TxBuf[2]+5;
            break;
        case 6:
            RegArray_16[Endian_Transform_16(*(u16*)&RxBuf[2])] = Endian_Transform_16(*(u16*)&RxBuf[4]);
            memcpy(TxBuf,RxBuf,8);
            *TxLen = 8;
            if((Endian_Transform_16(*(u16*)&RxBuf[2])) >= OUTPUTSTARTADDR \
                    && (Endian_Transform_16(*(u16*)&RxBuf[2])) <= OUTPUTENDADDR )
            {
                OutputUpdata();
            }
            break;
        case 16:
            EndianChangeAndCopy_16(&RegArray_8[(Endian_Transform_16(*(u16*)&RxBuf[2])) *2],&RxBuf[7],RxBuf[6]);
            memcpy(TxBuf,RxBuf,6);
            *(u16*)&TxBuf[6] = (Endian_Transform_16(crc16(TxBuf,6)));
            *TxLen = 8;
            if((Endian_Transform_16(*(u16*)&RxBuf[2])) <= OUTPUTENDADDR &&
                    ((Endian_Transform_16(*(u16*)&RxBuf[2])) + (Endian_Transform_16(*(u16*)&RxBuf[4]))) >= OUTPUTSTARTADDR)
            {
                OutputUpdata();
            }
            break;
			
		case 0xa7:
			//写处理
            EndianChangeAndCopy_16(&RegArray_8[(Endian_Transform_16(*(u16*)&RxBuf[6])) *2],&RxBuf[10],(Endian_Transform_16(*(u16*)&RxBuf[8])) *2);
            if((Endian_Transform_16(*(u16*)&RxBuf[6])) <= OUTPUTENDADDR &&
                    ((Endian_Transform_16(*(u16*)&RxBuf[6])) + (Endian_Transform_16(*(u16*)&RxBuf[8]))) >= OUTPUTSTARTADDR)
            {
                OutputUpdata();
            }
			//读处理
            if((Endian_Transform_16(*(u16*)&RxBuf[2])) <= INPUTENDADDR
                    && ((Endian_Transform_16(*(u16*)&RxBuf[2])) + (Endian_Transform_16(*(u16*)&RxBuf[4]))) >= INPUTSTARTADDR)
            {
                InputStatusUpdata();
            }
            if((Endian_Transform_16(*(u16*)&RxBuf[2])) <= OUTPUTENDADDR  && \
                    ((Endian_Transform_16(*(u16*)&RxBuf[2])) + (Endian_Transform_16(*(u16*)&RxBuf[4]))) >= OUTPUTSTARTADDR)
            {
                OutputStatusUpdata();
            }
			
            TxBuf[0] = RxBuf[0];
            TxBuf[1] = RxBuf[1];
            TxBuf[2] = RxBuf[2];
            TxBuf[3] = RxBuf[3];
            TxBuf[4] = RxBuf[4];
            TxBuf[5] = RxBuf[5];
            EndianChangeAndCopy_16(&TxBuf[6],&RegArray_8[(Endian_Transform_16(*(u16*)&RxBuf[2]))*2],(Endian_Transform_16(*(u16*)&RxBuf[4]))*2);
            *(u16*)&TxBuf[6 + (Endian_Transform_16(*(u16*)&RxBuf[4]))*2] = (Endian_Transform_16(crc16(TxBuf,(Endian_Transform_16(*(u16*)&RxBuf[4]))*2+6)));
            *TxLen = (Endian_Transform_16(*(u16*)&RxBuf[4]))*2+8;
			break;
        }
    }
}


extern u8 uart_sendbuf[][300];
extern u8 uart_recvbuf[][300];
extern u8 eth_txbuf[];
extern u8 eth_rxbuf[];

//0	:eth
//1	:485
//2	:232
void HZ_ModBus(u32 port,char ModbusLocalID)
{
    u32 rx_len,tx_len;
    if(0 == port)
    {
        rx_len = Eth_Recv();
        if(rx_len > 0)
        {
            Modbus_Do(rx_len,eth_rxbuf,&tx_len,eth_txbuf,globaldata,sysdatalen,userreadlen,ModbusLocalID);
            rx_len = 0;
            if(tx_len)
                Eth_Send(eth_txbuf,tx_len);
            tx_len = 0;
        }
    }
    else if(1 == port || 2 == port)
    {
        //n --;
        rx_len = Uart_Recv(port-1,uart_recvbuf);
        if(rx_len > 0)
        {
            Modbus_Do(rx_len,uart_recvbuf[port-1],&tx_len,uart_sendbuf[port-1],globaldata,sysdatalen,userreadlen,ModbusLocalID);
            rx_len = 0;
            if(tx_len)
                Uart_Send(port-1,uart_sendbuf[port-1],tx_len);
            tx_len = 0;
        }
    }
}



