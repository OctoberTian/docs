/* ---------------------------------- File Description --------------------------------------------
���ļ����ܡ���    
������ʱ�䡿��2010��6��3��������14:22:30
���ļ��汾����V1.0
�����������������ļ�����Ϊ��ҵ��,��ͨ��email��������ϵ: eric540@163.com
------------------------------------------------------------------------------------------ */
#ifndef SPI_H
#define SPI_H

#include "stm32f10x.h"	 
#include "serial_debug.h"

#define SPI_CS_Low()     GPIO_ResetBits(GPIOA, GPIO_Pin_4)
#define SPI_CS_High()    GPIO_SetBits(GPIOA, GPIO_Pin_4)

#define DUMMY_BYTE  0XFF

void SPI1_Configuration(void);
u8 SPI_Write(u8 byte);
u8 SPI_Read(void);

#endif
