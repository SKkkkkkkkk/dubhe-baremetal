#ifndef __NOR_FLASH_PORT__
#define __NOR_FLASH_PORT__

#define FLASH_Size       16384UL
#define FLASH_SectorSize 4096UL
#define FLASH_PageSize   256UL
// #define FLASH_ID         0x20BA18UL
/*FLASH 常用命令 */
#define Flash_ResetEnable  0x66
#define Flash_ResetMemory  0x99
#define Flash_WriteEnable 0x06
#define Flash_WriteDisable 0x04
#define Flash_ReadStatusReg 0x05
#define Flash_WriteStatusReg 0x01
#define Flash_ReadData 0x03
#define Flash_FastReadData 0x0B
#define Flash_FastReadDual 0x3B
#define Flash_PageProgram 0x02
#define Flash_BlockErase 0xD8
#define Flash_SectorErase 0x20
#define Flash_ChipErase 0xC7
#define Flash_PowerDown 0xB9
#define Flash_ReleasePowerDown 0xAB
#define Flash_DeviceID 0xAB
#define Flash_ManufactDeviceID 0x90
#define Flash_JedecDeviceID 0x9F

#define Flash_Read_Nonvolatile_Config_Reg 0xB5
#define Flash_Write_Nonvolatile_Config_Reg 0xB1
#define Flash_Read_Enhanced_Volatile_Config_Reg 0x65
#define Flash_Write_Enhanced_Volatile_Config_Reg 0x61

#define Flash_Read_Extended_Address_Register 0xC8
#define Flash_Write_Extended_Address_Register 0xC5

#define Flash_Enter_4Byte_Address_Mode 0xB7
#define Flash_Exit_4Byte_Address_Mode 0xE9

#define Flash_Enable_QPI 0x38
#define Flash_Disable_QPI 0xff
#define Flash_ReadStatusReg_High 0x35
#define Flash_Quad_Output_Fast_Read 0x6b


#define WRITE_ENABLE_LATCH 1
#define WRITE_IN_PROGRESS 0
#define Quad_Enable_Bit (1<<(9-8))

#endif