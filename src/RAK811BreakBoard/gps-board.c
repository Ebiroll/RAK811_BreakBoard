/*
 / _____)             _              | |
( (____  _____ ____ _| |_ _____  ____| |__
 \____ \| ___ |    (_   _) ___ |/ ___)  _ \
 _____) ) ____| | | || |_| ____( (___| | | |
(______/|_____)_|_|_| \__)_____)\____)_| |_|
    (C)2013 Semtech

Description: Generic low level driver for GPS receiver

License: Revised BSD License, see LICENSE.TXT file include in the project

Maintainer: Miguel Luis and Gregory Cristian
*/
#include "board.h"

/*!
 * FIFO buffers size
 */
//#define FIFO_TX_SIZE                                128
#define FIFO_RX_SIZE                                512

//uint8_t TxBuffer[FIFO_TX_SIZE];
/*
void GpsEnterPowerSaveModeFrame(uint8_t * buffer, uint16_t * size);

void GpsGNSSGpsOnlyFrame( uint8_t * buffer, uint16_t * size );
*/

/*!
 * \brief Buffer holding the  raw data received from the gps
 */
uint8_t NmeaString[512];

/*!
 * \brief Maximum number of data byte that we will accept from the GPS
 */
uint8_t NmeaStringSize = 0;

bool GpsMcuEnable = false;
Gpio_t GpsPowerEn;
Gpio_t GpsPps;

PpsTrigger_t PpsTrigger;
extern uint8_t GPS_RxBuffer[];

void GpsMcuOnPpsSignal( void )
{
#ifdef GPS_PPS
    bool parseData = false;

    GpsPpsHandler( &parseData );

    if( parseData == true )
    {
        UartMcuInit( &GpsUart, GPS_UART, GPS_UART_TX, GPS_UART_RX );
        UartMcuConfig( &GpsUart, RX_ONLY, 9600, UART_8_BIT, UART_1_STOP_BIT, NO_PARITY, NO_FLOW_CTRL );
    }
#endif
}

void GpsMcuInvertPpsTrigger( void )
{
#ifdef GPS_PPS
    if( PpsTrigger == PpsTriggerIsRising )
    {
        PpsTrigger = PpsTriggerIsFalling;
        GpioSetInterrupt( &GpsPps, IRQ_FALLING_EDGE, IRQ_VERY_LOW_PRIORITY, &GpsMcuOnPpsSignal );
    }
    else
    {
        PpsTrigger = PpsTriggerIsRising;
        GpioSetInterrupt( &GpsPps, IRQ_RISING_EDGE, IRQ_VERY_LOW_PRIORITY, &GpsMcuOnPpsSignal );
    }
#endif
}

void PrintFrame(uint8_t * buffer, uint32_t size){
	int n;
	
	printf("\r\n");
	for( n = 0; n < size; n++ ){
		printf("%02X ", buffer[n]);
		if( n % 4 == 3 ){
			printf("  ");
		}

		if( n % 8 == 7 ){
			printf("\r\n");
		}
	}
	printf("\r\n");

	return;
}

/*
void GpsPowerSave(void){
	uint8_t buffer[128];
	uint16_t size;

	size = 0;
	GpsEnterPowerSaveModeFrame(buffer, &size);
	PrintFrame(buffer, size);
	UartPutBuffer(&GpsUart, buffer, size);
	
	size = 0;
	GpsGNSSGpsOnlyFrame(buffer, &size);
	PrintFrame(buffer, size);
	UartPutBuffer(&GpsUart, buffer, size);
	
	return;
	
	
}
*/
void GpsMcuInit( void )
{
    NmeaStringSize = 0;
    PpsTrigger = PpsTriggerIsFalling;

    GpioInit( &GpsPowerEn, GPS_POWER_ON_PIN, PIN_OUTPUT, PIN_PUSH_PULL, PIN_NO_PULL, 0 );

    
#ifdef GPS_PPS
    GpioInit( &GpsPps, GPS_PPS_PIN, PIN_INPUT, PIN_PUSH_PULL, PIN_NO_PULL, 0 );
    GpioSetInterrupt( &GpsPps, IRQ_FALLING_EDGE, IRQ_VERY_LOW_PRIORITY, &GpsMcuOnPpsSignal );

#endif
    FifoInit( &GpsUart.FifoRx, GPS_RxBuffer, FIFO_RX_SIZE );
    GpsUart.IrqNotify = GpsMcuIrqNotify;

    //DelayMs(200);
  	GpsMcuStart( );
  	

#ifndef GPS_PPS     
    UartMcuInit( &GpsUart, GPS_UART, GPS_UART_TX, GPS_UART_RX );
    UartMcuConfig( &GpsUart, RX_ONLY, 9600, UART_8_BIT, UART_1_STOP_BIT, NO_PARITY, NO_FLOW_CTRL );
#endif
    
}

void GpsMcuStart( void )
{
    GpioWrite( &GpsPowerEn, 1 );    // power up the GPS
    GpsMcuEnable = true;
}

void GpsMcuStop( void )
{
    GpioWrite( &GpsPowerEn, 0 );    // power down the GPS
    GpsMcuEnable = false;
}

void GpsMcuProcess( void )
{

}

bool GpsMcuIsEnable(void)
{
	return GpsMcuEnable;
}

/*
static enum eGPS_PROTOCAL {
	GPS_PROTO_NMEA = 0,
	GPS_PROTO_UBX
} gps_protocal = GPS_PROTO_UBX;

static bool gps_in_recv = false;

struct s_gps_ubx_frame_head {
	uint8_t		sync_char_1;
	uint8_t 	sync_char_2;
	uint8_t		Class;
	uint8_t		id;
	uint8_t     Length_1;
	uint8_t     Length_2;
};

struct s_gps_ubx_frame {
	struct s_gps_ubx_frame_head head;
	uint16_t length;
	uint8_t payload[256];
	uint16_t payload_length;
	uint8_t CK_A;
	uint8_t CK_B;
} gps_ubx_frame = {0};

static enum GPS_UBX_RECV_STAT {
	UBX_SYNC_1 = 0,
	UBX_SYNC_2,
	UBX_CLASS,
	UBX_ID,
	UBX_LENGTH_1,
	UBX_LENGTH_2,
	UBX_PAYLOAD,
	UBX_CK_A,
	UBX_CK_B
} gps_ubx_stat = UBX_SYNC_1;

#define GPS_RXM_POWER_SAVE_MODE ( 0x01 << 1 )
#define GPS_RXM_CONTINUOUS_MODE ( 0x01 << 4 )

void GpsUbxFrameCheckSum(uint8_t * buffer, uint16_t size, uint8_t * CK_A, uint8_t * CK_B  ){
	uint16_t n = 0;

	for( n = 0; n < size; n++){
		*CK_A = *CK_A + buffer[n];
		*CK_B = *CK_B + *CK_A;
	}

	return;
}

struct GpsGnssCfgBlock {
		uint8_t gnssId;
		uint8_t resTrkCh;
		uint8_t maxTrkCh;
		uint8_t reserved1;
		uint8_t bitflag_0;
		uint8_t bitflag_1;
		uint8_t bitflag_2;
		uint8_t bitflag_3;
};


void GpsGNSSGpsOnlyFrame( uint8_t * buffer, uint16_t * size ){
	
	struct s_gps_ubx_frame frame = {{0}};
	struct GpsGnssCfgBlock * GPS, *SBAS, *QZSS, *GLONASS;
	
	uint8_t * header = 0;
	uint8_t CK_A = 0, CK_B = 0;
	uint16_t i = 0;
	uint16_t n = 0;
	
	frame.head.sync_char_1 = 0xB5;
	frame.head.sync_char_2 = 0x62;

	frame.head.Class = 0x06;
	frame.head.id = 0x3E;
	
	n = 0;
	frame.payload[n++] = 0;
	frame.payload[n++] = 0;
	frame.payload[n++] = 22;
	frame.payload[n++] = 4;

	GPS = (struct GpsGnssCfgBlock *)&(frame.payload[n]);
	n += sizeof(struct GpsGnssCfgBlock);
	
	GPS->gnssId = 0x00;
	GPS->resTrkCh = 0x08;
	GPS->maxTrkCh = 0xFF;
	GPS->reserved1 = 0x00;
	
	GPS->bitflag_0 = 0x00;
	GPS->bitflag_1 = 0X00;
	GPS->bitflag_2 = 0x00;
	GPS->bitflag_3 = 0x01;

	SBAS = (struct GpsGnssCfgBlock *)&(frame.payload[n]);
	n += sizeof(struct GpsGnssCfgBlock);

	SBAS->gnssId = 0x01;
	SBAS->resTrkCh = 0x00;
	SBAS->maxTrkCh = 0x03;
	SBAS->reserved1 = 0x00;

	SBAS->bitflag_0 = 0x00;
	SBAS->bitflag_1 = 0x00;
	SBAS->bitflag_2 = 0x00;
	SBAS->bitflag_3 = 0x01;

	QZSS = (struct GpsGnssCfgBlock *)&(frame.payload[n]);
	n += sizeof(struct GpsGnssCfgBlock);

	QZSS->gnssId = 0x05;
	QZSS->resTrkCh = 0x00;
	QZSS->maxTrkCh = 0x03;
	QZSS->reserved1 = 0x00;
	QZSS->bitflag_0 = 0x00;
	QZSS->bitflag_1 = 0x00;
	QZSS->bitflag_2 = 0x00;
	QZSS->bitflag_3 = 0x01;

	GLONASS = (struct GpsGnssCfgBlock *)&(frame.payload[n]);
	n += sizeof(struct GpsGnssCfgBlock);

	GLONASS->gnssId = 0x06;
	GLONASS->resTrkCh = 0x04;
	GLONASS->maxTrkCh = 0xFF;
	GLONASS->reserved1 = 0x00;
	GLONASS->bitflag_0 = 0x00;
	GLONASS->bitflag_1 = 0x00;
	GLONASS->bitflag_2 = 0x00;
	GLONASS->bitflag_3 = 0x00;

	(uint16_t)(frame.head.Length_1) = sizeof(frame.head) + n + 2;

	header = (uint8_t *)&(frame.head);
	for( i = 0 ; i < sizeof(frame.head); i++){
		buffer[(*size)++] = header[i];
	}

	for( i = 0; i < n; i++ ){
		buffer[(*size)++] = frame.payload[i];
	}

	GpsUbxFrameCheckSum(buffer, *size, &CK_A, &CK_B);
	buffer[(*size)++] = CK_A;
	buffer[(*size)++] = CK_B;

	return;
}

void GpsEnterPowerSaveModeFrame(uint8_t * buffer, uint16_t * size){
	struct s_gps_ubx_frame frame = {{0}};
	uint8_t * header = 0;
	uint8_t CK_A = 0, CK_B = 0;
	uint16_t i;
	
	frame.head.sync_char_1 = 0xB5;
	frame.head.sync_char_2 = 0x62;

	frame.head.Class = 0x06;
	frame.head.id = 0x11;

	(uint16_t)(frame.head.Length_1) = 0x02;

	frame.payload[0] = 0x08;
	frame.payload[1] = ( 0x00 | GPS_RXM_POWER_SAVE_MODE );

	header = (uint8_t *)&(frame.head);
	for( i = 0; i < sizeof(frame.head); i++ ){
		buffer[i] = header[i];
	}

	buffer[i++] = frame.payload[0];
	buffer[i++] = frame.payload[1];

	GpsUbxFrameCheckSum( buffer, i, &CK_A, &CK_B);
	buffer[i++] = CK_A;
	buffer[i++] = CK_B;

	*size = i;

	return;
	
}
*/
void GpsMcuIrqNotify( UartNotifyId_t id )
{
    uint8_t data;
   
    if( id == UART_NOTIFY_RX )
    { 	
        if( UartMcuGetChar( &GpsUart, &data ) == 0 )
        {
            if( ( data == '$' ) || ( NmeaStringSize >= 1024 ) )
            {
                NmeaStringSize = 0;
            }

            NmeaString[NmeaStringSize++] = ( int8_t )data;

            if( data == '\n' )
            {
                NmeaString[NmeaStringSize++] = '\0';
							  //printf("%d\n",NmeaStringSize);
                GpsParseGpsData( ( int8_t* )NmeaString, NmeaStringSize );
#ifdef GPS_PPS   
                UartMcuDeInit( &GpsUart );
#endif
                BlockLowPowerDuringTask ( false );
            }
        }
    }
}
