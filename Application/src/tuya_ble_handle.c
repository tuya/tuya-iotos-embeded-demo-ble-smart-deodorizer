/**
* @file tuya_ble_handle.c
* @author songjiang
* @email: songjiang.cao@tuya.com
* @brief tuya_ble_handle.c file function is DP processing
* @version 0.1
* @date 2021-11-20
*
* @copyright Copyright (c) tuya.inc 2021
*
*/

/*********************************************************************
********************************includes******************************
*********************************************************************/
#include "tuya_ble_handle.h"
#include "tuya_appointment_timing_function.h"
#include "tuya_key_process.h"
#include "tuya_battery_check.h"
#include "tuya_ble_log.h"

/*********************************************************************
****************************typedef define****************************
*********************************************************************/

extern ty_reservation_timing_off_flag_t appointment_timing_close;

/**
* @function:ty_dp_processing
* @brief: The function is to deal with the DP points issued by the panel.
* @param[in]: uint8_t* dp_data
* @return: success -> 0   fail -> else
*/
int ty_dp_processing(uint8_t* dp_data)
{
	uint8_t dp_id = 0;
	uint8_t dp_type = 0;
	dp_id = dp_data[0];
	dp_type = dp_data[4];
    TUYA_APP_LOG_DEBUG("dp_data %d %d %d %d %d", dp_data[0], dp_data[1], dp_data[2], dp_data[3], dp_data[4]);
	if(dp_id == 101)            //净化模式
    {
        switch (dp_type)
        {
            case 0:
                if(appointment_timing_close.reservation_timing_off_flag == 0)
                {
                    ty_deodorization_mode();    //除味模式
                }
                else
                {
                    ty_device_start_work();
                    ty_deodorization_mode();
                    
                    appointment_timing_close.reservation_timing_off_flag = 0;
                    
                }
                break;
            case 1:
                if(appointment_timing_close.reservation_timing_off_flag == 0)
                {
                    ty_fresh_keeping_mode();    //保鲜模式
                }
                else
                {
                    ty_device_start_work();
                    ty_fresh_keeping_mode();
                    appointment_timing_close.reservation_timing_off_flag = 0;
                }
                break;
            default:
                break;
        }
    } 
    else if(dp_id == 103)      //档位调节
    {
        switch (dp_type)
        {
            case 0:
                if(appointment_timing_close.reservation_timing_off_flag == 0)
                {
                    ty_one_mode();
                }
                else
                {
                    ty_device_start_work();
                    ty_one_mode();
                    appointment_timing_close.reservation_timing_off_flag = 0;
                }
                break;
            case 1:
                if(appointment_timing_close.reservation_timing_off_flag == 0)
                {
                    ty_two_mode();
                }
                else
                {
                    ty_device_start_work();
                    ty_two_mode();
                    appointment_timing_close.reservation_timing_off_flag = 0;
                }
                break;
            case 2:
                if(appointment_timing_close.reservation_timing_off_flag == 0)
                {
                    TUYA_APP_LOG_DEBUG("dp_three_gear");
                    ty_three_mode();
                }
                else
                {
                    ty_device_start_work();
                    ty_three_mode();
                    appointment_timing_close.reservation_timing_off_flag = 0;
                }
                break;
            default:
                break;
        }
    }
    else if(dp_id == 104)      //预约定时关闭
    {
        switch (dp_type)
        {
            case 0:
                ty_rtc_get_time_and_fifteen_points_update_flag();
                break;
            case 1:
                ty_rtc_get_time_and_thirty_points_update_flag();
                break;
            case 2:
                ty_rtc_get_time_and_one_hour_update_flag();
                break;
            case 3:
                ty_rtc_get_time_and_two_hour_update_flag();
                break;
            default:
                break;
        }
    }
    return 0;
}

