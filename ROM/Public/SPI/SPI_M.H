#ifndef __SPI_MASTER_H__
#define __SPI_MASTER_H__

#define SET_SPI0_CK(n) (SPI0_CK_SE = n) //SPI时钟设置函数

/*******************************************************************************
* Function Name  : SPI0HostInit( unsigned char mode ) 
* Description    : SPI主机模式初始化
* Input          : unsigned char mode						 
* Output         : None
* Return         : None
*******************************************************************************/
void SPI0HostInit(unsigned char mode);

/*******************************************************************************
* Function Name  : SPI0HostWrite(unsigned char dat)
* Description    : 硬件SPI写数据，主机模式
* Input          : unsigned char dat   数据
* Output         : None
* Return         : None
*******************************************************************************/
void SPI0HostWrite(unsigned char dat);

/*******************************************************************************
* Function Name  : SPI0HostRead( )
* Description    : 硬件SPI0读数据，主机模式
* Input          : None
* Output         : None
* Return         : unsigned char ret   
*******************************************************************************/
unsigned char SPI0HostRead();

#endif