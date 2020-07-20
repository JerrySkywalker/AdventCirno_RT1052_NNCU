/*
#include <sc_ac_key_5D.h>
 * @Descripttion: 
 * @version: 
 * @Author: JerryW
 * @Date: 2020-05-18 18:21:32
 * @LastEditors: JerryW
 * @LastEditTime: 2020-06-19 17:36:09
 */ 
#include "MIMXRT1052.h"
#include"fsl_gpio.h"
#include"pin_mux.h"
#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "task.h"
#include "sc_ac_key_5D.h"

/*TODO: Key definition for RT1052_ControlBoard 2nd Edition*/
#ifndef KEY_UP_GPIO
#define KEY_UP_GPIO GPIO2 
#define KEY_UP_PIN 1U
#endif

#ifndef KEY_DOWN_GPIO
#define KEY_DOWN_GPIO GPIO2 
#define KEY_DOWN_PIN 2U
#endif

#ifndef KEY_RIGHT_GPIO
#define KEY_RIGHT_GPIO GPIO2 
#define KEY_RIGHT_PIN 3U
#endif

#ifndef KEY_LEFT_GPIO
#define KEY_LEFT_GPIO GPIO1 
#define KEY_LEFT_PIN 9U
#endif

#ifndef KEY_ENTER_GPIO
#define KEY_ENTER_GPIO GPIO2 
#define KEY_ENTER_PIN 0U
#endif

#define KEY_NUM	5




void KEY_5D_Init()
{
    gpio_pin_config_t pinconfig;
	pinconfig.direction = kGPIO_DigitalInput;
    pinconfig.outputLogic = 1;
    GPIO_PinInit(KEY_UP_GPIO, KEY_UP_PIN , &pinconfig);
    GPIO_PinInit(KEY_DOWN_GPIO, KEY_DOWN_PIN , &pinconfig);
    GPIO_PinInit(KEY_LEFT_GPIO, KEY_LEFT_PIN , &pinconfig);
    GPIO_PinInit(KEY_RIGHT_GPIO, KEY_RIGHT_PIN , &pinconfig);
    GPIO_PinInit(KEY_ENTER_GPIO, KEY_ENTER_PIN , &pinconfig);
}


KEY_STATUS Key_Check(KEY_e key)
{
    if(KEY_UP==key)
    {
        if(KEY_P_DOWN==GPIO_PinRead(KEY_UP_GPIO,KEY_UP_PIN))
        {
            vTaskDelay(50);
            if(KEY_P_DOWN==GPIO_PinRead(KEY_UP_GPIO,KEY_UP_PIN))
                return KEY_P_DOWN; 
        }
    }
    if(KEY_DOWN==key)
    {
        if(KEY_P_DOWN==GPIO_PinRead(KEY_DOWN_GPIO,KEY_DOWN_PIN))
        {
            vTaskDelay(50);
            if(KEY_P_DOWN==GPIO_PinRead(KEY_DOWN_GPIO,KEY_DOWN_PIN))
                return KEY_P_DOWN; 
        }
    }
    if(KEY_LEFT==key)
    {
        if(KEY_P_DOWN==GPIO_PinRead(KEY_LEFT_GPIO,KEY_LEFT_PIN))
        {
            vTaskDelay(50);
            if(KEY_P_DOWN==GPIO_PinRead(KEY_LEFT_GPIO,KEY_LEFT_PIN))
                return KEY_P_DOWN; 
        }
    }
    if(KEY_RIGHT==key)
    {
        if(KEY_P_DOWN==GPIO_PinRead(KEY_RIGHT_GPIO,KEY_RIGHT_PIN))
        {
            vTaskDelay(50);
            if(KEY_P_DOWN==GPIO_PinRead(KEY_RIGHT_GPIO,KEY_RIGHT_PIN))
                return KEY_P_DOWN; 
        }
    }
    if(KEY_ENTER==key)
    {
        if(KEY_P_DOWN==GPIO_PinRead(KEY_ENTER_GPIO,KEY_ENTER_PIN))
        {
            vTaskDelay(50);
            if(KEY_P_DOWN==GPIO_PinRead(KEY_ENTER_GPIO,KEY_ENTER_PIN))
                return KEY_P_DOWN; 
        }
    }
    return KEY_P_UP;    
}
