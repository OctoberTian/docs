/* ---------------------------------- File Description --------------------------------------------
���ļ����ܡ���    
������ʱ�䡿��2010��6��3��������14:22:18
���ļ��汾����V1.0
�����������������ļ�����Ϊ��ҵ��,��ͨ��email��������ϵ: eric540@163.com
------------------------------------------------------------------------------------------ */
#include "spi.h"

/*---- S   P   I 1 _   C O N F I G U R A T I O N ----
�����ܡ���
����������****
�����ء���****
��˵������pclk2=72M  spi_clk=pclk2/SPI_InitStructure.SPI_BaudRatePrescaler 
--------------����:��ҵ��   2010��5��7��������09:27:31--------------------------------*/
void SPI1_Configuration(void)
{
    SPI_InitTypeDef  SPI_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;
    
    /* Configure SPI1 pins: NSS, SCK, MISO and MOSI */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    /* Configure PA.4 as Output push-pull, used as Chip select */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    /* Deselect the FLASH: Chip Select high */
    SPI_CS_High();

    /* SPI1 configuration */ 
    SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
    SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
    SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
    SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
    SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
    SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;
    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
    SPI_InitStructure.SPI_CRCPolynomial = 7;
    SPI_Init(SPI1, &SPI_InitStructure);

    /* Enable SPI1 NSS output for master mode */
    SPI_SSOutputCmd(SPI1, DISABLE);    
    /* Enable SPI1  */
    SPI_Cmd(SPI1, ENABLE); 
}

/*------------------- S P I _ W R I T E --------------------
�����ܡ�����SPI��дһ���ֽ�
����������Ҫд��1�ֽڵ�����
�����ء����ػ��ֽ� 
��˵������****
--------------����:��ҵ��   2009��3��20��14:48:22-------------------------*/
u8 SPI_Write(u8 byte)
{
    //�ȴ����ͻ����
    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET); 
    SPI_I2S_SendData(SPI1, byte);  
    //�ȴ����ջ�����յ�����
    while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET);
    /* Return the byte read from the SPI bus */
    return SPI_I2S_ReceiveData(SPI1);
}

/*------------------- S P I _ R E A D --------------------
�����ܡ�����SPI��һ�ֽ�����
������������
�����ء������ص�����
��˵������****
--------------����:��ҵ��   2009��3��20��14:49:25-------------------------*/
u8 SPI_Read(void)
{
    return (SPI_Write(DUMMY_BYTE));
}


