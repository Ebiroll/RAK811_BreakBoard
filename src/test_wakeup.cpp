#include "WakeUp.h"
#include "stm32l1xx_ll_pwr.h"

extern "C" void  test_wakeup() {
        //Set wakeup time for 2 seconds
        WakeUp::set_ms(2000);
        
        //Enter deepsleep, the program won't go beyond this point until it is woken up
        //HAL_PWR_EnterSLEEPMode(PWR_LOWPOWERREGULATOR_ON, PWR_SLEEPENTRY_WFx);
        LL_PWR_SetPowerMode(LL_PWR_MODE_STANDBY);
        //deepsleep();
}