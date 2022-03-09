/**
* @file tuya_appointment_timing_function.h
* @author songjiang
* @email: songjiang.cao@tuya.com
* @brief tuya_appointment_timing_function.c file is used for scheduled shutdown
* @version 0.1
* @date 2021-11-20
*
* @copyright Copyright (c) tuya.inc 2021
*
*/

#ifndef __TUYA_APPOINTMENT_TIMING_FUNCTION_H__
#define __TUYA_APPOINTMENT_TIMING_FUNCTION_H__

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
typedef enum
{
    APPOINTMENT_TIMING_FIFTEEN_POINTS = 900,     //定时15分钟
    APPOINTMENT_TIMING_THIRTY_POINTS = 1800,    //定时30分钟
    APPOINTMENT_TIMING_ONE_HOUR = 3600,         //定时1小时
    APPOINTMENT_TIMING_TWO_HOUR = 7200          //定时2小时

}ty_fixed_time_interval_t;

typedef struct {
    unsigned char fifteen_points_flag;
    unsigned char thirty_points_flag;
    unsigned char one_hour_flag;
    unsigned char two_hour_flag;
}ty_function_flag_bit_t;

typedef struct {
    uint32_t time_end;
}ty_appointment_time_arrival_t;

typedef struct {
    uint32_t time_end_flag;
}ty_time_end_flag_bit_t;

typedef struct {
    unsigned char reservation_timing_off_flag;
}ty_reservation_timing_off_flag_t;

/*********************************************************************
****************************function define***************************
*********************************************************************/
uint32_t ty_rtc_get_time_and_fifteen_points_update_flag(void);
uint32_t ty_rtc_get_time_and_thirty_points_update_flag(void);
uint32_t ty_rtc_get_time_and_one_hour_update_flag(void);
uint32_t ty_rtc_get_time_and_two_hour_update_flag(void);
uint32_t ty_check_appointment_time(void);


#ifdef __cplusplus
}
#endif

#endif
