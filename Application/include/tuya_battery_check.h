/**
* @file tuya_battery_check.h
* @author songjiang
* @email: songjiang.cao@tuya.com
* @brief tuya_battery_check.h file is used for power detection
* @version 0.1
* @date 2021-11-20
*
* @copyright Copyright (c) tuya.inc 2021
*
*/

#ifndef __TUYA_BATTERY_CHECK_H__
#define __TUYA_BATTERY_CHECK_H__

#ifdef __cplusplus
extern "C" 
{
#endif

/*********************************************************************
********************************includes******************************
*********************************************************************/
#include "tuya_ble_stdlib.h"

/*********************************************************************
****************************typedef define****************************
*********************************************************************/
typedef struct {
    uint16_t adc_values;
}ty_adc_value_t;

typedef struct {
    unsigned char app_low_power_alarm;
    unsigned char app_low_power_indicator_light_flash;
}ty_power_check_t;

typedef struct {
    uint32_t power_report_timeout_two_hour;
    uint32_t power_report_timeout_thirty;
    uint32_t rescheduling_mark;
}ty_power_report_t;

typedef struct {
    unsigned char low_power_glag;
}ty_low_power_t;

/*********************************************************************
****************************function define***************************
*********************************************************************/
uint32_t ty_battery_check_adc_init(void);
uint32_t ty_battery_check_adc_uninit(void);
uint32_t ty_get_battery_power(void);
uint32_t ty_low_power_alarm(void);
uint32_t ty_power_alarm_indicator_flash(void);
uint32_t ty_device_working_status(void);
uint32_t ty_device_start_work(void);
uint32_t ty_device_stop_work(void);
uint32_t ty_switch_off_fan(void);
uint32_t ty_switch_on_fan(void);
uint32_t ty_start_boost_pin(void);
uint32_t ty_get_low_power_state(void);
uint32_t ty_low_power_dp_send(void);
uint32_t ty_normal_power_dp_send(void);
uint32_t ty_detect_power_send_dp(void);
uint32_t ty_app_low_power_operation(void);
uint32_t ty_electricity_report(void);
uint32_t ty_power_10_dp_send(void);
uint32_t ty_power_20_dp_send(void);
uint32_t ty_power_30_dp_send(void);
uint32_t ty_power_40_dp_send(void);
uint32_t ty_power_50_dp_send(void);
uint32_t ty_power_60_dp_send(void);
uint32_t ty_power_70_dp_send(void);
uint32_t ty_power_80_dp_send(void);
uint32_t ty_power_90_dp_send(void);
uint32_t ty_power_dp_send(uint8_t);
uint32_t ty_read_gpio_pin(uint32_t pin_num);
uint32_t ty_stop_boost_pin(void);
uint32_t ty_battery_power_check_task(void);

#ifdef __cplusplus
}
#endif

#endif
