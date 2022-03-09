/**
* @file tuya_key_process.c
* @author songjiang
* @email: songjiang.cao@tuya.com
* @brief tuya_key_process.c file is used for 5s distribution network and gear cycle adjustment
* @version 0.1
* @date 2021-11-20
*
* @copyright Copyright (c) tuya.inc 2021
*
*/

/*********************************************************************
********************************includes******************************
*********************************************************************/
#include "tuya_key_process.h"
#include "tuya_pwm.h"
#include "nrf_delay.h"
#include "tuya_ble_log.h"
#include "nrf_gpio.h"
#include "tuya_ble_data_handler.h"
#include "tuya_ble_main.h"
#include "tuya_ble_stdlib.h"
#include "tuya_ble_api.h"
#include "tuya_ble_type.h"

/*********************************************************************
******************************macro define****************************
*********************************************************************/
#define PIN_FAN 5
#define KEY_SHORT_PRESS 1
#define KEY_LONG_PRESS 2
#define PIN_BOOST 16

/*********************************************************************
****************************typedef define****************************
*********************************************************************/
ty_key_state ty_reset_key_state = {0};

ty_led_state ty_distribution_network_led_state = {0};

ty_ble_band_state ty_bluetooth_connection = {0};

ty_mode_selection_t mode_selection = {0};

tuya_dp_id_t tuya_dp_id = {0};

extern ty_device_gear_mode ty_device_mode;
tuya_ble_timer_t key_timer;
/*********************************************************************
****************************function define***************************
*********************************************************************/
uint32_t ty_device_default_status(void);
uint32_t ty_first_gear(void);
uint32_t ty_second_gear(void);
uint32_t ty_third_gear(void);

/**
* @function: ty_gpio_cfg_output
* @brief: Set GPIO mode
* @param[in]: void
* @return: success -> 0   fail -> else
*/
uint32_t ty_gpio_cfg_output(void)
{
   nrf_gpio_cfg_output(ONE_INDICATOR_LIGHT);
   nrf_gpio_cfg_output(TWO_INDICATOR_LIGHT);
   nrf_gpio_cfg_output(THREE_INDICATOR_LIGHT);
   nrf_gpio_cfg_output(PIN_FAN);
   nrf_gpio_cfg_output(PIN_BOOST);
   nrf_gpio_pin_write(PIN_BOOST, 1);
   nrf_gpio_cfg_input(KEY_PRESS, NRF_GPIO_PIN_PULLUP);

   return 0;
}

/**
* @function: ty_read_pin_levlel_state
* @brief: Read the pin level state
* @param[in]: void
* @return: success -> 0   fail -> else
*/
uint32_t ty_read_pin_levlel_state(uint32_t pin_num)
{
   uint32_t ret = 0;

   ret = nrf_gpio_pin_read(pin_num);

   return ret;
}

/**
* @function: ty_first_gear_indicator_light
* @brief: The first gear light is on
* @param[in]: void
* @return: success -> 0   fail -> else
*/
uint32_t ty_first_gear_indicator_light(void)
{
   ty_indicator_light_pin_set(ONE_INDICATOR_LIGHT, 0);
   ty_indicator_light_pin_set(TWO_INDICATOR_LIGHT, 1);
   ty_indicator_light_pin_set(THREE_INDICATOR_LIGHT, 1);
   return 0;
}

/**
* @function: ty_second_gear_indicator_light
* @brief: The light is on in second gear
* @param[in]: void
* @return: success -> 0   fail -> else
*/
uint32_t ty_second_gear_indicator_light(void)
{
   ty_indicator_light_pin_set(ONE_INDICATOR_LIGHT, 1);
   ty_indicator_light_pin_set(TWO_INDICATOR_LIGHT, 0);
   ty_indicator_light_pin_set(THREE_INDICATOR_LIGHT, 1);
   return 0;
}

/**
* @function: ty_third_gear_indicator_light
* @brief: The light is on in third gear
* @param[in]: void
* @return: success -> 0   fail -> else
*/
uint32_t ty_third_gear_indicator_light(void)
{
    TUYA_APP_LOG_DEBUG("333333");
   ty_indicator_light_pin_set(ONE_INDICATOR_LIGHT, 1);
   ty_indicator_light_pin_set(TWO_INDICATOR_LIGHT, 1);
   ty_indicator_light_pin_set(THREE_INDICATOR_LIGHT, 0);
   return 0;
}

/**
* @function: ty_delay_us
* @brief: The time delay function
* @param[in]: void
* @return: void
*/
void ty_delay_us(uint32_t time_us)
{
   nrf_delay_us(time_us);
   return;
}

/**
* @function: ty_indicator_light_pin_set
* @brief: Set the I/O port level
* @param[in]: void
* @return: success -> 0   fail -> else
*/
uint32_t ty_indicator_light_pin_set(uint32_t pin,uint32_t level)
{
   nrf_gpio_pin_write(pin, level);
   return 0;
}

/**
* @function: ty_indicator_light
* @brief: Select the corresponding indicator light according to the current position
* @param[in]: void
* @return: success -> 0   fail -> else
*/
uint32_t ty_indicator_light(void)
{
   uint32_t ret = 0;
   ret = ty_get_device_gear_mode();
   TUYA_APP_LOG_DEBUG("gear %d", ret);
   switch(ret)
   {
       case 1:
           ty_first_gear_indicator_light();
           break;
       case 2:
           ty_second_gear_indicator_light();
           break;
       case 3:
           ty_third_gear_indicator_light();
           break;
       default:
           break;
   }

   return 0;
}
/**
* @function: ty_device_key_gear_adjust
* @brief: Ozone generator and anion generator gear adjustment
* @param[in]: void
* @return: success -> 0   fail -> else
*/
uint32_t ty_device_key_gear_adjust(void)
{
    uint32_t gear_mode_ret = 0;
    uint32_t pin_level_state = 0;
    pin_level_state = ty_key_scan_process();
    if (pin_level_state == KEY_SHORT_PRESS)
    {
        if (pin_level_state == 1)
        {
            gear_mode_ret = ty_get_device_gear_mode();              //获取当前的档位

            switch (gear_mode_ret)
            {
                case 1:
                    if(mode_selection.fresh_keeping_mode == 1)
                    {
                        ty_negative_ion_second_gear_pwm_init();     //保鲜模式二档
                        ty_indicator_light();
                        ty_second_gear();                           //上报当前档位
                    }
                    if(mode_selection.deodorization_mode == 1)
                    {
                        ty_ozone_second_gear_pwm_init();            //除味模式二档
                        ty_indicator_light();
                        ty_second_gear();                           //上报当前档位
                    }

                    break;
                case 2:
                    if (mode_selection.fresh_keeping_mode == 1)
                    {
                        ty_negative_ion_three_gear_pwm_init();      //保鲜模式二档
                        ty_indicator_light();
                        ty_third_gear();                            //上报当前档位
                    }
                    if (mode_selection.deodorization_mode == 1)
                    {
                        ty_ozone_three_gear_pwm_init();             //除味模式二档
                        ty_indicator_light();
                        ty_third_gear();                            //上报当前档位
                    }

                    break;
                case 3:
                    if (mode_selection.fresh_keeping_mode == 1)
                    {
                        ty_negative_ion_first_gear_pwm_init();      //保鲜模式二档
                        ty_indicator_light();
                        ty_first_gear();                            //上报当前档位
                    }
                    if (mode_selection.deodorization_mode == 1)
                    {
                        ty_ozone_first_gear_pwm_init();             //除味模式二档
                        ty_indicator_light();
                        ty_first_gear();                            //上报当前档位
                    }

                    break;
                default:
                    break;
            }
            ty_sht3x_reset(); //i2c禁止初化
            ty_sht3x_init();  //温湿度传感器初始化

        }
    }
    ty_reset_key_state.key_short_press = 0;
    return 0;

}
/**
* @function: ty_key_scan_process
* @brief: Determine whether the key is long pressed
* @param[in]: void
* @return: success -> 0   fail -> else
*/
uint32_t ty_key_scan_process(void)
{
    uint32_t level = 0;
    static uint32_t key_up = 1;  //按键松开标志，按下为0，松开为1
    static uint32_t key_time_count = 0;
    level = ty_read_pin_levlel_state(KEY_PRESS);

    if (key_up && (level == 0)) // 按下
    {
        ty_delay_us(1); //去抖动
        if (level == 0) {
            key_up = 0;
        }
            
    }
    else if (level)     // 松开
    {
        if (key_up == 0)
        {
            key_up = 1;
            if (key_time_count < 10)
            {
                key_time_count = 0;
                ty_reset_key_state.key_short_press = 1;
                return KEY_SHORT_PRESS;
            }
            key_time_count = 0;
        }
    }
    if (key_up == 0)           //如果按键按下则按10ms开始计时
    {
        ty_delay_us(1);
        key_time_count++;
        if (key_time_count > 10)    //如果按下时间大于5秒，则返回长按
        {
            ty_reset_key_state.key_long_press = 1;
            return KEY_LONG_PRESS;
        }
    }
    return 0;
}


void ty_key_process_cb(void)
{
    uint8_t gear = 0;
    uint8_t pin_state = 0;
    static uint8_t count = 0, bat_count = 0;

    bat_count++;
    pin_state = ty_read_pin_levlel_state(KEY_PRESS);
    if (0 == pin_state) {
        count++;
        if (25 == count) {      // long press 5s 
            ty_equipment_network_mode();   //进入配网模式
            count = 0;
        }
        gear = ty_get_device_gear_mode();              //获取当前的档位
        switch (gear) {           
            case 1: 
                if(mode_selection.fresh_keeping_mode == 1)
                {
                    ty_negative_ion_second_gear_pwm_init();     //保鲜模式二档
                    ty_indicator_light();
                    ty_second_gear();                           //上报当前档位
                }
                if(mode_selection.deodorization_mode == 1)
                {
                    ty_ozone_second_gear_pwm_init();            //除味模式二档
                    ty_indicator_light();
                    ty_second_gear();                           //上报当前档位
                }

                break;
            case 2:
                if (mode_selection.fresh_keeping_mode == 1)
                {
                    ty_negative_ion_three_gear_pwm_init();      //保鲜模式二档
                    ty_indicator_light();
                    ty_third_gear();                            //上报当前档位
                }
                if (mode_selection.deodorization_mode == 1)
                {
                    ty_ozone_three_gear_pwm_init();             //除味模式二档
                    ty_indicator_light();
                    ty_third_gear();                            //上报当前档位
                }

                break;
            case 3:
                if (mode_selection.fresh_keeping_mode == 1)
                {
                    ty_negative_ion_first_gear_pwm_init();      //保鲜模式二档
                    ty_indicator_light();
                    ty_first_gear();                            //上报当前档位
                }
                if (mode_selection.deodorization_mode == 1)
                {
                    ty_ozone_first_gear_pwm_init();             //除味模式二档
                    ty_indicator_light();
                    ty_first_gear();                            //上报当前档位
                }

                break;
            default:
                break;
        }
    }

}

void ty_key_process()
{
    tuya_ble_timer_create(&key_timer, 200, TUYA_BLE_TIMER_REPEATED, ty_key_process_cb);
    tuya_ble_timer_start(key_timer);

    return ;
}
void ty_key_precess_off(void)
{
    tuya_ble_timer_stop(key_timer);
    tuya_ble_timer_delete(key_timer);

    return ;
}

/**
* @function: ty_equipment_network_mode
* @brief: Hold down for 5 seconds to enter the network mode
* @param[in]: void
* @return: success -> 0   fail -> else
*/
uint32_t ty_equipment_network_mode(void)
{
    unsigned char flag = 0;
    flag = ty_key_scan_process();
    if(flag)
    {
        ty_reset_key_state.key_long_press = 0;
        ty_distribution_network_led_state.power_on = 1;        //开启配网指示灯闪烁
        tuya_ble_device_unbond();
    }

    return 0;
}

/**
* @function: ty_switch_off_indicator_light
* @brief: All the gear indicators are off
* @param[in]: void
* @return: success -> 0   fail -> else
*/
uint32_t ty_switch_off_indicator_light(void)
{
    ty_indicator_light_pin_set(ONE_INDICATOR_LIGHT, 1);
    ty_indicator_light_pin_set(TWO_INDICATOR_LIGHT, 1);
    ty_indicator_light_pin_set(THREE_INDICATOR_LIGHT, 1);
    return 0;
}

/**
* @function: ty_led_display_process
* @brief: The network indicator is blinking
* @param[in]: void
* @return: success -> 0   fail -> else
*/
uint32_t ty_led_display_process(void)
{
    ty_bluetooth_connection.ble_band_state = tuya_ble_connect_status_get();
    if (ty_distribution_network_led_state.power_on == 1 && ty_bluetooth_connection.ble_band_state != 3)
    {
        switch (ty_distribution_network_led_state.led_display_step)
        {
            case 0:
                if (ty_bluetooth_connection.ble_band_state == 0)
                {
                    ty_distribution_network_led_state.led_display_step = 1;
                    ty_switch_off_indicator_light();
                    ty_indicator_light_pin_set(ty_distribution_network_led_state.indicator_gpio_pin, ty_distribution_network_led_state.led_polarity);
                }
                
                break;
            case 1:
                if (ty_distribution_network_led_state.count > 0 && ty_bluetooth_connection.ble_band_state == 0)
                {
                    ty_delay_us(100000);
                    ty_distribution_network_led_state.count--;
                    ty_distribution_network_led_state.led_polarity ^= 1;
                    ty_indicator_light_pin_set(ty_distribution_network_led_state.indicator_gpio_pin, ty_distribution_network_led_state.led_polarity);
                }
                else if (ty_bluetooth_connection.ble_band_state != 0)
                {
                    ty_device_default_status();
                    ty_indicator_light_pin_set(ty_distribution_network_led_state.indicator_gpio_pin, (ty_distribution_network_led_state.led_polarity | 1));
                    ty_distribution_network_led_state.led_display_step = 2;
                }
                else
                {
                    ty_indicator_light_pin_set(ty_distribution_network_led_state.indicator_gpio_pin, (ty_distribution_network_led_state.led_polarity | 1));
                    ty_distribution_network_led_state.led_display_step = 2;
                }
                break;
            case 2:
                ty_distribution_network_led_state.led_display_step = 0;
                ty_distribution_network_led_state.power_on = 0;
                ty_distribution_network_led_state.count = 75;
                break;
            default:
                break;
        }
    }
    //TUYA_APP_LOG_INFO("ty_distribution_network_led_state.count = %d", ty_distribution_network_led_state.count);
    return 0;
}

/**
* @function: ty_demo_para_init
* @brief: The distribution network status is initialized. Procedure
* @param[in]: void
* @return: success -> 0   fail -> else
*/
uint32_t ty_demo_para_init(void)
{
    ty_distribution_network_led_state.power_on = 1;
    ty_distribution_network_led_state.led_display_step = 0;
    ty_distribution_network_led_state.led_polarity = 1;
    ty_distribution_network_led_state.indicator_gpio_pin = ONE_INDICATOR_LIGHT;
    ty_distribution_network_led_state.count = 75;
    tuya_dp_id.low_power_alarm_id = 0x03;
    tuya_dp_id.low_power_alarm_type = 0x04;
    tuya_dp_id.gear_adjustment_id = 0x67;
    tuya_dp_id.gear_adjustment_type = 0x04;
    tuya_dp_id.power_display_id = 0x04;
    tuya_dp_id.power_display_type = 0x02;
    tuya_dp_id.work_mode_id = 0x65;
    tuya_dp_id.work_mode_type = 0x04;
    tuya_dp_id.sn_data = 0;

    return 0;
}


/**
* @function: ty_switch_on_light_flash
* @brief: The main switch is on and the three indicators are on and off for 3 times
* @param[in]: void
* @return: success -> 0   fail -> else
*/
uint32_t ty_switch_on_light_flash(void)
{
    
    uint32_t i = 0;
    for(i = 0; i < 3; i++)
    {
        ty_indicator_light_pin_set(ONE_INDICATOR_LIGHT, 0);
        ty_indicator_light_pin_set(TWO_INDICATOR_LIGHT, 0);
        ty_indicator_light_pin_set(THREE_INDICATOR_LIGHT, 0);
        ty_delay_us(500000);
        ty_indicator_light_pin_set(ONE_INDICATOR_LIGHT, 1);
        ty_indicator_light_pin_set(TWO_INDICATOR_LIGHT, 1);
        ty_indicator_light_pin_set(THREE_INDICATOR_LIGHT, 1);
        ty_delay_us(500000);
    }

    return 0;
}

/**
* @function: ty_fresh_keeping_mode
* @brief: Intelligent deodorizer fresh-keeping mode
* @param[in]: void
* @return: success -> 0   fail -> else
*/
uint32_t ty_fresh_keeping_mode(void)
{
    uint32_t ret = 0;
    uint32_t gear_mode_ret = 0;
    mode_selection.fresh_keeping_mode = 1;
    mode_selection.deodorization_mode = 0;

    gear_mode_ret = ty_get_device_gear_mode();

   ret = ty_ozone_pwm_stop();                                                  //关闭臭氧离子pwm初始化，关闭臭氧发生器
    if(ret != 0)
    {
        TUYA_APP_LOG_INFO("Failure to shut down the ozone ion generator");
        return 1;
    }
    switch (gear_mode_ret)
    {
        case 1:
            ty_negative_ion_first_gear_pwm_init();
            ty_first_gear_indicator_light();
            break;
        case 2:
            ty_negative_ion_second_gear_pwm_init();
            ty_second_gear_indicator_light();
            break;
        case 3:
            ty_negative_ion_three_gear_pwm_init();
            ty_third_gear_indicator_light();
            break;
        default:
            break;
    }
    ty_sht3x_reset(); //i2c禁止初化
    ty_sht3x_init();  //温湿度传感器初始化
    return 0;
}

/**
* @function: ty_deodorization_mode
* @brief: Intelligent deodorizer deodorizer mode
* @param[in]: void
* @return: success -> 0   fail -> else
*/
uint32_t ty_deodorization_mode(void)
{
    uint32_t ret = 0;
    mode_selection.deodorization_mode = 1;
    mode_selection.fresh_keeping_mode = 0;
    ret = ty_get_device_gear_mode();
    switch(ret)
    {
        case 1:
            ty_ozone_first_gear_pwm_init();
            ty_indicator_light();
            break;
        case 2:
            ty_ozone_second_gear_pwm_init();
            ty_indicator_light();
            break;
        case 3:
            ty_ozone_three_gear_pwm_init();
            ty_indicator_light();
            break;
        default:
            break;
    }
    ty_sht3x_reset();  //i2c禁止初化
    ty_sht3x_init();  //温湿度传感器初始化
    return 0;

}

/**
* @function: ty_turn_on_the_equipment
* @brief: Start the device, obtain the working mode and working gear, and turn on the corresponding indicator
* @param[in]: void
* @return: success -> 0   fail -> else
*/
uint32_t ty_turn_on_the_equipment(void)
{
    uint32_t gear_mode_ret = 0;
    gear_mode_ret = ty_get_device_gear_mode();
    switch (gear_mode_ret)
    {
        case 1:
            if (mode_selection.fresh_keeping_mode == 1)
            {
                ty_negative_ion_first_gear_pwm_init();
                ty_indicator_light();
            }
            if (mode_selection.deodorization_mode == 1)
            {
                ty_ozone_first_gear_pwm_init();
                ty_indicator_light();
            }
            break;
        case 2:
            if (mode_selection.fresh_keeping_mode == 1)
            {
                ty_negative_ion_second_gear_pwm_init();
                ty_indicator_light();
            }
            if (mode_selection.deodorization_mode == 1)
            {
                ty_ozone_second_gear_pwm_init();
                ty_indicator_light();
            }
            break;
        case 3:
            if (mode_selection.fresh_keeping_mode == 1)
            {
                ty_negative_ion_three_gear_pwm_init();
                ty_indicator_light();
            }
            if (mode_selection.deodorization_mode == 1)
            {
                ty_ozone_three_gear_pwm_init();
                ty_indicator_light();
            }
            break;
        default:
            break;
    }

    return 0;
}

/**
* @function: ty_device_default_status
* @brief: With one button on, the device is in odor mode 2 by default
* @param[in]: void
* @return: success -> 0   fail -> else
*/
uint32_t ty_device_default_status(void)
{
   ty_negative_ion_second_gear_pwm_init();  //开启负离子发生器二档模式
   ty_indicator_light();                   //二档对应指示灯亮起
   mode_selection.deodorization_mode = 0;  //除味模式标志位置0
   mode_selection.fresh_keeping_mode = 1;  //保鲜模式标志位置1
   ty_second_gear();
   return 0;
}

/**
* @function: ty_one_mode
* @brief: The first working mode of deodorant mode or insurance mode
* @param[in]: void
* @return: success -> 0   fail -> else
*/
uint32_t ty_one_mode(void)
{
   if (mode_selection.fresh_keeping_mode == 1)
   {
       ty_negative_ion_first_gear_pwm_init();
       ty_indicator_light();
   }
   else if (mode_selection.deodorization_mode == 1)
   {
       ty_ozone_first_gear_pwm_init();
       ty_indicator_light();
   }
   else
   {
       ;
   }
   ty_sht3x_reset();  //i2c禁止初化
   ty_sht3x_init();  //温湿度传感器初始化
   return 0;
}

/**
* @function: ty_two_mode
* @brief: Odor mode or insurance mode of the second working mode
* @param[in]: void
* @return: success -> 0   fail -> else
*/
uint32_t ty_two_mode(void)
{
   if (mode_selection.fresh_keeping_mode == 1)
   {
       ty_negative_ion_second_gear_pwm_init();
       ty_indicator_light();
   }
   else if (mode_selection.deodorization_mode == 1)
   {
       ty_ozone_second_gear_pwm_init();
       ty_indicator_light();
   }
   else
   {
       ;
   }
   ty_sht3x_reset();  //i2c禁止初化
   ty_sht3x_init();  //温湿度传感器初始化
   return 0;
}

/**
* @function: ty_three_mode
* @brief: Three working modes of deodorant mode or insurance mode
* @param[in]: void
* @return: success -> 0   fail -> else
*/
uint32_t ty_three_mode(void)
{
    if (mode_selection.fresh_keeping_mode == 1)
    {
        ty_negative_ion_three_gear_pwm_init();
        ty_indicator_light();
    }
    else if (mode_selection.deodorization_mode == 1)
    {
        ty_ozone_three_gear_pwm_init();
        ty_indicator_light();
    }
    else
    {
        ;
    }
   ty_sht3x_reset();  //i2c禁止初化
   ty_sht3x_init();  //温湿度传感器初始化
    return 0;
}

/**
* @function: ty_first_gear
* @brief: Adjust one gear to report DP
* @param[in]: void
* @return: success -> 0   fail -> else
*/
uint32_t ty_first_gear(void)
{
   uint32_t ret = 0;
   uint8_t dp_length = 5;
   uint8_t send_buf[5] = {0};
   send_buf[0] = tuya_dp_id.gear_adjustment_id;
   send_buf[1] = tuya_dp_id.gear_adjustment_type;
   send_buf[2] = 0x00;
   send_buf[3] = 0x01;
   send_buf[4] = 0x00;
   ret = tuya_ble_dp_data_send(tuya_dp_id.sn_data + 2, DP_SEND_TYPE_ACTIVE, DP_SEND_FOR_CLOUD_PANEL, DP_SEND_WITHOUT_RESPONSE, send_buf, dp_length);
   return 0;
}

/**
* @function: ty_second_gear
* @brief: Adjust the second gear to report DP
* @param[in]: void
* @return: success -> 0   fail -> else
*/
uint32_t ty_second_gear(void)
{
   uint32_t ret = 0;
   uint8_t dp_length = 5;
   uint8_t send_buf[5] = {0};
   send_buf[0] = tuya_dp_id.gear_adjustment_id;
   send_buf[1] = tuya_dp_id.gear_adjustment_type;
   send_buf[2] = 0x00;
   send_buf[3] = 0x01;
   send_buf[4] = 0x01;
   ret = tuya_ble_dp_data_send(tuya_dp_id.sn_data + 3, DP_SEND_TYPE_ACTIVE, DP_SEND_FOR_CLOUD_PANEL, DP_SEND_WITHOUT_RESPONSE, send_buf, dp_length);
   return 0;
}

/**
* @function: ty_third_gear
* @brief: Adjust the gear of the third gear to report DP
* @param[in]: void
* @return: success -> 0   fail -> else
*/
uint32_t ty_third_gear(void)
{
   ty_keep_mode();
   uint32_t ret = 0;
   uint8_t dp_length = 5;
   uint8_t send_buf[5] = {0};
   send_buf[0] = tuya_dp_id.gear_adjustment_id;
   send_buf[1] = tuya_dp_id.gear_adjustment_type;
   send_buf[2] = 0x00;
   send_buf[3] = 0x01;
   send_buf[4] = 0x02;
   ret = tuya_ble_dp_data_send(tuya_dp_id.sn_data + 4, DP_SEND_TYPE_ACTIVE, DP_SEND_FOR_CLOUD_PANEL, DP_SEND_WITHOUT_RESPONSE, send_buf, dp_length);
   return 0;
}

/**
* @function: ty_deodorize_mode
* @brief: In deodorant mode, DP reports
* @param[in]: void
* @return: success -> 0   fail -> else
*/

uint32_t ty_deodorize_mode(void)
{
   uint32_t ret = 0;
   uint8_t dp_length = 5;
   uint8_t send_buf[5] = {0};
   send_buf[0] = tuya_dp_id.work_mode_id;
   send_buf[1] = tuya_dp_id.work_mode_type;
   send_buf[2] = 0x00;
   send_buf[3] = 0x01;
   send_buf[4] = 0x00;

   ret = tuya_ble_dp_data_send(tuya_dp_id.sn_data + 5, DP_SEND_TYPE_ACTIVE, DP_SEND_FOR_CLOUD_PANEL, DP_SEND_WITHOUT_RESPONSE, send_buf, dp_length);
   return 0;
}

/**
* @function: ty_keep_mode
* @brief: Preservation mode DP report
* @param[in]: void
* @return: success -> 0   fail -> else
*/

uint32_t ty_keep_mode(void)
{
   
   
   uint32_t ret = 0;
   uint8_t dp_length = 5;
   uint8_t send_buf[5] = {0};
   send_buf[0] = tuya_dp_id.work_mode_id;
   send_buf[1] = tuya_dp_id.work_mode_type;
   send_buf[2] = 0x00;
   send_buf[3] = 0x01;
   send_buf[4] = 0x01;

   ret = tuya_ble_dp_data_send(tuya_dp_id.sn_data + 6, DP_SEND_TYPE_ACTIVE, DP_SEND_FOR_CLOUD_PANEL, DP_SEND_WITHOUT_RESPONSE, send_buf, dp_length);
   return 0;
}

