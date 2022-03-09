/**
* @file tuya_pwm.c
* @author songjiang
* @email: songjiang.cao@tuya.com
* @brief tuya_pwm.c file is used to initialize ozone anion generators and anion generators
* @version 0.1
* @date 2021-11-20
*
* @copyright Copyright (c) tuya.inc 2021
*
*/

/*********************************************************************
********************************includes******************************
*********************************************************************/
#include "tuya_pwm.h"
#include "tuya_ble_log.h"
#include "nrf_drv_pwm.h"
#include "nrfx_pwm.h"


/*********************************************************************
****************************typedef define****************************
*********************************************************************/
ty_pwm_t ozone_pwm = {0};

ty_pwm_t negative_ion_pwm = {0};
ty_device_gear_mode ty_device_mode = {0};

/*********************************************************************
****************************function define***************************
*********************************************************************/

/**
* @function: ty_ozone_first_gear_pwm_init
* @brief: Ozone negative ion generator working mode
* @param[in]: void
* @return: success -> 0   fail -> else
*/
uint32_t ty_ozone_first_gear_pwm_init(void)
{
   uint32_t ret = 0;
   ozone_pwm.pin = OZONE_PWM_PIN;            //臭氧离子发生器连接引脚
   ozone_pwm.pin2 = NEGATIVE_ION_PWM_PIN;    //负离子发生器连接引脚
   ozone_pwm.polarity = 0;
   ozone_pwm.freq = 1000;
   ozone_pwm.duty = 50;
   negative_ion_pwm.duty = 50;

   ret = ty_pwm_init(&ozone_pwm);
   if(ret != 0){
       TUYA_APP_LOG_INFO("OZONE first pwm failed to initialize");
       return 1;
   }
   ty_pwm_start(&ozone_pwm);
   return 0;
}

/**
* @function: ty_ozone_second_gear_pwm_init
* @brief: Ozone anion generator working mode in second gear
* @param[in]: void
* @return: success -> 0   fail -> else
*/
uint32_t ty_ozone_second_gear_pwm_init(void)
{
   uint32_t ret = 0;
   ozone_pwm.pin = OZONE_PWM_PIN;
   ozone_pwm.pin2 = NEGATIVE_ION_PWM_PIN;
   ozone_pwm.polarity = 0;
   ozone_pwm.freq = 1000;
   ozone_pwm.duty = 70;
   negative_ion_pwm.duty = 70;

   ret = ty_pwm_init(&ozone_pwm);
   if (ret != 0)
   {
       TUYA_APP_LOG_INFO("OZONE second pwm failed to initialize");
       return 1;
   }
   ty_pwm_start(&ozone_pwm);

   return 0;
}

/**
* @function: ty_ozone_three_gear_pwm_init
* @brief: Ozone negative ion generator three working mode
* @param[in]: void
* @return: success -> 0   fail -> else
*/
uint32_t ty_ozone_three_gear_pwm_init(void)
{
   uint32_t ret = 0;
   ozone_pwm.pin = OZONE_PWM_PIN;
   ozone_pwm.pin2 = NEGATIVE_ION_PWM_PIN;
   ozone_pwm.polarity = 0;
   ozone_pwm.freq = 1000;
   ozone_pwm.duty = 100;
   negative_ion_pwm.duty = 100;
   
   ret = ty_pwm_init(&ozone_pwm);
   if (ret != 0)
   {
       TUYA_APP_LOG_INFO("OZONE three pwm failed to initialize");
       return 1;
   }
   ty_pwm_start(&ozone_pwm);

   return 0;
}

/**
* @function: ty_ozone_pwm_stop
* @brief: Ozone negative ion generator is not allowed to work
* @param[in]: void
* @return: success -> 0   fail -> else
*/
uint32_t ty_ozone_pwm_stop(void)
{
   uint32_t ret = 0;
   ozone_pwm.pin = NRFX_PWM_PIN_NOT_USED;
   ozone_pwm.pin2 = NEGATIVE_ION_PWM_PIN;
   
   ret = ty_pwm_init(&ozone_pwm);
   if (ret != 0)
   {
       TUYA_APP_LOG_INFO("OZONE stop failed to initialize");
       return 1;
   }
   ty_pwm_start(&ozone_pwm);
   return 0;

}

/**
* @function: ty_negative_ion_first_gear_pwm_init
* @brief: Anion generator working mode
* @param[in]: void
* @return: success -> 0   fail -> else
*/
uint32_t ty_negative_ion_first_gear_pwm_init(void)
{
   uint32_t ret = 0;
   negative_ion_pwm.pin = NRFX_PWM_PIN_NOT_USED;
   negative_ion_pwm.pin2 = NEGATIVE_ION_PWM_PIN;
   negative_ion_pwm.polarity = 0;
   negative_ion_pwm.freq = 1000;
   negative_ion_pwm.duty = 50;

   ret = ty_pwm_init(&negative_ion_pwm);
   if (ret != 0)
   {
       TUYA_APP_LOG_INFO("NEGATIVE_ION first pwm failed to initialize");
       return 1;
   }
   return 0;
}

/**
* @function: ty_negative_ion_second_gear_pwm_init
* @brief: Anion generator working mode in second gear
* @param[in]: void
* @return: success -> 0   fail -> else
*/
uint32_t ty_negative_ion_second_gear_pwm_init(void)
{
       uint32_t ret = 0;
       negative_ion_pwm.pin = NRFX_PWM_PIN_NOT_USED;
       negative_ion_pwm.pin2 = NEGATIVE_ION_PWM_PIN;
       negative_ion_pwm.polarity = 0;
       negative_ion_pwm.freq = 1000;
       negative_ion_pwm.duty = 70;

       ret = ty_pwm_init(&negative_ion_pwm);
       if (ret != 0)
       {
           TUYA_APP_LOG_INFO("NEGATIVE_ION second pwm failed to initialize");
           return 1;
       }
       ty_pwm_start(&negative_ion_pwm);
   return 0;
}

/**
* @function: ty_negative_ion_three_gear_pwm_init
* @brief: Anion generator working mode in three gears
* @param[in]: void
* @return: success -> 0   fail -> else
*/
uint32_t ty_negative_ion_three_gear_pwm_init(void)
{
   uint32_t ret = 0;
   negative_ion_pwm.pin = NRFX_PWM_PIN_NOT_USED;
   negative_ion_pwm.pin2 = NEGATIVE_ION_PWM_PIN;
   negative_ion_pwm.polarity = 0;
   negative_ion_pwm.freq = 1000;
   negative_ion_pwm.duty = 100;

   ret = ty_pwm_init(&negative_ion_pwm);
   if (ret != 0)
   {
       TUYA_APP_LOG_INFO("NEGATIVE_ION three pwm failed to initialize");
       return 1;
   }
   ty_pwm_start(&negative_ion_pwm);

   return 0;
}

/**
* @function: ty_negative_ion_pwm_stop
* @brief: The anion generator stops working
* @param[in]: void
* @return: success -> 0   fail -> else
*/
uint32_t ty_negative_ion_pwm_stop(void)
{
   uint32_t ret = 0; 
   negative_ion_pwm.pin = NRFX_PWM_PIN_NOT_USED;
   negative_ion_pwm.pin2 = NRFX_PWM_PIN_NOT_USED;
   negative_ion_pwm.polarity = 0;
   negative_ion_pwm.freq = 1000;
   negative_ion_pwm.duty = 0;
   ret = ty_pwm_init(&negative_ion_pwm);
   if(ret != 0){
       TUYA_APP_LOG_INFO("NEGATIVE_ION failed to pwm uninit");
       return 1;
   }
   ty_pwm_uninit(&negative_ion_pwm);        //关闭臭氧离子发生器和负离子发生器
   return 0;
}

/**
* @function: ty_get_device_gear_mode
* @brief: Obtain the working gear mode of ozone generator and anion generator
* @param[in]: void
* @return: success -> 0   fail -> else
*/
uint32_t ty_get_device_gear_mode(void)
{
   ty_device_mode.ozone_duty        = ozone_pwm.duty;                     //获取供电臭氧发生器引脚生成PWN占空比
   ty_device_mode.negative_ion_duty = negative_ion_pwm.duty;              //获取供电负离子发生器引脚生成PWN占空比
   if ((ty_device_mode.ozone_duty == 50) || (ty_device_mode.negative_ion_duty == 50))
   {
       ty_device_mode.device_mode = 1;
   }
   else if ((ty_device_mode.ozone_duty == 70) || (ty_device_mode.negative_ion_duty == 70))
   {
       ty_device_mode.device_mode = 2;
   }
   else if ((ty_device_mode.ozone_duty == 100) || (ty_device_mode.negative_ion_duty == 100))
   {
       ty_device_mode.device_mode = 3;
   }
   else
   {
       ;
   }

   return ty_device_mode.device_mode;
}

/**
* @function: ty_get_gear_mode
* @brief: Get the work gear
* @param[in]: void
* @return: success -> 0   fail -> else
*/
uint32_t ty_get_gear_mode(void)
{
   return ty_device_mode.device_mode;
}

