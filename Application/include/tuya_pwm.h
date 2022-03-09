/**
* @file tuya_pwm.h
* @author songjiang
* @email: songjiang.cao@tuya.com
* @brief tuya_pwm.h file is used to initialize ozone anion generators and anion generators
* @version 0.1
* @date 2021-11-20
*
* @copyright Copyright (c) tuya.inc 2021
*
*/

#ifndef __TUYA_PWM_H__
#define __TUYA_PWM_H__

#ifdef __cplusplus
extern "C"
{
#endif

/*********************************************************************
********************************includes******************************
*********************************************************************/
#include "ty_pwm.h"
#include "nrf_gpio.h"

/*********************************************************************
******************************macro define****************************
*********************************************************************/
#define OZONE_PWM_PIN 4
#define NEGATIVE_ION_PWM_PIN 3

/*********************************************************************
****************************typedef define****************************
*********************************************************************/
typedef struct {
    uint32_t ozone_duty;
    uint32_t negative_ion_duty;
    uint32_t device_mode;
}ty_device_gear_mode;

/*********************************************************************
****************************function define***************************
*********************************************************************/
uint32_t ty_ozone_first_gear_pwm_init(void);
uint32_t ty_ozone_second_gear_pwm_init(void);
uint32_t ty_ozone_three_gear_pwm_init(void);
uint32_t ty_ozone_pwm_stop(void);
uint32_t ty_negative_ion_first_gear_pwm_init(void);
uint32_t ty_negative_ion_second_gear_pwm_init(void);
uint32_t ty_negative_ion_three_gear_pwm_init(void);
uint32_t ty_negative_ion_pwm_stop(void);
uint32_t ty_get_device_gear_mode(void);
uint32_t ty_get_gear_mode(void);

#ifdef __cplusplus
}
#endif

#endif
