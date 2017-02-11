
#include "my_port.h"
#include "my_bsp.h"
#include "gpio.h"

#define xDot 128
#define yDot 296

unsigned char GDOControl[]={0x01,(yDot-1)%256,(yDot-1)/256,0x00}; //for 2.9inch
unsigned char softstart[]={0x0c,0xd7,0xd6,0x9d};
unsigned char Rambypass[] = {0x21,0x8f};		// Display update
unsigned char MAsequency[] = {0x22,0xf0};		// clock 
unsigned char GDVol[] = {0x03,0xea};	// Gate voltage +15V/-15V
unsigned char SDVol[] = {0x04,0x0a};	// Source voltage +15V/-15V
unsigned char VCOMVol[] = {0x2c,0xa8};	// VCOM 7c
unsigned char BOOSTERFB[] = {0xf0,0x1f};	// Source voltage +15V/-15V
unsigned char DummyLine[] = {0x3a,0x1a};	// 4 dummy line per gate
unsigned char Gatetime[] = {0x3b,0x08};	// 2us per line
unsigned char BorderWavefrom[] = {0x3c,0x33};	// Border
unsigned char RamDataEntryMode[] = {0x11,0x01};	// Ram data entry mode


unsigned char GPIO_ReadOutputDataBit(GPIO_TypeDef* GPIOx, unsigned short GPIO_Pin)
{
  unsigned char  bitstatus = 0x00;
  /* Check the parameters */
  assert_param(IS_GPIO_PIN(GPIO_Pin));
  
  if ((GPIOx->ODR & GPIO_Pin) != 0)
  {
    bitstatus = 1;
  }
  else
  {
    bitstatus = 0;
  }
  return bitstatus;
}
	

#ifdef GDE0213B1
const unsigned char LUTDefault_full[]={
0x32,	// command

0x22,
0x55,
0xAA,
0x55,
0xAA,
0x55,
0xAA,
0x11,
0x00,
0x00,
0x00,
0x00,
0x00,
0x00,
0x00,
0x00,
0x1E,
0x1E,
0x1E,
0x1E,
0x1E,
0x1E,
0x1E,
0x1E,
0x01,
0x00,
0x00,
0x00,
0x00,


};

const unsigned char LUTDefault_part[]={
0x32,	// command

0x18,
0x00,
0x00,
0x00,
0x00,
0x00,
0x00,
0x00,
0x00,
0x00,
0x00,
0x00,
0x00,
0x00,
0x00,
0x00,
0x0F,
0x01,
0x00,
0x00,
0x00,
0x00,
0x00,
0x00,
0x00,
0x00,
0x00,
0x00,
0x00,
};

#else
const unsigned char LUTDefault_part[31] = {
	0x32,	// command

/*	0x00,0x00,0x00,0x18,	
	0x18,0x18,0x18,0x00,
	0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,
	0x40,0x14,0x34,0x01,
	0x00,0x00,0x00,0x00,
	0x00,0x00,
*/
 0x10 //C221 25C partial update waveform
,0x18
,0x18
,0x08
,0x18
,0x18
,0x08
,0x00
,0x00
,0x00
,0x00
,0x00
,0x00
,0x00
,0x00
,0x00
,0x00
,0x00
,0x00
,0x00
,0x13
,0x14
,0x44
,0x12
,0x00
,0x00
,0x00
,0x00
,0x00
,0x00

	};



const unsigned char LUTDefault_full[31] = {
	0x32,	// command
/*	0x11,0x11,0x10,0x02,	// full
	0x02,0x22,0x22,0x22,
	0x22,0x22,0x51,0x51,
	0x55,0x88,0x08,0x08,
    0x88,0x88,0x00,0x00,
	0x34,0x23,0x12,0x21,
	0x24,0x28,0x22,0x21,
	0xA1,0x01
*/
 0x02 //C221 25C Full update waveform
,0x02
,0x01
,0x11
,0x12
,0x12
,0x22
,0x22
,0x66
,0x69
,0x69
,0x59
,0x58
,0x99
,0x99
,0x88
,0x00
,0x00
,0x00
,0x00
,0xF8
,0xB4
,0x13
,0x51
,0x35
,0x51
,0x51
,0x19
,0x01
,0x00
	};
#endif


void SPI_Write(unsigned char value)                                    
{                                                           
    unsigned char i;
    for(i=0; i<8; i++)   
    {
        EPD_W21_CLK_0;
        if(value & 0x80)
        	EPD_W21_MOSI_1;
        else
        	EPD_W21_MOSI_0;		
        value = (value << 1); 
        EPD_W21_CLK_1;  
    }
}

unsigned char ReadBusy(void)
{
	unsigned long i=0;  
	for(i=0;i<400;i++)
		{
			if(isEPD_W21_BUSY==EPD_W21_BUSY_LEVEL) {
			return 1;
			}
			HAL_Delay(10);
		}
	return 0;
}

void EPD_W21_WriteCMD(unsigned char command)
{
	HAL_Delay(1);
	EPD_W21_CS_0;                   
	EPD_W21_DC_0;		// command write
	SPI_Write(command);
	EPD_W21_CS_1;
}



//static void EPD_W21_WriteDATA(unsigned char command)
//{
//    EPD_W21_CS_0;                   
//	EPD_W21_DC_1;		// command write
//	SPI_Write(command);
//	EPD_W21_CS_1;
//}
	
static void EPD_W21_WriteCMD_p1(unsigned char command,unsigned char para)
{
	//while(isEPD_W21_BUSY == 1);	// wait
	ReadBusy();	

	EPD_W21_CS_0;                   
	EPD_W21_DC_0;		// command write
	SPI_Write(command);
	EPD_W21_DC_1;		// command write
	SPI_Write(para);
	EPD_W21_CS_1;
}

//static void EPD_W21_WriteCMD_p2(unsigned char command,unsigned char para1,unsigned char para2)
//{
//	while(isEPD_W21_BUSY == 1);	// wait	
//
//    EPD_W21_CS_0;                   
//	EPD_W21_DC_0;		// command write
//	SPI_Write(command);
//	EPD_W21_DC_1;		// command write
//	SPI_Write(para1);
//	SPI_Write(para2);
//	EPD_W21_CS_1;
//} 
/*********************************************
���ܣ���һ���ֽ�д���������ֵ��ʣ����ǲ�����
�����ý׶�ʹ��

*********************************************/
static void EPD_W21_Write(unsigned char *value, unsigned char datalen)
{
	unsigned char i = 0;
	unsigned char *ptemp;
	ptemp = value;
	EPD_W21_CS_0;                   	
	EPD_W21_DC_0;		// command write

	SPI_Write(*ptemp);
	ptemp++;

	EPD_W21_DC_1;		// data write
	
	for(i= 0;i<datalen-1;i++)	// sub the command
	{
		SPI_Write(*ptemp);
		ptemp++;
	}

	EPD_W21_CS_1;

}
/*********************************************
���ܣ�д��ʾbuffer
������ 	XSize  x��������  128�㷽�򣬵���Ϊ8��������
		YSize  y��������	 	   	
		Dispbuff ��ʾ���ݱ���λ�á� Ҫ�����ݵ����з�ʽ������ȷ
*********************************************/
static void EPD_W21_WriteDispRam(unsigned char XSize,unsigned int YSize,
							unsigned char *Dispbuff)
{
	int i = 0,j = 0;

	if(XSize%8 != 0)
	{
		XSize = XSize+(8-XSize%8);
	}
	XSize = XSize/8;

	//while(isEPD_W21_BUSY == 1);	//wait
	ReadBusy();		
	
    	EPD_W21_CS_0;                   
	EPD_W21_DC_0;		//command write
	SPI_Write(0x24);
	
	EPD_W21_DC_1;		//data write
	for(i=0;i<YSize;i++)
	{
		for(j=0;j<XSize;j++)
		{
			SPI_Write(*Dispbuff);
			Dispbuff++;
		}
	}
	
	EPD_W21_CS_1;
}
/*********************************************
���ܣ�  д��ʾbuffer ������ĳһ������д����ͬ����ʾ���ݡ�
������ 	XSize  x��������  128�㷽�򣬵���Ϊ8��������
		YSize  y��������	 	   	
		dispdata ��ʾ���ݡ�
*********************************************/
static void EPD_W21_WriteDispRamMono(unsigned char XSize,unsigned int YSize,
							unsigned char dispdata)
{
	int i = 0,j = 0;

	if(XSize%8 != 0)
	{
		XSize = XSize+(8-XSize%8);
	}
	XSize = XSize/8;
	//while(isEPD_W21_BUSY == 1);	// wait	
	ReadBusy();	

   	EPD_W21_CS_0;                   
	EPD_W21_DC_0;		// command write
	SPI_Write(0x24);
	
	EPD_W21_DC_1;		// data write
	for(i=0;i<YSize;i++)
	{
		for(j=0;j<XSize;j++)
		{
		 SPI_Write(dispdata);
		}
	}
	
	EPD_W21_CS_1;
}

static void EPD_W21_POWERON(void)
{
	EPD_W21_WriteCMD_p1(0x22,0xc0);
	EPD_W21_WriteCMD(0x20);
	//EPD_W21_WriteCMD(0xff);
}
//static void EPD_W21_POWEROFF(void)
//{  	EPD_W21_WriteCMD_p1(0x22,0xc3);
//	EPD_W21_WriteCMD(0x20);
////	EPD_W21_WriteCMD(0xff);
//}


static void EPD_W21_SetRamArea(unsigned char Xstart,unsigned char Xend,
						unsigned char Ystart,unsigned char Ystart1,unsigned char Yend,unsigned char Yend1)
{
	unsigned char RamAreaX[3];	// X start and end
	unsigned char RamAreaY[5]; 	// Y start and end
	RamAreaX[0] = 0x44;	// command
	RamAreaX[1] = Xstart;
	RamAreaX[2] = Xend;
	RamAreaY[0] = 0x45;	// command
	RamAreaY[1] = Ystart;
	RamAreaY[2] = Ystart1;
	RamAreaY[3] = Yend;
	RamAreaY[4] = Yend1;
	EPD_W21_Write(RamAreaX, sizeof(RamAreaX));
	EPD_W21_Write(RamAreaY, sizeof(RamAreaY));
}
static void EPD_W21_SetRamPointer(unsigned char addrX,unsigned char addrY,unsigned char addrY1)
{
	unsigned char RamPointerX[2];	// default (0,0)
	unsigned char RamPointerY[3]; 	
	RamPointerX[0] = 0x4e;
	RamPointerX[1] = addrX;
	RamPointerY[0] = 0x4f;
	RamPointerY[1] = addrY;
	RamPointerY[2] = addrY1;

	EPD_W21_Write(RamPointerX, sizeof(RamPointerX));
	EPD_W21_Write(RamPointerY, sizeof(RamPointerY));
}
static void part_display(unsigned char RAM_XST,unsigned char RAM_XEND,unsigned char RAM_YST,unsigned char RAM_YST1,unsigned char RAM_YEND,unsigned char RAM_YEND1)
{    
	EPD_W21_SetRamArea(RAM_XST,RAM_XEND,RAM_YST,RAM_YST1,RAM_YEND,RAM_YEND1);  	/*set w h*/
	EPD_W21_SetRamPointer (RAM_XST,RAM_YST,RAM_YST1);		    /*set orginal*/
}
//static void EPD_W21_UpdataDisplay(unsigned char *imgbuff,unsigned char xram,unsigned int yram)
//{
//	EPD_W21_WriteDispRam(xram, yram, imgbuff);
////	EPD_W21_Update();
//}

//=========================functions============================

//-------------------------------------------------------
//Func	: void EPD_W21_DispInit(void)
//Desc	: display parameters initinal
//Input	: none
//Output: none
//Return: none
//Author: 
//Date	: 2011/12/24
//-------------------------------------------------------
//-------------------------------------------------------
//Func	: EPD_W21_EnableChargepump
//Desc	: 
//Input	:
//Output: 
//Return: 
//Author: 
//Date	: 2011/12/24
//-------------------------------------------------------
//static void EPD_W21_EnableChargepump(void)
//{
//	EPD_W21_WriteCMD_p1(0xf0,0x8f);
//	EPD_W21_WriteCMD_p1(0x22,0xc0);
//	EPD_W21_WriteCMD(0x20);
//	EPD_W21_WriteCMD(0xff);
//}

//-------------------------------------------------------
//Func	: EPD_W21_DisableChargepump
//Desc	: 
//Input	:
//Output: 
//Return: 
//Author: 
//Date	: 2011/12/24
//-------------------------------------------------------
//static void EPD_W21_DisableChargepump(void)
//{
//	EPD_W21_WriteCMD_p1(0x22,0xf0);
//	EPD_W21_WriteCMD(0x20);
//	EPD_W21_WriteCMD(0xff);
//}
//-------------------------------------------------------
//Func	: EPD_W21_Update
//Desc	: 
//Input	:
//Output: 
//Return: 
//Author: 
//Date	: 2011/12/24
//-------------------------------------------------------
static void EPD_W21_Update(void)
{
	EPD_W21_WriteCMD_p1(0x22,0xc7);
	EPD_W21_WriteCMD(0x20);
	EPD_W21_WriteCMD(0xff);
}

static void EPD_W21_Update_Part(void)
{
	EPD_W21_WriteCMD_p1(0x22,0x04);
	//EPD_W21_WriteCMD_p1(0x22,0x08);
	EPD_W21_WriteCMD(0x20);
	EPD_W21_WriteCMD(0xff);
}
//-------------------------------------------------------
//Func	: EPD_W21_WirteLUT(unsigned char *LUTvalue)
//Desc	: write the waveform to the dirver's ram 
//Input	: *LUTvalue, the wavefrom tabe address
//Output: none
//Return: none
//Author: 
//Date	: 2011/12/24
//-------------------------------------------------------
static void EPD_W21_WirteLUT(unsigned char *LUTvalue,unsigned char Size)
{	
	EPD_W21_Write(LUTvalue, Size);
}


/**********************************************************
������ Label  
       =1 ��ʾ DisBuffer�е�����
	   =0 ��ʾ DisBuffer�е�һ���ֽڵ�����
***********************************************************/

void EPD_Dis_Full(unsigned char *DisBuffer,unsigned char Label)
{
	EPD_W21_SetRamPointer(0x00,(yDot-1)%256,(yDot-1)/256);	// set ram
	if(Label==0)
	{
		EPD_W21_WriteDispRamMono(xDot, yDot, 0xff);	// white	
	}
	else
	{
		EPD_W21_WriteDispRam(xDot, yDot, (unsigned char *)DisBuffer);	// white
	}	
	EPD_W21_Update();	
	
}

/**********************************************************
������ 
		xStart
			x����ʼ����
		xEnd
			x�����������
		yStart
			y����ʼ����
		yEnd
			y�����������
		DisBuffer
			��ʾbuffer
		Label  
       		=1 ��ʾ DisBuffer�е�����
	   		=0 ��ʾ DisBuffer�е�һ���ֽڵ�����
	   
***********************************************************/

void EPD_Dis_Part(unsigned char xStart,unsigned char xEnd,unsigned long yStart,unsigned long yEnd,unsigned char *DisBuffer,unsigned char Label)
{
	if(Label==0)
	{
		part_display(xStart/8,xEnd/8,yEnd%256,yEnd/256,yStart%256,yStart/256);
		EPD_W21_WriteDispRamMono(xEnd-xStart, yEnd-yStart+1, DisBuffer[0]);	// black
 		EPD_W21_Update_Part();
		HAL_Delay(320);
		part_display(xStart/8,xEnd/8,yEnd%256,yEnd/256,yStart%256,yStart/256);	
		EPD_W21_WriteDispRamMono(xEnd-xStart, yEnd-yStart+1, DisBuffer[0]);	// black
		//HAL_Delay(300);	
	}
	else
	{
		part_display(xStart/8,xEnd/8,yEnd%256,yEnd/256,yStart%256,yStart/256);	// set ram	
		EPD_W21_WriteDispRam(xEnd-xStart, yEnd-yStart+1,DisBuffer);
		EPD_W21_Update_Part();
		HAL_Delay(320);
		part_display(xStart/8,xEnd/8,yEnd%256,yEnd/256,yStart%256,yStart/256);
		EPD_W21_WriteDispRam(xEnd-xStart, yEnd-yStart+1,DisBuffer);
		//HAL_Delay(300);
	}
}
/***********************************************************
						end file
***********************************************************/



static void EPD_W21_DispInit(void)
{
//	EPD_W21_Write(softstart, sizeof(softstart));	// X decrease, Y decrease
//
//	EPD_W21_Write(GDOControl, sizeof(GDOControl));	// Pannel configuration, Gate selection
//
//	EPD_W21_Write(Rambypass, sizeof(Rambypass));	// Rambypass
//
//	EPD_W21_Write(MAsequency, sizeof(MAsequency));	// MAsequency
//
//	EPD_W21_Write(GDVol, sizeof(GDVol));		    // GDVol setting
//
//	EPD_W21_Write(SDVol, sizeof(SDVol));		    // SDVol setting
//
//	EPD_W21_Write(VCOMVol, sizeof(VCOMVol));		// VCOM setting
//
//	EPD_W21_Write(DummyLine, sizeof(DummyLine));	// dummy line per gate
//	
//	EPD_W21_Write(Gatetime, sizeof(Gatetime));		// Gage time setting
//	
//	EPD_W21_Write(RamDataEntryMode, sizeof(RamDataEntryMode));	// X increase, Y decrease

	EPD_W21_Write(GDOControl, sizeof(GDOControl));	// Pannel configuration, Gate selection	
	EPD_W21_Write(softstart, sizeof(softstart));	// X decrease, Y decrease
	//EPD_W21_Write(Rambypass, sizeof(Rambypass));	// RAM bypass setting
	//	EPD_W21_Write(MAsequency, sizeof(MAsequency));	// clock enable
	//EPD_W21_Write(GDVol, sizeof(GDVol));			// Gate voltage setting
	//EPD_W21_Write(SDVol, sizeof(SDVol));			// Source voltage setting
	EPD_W21_Write(VCOMVol, sizeof(VCOMVol));		// VCOM setting
	//EPD_W21_Write(BOOSTERFB, sizeof(BOOSTERFB));	// Hi-V feedback selection
	EPD_W21_Write(DummyLine, sizeof(DummyLine));	// dummy line per gate
	EPD_W21_Write(Gatetime, sizeof(Gatetime));		// Gage time setting
	EPD_W21_Write(RamDataEntryMode, sizeof(RamDataEntryMode));	// X increase, Y decrease	
	EPD_W21_SetRamArea(0x00,(xDot-1)/8,(yDot-1)%256,(yDot-1)/256,0x00,0x00);	// X-source area,Y-gage area
	EPD_W21_SetRamPointer(0x00,(yDot-1)%256,(yDot-1)/256);	// set ram

}


void EPD_W21_Init(void)
{
	EPD_W21_BS_0;		// 4 wire spi mode selected

	EPD_W21_RST_0;		// Module reset
	HAL_Delay(100);
	EPD_W21_RST_1;
	HAL_Delay(100); 

	EPD_W21_DispInit();		// pannel configure
}

void EPD_init_Full(void)
	{		
		//HAL_Delay(4900);
		//HAL_Delay(100000);
		EPD_W21_Init();			// display
		EPD_W21_WirteLUT((unsigned char *)LUTDefault_full,sizeof(LUTDefault_full));

		EPD_W21_POWERON();
		//HAL_Delay(100000); 		
	}


void EPD_init_Part(void)
{		
	EPD_W21_Init();			// display

	EPD_W21_WirteLUT((unsigned char *)LUTDefault_part,sizeof(LUTDefault_part));
	EPD_W21_POWERON();
	//	//�����ɫ
	//	//EPD_W21_SetRamArea(0x00,0x0f,0x27,0x01,0x00,0x00);	  
	//	//EPD_W21_SetRamPointer(0x00,0x27,0x01);	// set ram
	//	part_display(0,xDot/8,yDot%256,yDot/256,0,0);
	//	EPD_W21_WriteDispRamMono(xDot, yDot, 0xff);	// white
	//	EPD_W21_Update_Part();
	//	HAL_Delay(500);
	//	
	//	
	//	part_display(0,xDot/8,yDot%256,yDot/256,0,0);
	//	EPD_W21_WriteDispRamMono(xDot, yDot, 0xff);	// white	

	//HAL_Delay(2500);

}
