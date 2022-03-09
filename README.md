## Tuya IoTOS embedded demonstration Intelligent deodorizer

[English](./README.md) | [中文](./README_zh.md)

## Brief introduction

This demo is based on graffiti intelligent cloud platform, graffiti intelligent APP, loTOS Embeded Ble SDK, and uses graffiti BLE series modules to develop an intelligent deodorizer demo. Basic functions include: working mode switching, automatic gear adjustment, manual gear adjustment, equipment timing.



## Get started quickly

### Development environment building

- The IDE is installed according to the SDK requirements of the original manufacturer of the chip.

- The Tuya Ble SDK Demo Project download address is shown in the table below. Please refer to the README.md file under each branch to complete the project import.

  | Chip platform | Model    | Download address                                             |
  | :------------ | :------- | :----------------------------------------------------------- |
  | Nordic        | nrf52832 | [tuya_ble_sdk_Demo_Project_nrf52832.git](https://github.com/TuyaInc/tuya_ble_sdk_Demo_Project_nrf52832.git) |
  | Realtek       | RTL8762C | [tuya_ble_sdk_Demo_Project_rtl8762c.git](https://github.com/TuyaInc/tuya_ble_sdk_Demo_Project_rtl8762c.git) |
  | Telink        | TLSR825x | [tuya_ble_sdk_Demo_Project_tlsr8253.git](https://github.com/TuyaInc/tuya_ble_sdk_Demo_Project_tlsr8253.git) |
  | Silicon Labs  | BG21     | It is under planning. Please look forward to it              |
  | Beken         | BK3431Q  | [Tuya_ble_sdk_demo_project_bk3431q.git](https://github.com/TuyaInc/Tuya_ble_sdk_demo_project_bk3431q.git) |
  | Beken         | BK3432   | [tuya_ble_sdk_Demo_Project_bk3432.git](https://github.com/TuyaInc/tuya_ble_sdk_Demo_Project_bk3432.git) |
  | Cypress       | Psoc63   | [tuya_ble_sdk_Demo_Project_PSoC63.git](https://github.com/TuyaInc/tuya_ble_sdk_Demo_Project_PSoC63.git) |



- ### Compile and burn

  - code modifications


  1. Fill in `tuya_ble_sdk_demo.h` with the PID of the smart product created in the Tuya IoT workbench.

     ```
     #define TY_DEVICE_PID  "xxxxxxxx" 
     ```

     Replace `xxxxxxxx` with the PID.

  2. Fill in `tuya_ble_app_demo.c` with the requested authorization code (uuid and auth key).

     ```
     #define TY_DEVICE_DID         "zzzzzzzz" //16Bytes
     #define TY_DEVICE_AUTH_KEY    "yyyyyyyy" //32Bytes
     ```

     where `yyyyyyyy` is replaced with auth key and `zzzzzzzz` is replaced with uuid.

  - Compile and run the demo code

    Compile the code and download the firmware to the hardware (you may also need to download the stack and bootloader depending on the chip model you have chosen), watch the logs and use a third party Bluetooth debugging app (e.g. lightBlue for IOS) to scan the device to make sure it is broadcasting properly.



### Document introduction

### 

```shell
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
    │   │   │   ├── tuya_appointment_timing_function.c			Device timing
    │   │   │   ├── tuya_battery_check.c						The deodorizer completes the reconnection and reports the data.
    │   │   │   ├── tuya_ble_handle.c							DP point data processing
    │   │   │   ├── tuya_deodorizer_temperature_humidity.c		Automatic gear adjustment
    │   │   │   ├── tuya_key_process.c							Manual gear adjustment
    │   │   │   └── tuya_pwm.c									Deodorization mode and fresh-keeping mode
```

<br>

### Application entry

Entry file：\tuya_ble_sdk_demo\main.c

- `void tuya_ble_sdk_demo_init (void)` does some necessary initialization of Tuya loTOS Embeded Ble SDK, and this function is executed only once.



### DP point correlation

| Function name       | tuya_ble_dp_data_send                                        |
| ------------------- | ------------------------------------------------------------ |
| Function prototype  | tuya_ble_status_t tuya_ble_dp_data_send(<br/>uint32_t sn,<br/>tuya_ble_dp_data_send_type_t type,<br/>tuya_ble_dp_data_send_mode_t mode,<br/>tuya_ble_dp_data_send_ack_t ack,<br/>uint8_t *p_dp_data,<br/>uint32_t dp_data_len<br/>) ; |
| Functional Overview | Send DP data                                                 |
| Parameters          | sn[in]：Send serial number<br/>type[in]：Sending type, which can be divided into active sending and passive sending.<br/>mode[in]：Sending mode<br/>ack[in]：Do you need an answer flag<br/>p_dp_data[in]：DP data<br/>dp_data_len[in]：Data length, the maximum cannot exceed `TUYA_BLE_SEND_MAX_DATA_LEN-7`, where `TUYA_BLE_SEND_MAX_DATA_LEN` is configurable |
| Return value        | `TUYA_BLE_SUCCESS`：Sent successfully<br/>`TUYA_BLE_ERR_INVALID_PARAM`：Invalid parameter<br/>`TUYA_BLE_ERR_INVALID_STATE`：Sending is not supported in the current status, such as Bluetooth disconnect<br/>`TUYA_BLE_ERR_NO_MEM`：Memory request failed<br/>`TUYA_BLE_ERR_INVALID_LENGTH`：Data length error<br/>`TUYA_BLE_ERR_NO_EVENT`：Other errors |
| Remarks             | The application reports DP data to APP by calling this function |

Parameter description：

The graffiti cloud platform manages data in the way of dp points. The data generated by any device needs to be abstracted into the form of dp points. A complete DP point data consists of four parts (refer to the relevant contents on the loT workbench for details):

- Dp_id：1 byte, the dp_id serial number registered with the development platform.

- Dp_type：1 byte, DP point type.

  ```
  #define DT_RAW    0   //raw type;
  
  #define DT_BOOL   1   // boolean type;
  
  #define DT_VALUE  2   //numeric type; (whose range is specified at the time of iot platform registration)
  
  #define DT_STRING 3   //string type;
  
  #define DT_ENUM   4   // enumeration type;
  
  #define DT_BITMAP 5   //bitmap type;
  ```

- Dp_len：1 byte or 2 bytes.
- Dp_data：Data, dp_len bytes.

The data pointed to by the parameter p_data of the reporting function at this dp point must be assembled and reported in the following table format:

| Data for Dp point 1 |         |        |        |         | ~    | Data of Dp point n |         |        |        |         |
| ------------------- | ------- | ------ | ------ | ------- | ---- | ------------------ | ------- | ------ | ------ | ------- |
| 1                   | 2       | 3      | 4      | 5       | ~    | n                  | n+1     | n+2    | n+3    | n+4     |
| Dp_id               | Dp_type | Dp_len | Dp_len | Dp_data | ~    | Dp_id              | Dp_type | Dp_len | Dp_len | Dp_data |

When this function is called, the maximum length of the parameter len is `TUYA_BLE_REPORT_MAX_DP_DATA_LEN` (currently 255room3)



### Related documents

- [BLE SDK description](https://developer.tuya.com/en/docs/iot-device-dev/tuya-ble-sdk-user-guide?id=K9h5zc4e5djd9#title-17-tuya%20ble%20sdk%20callback%20event%20%E4%BB%8B%E7%BB%8D)

- [BLE SDK Demo description](https://developer.tuya.com/en/docs/iot-device-dev/tuya-ble-sdk-demo-instruction-manual?id=K9gq09szmvy2o)
- [Graffiti Demo Center](https://developer.tuya.com/en/demo)



### Technical support：

You can get support for graffiti in the following ways：

- [Developer Center](https://developer.tuya.com/en/)
- [Help center](https://support.tuya.com/en/help)
- [Technical support work order Center](https://service.console.tuya.com)

