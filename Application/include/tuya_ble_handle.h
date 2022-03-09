/**
* @file tuya_ble_handle.h
* @author songjiang
* @email: songjiang.cao@tuya.com
* @brief tuya_ble_handle.h file function is DP processing
* @version 0.1
* @date 2021-11-20
*
* @copyright Copyright (c) tuya.inc 2021
*
*/

#ifndef __TUYA_BLE_HANDLE_H_
#define __TUYA_BLE_HANDLE_H_

#ifdef __cplusplus
extern "C"
{
#endif

/*********************************************************************
******************************macro define****************************
*********************************************************************/
typedef unsigned char uint8_t;

/*********************************************************************
****************************function define***************************
*********************************************************************/
int ty_dp_processing(uint8_t* dp_data);

#ifdef __cplusplus
}
#endif

#endif
