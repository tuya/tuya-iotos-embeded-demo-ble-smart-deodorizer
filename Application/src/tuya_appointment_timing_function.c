/**
* @file tuya_appointment_timing_function.c
* @author songjiang
* @email: songjiang.cao@tuya.com
* @brief tuya_appointment_timing_function.c file is used for scheduled shutdown
* @version 0.1
* @date 2021-11-20
*
* @copyright Copyright (c) tuya.inc 2021
*
*/

/*********************************************************************
********************************includes******************************
*********************************************************************/
#include "tuya_appointment_timing_function.h"
#include "ty_rtc.h"
#include "nrf_drv_rtc.h"
#include "tuya_battery_check.h"
#include "tuya_ble_log.h"

/*********************************************************************
****************************typedef define****************************
*********************************************************************/
ty_appointment_time_arrival_t appointment_time_arrival = {0};
ty_time_end_flag_bit_t time_end_flag_bit = {0};
ty_reservation_timing_off_flag_t appointment_timing_close = {0};

/*********************************************************************
****************************function define***************************
*********************************************************************/
/**

/**
* @function:ty_rtc_get_time_and_fifteen_points_update_flag
* @brief: Make an appointment for 15 minutes, obtain the time and update the flag bit
* @param[in]: void
* @return: success -> 0   fail -> else
*/
uint32_t ty_rtc_get_time_and_fifteen_points_update_flag(void)
{
   uint32_t current_time = 0;          //获取当前RTC时间
   uint8_t send_buf[8] = {0};

   ty_rtc_get_time(&current_time);
   appointment_time_arrival.time_end = current_time + APPOINTMENT_TIMING_FIFTEEN_POINTS;  //获取当前时间加上15分钟后的时间戳
   memset(send_buf, 0, sizeof(send_buf));
   send_buf[0] = 104;
   send_buf[1] = 4;
   send_buf[2] = 0x00;
   send_buf[3] = 0x01;
   send_buf[4] = 0x00;
   tuya_ble_dp_data_send(0, DP_SEND_TYPE_ACTIVE, DP_SEND_FOR_CLOUD_PANEL, DP_SEND_WITHOUT_RESPONSE, send_buf, 5);
   time_end_flag_bit.time_end_flag = 1;

   return 0;
}

/**
* @function:ty_rtc_get_time_and_thirty_points_update_flag
* @brief: Make an appointment for 30 minutes, obtain the time and update the flag bit
* @param[in]: void
* @return: success -> 0   fail -> else
*/
uint32_t ty_rtc_get_time_and_thirty_points_update_flag(void)
{
   uint32_t current_time = 0;
   uint8_t send_buf[8] = {0};

   ty_rtc_get_time(&current_time);
   appointment_time_arrival.time_end = current_time + APPOINTMENT_TIMING_THIRTY_POINTS;
   memset(send_buf, 0, sizeof(send_buf));
   send_buf[0] = 104;
   send_buf[1] = 4;
   send_buf[2] = 0x00;
   send_buf[3] = 0x01;
   send_buf[4] = 0x01;
   tuya_ble_dp_data_send(1, DP_SEND_TYPE_ACTIVE, DP_SEND_FOR_CLOUD_PANEL, DP_SEND_WITHOUT_RESPONSE, send_buf, 5);
   time_end_flag_bit.time_end_flag = 1;

   return 0;
}

/**
* @function:ty_rtc_get_time_and_one_hour_update_flag
* @brief: Appointment time 1 hour, obtain the time, update the flag bit
* @param[in]: void
* @return: success -> 0   fail -> else
*/
uint32_t ty_rtc_get_time_and_one_hour_update_flag(void)
{
   uint32_t current_time = 0;
   ty_rtc_get_time(&current_time);
   uint8_t send_buf[8] = {0};

   appointment_time_arrival.time_end = current_time + APPOINTMENT_TIMING_ONE_HOUR;
   memset(send_buf, 0, sizeof(send_buf));
   send_buf[0] = 104;
   send_buf[1] = 4;
   send_buf[2] = 0x00;
   send_buf[3] = 0x01;
   send_buf[4] = 0x02;
    tuya_ble_dp_data_send(2, DP_SEND_TYPE_ACTIVE, DP_SEND_FOR_CLOUD_PANEL, DP_SEND_WITHOUT_RESPONSE, send_buf, 5);
   time_end_flag_bit.time_end_flag = 1;

   return 0;
}

/**
* @function:ty_rtc_get_time_and_two_hour_update_flag
* @brief: The appointment time is 2 hours, the time is obtained, and the flag bit is updated
* @param[in]: void
* @return: success -> 0   fail -> else
*/
uint32_t ty_rtc_get_time_and_two_hour_update_flag(void)
{
   uint32_t current_time = 0;
   ty_rtc_get_time(&current_time);
   uint8_t send_buf[8] = {0};

   appointment_time_arrival.time_end = current_time + APPOINTMENT_TIMING_TWO_HOUR;
   memset(send_buf, 0, sizeof(send_buf));
   send_buf[0] = 104;
   send_buf[1] = 4;
   send_buf[2] = 0x00;
   send_buf[3] = 0x01;
   send_buf[4] = 0x03;
    tuya_ble_dp_data_send(3, DP_SEND_TYPE_ACTIVE, DP_SEND_FOR_CLOUD_PANEL, DP_SEND_WITHOUT_RESPONSE, send_buf, 5);
   time_end_flag_bit.time_end_flag = 1;

   return 0;
}

/**
* @function:ty_check_appointment_time
* @brief: Determine whether the appointment time has arrived
* @param[in]: void
* @return: success -> 0   fail -> else
*/
uint32_t ty_check_appointment_time(void)
{
   static uint32_t appointment_time = 0;
   if(!time_end_flag_bit.time_end_flag) {
      return 0;
   }
         
   // while (time_end_flag_bit.time_end_flag)
   // {
      ty_rtc_get_time(&appointment_time);
      if (appointment_time >= appointment_time_arrival.time_end)    //判断预约除味器工作时长时间是否到达
      {
         appointment_time_arrival.time_end = 0;
         time_end_flag_bit.time_end_flag = 0;
         ty_device_stop_work();                                   //关闭设备，进入低功耗状态
      }
   // }

   return 0;
}

