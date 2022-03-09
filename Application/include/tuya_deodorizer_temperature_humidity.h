/**
* @file tuya_deodorizer_temperature_humidity.h
* @author songjiang
* @email: songjiang.cao@tuya.com
* @brief tuya_deodorizer_temperature_humidity.h file is used to collect temperature
* @version 0.1
* @date 2021-11-20
*
* @copyright Copyright (c) tuya.inc 2021
*
*/

#ifndef __TUYA_DEODORIZER_TEMPERATURE_HUMIDITY_H__
#define __TUYA_DEODORIZER_TEMPERATURE_HUMIDITY_H__

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
typedef struct
{
    int temperature_value;
    int humidity_value;
}Tuya_sht3x_collect_state;

/*枚举SHT3x命令列表*/
typedef enum
{
    /* 软件复位命令 */
    SOFT_RESET_CMD = 0X30A2,
    /* 周期测量模式读取数据命令 */
    READOUT_FOR_PREIODIC_MODE = 0xE000,
    /* 周期测量模式
    命名格式：Repeatability_MPS_CMD
    MPS：measurement per second */
    HIGH_0_5_CMD = 0x2032,
    MEDIUM_0_5_CMD = 0x2024,
    LOW_0_5_CMD = 0x202F,
    HIGH_1_CMD = 0x2130,
    MEDIUM_1_CMD = 0x2126,
    LOW_1_CMD = 0x212D,
    HIGH_2_CMD = 0x2236,
    MEDIUM_2_CMD = 0x2220,
    LOW_2_CMD = 0x222B,
    HIGH_4_CMD = 0x2334,
    MEDIUM_4_CMD = 0x2322,
    LOW_4_CMD = 0x2329,
    HIGH_10_CMD = 0x2737,
    MEDIUM_10_CMD = 0x2721,
    LOW_10_CMD = 0x272A,
    /* 单次测量模式
    命名格式：Repeatability_CS_CMD
    CS： Clock stretching */
    HIGH_ENABLED_CMD = 0x2C06,
    MEDIUM_ENABLED_CMD = 0x2C0D,
    LOW_ENABLED_CMD = 0x2C10,
    HIGH_DISABLED_CMD = 0x2400,
    MEDIUM_DISABLED_CMD = 0x240B,
    LOW_DISABLED_CMD = 0x2416,
} ty_sht3x_cmd;

/*********************************************************************
****************************function define***************************
*********************************************************************/
void ty_sht3x_reset(void);
uint32_t ty_sht3x_init(void);
static uint8_t generate_crc(const uint8_t *data, uint16_t count);
uint8_t tuya_sht3x_sensor_check_crc(const uint8_t *data, uint8_t count, uint8_t checksum);
uint32_t tuya_seneor_collect_process(void);
uint32_t tuya_first_gear(void);
uint32_t tuya_second_gear(void);
uint32_t tuya_third_gear(void);
uint32_t ty_gear_adjustment_using_temmperature_humidity(void);
void ty_sht3x_single_messure(void);


#ifdef __cplusplus
}
#endif

#endif

