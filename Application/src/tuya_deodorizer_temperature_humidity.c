/**
* @file tuya_deodorizer_temperature_humidity.c
* @author songjiang
* @email: songjiang.cao@tuya.com
* @brief tuya_deodorizer_temperature_humidity.c file is used to collect temperature
* @version 0.1
* @date 2021-11-20
*
* @copyright Copyright (c) tuya.inc 2021
*
*/

/*********************************************************************
********************************includes******************************
*********************************************************************/
#include "tuya_ble_log.h"
#include "tuya_ble_type.h"
#include "ty_system.h"
#include "ty_i2c.h"
#include "tuya_deodorizer_temperature_humidity.h"
#include "nrf_drv_twi.h"
#include "nrf_gpio.h"
#include "nrf_delay.h"
#include "app_util_platform.h"
#include "tuya_pwm.h"
#include "tuya_key_process.h"
#include "tuya_ble_main.h"
#include "tuya_ble_api.h"

/*********************************************************************
******************************macro define****************************
*********************************************************************/
#define  ADDR_SHT3X 0x44
extern ty_mode_selection_t mode_selection;


/****************************
 * STRUCT
 */
Tuya_sht3x_collect_state tuya_sht3x_collect_state = {0};

/**
* @function: ty_sht3x_reset
* @brief: Do not initialize the T/H sensor
* @param[in]: void
* @return: void
*/
void ty_sht3x_reset(void)
{
    ty_i2c_uninit();    //禁止I2C初始化
    return;
}

/**
* @function: ty_sht3x_single_messure
* @brief: Turn on the single measurement mode of temperature and humidity sensor
* @param[in]: void
* @return: void
*/
void ty_sht3x_single_messure(void)
{
    uint32_t ret = 0;
    uint8_t tx_cmd_buffer[2] = {0};
    tx_cmd_buffer[0] = (uint8_t)(LOW_DISABLED_CMD >> 8);
    tx_cmd_buffer[1] = (uint8_t)(LOW_DISABLED_CMD);
    ty_i2c_init();                                             //i2c初始化
    ty_i2c_start();                                            //使能i2c
    ret = ty_i2c_send((uint8_t)ADDR_SHT3X, (uint8_t *)tx_cmd_buffer, 2);

    return;
}


/**
* @function: ty_sht3x_init
* @brief: The T/H sensor is initialized
* @param[in]: void
* @return: success -> 0   fail -> else
*/
uint32_t ty_sht3x_init(void)
{
    uint32_t ret = 0;
    uint8_t cmd_buffer[2] = {0};
    uint8_t tx_cmd_buffer[2] = {0};
    
    tx_cmd_buffer[0] = (uint8_t)(READOUT_FOR_PREIODIC_MODE >> 8);
    tx_cmd_buffer[1] = (uint8_t)(READOUT_FOR_PREIODIC_MODE);

    cmd_buffer[0] = (uint8_t)(LOW_10_CMD >> 8);
    cmd_buffer[1] = (uint8_t)(LOW_10_CMD);
    ty_i2c_init();                                             //i2c初始化
    ty_i2c_start();                                            //使能i2c
    ret = ty_i2c_send((uint8_t)ADDR_SHT3X, (uint8_t*)cmd_buffer, 2);
    if(ret != 0)
    {
        TUYA_APP_LOG_INFO("ty_i2c_send cmd_buffer to failed");
    }
    ret = ty_i2c_send((uint8_t)ADDR_SHT3X, (uint8_t *)tx_cmd_buffer, 2);
    if(ret != 0)
    {
        TUYA_APP_LOG_INFO("ty_i2c_send tx_cmd_buffer to failed");
    }
    return  0;
}

/**
* @function: generate_crc
* @brief: Computes the value of CRC
* @param[in]: void
* @return: success -> 0   fail -> else
*/
static uint8_t generate_crc(const uint8_t *data, uint16_t count)
{
    uint16_t current_byte = 0;
    uint8_t crc = 0xFF;
    uint8_t crc_bit = 0;

    for (current_byte = 0; current_byte < count; ++current_byte)
    {
        crc ^= data[current_byte];
        for (crc_bit = 8; crc_bit > 0; --crc_bit)
        {
            if (crc & 0x80)
                crc = (crc << 1) ^ 0x31;
            else
                crc = (crc << 1);
        }
    }
    return crc;
}

/**
* @function: tuya_sht3x_sensor_check_crc
* @brief: Verify that the collected temperature and humidity data are correct
* @param[in]: void
* @return: success -> 0   fail -> else
*/
uint8_t tuya_sht3x_sensor_check_crc(const uint8_t* data, uint8_t count, uint8_t checksum)
{
    if(generate_crc(data, count) != checksum)
        return 1;
    return 0;
}

/**
* @function:tuya_seneor_collect_process
* @brief: Temperature and humidity data acquisition function, periodically obtain
* @param[in]: void
* @return: success -> 0   fail -> else
*/
uint32_t tuya_seneor_collect_process(void)
{
    int32_t temperature_raw = 0;
    uint32_t humidity_raw = 0;
    uint8_t sensor_buf[6] = {0};
    unsigned ret = 0;
    static uint8_t count = 0; 
	int8_t temp_buf[8] = {0x01, 0x02, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00};
    uint8_t hum_buf[8] = {0x02, 0x02, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00};
    // ty_sht3x_reset();  //i2c禁止初化
    // ty_sht3x_init();  //温湿度传感器初始化
    ret = ty_i2c_recv((uint8_t)ADDR_SHT3X, (uint8_t*)sensor_buf, 6);
    if(ret != 0)
    {
        TUYA_APP_LOG_INFO("ty_i2c_recv to failed");
    }
    temperature_raw = (uint32_t)(sensor_buf[0]<<8 | sensor_buf[1]);    //温度数据拼接
    humidity_raw = (uint32_t)(sensor_buf[3]<<8 | sensor_buf[4]);       //湿度数据拼接
    if(tuya_sht3x_sensor_check_crc(sensor_buf, 2, sensor_buf[2]))
    {
        TUYA_APP_LOG_INFO("tuya sht3x sensor temperature check failed sensor_buf[2] = %d", sensor_buf[2]);
        return 1;
    }
    else
    {
        tuya_sht3x_collect_state.temperature_value = (175 * temperature_raw / 65535 - 45);
        // TUYA_APP_LOG_INFO("Temperature = %d", tuya_sht3x_collect_state.temperature_value);
    }
    if(tuya_sht3x_sensor_check_crc(sensor_buf + 3, 2, sensor_buf[5]))
    {
        TUYA_APP_LOG_INFO("tuya sht3x sensor humidity check failed");
        return 1;
    }
    else
    {
        tuya_sht3x_collect_state.humidity_value = (100.0 * (double)humidity_raw / 65535.0);
        // TUYA_APP_LOG_INFO("Humidity = %d",tuya_sht3x_collect_state.humidity_value);
    }

    /* 过滤错误数据 */
    if((tuya_sht3x_collect_state.temperature_value < -20) || (tuya_sht3x_collect_state.temperature_value > 125) || \
		(tuya_sht3x_collect_state.humidity_value < 0) || (tuya_sht3x_collect_state.humidity_value > 100)) {
			return 1;
		}
    
	hum_buf[4] = (tuya_sht3x_collect_state.humidity_value >> 24) & 0xff;
    hum_buf[5] = (tuya_sht3x_collect_state.humidity_value >> 16) & 0xff;
    hum_buf[6] = (tuya_sht3x_collect_state.humidity_value >> 8) & 0xff;
    hum_buf[7] = (tuya_sht3x_collect_state.humidity_value >> 0) & 0xff;

    temp_buf[4] = (tuya_sht3x_collect_state.temperature_value >> 24) & 0xff;
    temp_buf[5] = (tuya_sht3x_collect_state.temperature_value >> 16) & 0xff;
    temp_buf[6] = (tuya_sht3x_collect_state.temperature_value >> 8) & 0xff;
    temp_buf[7] = (tuya_sht3x_collect_state.temperature_value >> 0) & 0xff; 
    count++;
	if (3 == tuya_ble_connect_status_get() && 10 == count) {
        count = 0;
        tuya_ble_dp_data_send(0, DP_SEND_TYPE_ACTIVE, DP_SEND_FOR_CLOUD_PANEL, DP_SEND_WITHOUT_RESPONSE, hum_buf, sizeof(hum_buf));
        tuya_ble_dp_data_send(0, DP_SEND_TYPE_ACTIVE, DP_SEND_FOR_CLOUD_PANEL, DP_SEND_WITHOUT_RESPONSE, temp_buf, sizeof(temp_buf));
    }
    return 0;
}

/**
* @function: tuya_first_gear
* @brief: Smart deodorizer first gear
* @param[in]: void
* @return: success -> 0   fail -> else
*/
uint32_t tuya_first_gear(void)
{
    ty_ozone_first_gear_pwm_init();
    ty_get_device_gear_mode();
    ty_indicator_light();

    return 0;
}

/**
* @function: tuya_second_gear
* @brief: Smart deodorizer second gear
* @param[in]: void
* @return: success -> 0   fail -> else
*/
uint32_t tuya_second_gear(void)
{
    ty_ozone_second_gear_pwm_init();
    ty_get_device_gear_mode();
    ty_indicator_light();

    return 0;
}

/**
* @function: tuya_third_gear
* @brief: Smart deodorizer three gear
* @param[in]: void
* @return: success -> 0   fail -> else
*/
uint32_t tuya_third_gear(void)
{
    ty_ozone_three_gear_pwm_init();
    ty_get_device_gear_mode();
    ty_indicator_light();

    return 0;
}

/**
* @function: ty_gear_adjustment_using_temmperature_humidity
* @brief: Automatic gear adjustment according to different temperature and humidity
* @param[in]: void
* @return: success -> 0   fail -> else
*/
uint32_t ty_gear_adjustment_using_temmperature_humidity(void)
{
    int32_t temperature = 0;
    int32_t humidity = 0;
    temperature = (int32_t)tuya_sht3x_collect_state.temperature_value;
    humidity = (int32_t)tuya_sht3x_collect_state.humidity_value;

    if ((temperature > 10) && (temperature < 15) && (humidity > 60) && (humidity < 65))
    {
        if (mode_selection.fresh_keeping_mode == 1)
        {
            ty_ozone_three_gear_pwm_init();
            ty_indicator_light();
            ty_third_gear();
        }
        if (mode_selection.deodorization_mode == 1)
        {
            ty_ozone_three_gear_pwm_init();
            ty_negative_ion_three_gear_pwm_init();
            ty_indicator_light();
            ty_third_gear();
        }
    }
    else if ((temperature > 15) && (temperature < 22) && (humidity > 30) && (humidity < 50))
    {
        if (mode_selection.fresh_keeping_mode == 1)
        {
            ty_ozone_second_gear_pwm_init();
            ty_indicator_light();
            ty_second_gear();
        }
        if (mode_selection.deodorization_mode == 1)
        {
            ty_ozone_second_gear_pwm_init();
            ty_negative_ion_second_gear_pwm_init();
            ty_indicator_light();
            ty_second_gear();
        }
    }
    else if ((temperature > 22) && (temperature < 29) && (humidity > 55) && (humidity < 75))
    {
        if (mode_selection.fresh_keeping_mode == 1)
        {
            ty_ozone_first_gear_pwm_init();
            ty_indicator_light();
            ty_first_gear();
        }
        if (mode_selection.deodorization_mode == 1)
        {
            ty_ozone_first_gear_pwm_init();
            ty_negative_ion_first_gear_pwm_init();
            ty_indicator_light();
            ty_first_gear();
        }
    }
    else
    {
        ;
    }

    return 0;
}
