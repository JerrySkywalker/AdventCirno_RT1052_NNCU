/*
 * GENERATED FILE - DO NOT EDIT
 * Copyright (c) 2008-2013 Code Red Technologies Ltd,
 * Copyright 2015, 2018-2019 NXP
 * (c) NXP Semiconductors 2013-2020
 * Generated linker script file for MIMXRT1052xxxxB
 * Created from memory.ldt by FMCreateLinkMemory
 * Using Freemarker v2.3.23
 * MCUXpresso IDE v11.1.1 [Build 3241] [2020-03-02] on 2020-7-21 21:27:44
 */

MEMORY
{
  /* Define each memory region */
  Flash_00 (rx) : ORIGIN = 0x60000000, LENGTH = 0x300000 /* 3M bytes (alias Flash) */  
  Flash_01 (rx) : ORIGIN = 0x60300000, LENGTH = 0x100000 /* 1M bytes (alias Flash2) */  
  SRAM_DTC (rwx) : ORIGIN = 0x20000000, LENGTH = 0x20000 /* 128K bytes (alias RAM) */  
  SRAM_ITC (rwx) : ORIGIN = 0x0, LENGTH = 0x20000 /* 128K bytes (alias RAM2) */  
  SRAM_OC (rwx) : ORIGIN = 0x20200000, LENGTH = 0x40000 /* 256K bytes (alias RAM3) */  
  SDRAM (rwx) : ORIGIN = 0x80000000, LENGTH = 0x800000 /* 8M bytes (alias RAM4) */  
  SDRAM_NOCACHE (rwx) : ORIGIN = 0x80800000, LENGTH = 0x800000 /* 8M bytes (alias RAM5) */  
}

  /* Define a symbol for the top of each memory region */
  __base_Flash_00 = 0x60000000  ; /* Flash_00 */  
  __base_Flash = 0x60000000 ; /* Flash */  
  __top_Flash_00 = 0x60000000 + 0x300000 ; /* 3M bytes */  
  __top_Flash = 0x60000000 + 0x300000 ; /* 3M bytes */  
  __base_Flash_01 = 0x60300000  ; /* Flash_01 */  
  __base_Flash2 = 0x60300000 ; /* Flash2 */  
  __top_Flash_01 = 0x60300000 + 0x100000 ; /* 1M bytes */  
  __top_Flash2 = 0x60300000 + 0x100000 ; /* 1M bytes */  
  __base_SRAM_DTC = 0x20000000  ; /* SRAM_DTC */  
  __base_RAM = 0x20000000 ; /* RAM */  
  __top_SRAM_DTC = 0x20000000 + 0x20000 ; /* 128K bytes */  
  __top_RAM = 0x20000000 + 0x20000 ; /* 128K bytes */  
  __base_SRAM_ITC = 0x0  ; /* SRAM_ITC */  
  __base_RAM2 = 0x0 ; /* RAM2 */  
  __top_SRAM_ITC = 0x0 + 0x20000 ; /* 128K bytes */  
  __top_RAM2 = 0x0 + 0x20000 ; /* 128K bytes */  
  __base_SRAM_OC = 0x20200000  ; /* SRAM_OC */  
  __base_RAM3 = 0x20200000 ; /* RAM3 */  
  __top_SRAM_OC = 0x20200000 + 0x40000 ; /* 256K bytes */  
  __top_RAM3 = 0x20200000 + 0x40000 ; /* 256K bytes */  
  __base_SDRAM = 0x80000000  ; /* SDRAM */  
  __base_RAM4 = 0x80000000 ; /* RAM4 */  
  __top_SDRAM = 0x80000000 + 0x800000 ; /* 8M bytes */  
  __top_RAM4 = 0x80000000 + 0x800000 ; /* 8M bytes */  
  __base_SDRAM_NOCACHE = 0x80800000  ; /* SDRAM_NOCACHE */  
  __base_RAM5 = 0x80800000 ; /* RAM5 */  
  __top_SDRAM_NOCACHE = 0x80800000 + 0x800000 ; /* 8M bytes */  
  __top_RAM5 = 0x80800000 + 0x800000 ; /* 8M bytes */  
