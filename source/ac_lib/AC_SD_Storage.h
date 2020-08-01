/*
 * AC_SD_Storage.h
 *
 *  Created on: 2020年7月28日
 *      Author: jerry
 */

#ifndef AC_LIB_AC_SD_STORAGE_H_
#define AC_LIB_AC_SD_STORAGE_H_

#include "fsl_common.h"

/**
 * @brief Identifier the type of SD storage program.
 *
 *         ModeSync：    For background service。  File name is Sync
 *         Modedefault  All functions, including user defined path
 *         ModeBoot     For booot only, no message on OLED. File name is same as ModeSync
 */
typedef enum {
    ModeSync,
    ModeDefault,
    ModeBoot,
}StorageMode_t;



/*!
 * @brief SD Menu Save
 */
status_t AC_SD_MenuSave(StorageMode_t mode);


/*!
 * @brief SD Menu Load
 */
status_t AC_SD_MenuLoad(StorageMode_t mode);

#endif /* AC_LIB_AC_SD_STORAGE_H_ */
