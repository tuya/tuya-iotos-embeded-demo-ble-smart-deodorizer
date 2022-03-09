/**
* @file tuya_battery_check.c
* @author songjiang
* @email: songjiang.cao@tuya.com
* @brief tuya_battery_check.c file is used for power detection
* @version 0.1
* @date 2021-11-20
*
* @copyright Copyright (c) tuya.inc 2021
*
*/

/*********************************************************************
********************************includes******************************
*********************************************************************/
#include "tuya_battery_check.h"
#include "ty_adc.h"
#include "nrf_gpio.h"
#include "tuya_key_process.h"
#include "tuya_deodorizer_temperature_humidity.h"
#include "tuya_pwm.h"
#include "tuya_ble_stdlib.h"
#include "tuya_ble_log.h"
#include "tuya_ble_api.h"
#include "tuya_ble_type.h"
#include "ty_rtc.h"
#include "ty_i2c.h"
#include "ty_spi.h"
#include "ty_uart.h"
#include "ty_oled.h"
#include "ty_ble.h"
#include "tuya_ble_sdk_test.h"
#include "tuya_appointment_timing_function.h"


/*********************************************************************
******************************macro define****************************
*********************************************************************/
#define BOOST_PIN 16
#define FAN_PIN 5
#define THIRTY 1800
#define TWO_HOUR 7200
#define DEODORIZER_BATTERY_CHANNEL 0
/*********************************************************************
****************************typedef define****************************
*********************************************************************/
ty_adc_value_t deodorizer_adc_value = {0};

ty_adc_t battery_check_adc = {0};

ty_power_check_t power_check_status = {0};

ty_power_report_t power_report = {0,0,1};

ty_low_power_t low_power = {0};

extern ty_reservation_timing_off_flag_t appointment_timing_close;
extern ty_mode_selection_t mode_selection;

extern tuya_dp_id_t tuya_dp_id;
extern ty_pwm_t ozone_pwm;
extern ty_pwm_t negative_ion_pwm;
static uint32_t low_power_flag = 0;

/*********************************************************************
****************************function define***************************
*********************************************************************/
uint32_t ty_power_10_dp_send(void);
uint32_t ty_power_20_dp_send(void);
uint32_t ty_power_30_dp_send(void);
uint32_t ty_power_40_dp_send(void);
uint32_t ty_power_50_dp_send(void);
uint32_t ty_power_60_dp_send(void);
uint32_t ty_power_70_dp_send(void);
uint32_t ty_power_80_dp_send(void);
uint32_t ty_power_90_dp_send(void);
uint32_t ty_power_100_dp_send(void);
uint32_t ty_app_power_management(void);

/**
* @function:ty_battery_check_adc_init
* @brief: ADC detects initialization
* @param[in]: void
* @return: success -> 0   fail -> else
*/
uint32_t ty_battery_check_adc_init(void)
{
   battery_check_adc.channel = DEODORIZER_BATTERY_CHANNEL;
   ty_adc_init(&battery_check_adc);

   return 0;

}

/**
* @function:ty_battery_check_adc_uninit
* @brief: ADC detection disables initialization
* @param[in]: void
* @return: success -> 0   fail -> else
*/
uint32_t ty_battery_check_adc_uninit(void)
{
   ty_adc_uninit(&battery_check_adc);
   
   return 0;
}

/**
* @function:ty_get_battery_power
* @brief: ADC detects and obtains the current power value
* @param[in]: void
* @return: success -> 0   fail -> else
*/
uint32_t ty_get_battery_power(void)
{
   uint32_t sample = 35;
   uint32_t average_value = 0;
   for(int i = 0; i < 5; i++)
   {
       ty_adc_start(&battery_check_adc);
       average_value += (battery_check_adc.value * sample * 2);
   }
   deodorizer_adc_value.adc_values = (average_value / 5);

//    TUYA_APP_LOG_INFO("get battery power value = %d  sample = %d", deodorizer_adc_value.adc_values, sample);

   return 0;
}

/**
* @function:ty_low_power_alarm
* @brief: The ADC detects whether the power is low
* @param[in]: void
* @return: success -> 0   fail -> else
*/
uint32_t ty_low_power_alarm(void)
{
    uint8_t dp_length = 5;
    uint8_t send_buf[5];

    memset(send_buf, 0, sizeof(send_buf));
    send_buf[0] = tuya_dp_id.low_power_alarm_id;
    send_buf[1] = tuya_dp_id.low_power_alarm_type;
    send_buf[2] = 0x00;
    send_buf[3] = 0x01;
    if (deodorizer_adc_value.adc_values <= 32000) {     // low power
        send_buf[4] = 0x01;
        power_check_status.app_low_power_alarm = 1;
        power_check_status.app_low_power_indicator_light_flash = 1;
        tuya_ble_dp_data_send(0, DP_SEND_TYPE_ACTIVE, DP_SEND_FOR_CLOUD_PANEL, DP_SEND_WITHOUT_RESPONSE, send_buf, dp_length);
    } else if (deodorizer_adc_value.adc_values > 32000) {   // full power
        send_buf[4] = 0x00;
        low_power_flag = 0;
        power_check_status.app_low_power_alarm = 0;
        power_check_status.app_low_power_indicator_light_flash = 0;
        tuya_ble_dp_data_send(0, DP_SEND_TYPE_ACTIVE, DP_SEND_FOR_CLOUD_PANEL, DP_SEND_WITHOUT_RESPONSE, send_buf, dp_length);
    }

   return 0;
}

/**
* @function:ty_power_alarm_indicator_flash
* @brief: The indicator light of the deodorizer blinks when the power is low
* @param[in]: void
* @return: success -> 0   fail -> else
*/
uint32_t ty_power_alarm_indicator_flash(void)
{
   if(power_check_status.app_low_power_indicator_light_flash && low_power_flag == 0)
   {
       ty_indicator_light_pin_set(ONE_INDICATOR_LIGHT, 0);
       ty_indicator_light_pin_set(TWO_INDICATOR_LIGHT, 0);
       ty_indicator_light_pin_set(THREE_INDICATOR_LIGHT, 0);
       ty_delay_us(1000000);
       ty_indicator_light_pin_set(ONE_INDICATOR_LIGHT, 1);
       ty_indicator_light_pin_set(TWO_INDICATOR_LIGHT, 1);
       ty_indicator_light_pin_set(THREE_INDICATOR_LIGHT, 1);
       low_power_flag++;
   }
   
   return 0;
}

/**
* @function:ty_device_working_status
* @brief: The equipment stops working when the power is lower than low or the normal power is working properly
* @param[in]: void
* @return: success -> 0   fail -> else
*/
uint32_t ty_device_working_status(void)
{
   unsigned char status = 0;
   static unsigned char flags = 1;
   status = power_check_status.app_low_power_alarm;
   if (status && (flags == 1))
   {
       ty_device_stop_work();

       flags = 2;
   }
   else if((!status) && (flags ==2))
   {
       ty_device_start_work();
       flags = 1;
   }
   else
   {
       ;
   }

   return 0;
}

/**
* @function:ty_switch_off_fan
* @brief: Close the fan
* @param[in]: void
* @return: success -> 0   fail -> else
*/
uint32_t ty_switch_off_fan(void)
{
   nrf_gpio_pin_write(FAN_PIN, 0);
   return 0;
}

/**
* @function:ty_switch_on_fan
* @brief: Open the fan
* @param[in]: void
* @return: success -> 0   fail -> else
*/
uint32_t ty_switch_on_fan(void)
{
   nrf_gpio_pin_write(FAN_PIN, 1);
   return 0;
}

/**
* @function:ty_device_start_work
* @brief: Initialize pins and peripherals, low power exit
* @param[in]: void
* @return: success -> 0   fail -> else
*/
uint32_t ty_device_start_work(void)
{
    nrf_gpio_pin_write(BOOST_PIN, 1);        //开启升压电路
    ty_switch_on_fan();                      //开启风扇
    ty_sht3x_init();                         //初始化温湿度传感器
    ty_turn_on_the_equipment();              //开启臭氧离子发生器和负离子发生器
    ty_rtc_init();                          //rtc初始化
    nrf_gpio_cfg_input(KEY_PRESS, NRF_GPIO_PIN_PULLUP);    //按键引脚设置为输入模式
    //ty_ble_start_adv();
    ty_battery_check_adc_init();            //ADC电量检测初始化
    low_power.low_power_glag = 0;           //设备低功耗状态退出
    appointment_timing_close.reservation_timing_off_flag = 0;

   return 0;
}

/**
* @function:ty_device_stop_work
* @brief: Turn off the peripherals and the device enters low power consumption
* @param[in]: void
* @return: success -> 0   fail -> else
*/

uint32_t ty_device_stop_work(void)
{
   ty_sht3x_single_messure();                //开启温湿度传感器单次测量模式，降低功耗
   nrf_gpio_pin_write(BOOST_PIN,0);          //关闭升压电路
   ty_switch_off_fan();                      //关闭电扇
   ty_switch_off_indicator_light();          //关闭指示灯
   ty_negative_ion_pwm_stop();               //关闭臭氧离子发生器和负离子发生器
   //ty_ble_stop_adv();                      //蓝牙广播关闭函数
   ty_uart_uninit();                          //禁止串口初始化
   ty_uart2_uninit();                          //禁止串口初始化
   ty_rtc_uninit();                            //禁止RTC初始化
   ty_spi_disable();                           //禁止spi初始化
    ty_key_precess_off();
   ty_adc_uninit(&battery_check_adc);         //禁止电量检测
   ty_sht3x_reset();                         //关闭温湿度传感器
   low_power.low_power_glag = 1;               //设备进入低功耗状态
   appointment_timing_close.reservation_timing_off_flag = 1;
   return 0;
}

/**
* @function:ty_start_boost_pin
* @brief: Turn on the boost circuit
* @param[in]: void
* @return: success -> 0   fail -> else
*/
uint32_t ty_start_boost_pin(void)
{
   nrf_gpio_pin_write(BOOST_PIN,1);
   
   return 0;
}


/**
* @function:ty_stop_boost_pin
* @brief: Turn off the boost circuit
* @param[in]: void
* @return: success -> 0   fail -> else
*/
uint32_t ty_stop_boost_pin(void)
{
   nrf_gpio_pin_write(BOOST_PIN,0);
   return 0;
}

/**
* @function:ty_get_low_power_state
* @brief: Get power status (low or normal power)
* @param[in]: void
* @return: success -> 0   fail -> else
*/
uint32_t ty_get_low_power_state(void)
{
   return power_check_status.app_low_power_alarm;
}

/**
* @function:ty_low_power_dp_send
* @brief: Low power report
* @param[in]: void
* @return: success -> 0   fail -> else
*/
uint32_t ty_low_power_dp_send(void)
{
   uint32_t ret = 0;
   uint8_t dp_length = 5;
   uint8_t send_buf[5] = {0};
   send_buf[0] = tuya_dp_id.low_power_alarm_id;
   send_buf[1] = tuya_dp_id.low_power_alarm_type;
   send_buf[2] = 0x00;
   send_buf[3] = 0x01;
   send_buf[4] = 0x01;

   ret = tuya_ble_dp_data_send(tuya_dp_id.sn_data, DP_SEND_TYPE_ACTIVE, DP_SEND_FOR_CLOUD_PANEL, DP_SEND_WITHOUT_RESPONSE, send_buf, dp_length);
   if(ret != 0)
   {
       TUYA_APP_LOG_INFO("ty_low_power_dp_send  failed to send");
   }
   return 0;
}

/**
* @function:ty_normal_power_dp_send
* @brief: Normal electric quantity report
* @param[in]: void
* @return: success -> 0   fail -> else
*/
uint32_t ty_normal_power_dp_send(void)
{
   uint32_t ret = 0;
   uint8_t dp_length = 5;
   uint8_t send_buf[5] = {0};
   send_buf[0] = tuya_dp_id.low_power_alarm_id;
   send_buf[1] = tuya_dp_id.low_power_alarm_type;
   send_buf[2] = 0x00;
   send_buf[3] = 0x01;
   send_buf[4] = 0x00;

    ret = tuya_ble_dp_data_send(tuya_dp_id.sn_data+1, DP_SEND_TYPE_ACTIVE, DP_SEND_FOR_CLOUD_PANEL, DP_SEND_WITHOUT_RESPONSE, send_buf, dp_length);
   if(ret != 0)
   {
       TUYA_APP_LOG_INFO("ty_normal_power_dp_send filed to send");
   }
   return 0;
}

/**
* @function:ty_detect_power_send_dp
* @brief: Send low electric quantity or report normal electric quantity once
* @param[in]: void
* @return: success -> 0   fail -> else
*/
uint32_t ty_detect_power_send_dp(void)
{
   static uint8_t flag = 1;
   static uint8_t status = 1;

   if(power_check_status.app_low_power_alarm == 1 && status == 1)
   {
       TUYA_APP_LOG_DEBUG("low_power");
       ty_low_power_dp_send();
       status = 0;
       flag = 1;
   }
   else if(power_check_status.app_low_power_alarm == 0 && flag == 1)
   {
       TUYA_APP_LOG_DEBUG("normal_power");
       ty_normal_power_dp_send();
       flag = 0;
       status = 1;
   }
   else
   {
   
   }

   return 0;
}

uint32_t ty_app_low_power_operation(void)
{
   return 0;
}

/**
* @function:ty_electricity_report
* @brief: Report per cent of electricity
* @param[in]: void
* @return: success -> 0   fail -> else
*/
uint32_t ty_electricity_report(void)
{
   if(deodorizer_adc_value.adc_values >= 41000)
   {
       ty_power_dp_send(100);
   }
   else if ((deodorizer_adc_value.adc_values >= 40000) && (deodorizer_adc_value.adc_values < 41000))
   {
       ty_power_dp_send(90);
   }
   else if ((deodorizer_adc_value.adc_values >= 37000) && (deodorizer_adc_value.adc_values < 40000))
   {
       ty_power_dp_send(80);
   }
   else if ((deodorizer_adc_value.adc_values >= 36000) && (deodorizer_adc_value.adc_values < 37000))
   {
       ty_power_dp_send(70);
   }
   else if ((deodorizer_adc_value.adc_values >= 35000) && (deodorizer_adc_value.adc_values < 36000))
   {
       ty_power_dp_send(60);
   }
   else if ((deodorizer_adc_value.adc_values >= 34000) && (deodorizer_adc_value.adc_values < 35000))
   {
       ty_power_dp_send(40);
   }
   else if ((deodorizer_adc_value.adc_values >= 33500) && (deodorizer_adc_value.adc_values < 34000))
   {
       ty_power_dp_send(30);
   }
   else if ((deodorizer_adc_value.adc_values >= 33000) && (deodorizer_adc_value.adc_values < 33500))
   {
       ty_power_dp_send(20);
   }
   else if ((deodorizer_adc_value.adc_values >= 32000) && (deodorizer_adc_value.adc_values < 33000))
   {
       ty_power_dp_send(10);
   }
   else if (deodorizer_adc_value.adc_values < 32000)
   {
       ty_power_dp_send(0);
   }

   return 0;
}

/**
* @function:ty_power_10_dp_send
* @brief: 10% electric quantity reported
* @param[in]: void
* @return: success -> 0   fail -> else
*/
uint32_t ty_power_10_dp_send(void)
{
   uint32_t ret = 0;
   uint8_t dp_length = 8;
   uint8_t send_buf[8];

   memset(send_buf, 0, sizeof(send_buf));
   send_buf[0] = tuya_dp_id.power_display_id;
   send_buf[1] = tuya_dp_id.power_display_type;
   send_buf[2] = 0x00;
   send_buf[3] = 0x04;
   send_buf[7] = 0x0A;
   ret = tuya_ble_dp_data_send(tuya_dp_id.sn_data+5, DP_SEND_TYPE_ACTIVE, DP_SEND_FOR_CLOUD_PANEL, DP_SEND_WITHOUT_RESPONSE, send_buf, dp_length);

   return 0;
}

/**
* @function:ty_power_20_dp_send
* @brief: 20% electric quantity reported
* @param[in]: void
* @return: success -> 0   fail -> else
*/
uint32_t ty_power_20_dp_send(void)
{
   uint32_t ret = 0;
   uint8_t dp_length = 8;
    uint8_t send_buf[8];

   memset(send_buf, 0, sizeof(send_buf));
   send_buf[0] = tuya_dp_id.power_display_id;
   send_buf[1] = tuya_dp_id.power_display_type;
   send_buf[2] = 0x00;
   send_buf[3] = 0x04;
   send_buf[7] = 0x14;

   ret = tuya_ble_dp_data_send(tuya_dp_id.sn_data+6, DP_SEND_TYPE_ACTIVE, DP_SEND_FOR_CLOUD_PANEL, DP_SEND_WITHOUT_RESPONSE, send_buf, dp_length);

   return 0;
}

/**
* @function:ty_power_30_dp_send
* @brief: 30% electric quantity reported
* @param[in]: void
* @return: success -> 0   fail -> else
*/
uint32_t ty_power_30_dp_send(void)
{
   uint32_t ret = 0;
   uint8_t dp_length = 8;
    uint8_t send_buf[8];

   memset(send_buf, 0, sizeof(send_buf));
   send_buf[0] = tuya_dp_id.power_display_id;
   send_buf[1] = tuya_dp_id.power_display_type;
   send_buf[2] = 0x00;
   send_buf[3] = 0x04;
   send_buf[7] = 0x1E;


   ret = tuya_ble_dp_data_send(tuya_dp_id.sn_data + 7, DP_SEND_TYPE_ACTIVE, DP_SEND_FOR_CLOUD_PANEL, DP_SEND_WITHOUT_RESPONSE, send_buf, dp_length);

   return 0;
}

/**
* @function:ty_power_40_dp_send
* @brief: 40% electric quantity reported
* @param[in]: void
* @return: success -> 0   fail -> else
*/
uint32_t ty_power_40_dp_send(void)
{
   uint32_t ret = 0;
   uint8_t dp_length = 8;
uint8_t send_buf[8];

   memset(send_buf, 0, sizeof(send_buf));
   send_buf[0] = tuya_dp_id.power_display_id;
   send_buf[1] = tuya_dp_id.power_display_type;
   send_buf[2] = 0x00;
   send_buf[3] = 0x04;
   send_buf[7] = 0x28;


   ret = tuya_ble_dp_data_send(tuya_dp_id.sn_data+7, DP_SEND_TYPE_ACTIVE, DP_SEND_FOR_CLOUD_PANEL, DP_SEND_WITHOUT_RESPONSE, send_buf, dp_length);

   return 0;
}

/**
* @function:ty_power_50_dp_send
* @brief: 50% electric quantity reported
* @param[in]: void
* @return: success -> 0   fail -> else
*/
uint32_t ty_power_50_dp_send(void)
{
   uint32_t ret = 0;
   uint8_t dp_length = 8;
uint8_t send_buf[8];

   memset(send_buf, 0, sizeof(send_buf));
   send_buf[0] = tuya_dp_id.power_display_id;
   send_buf[1] = tuya_dp_id.power_display_type;
   send_buf[2] = 0x00;
   send_buf[3] = 0x04;
   send_buf[7] = 0x32;


   ret = tuya_ble_dp_data_send(tuya_dp_id.sn_data + 8, DP_SEND_TYPE_ACTIVE, DP_SEND_FOR_CLOUD_PANEL, DP_SEND_WITHOUT_RESPONSE, send_buf, dp_length);

   return 0;
}

/**
* @function:ty_power_60_dp_send
* @brief: 60% electric quantity reported
* @param[in]: void
* @return: success -> 0   fail -> else
*/
uint32_t ty_power_60_dp_send(void)
{
   uint32_t ret = 0;
   uint8_t dp_length = 8;
uint8_t send_buf[8];

   memset(send_buf, 0, sizeof(send_buf));
   send_buf[0] = tuya_dp_id.power_display_id;
   send_buf[1] = tuya_dp_id.power_display_type;
   send_buf[2] = 0x00;
   send_buf[3] = 0x04;
   send_buf[7] = 0x3C;


   ret = tuya_ble_dp_data_send(tuya_dp_id.sn_data+8, DP_SEND_TYPE_ACTIVE, DP_SEND_FOR_CLOUD_PANEL, DP_SEND_WITHOUT_RESPONSE, send_buf, dp_length);

   return 0;
}

/**
* @function:ty_power_70_dp_send
* @brief: 70% electric quantity reported
* @param[in]: void
* @return: success -> 0   fail -> else
*/
uint32_t ty_power_70_dp_send(void)
{
   uint32_t ret = 0;
   uint8_t dp_length = 8;
uint8_t send_buf[8];

   memset(send_buf, 0, sizeof(send_buf));
   send_buf[0] = tuya_dp_id.power_display_id;
   send_buf[1] = tuya_dp_id.power_display_type;
   send_buf[2] = 0x00;
   send_buf[3] = 0x04;
   send_buf[7] = 0x46;


   ret = tuya_ble_dp_data_send(tuya_dp_id.sn_data + 9, DP_SEND_TYPE_ACTIVE, DP_SEND_FOR_CLOUD_PANEL, DP_SEND_WITHOUT_RESPONSE, send_buf, dp_length);

   return 0;
}

/**
* @function:ty_power_80_dp_send
* @brief: 80% electric quantity reported
* @param[in]: void
* @return: success -> 0   fail -> else
*/
uint32_t ty_power_80_dp_send(void)
{
   uint32_t ret = 0;
   uint8_t dp_length = 8;
uint8_t send_buf[8];

   memset(send_buf, 0, sizeof(send_buf));
   send_buf[0] = tuya_dp_id.power_display_id;
   send_buf[1] = tuya_dp_id.power_display_type;
   send_buf[2] = 0x00;
   send_buf[3] = 0x04;
   send_buf[7] = 0x50;


   ret = tuya_ble_dp_data_send(tuya_dp_id.sn_data + 9, DP_SEND_TYPE_ACTIVE, DP_SEND_FOR_CLOUD_PANEL, DP_SEND_WITHOUT_RESPONSE, send_buf, dp_length);

   return 0;
}

/**
* @function:ty_power_90_dp_send
* @brief: 90% electric quantity reported
* @param[in]: void
* @return: success -> 0   fail -> else
*/
uint32_t ty_power_90_dp_send(void)
{
   uint32_t ret = 0;
   uint8_t dp_length = 8;
uint8_t send_buf[8];

   memset(send_buf, 0, sizeof(send_buf));
   send_buf[0] = tuya_dp_id.power_display_id;
   send_buf[1] = tuya_dp_id.power_display_type;
   send_buf[2] = 0x00;
   send_buf[3] = 0x04;
   send_buf[7] = 0x5A;

   return 0;
}

/**
* @function:ty_power_100_dp_send
* @brief: 100% electric quantity reported
* @param[in]: void
* @return: success -> 0   fail -> else
*/
uint32_t ty_power_dp_send(uint8_t bat_val)
{
    uint32_t ret = 0;
    uint8_t dp_length = 8;
    uint8_t send_buf[8];

   memset(send_buf, 0, sizeof(send_buf));
   send_buf[0] = tuya_dp_id.power_display_id;
   send_buf[1] = tuya_dp_id.power_display_type;
   send_buf[2] = 0x00;
   send_buf[3] = 0x04;
   send_buf[7] = bat_val;
   tuya_ble_dp_data_send(0, DP_SEND_TYPE_ACTIVE, DP_SEND_FOR_CLOUD_PANEL, DP_SEND_WITHOUT_RESPONSE, send_buf, dp_length);

   return 0;
}

/**
* @function:ty_read_gpio_pin
* @brief: Read the pin level
* @param[in]: void
* @return: success -> 0   fail -> else
*/
uint32_t ty_read_gpio_pin(uint32_t pin_num)
{
   uint32_t ret = 0;
   ret = nrf_gpio_pin_out_read(pin_num);
   //TUYA_APP_LOG_INFO("ty_read_gpio_pin pin_num %d= %d",pin_num,ret);

   return 0;
}

/**
* @function:ty_battery_power_check_task
* @brief: Bluetooth completes the connection and reports the current device gear, working mode and power data.
* @param[in]: void
* @return: success -> 0   fail -> else
*/
uint32_t ty_battery_power_check_task(void)
{
   tuya_ble_connect_status_t blue_connect_status = 0;
   uint32_t gear_mode = 0;
   static unsigned char blue_connect_flag = 1;
   blue_connect_status = tuya_ble_connect_status_get();
   gear_mode = ty_get_device_gear_mode();
   if (blue_connect_status == 3 && blue_connect_flag == 1) //蓝牙刚刚完成回连，立即上报档位，电量，工作模式
   {   
       switch(gear_mode)
       {
           case 1:
               ty_first_gear();
               break;
           case 2:
               ty_second_gear();
               break;
           case 3:
               ty_third_gear();
               break;
           default:
               break;
       }
       if(mode_selection.deodorization_mode == 1)
       {
           ty_deodorize_mode();       //DP上报除味模式
       }
       else
       {
           ty_keep_mode();       //DP上报保鲜模式
           
       }
       
       blue_connect_flag = 0;
   }
   if (blue_connect_status == 2 && blue_connect_flag == 0)
   {
       blue_connect_flag = 1;
   }
   if(power_report.rescheduling_mark)
   {
       uint32_t previous_time = 0;
       ty_rtc_get_time(&previous_time);
       power_report.power_report_timeout_two_hour = previous_time + TWO_HOUR;
       power_report.power_report_timeout_thirty = previous_time + THIRTY;
       power_report.rescheduling_mark = 0;
   }
   if(deodorizer_adc_value.adc_values > 3200)
   {
       uint32_t now_time = 0;
       ty_rtc_get_time(&now_time);
       if (now_time >= power_report.power_report_timeout_two_hour)
       {
           ty_electricity_report();
           power_report.rescheduling_mark = 1;
       }
   }
   else if (deodorizer_adc_value.adc_values < 3200)
   {
       /*uint32_t now_time = 0;
       ty_rtc_get_time(&now_time);
       if (now_time >= power_report.power_report_timeout_thirty)
       {
           ty_electricity_report();
           power_report.rescheduling_mark = 1;
       }*/
   }
   else
   {
       ;
   }


   return 0;
}


