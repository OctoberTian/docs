/* ---------------------------------- File Description --------------------------------------------
���ļ����ܡ������ļ�Ϊ MAX25L1606E  SpiFLASHоƬ�����ļ�
              MAX25L1606E �洢��Ϊ16M bit  ��Ϊ512��������ÿ��������4K byte  16������Ϊһ��block �ܹ���32��block
              ÿpage Ϊ256���ֽ�
              �������ٶȸߴ�86MHz
������ʱ�䡿��2012��10��22������һ13:20:40
���ļ��汾����V1.0
�����������������ļ�����Ϊ��ҵ��,��ͨ��email��������ϵ: eric540@163.com
------------------------------------------------------------------------------------------ */
#include "stm32f10x.h"
#include "mx25L1606E.h"
#include "serial_debug.h"


void SpiFlash_Init(void)
{
    flashInfoTypedef flashInfo;   
    //WP
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
    FLASH_WP_High();
    SPI1_Configuration();
    
    FlashGet_ElectronicInfo(&flashInfo);
//	    printf("ManufacturerID=%02X\r\n",flashInfo.ManufacturerID);
//	    printf("DeviceID=%02X\r\n",flashInfo.DeviceID[0]);
    if((flashInfo.ManufacturerID==0xc2)&&(flashInfo.DeviceID[0]==0x14))
    {
        printf("\tFlash Info: MXIC(Macronix International Co.,Ltd\r\n");
    }
    FlashGet_Info(&flashInfo);
//	    printf("ManufacturerID=%02X\r\n",flashInfo.ManufacturerID);
//	    printf("DeviceID=%02X%02X\r\n",flashInfo.DeviceID[0],flashInfo.DeviceID[1]);
    if((flashInfo.DeviceID[0]==0x20)&&(flashInfo.DeviceID[1]==0x15))
    {
        printf("\tFlashType:MX25L1606E  FlashSize:16Mbit\r\n\tTotal 32 Blocks  16 Sectors/Block  4KB/Sector\r\n");
    }

#if 0    
    FlashEnter_DeepPowerDown();
    tmp=FlashReleaseFrom_DeepPowerDownE();
    printf("ElectronicSignature=%02x\r\n",tmp);  //��ӡ���Ϊ: 0x14
#endif    
}

void FlashWait_Busy(void)
{
    u8 flashSta;
    u32 retry=0;
    FLASH_CS_Low();
    SPI_Write(RDSD_CMD);
    do{
        
        flashSta=SPI_Read();
        if(retry++>FLASH_BUSY_TIMEOUT)
            break;
    }while(flashSta&0x80);
    //printf("%d\r\n",retry);
    SPI_CS_High();    
}
void FlashGet_Info(flashInfoTypedef * pFlashInfo)
{
//	    FlashWait_Busy();
    FLASH_CS_Low();
    SPI_Write(RDID_CMD);
    pFlashInfo->ManufacturerID=SPI_Read();
    pFlashInfo->DeviceID[0]=SPI_Read();
    pFlashInfo->DeviceID[1]=SPI_Read();
    SPI_CS_High();    
}
void FlashGet_ElectronicInfo(flashInfoTypedef * pFlashInfo)
{
//	    FlashWait_Busy();
    FLASH_CS_Low();
    SPI_Write(REMS_CMD);
    SPI_Write(DUMMY_BYTE);
    SPI_Write(DUMMY_BYTE);
    SPI_Write(0x00);  //manufacturer's ID first
    pFlashInfo->ManufacturerID=SPI_Read();
    pFlashInfo->DeviceID[0]=SPI_Read();   
    SPI_CS_High();    
}
void FlashEnter_DeepPowerDown(void)
{
    FLASH_CS_Low();
    SPI_Write(DP_CMD);   
    SPI_CS_High();    
}
void FlashReleaseFrom_DeepPowerDown(void)
{
    FLASH_CS_Low();
    SPI_Write(RDP_CMD);   
    SPI_CS_High();    
}
u8 FlashReleaseFrom_DeepPowerDownE(void)
{   u8 ElectronicSignature;
    FLASH_CS_Low();
    SPI_Write(RES_CMD);
    SPI_Write(DUMMY_BYTE);
    SPI_Write(DUMMY_BYTE);
    SPI_Write(DUMMY_BYTE);
    ElectronicSignature=SPI_Read();
    SPI_CS_High();    
    return ElectronicSignature;
}
void FlashWriteEnable(void)
{
    FLASH_CS_Low();
    SPI_Write(WREN_CMD);   
    SPI_CS_High();    
}
void FlashWriteDisable(void)
{
    FLASH_CS_Low();
    SPI_Write(WRDI_CMD);   
    SPI_CS_High();    
}
void FlashWrite_StaRegister(u8 flashSta)
{
    FLASH_CS_Low();
    SPI_Write(WRSR_CMD); 
    SPI_Write(flashSta); 
    SPI_CS_High();    
}
/*---- F L A S H   P A G E _   W R I T E ----
�����ܡ���****
����������pageAddr:ҳ��ַ���ں������ٳ���ҳ��С����ΪҪ�����24bit��ַ��  ҳ��Χ(0~8191) 
          *ptr:     ָ��Ҫд�����ݵ���ַ��
          pageSize: ҳ�Ĵ�С��һ���� FLASH_PAGE_SIZE 
�����ء���****
��˵���������ڴ�flash ����������ĵ�ַλΪ24bit �����8λ��Ϊȫ0ʱ��������ҳ��ַ�ķ�Χ���ݻḲ��ҳ��ʼ��Χ����
          ���С��ҳ��Χ����ҳ����ԭ���洢�����ݲ���ı䡣
--------------����:��ҵ��   2012��10��23�����ڶ�13:52:53--------------------------------*/
void FlashPage_Write(u16 pageAddr,u8 *ptr,u32 pageSize)
{
    u32 addr,i;
//	    addr=pageAddr*FLASH_PAGE_SIZE;
    addr=((u32)pageSize)<<8;
    FlashWait_Busy();
    FlashWriteEnable();
    FLASH_CS_Low();
    SPI_Write(PP_CMD); 
    SPI_Write(addr>>16);
    SPI_Write(addr>>8);
    SPI_Write(addr); 
    for(i=0;i<pageSize;i++)
        SPI_Write(*ptr++); 
    SPI_CS_High();    
}
void FlashChip_Erase(void)
{
    FlashWait_Busy();
    FlashWriteEnable();
    FlashWriteEnable();
    FLASH_CS_Low();
    SPI_Write(CE_CMD);     
    SPI_CS_High();    
}
/*---- F L A S H   B L O C K _   E R A S E ----
�����ܡ���****
����������block: 0~31
�����ء���****
��˵������****
--------------����:��ҵ��   2012��10��23�����ڶ�14:43:05--------------------------------*/
void FlashBlock_Erase(u8 blockAddr)
{
    u32 addr;//,i;
//	    addr=blockAddr*16*4096;  //ÿ��block��16��������ÿ��������4KByte;
    addr=((u32)blockAddr)<<16;
    FlashWait_Busy();
    FlashWriteEnable();
    FLASH_CS_Low();
    SPI_Write(BE_CMD); 
    SPI_Write(addr>>16);
    SPI_Write(addr>>8);
    SPI_Write(addr); 
    SPI_CS_High();    
}
/*---- F L A S H   S E C T O R _   E R A S E ----
�����ܡ���****
����������sectorAddr: 0~511
�����ء���****
��˵������****
--------------����:��ҵ��   2012��10��23�����ڶ�14:45:28--------------------------------*/
void FlashSector_Erase(u8 sectorAddr)
{
    u32 addr;//,i;
    addr=((u32)sectorAddr)<<12;
    FlashWait_Busy();
    FlashWriteEnable();
    FLASH_CS_Low();
    SPI_Write(SE_CMD); 
    SPI_Write(addr>>16);
    SPI_Write(addr>>8);
    SPI_Write(addr); 
    SPI_CS_High();    
}
void FlashRead(u32 addr,u8 *ptr,u32 len)
{
    u32 addr_tmp,i;
    addr_tmp=addr&0x001fffff;
    FlashWait_Busy();    
    FLASH_CS_Low();
    SPI_Write(READ_CMD); 
    SPI_Write(addr_tmp>>16);
    SPI_Write(addr_tmp>>8);
    SPI_Write(addr_tmp); 
    for(i=0;i<len;i++)
        *ptr++=SPI_Read(); 
    SPI_CS_High();    
}
/*---- F L A S H   P A G E _   R E A D ----
�����ܡ���****
����������pageAddr:ҳ��ַ���ں������ٳ���ҳ��С����ΪҪ�����24bit��ַ��  ҳ��Χ(0~8191) 
�����ء���****
��˵������****
--------------����:��ҵ��   2012��10��23�����ڶ�15:11:50--------------------------------*/
void FlashPage_Read(u16 pageAddr,u8 *ptr,u32 pageSize)
{
    u32 addr,i;
//	    addr=pageAddr*FLASH_PAGE_SIZE;
    addr=((u32)pageSize)<<8;
    FlashWait_Busy();    
    FLASH_CS_Low();
    SPI_Write(READ_CMD); 
    SPI_Write(addr>>16);
    SPI_Write(addr>>8);
    SPI_Write(addr); 
    for(i=0;i<pageSize;i++)
        *ptr++=SPI_Read(); 
    SPI_CS_High();    
}
void FlashFastRead(u32 addr,u8 *ptr,u32 len)
{
    u32 addr_tmp,i;
    addr_tmp=addr&0x001fffff;
    FlashWait_Busy();    
    FLASH_CS_Low();
    SPI_Write(FREAD_CMD); 
    SPI_Write(addr_tmp>>16);
    SPI_Write(addr_tmp>>8);
    SPI_Write(addr_tmp); 
    SPI_Write(DUMMY_BYTE); 
    for(i=0;i<len;i++)
        *ptr++=SPI_Read(); 
    SPI_CS_High();    
}
#if 0
void FlashDoubleRead(u32 addr,u8 *ptr,u32 len)
{
    u32 addr_tmp,i;
    addr_tmp=addr&0x001fffff;
    FlashWait_Busy();    
    FLASH_CS_Low();
    SPI_Write(DREAD_CMD); 
    SPI_Write(addr_tmp>>16);
    SPI_Write(addr_tmp>>8);
    SPI_Write(addr_tmp); 
    SPI_Write(DUMMY_BYTE); 
    for(i=0;i<len;i++)
    {
        //��˫������,��Ҫ���
        //*ptr++=SPI_Read(); 
    }
    SPI_CS_High();    
}
#endif
