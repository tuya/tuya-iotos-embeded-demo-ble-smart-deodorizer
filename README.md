## TuyaOS Embedded Smart Deodorizer

[English](./README.md) | [中文](./README_zh.md)

## Overview

In this demo, we will show you how to develop a smart deodorizer. Based on the [Tuya IoT Development Platform](https://iot.tuya.com/), we use Tuya's Bluetooth Low Energy (LE) module, SDK, and the Smart Life app to connect this thing to the cloud. These basic features are supported: switching between working modes, manual or automatic adjustment of working levels, and scheduled tasks.



## Get started

### Set up IDE

- Install the integrated development environment (IDE) as per the requirements of the original chip SDK.

- Find the download URL of the Tuya Bluetooth LE SDK Demo Project from the following table. Refer to the `README.md` file under each branch to import the project.

  | Chip platform | Model | Download URL |
  | :----------- | :------- | :----------------------------------------------------------- |
  | Nordic       | nrf52832 | [tuya_ble_sdk_Demo_Project_nrf52832.git](https://github.com/TuyaInc/tuya_ble_sdk_Demo_Project_nrf52832.git) |
  | Realtek      | RTL8762C | [tuya_ble_sdk_Demo_Project_rtl8762c.git](https://github.com/TuyaInc/tuya_ble_sdk_Demo_Project_rtl8762c.git) |
  | Telink       | TLSR825x | [tuya_ble_sdk_Demo_Project_tlsr8253.git](https://github.com/TuyaInc/tuya_ble_sdk_Demo_Project_tlsr8253.git) |
  | Silicon Labs | BG21 | Coming soon |
  | Beken        | BK3431Q  | [Tuya_ble_sdk_demo_project_bk3431q.git](https://github.com/TuyaInc/Tuya_ble_sdk_demo_project_bk3431q.git) |
  | Beken        | BK3432   | [tuya_ble_sdk_Demo_Project_bk3432.git](https://github.com/TuyaInc/tuya_ble_sdk_Demo_Project_bk3432.git) |
  | Cypress      | Psoc63   | [tuya_ble_sdk_Demo_Project_PSoC63.git](https://github.com/TuyaInc/tuya_ble_sdk_Demo_Project_PSoC63.git) |



### Compilation and flashing

- Edit code

   1. In `tuya_ble_sdk_demo.h`, specify the PID of the product you created on the Tuya IoT Development Platform.

     ```
      #define TY_DEVICE_PID     "xxxxxxxx"
     ```

      Change `xxxxxxxx` to the PID.

   2. In `tuya_ble_app_demo.c`, specify the `authkey` and `UUID`.

     ```
     #define TY_DEVICE_DID         "zzzzzzzz" // 16 bytes
     #define TY_DEVICE_AUTH_KEY    "yyyyyyyy" // 32 bytes
     ```

     Change `yyyyyyyy` to your authkey and `zzzzzzzz` to your UUID.

- Compile code

  Compile the edited code, download the code to the hardware, and run it. You may need to download the stack and bootloader depending on your chip models. Check the logs and use the third-party Bluetooth debugging app (such as LightBlue for iOS) to verify the Bluetooth broadcast.



### File introduction



```shell
Application/
└── tuya_ble_sdk_demo
    ├── app
    ├── board
    │   ├── nRF52832
    │   │   ├── include
    │   │   │   ├── tuya_appointment_timing_function.h
    │   │   │   ├── tuya_battery_check.h
    │   │   │   ├── tuya_ble_handle.h
    │   │   │   ├── tuya_deodorizer_temperature_humidity.h
    │   │   │   ├── tuya_key_process.h
    │   │   │   └── tuya_pwm.h
    │   │   ├── src
    │   │   │   ├── tuya_appointment_timing_function.c					Set device schedule tasks.
    │   │   │   ├── tuya_battery_check.c								The deodorizer is connected and reports data.
    │   │   │   ├── tuya_ble_handle.c									Process data point (DP) data.
    │   │   │   ├── tuya_deodorizer_temperature_humidity.c				Automatically adjust the working level.
    │   │   │   ├── tuya_key_process.c									Manually adjust the working level.
    │   │   │   └── tuya_pwm.c											The deodorization mode and preservation mode.
```

<br>

### Entry to application

Entry file: \tuya_ble_sdk_demo\main.c

- `void tuya_ble_sdk_demo_init(void)` is run to initialize the SDK. This function is run only once.



### Data point (DP)

| Function name | tuya_ble_dp_data_send |
| -------- | ------------------------------------------------------------ |
| Function prototype | tuya_ble_status_t tuya_ble_dp_data_send(<br/>uint32_t sn,<br/>tuya_ble_dp_data_send_type_t type,<br/>tuya_ble_dp_data_send_mode_t mode,<br/>tuya_ble_dp_data_send_ack_t ack,<br/>uint8_t *p_dp_data,<br/>uint32_t dp_data_len<br/>) ; |
| Feature overview | Send DP data to the cloud. |
| Parameters | sn[in]: the serial number. <br/>type[in]: the type of data sending, which can be a proactive notification or a follow-up response. <br/>mode[in]: the delivery mode. <br/>ack[in]: whether an ACK message is required. <br/>p_dp_data[in]: the DP data. <br/>dp_data_len[in]: the length of data, no more than `TUYA_BLE_SEND_MAX_DATA_LEN-7`. `TUYA_BLE_SEND_MAX_DATA_LEN` can be set. |
| Return value | `TUYA_BLE_SUCCESS`: succeeded. <br/>`TUYA_BLE_ERR_INVALID_PARAM`: invalid parameter. <br/>`TUYA_BLE_ERR_INVALID_STATE`: failed to send data due to the current Bluetooth connection, such as Bluetooth disconnected. <br/>`TUYA_BLE_ERR_NO_MEM`: failed to request memory allocation. <br/>`TUYA_BLE_ERR_INVALID_LENGTH`: data length error. <br/>`TUYA_BLE_ERR_NO_EVENT`: other errors. |
| Notes | The application calls this function to send DP data to the mobile app. |

Parameter description:

The [Tuya IoT Development Platform](https://iot.tuya.com/) manages data through DPs. The data generated by any device is abstracted into a DP. DP data consists of four parts, as described below.

- `Dp_id`: The 1-byte parameter indicates the ID of a DP defined on the Tuya IoT Development Platform.

- `Dp_type`: The 1-byte parameter indicates the data type.

  ```
  #define DT_RAW    0   // Raw type.
  
   #define DT_BOOL   1   // Boolean type.
  
  #define DT_VALUE  2   // Value type. The value range is specified when a DP of value type is created on the Tuya IoT Development Platform.
  
  #define DT_STRING 3   // String type.
  
  #define DT_ENUM   4   // Enum type.
  
  #define DT_BITMAP 5   // Bitmap type.
  ```

- `Dp_len`: It can be one or two bytes in length.
- `Dp_data`: the DP data with the length of `dp_len`.

The data that the parameter `p_data` points to must be packaged in the following format for reporting.

| DP 1 data |         |        |        |         | — | DP n data |         |        |        |         |
| ----------- | ------- | ------ | ------ | ------- | ---- | ----------- | ------- | ------ | ------ | ------- |
| 1 | 2 | 3 | 4 | 5 | — | n | n+1 | n+2 | n+3 | n+4 |
| Dp_id | Dp_type | Dp_len | Dp_len | Dp_data | — | Dp_id | Dp_type | Dp_len | Dp_len | Dp_data |

When this function is called, the maximum data length is `TUYA_BLE_REPORT_MAX_DP_DATA_LEN`, which is `255+3` currently.



### References

- [Bluetooth SDK Guide](https://developer.tuya.com/en/docs/iot-device-dev/tuya-ble-sdk-user-guide?id=K9h5zc4e5djd9#title-17-tuya%20ble%20sdk%20callback%20event%20%E4%BB%8B%E7%BB%8D)

- [Bluetooth SDK Demo Overview](https://developer.tuya.com/en/docs/iot-device-dev/tuya-ble-sdk-demo-instruction-manual?id=K9gq09szmvy2o)

- [Tuya Project Hub](https://developer.tuya.com/en/demo)



### Technical support

You can get technical support from Tuya in the following services:

- [Tuya for Developer](https://developer.tuya.com/en/)

- [Help Center](https://support.tuya.com/en/help)

- [Service & Support](https://service.console.tuya.com/8/3/list?source=support_center)
