# Changelog

This File records the changes in project SDK. Please update it frequently.

## board

### board.c/h 

Completely changed for board setups. It also includes GNU attributes macro for storage modifications.

## amazon_freertos/portable/MemMang

### heap_5.c
We use heap_5.c to support custom heap settings. other heap usage options are exclude from build.

## amazon_freertos/portable/GCC/ARM_CM4F

## port.c

- line 487-489: To support SDMMC Clock , we extern  g_eventTimeMilliseconds.


## source

### ffconf.h

- line 13:  MSDK adaptation configuration, we use SD_DISK_ENABLE

### FreeRTOSConfig.h





