/**
* @file tuya_key_process.h
* @author songjiang
* @email: songjiang.cao@tuya.com
* @brief tuya_key_process.h file is used for 5s distribution network and gear cycle adjustment
* @version 0.1
* @date 2021-11-20
*
* @copyright Copyright (c) tuya.inc 2021
*
*/

#ifndef __TUYA_KEY_PROCESS_H__
#define __TUYA_KEY_PROCESS_H__

#ifdef __cplusplus
extern "C"
{
#endif

/*********************************************************************
********************************includes******************************
*********************************************************************/
#include "nrf_gpio.h"

/*********************************************************************
******************************macro define****************************
*********************************************************************/
#define KEY_PRESS 11
#define ONE_INDICATOR_LIGHT 18
#define TWO_INDICATOR_LIGHT 14
#define THREE_INDICATOR_LIGHT 15

/*********************************************************************
****************************typedef define****************************
*********************************************************************/
typedef struct {
    unsigned char key_long_press;
    unsigned char key_short_press;
}ty_key_state;

typedef struct {
    unsigned char power_on;
    unsigned char led_display_step;
    unsigned int led_polarity;
    unsigned int indicator_gpio_pin;
    uint32_t count;
}ty_led_state;

typedef struct {
    uint32_t ble_band_state;
}ty_ble_band_state;

typedef struct {
    unsigned char fresh_keeping_mode;
    unsigned char deodorization_mode;
}ty_mode_selection_t;

typedef struct {
    unsigned char low_power_alarm_id;
    unsigned char gear_adjustment_id;
    unsigned char work_mode_id;
    unsigned char low_power_alarm_type;
    unsigned char gear_adjustment_type;
    unsigned char work_mode_type;
    unsigned char power_display_id;
    unsigned char power_display_type;
    unsigned char sn_data;
}tuya_dp_id_t;

/*********************************************************************
****************************function define***************************
*********************************************************************/
void ty_delay_us(uint32_t time_us);
uint32_t ty_gpio_cfg_output(void);
uint32_t ty_indicator_light_pin_set(uint32_t pin, uint32_t level);
uint32_t ty_indicator_light(void);
uint32_t ty_device_key_gear_adjust(void);
uint32_t ty_key_scan_process(void);
uint32_t ty_equipment_network_mode(void);
uint32_t ty_led_display_process(void);
uint32_t ty_demo_para_init(void);
uint32_t ty_switch_on_light_flash(void);
uint32_t ty_fresh_keeping_mode(void);
uint32_t ty_switch_off_indicator_light(void);
uint32_t ty_read_pin_levlel_state(uint32_t pin_num);
uint32_t ty_first_gear_indicator_light(void);
uint32_t ty_second_gear_indicator_light(void);
uint32_t ty_third_gear_indicator_light(void);
uint32_t ty_deodorization_mode(void);
uint32_t ty_turn_on_the_equipment(void);
uint32_t ty_device_default_status(void);
uint32_t ty_one_mode(void);
uint32_t ty_two_mode(void);
uint32_t ty_three_mode(void);
uint32_t ty_first_gear(void);
uint32_t ty_second_gear(void);
uint32_t ty_third_gear(void);
uint32_t ty_deodorize_mode(void);
uint32_t ty_keep_mode(void);




#ifdef __cplusplus
}
#endif

#endif
