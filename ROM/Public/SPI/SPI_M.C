
/********************************** (C) COPYRIGHT *******************************
* File Name          : SPI.C
* Author             : WCH
* Version            : V1.0
* Date               : 2017/07/05
* Description        : CH554 SPI主、从模式接口函数
注：片选有效时，从机会自动加载SPI0_S_PRE的预置值到发送移位缓冲区，所以最好可以在片选
有效前向SPI0_S_PRE寄存器写入预发值，或者在主机端丢弃首个接收字节，发送时注意主机会先 
取走SPI0_S_PRE里面的值产生一个S0_IF_BYTE中断。
如果片选从无效到有效，从机首先进行发送的话，最好把输出的首字节放到SPI0_S_PRE寄存器中；
如果已经处于片选有效的话，数据数据使用SPI0_DATA就可以
*******************************************************************************/

#include "SPI_M.H"
#include "PinMap.H"

#ifdef __CH552_H__

#pragma NOAREGS

/*******************************************************************************
* Function Name  : SPI0HostInit( unsigned char mode ) 
* Description    : SPI主机模式初始化
* Input          : unsigned char mode						 
* Output         : None
* Return         : None
*******************************************************************************/
void SPI0HostInit(unsigned char mode)
{
  SPI0_SETUP = 0; //Host模式,高位在前
  if (mode == 0)
  {
    SPI0_CTRL = 0x60; //模式0
  }
  else if (mode == 3)
  {
    SPI0_CTRL = 0x68; //模式3
  }
  P1_MOD_OC |= 0xFF;
  P1_DIR_PU |= 0xFF; //SCS,MOSI,SCK设推挽输出
  SET_SPI0_CK(4);    //4分频
}

/*******************************************************************************
* Function Name  : SPI0HostWrite(unsigned char dat)
* Description    : CH559硬件SPI写数据
* Input          : unsigned char dat 数据
* Output         : None
* Return         : None
*******************************************************************************/
void SPI0HostWrite(unsigned char dat)
{
  SPI0_DATA = dat;
  while (S0_FREE == 0)
    ; //等待传输完成
  //如果bS0_DATA_DIR为1，此处可以直接读取一个字节的数据用于快速读写
}

/*******************************************************************************
* Function Name  : CH554SPIHostRead( )
* Description    : CH554硬件SPI0读数据，主机模式
* Input          : None
* Output         : None
* Return         : unsigned char ret   
*******************************************************************************/
unsigned char SPI0HostRead()
{
  SPI0_DATA = 0xff;
  while (S0_FREE == 0)
    ;
  return SPI0_DATA;
}

#endif

#ifdef __CH558_H__

#pragma NOAREGS

/*******************************************************************************
* Function Name  : SPI0HostWrite(unsigned char dat)
* Description    : CH559硬件SPI写数据
* Input          : unsigned char dat   数据
* Output         : None
* Return         : None
*******************************************************************************/
void SPI0HostWrite(unsigned char dat)
{
  SPI0_DATA = dat;
  while (S0_FREE == 0)
    ; //等待传输完成
  //如果bS0_DATA_DIR为1，此处可以直接读取一个字节的数据用于快速读写
}

/*******************************************************************************
* Function Name  : SPI0HostInit(unsigned char mode)
* Description    : CH559SPI0初始化
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SPI0HostInit(unsigned char mode)
{
  PORT_CFG &= ~bP1_OC;
  P1_DIR |= (bSCK | bMOSI | bSCS);
  P1_IE |= bMISO; //引脚配置

  SPI0_SETUP &= ~(bS0_MODE_SLV | bS0_BIT_ORDER); //设置为主机模式，字节顺序为大端模式
  // SPI0_CTRL |=  bS0_MOSI_OE  | bS0_SCK_OE ;                                  //MISO输出使能，SCK输出使能
  // SPI0_CTRL &= ~(bS0_MST_CLK | bS0_2_WIRE);
  // SPI0_CTRL &=  ~(bS0_DATA_DIR);                                             //主机写，默认不启动写传输，如果使能bS0_DATA_DIR，
  //那么发送数据后自动产生一个字节的时钟，用于快速数据收发

  if (mode == 0)
  {
    SPI0_CTRL = 0x60; //模式0
  }
  else if (mode == 3)
  {
    SPI0_CTRL = 0x68; //模式3
  }

  SET_SPI0_CK(8);            //8分频
  SPI0_CTRL &= ~bS0_CLR_ALL; //清空SPI0的FIFO,默认是1，必须置零才能发送数据
}

/*******************************************************************************
* Function Name  : SPI0HostRead( )
* Description    : CH559硬件SPI0读数据，主机模式
* Input          : None
* Output         : None
* Return         : unsigned char ret   
*******************************************************************************/
unsigned char SPI0HostRead()
{
  SPI0_DATA = 0xff;
  while (S0_FREE == 0)
    ;
  return SPI0_DATA;
}
#endif