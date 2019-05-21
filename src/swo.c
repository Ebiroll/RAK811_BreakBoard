//#include "tpiu.h"
#include "stm32l151xba.h"
#include "swo.h"

extern uint32_t SystemCoreClock;

/*!
 * \brief Initialize the SWO trace port for debug message printing
 * \param portBits Port bit mask to be configured
 * \param cpuCoreFreqHz CPU core clock frequency in Hz
 */
void SWO_Init(uint32_t portBits, uint32_t cpuCoreFreqHz) {
  uint32_t SWOSpeed = 115200; /* default 115200 baud rate */
  uint32_t SWOPrescaler = (cpuCoreFreqHz / SWOSpeed) - 1; /* SWOSpeed in Hz, note that cpuCoreFreqHz is expected to be match the CPU core clock */
 

    // Disable Jtag
    //RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;
    //AFIO->MAPR |= (2 << 24); // Disable JTAG to release TRACESWO
    DBGMCU->CR |= DBGMCU_CR_TRACE_IOEN; // Enable IO trace pins

    if (SystemCoreClock!=cpuCoreFreqHz) {
        printf("swo clock missmatch %d %d\n",SystemCoreClock,cpuCoreFreqHz);
    }

    printf("swo prescaler %d \n",SWOPrescaler);


  CoreDebug->DEMCR = CoreDebug_DEMCR_TRCENA_Msk; /* enable trace in core debug */
  *((volatile unsigned *)(ITM_BASE + 0x400F0)) = 0x00000002; /* "Selected PIN Protocol Register": Select which protocol to use for trace output (2: SWO NRZ, 1: SWO Manchester encoding) */
  *((volatile unsigned *)(ITM_BASE + 0x40010)) = SWOPrescaler; /* "Async Clock Prescaler Register". Scale the baud rate of the asynchronous output */
  *((volatile unsigned *)(ITM_BASE + 0x00FB0)) = 0xC5ACCE55; /* ITM Lock Access Register, C5ACCE55 enables more write access to Control Register 0xE00 :: 0xFFC */
  ITM->TCR = ITM_TCR_TraceBusID_Msk | ITM_TCR_SWOENA_Msk | ITM_TCR_SYNCENA_Msk | ITM_TCR_ITMENA_Msk; /* ITM Trace Control Register */
  ITM->TPR = ITM_TPR_PRIVMASK_Msk; /* ITM Trace Privilege Register */
  ITM->TER = portBits; /* ITM Trace Enable Register. Enabled tracing on stimulus ports. One bit per stimulus port. */
  *((volatile unsigned *)(ITM_BASE + 0x01000)) = 0x400003FE; /* DWT_CTRL */
  *((volatile unsigned *)(ITM_BASE + 0x40304)) = 0x00000100; /* Formatter and Flush Control Register */
}

#define portNo 0
/**
 * SWO_PrintChar()
 *
 * @brief 
 *   Checks if SWO is set up. If it is not, return,
 *    to avoid program hangs if no debugger is connected.
 *   If it is set up, print a character to the ITM_STIM register
 *    in order to provide data for SWO.
 * @param c The Character to be printed.
 * @notes   Additional checks for device specific registers can be added.
 */
void SWO_PrintChar(char c) {
 
#if(1)
//Use CMSIS_core_DebugFunctions. See core_cm3.h

 // Only ouput to debug registers if debug enabled
 if ((CoreDebug->DHCSR & CoreDebug_DHCSR_C_DEBUGEN_Msk)==CoreDebug_DHCSR_C_DEBUGEN_Msk) {

    volatile int timeout;
    
    /* Check if Trace Control Register (ITM->TCR at 0xE0000E80) is set */
    if ((ITM->TCR&ITM_TCR_ITMENA_Msk) == 0) { /* check Trace Control Register if ITM trace is enabled*/
        return; /* not enabled? */
    }
    /* Check if the requested channel stimulus port (ITM->TER at 0xE0000E00) is enabled */
    if ((ITM->TER & (1ul<<portNo))==0) { /* check Trace Enable Register if requested port is enabled */
        return; /* requested port not enabled? */
    }
    timeout = 5000; /* arbitrary timeout value */
    while (ITM->PORT[0].u32 == 0) {
        /* Wait until STIMx is ready, then send data */
        timeout--;
        if (timeout==0) {
        return; /* not able to send */
        }
    }
    ITM->PORT[0].u16 = 0x08 | (c<<8);

  }
  //ITM_SendChar (c);
 
#else
//Use Segger example. Basically same as CMSIS
 
  // Check if ITM_TCR.ITMENA is set
  if ((ITM_TCR & 1) == 0) {
    return;
  }
 
  // Check if stimulus port is enabled
  if ((ITM_ENA & 1) == 0) {
    return;
  }
 
  // Wait until STIMx FIFO is ready, then send data
//  while ((ITM_STIM_U8(0) & 1) == 0);
  while (!(ITM_STIM_U8(0) & ITM_STIM_FIFOREADY));    
  ITM_STIM_U8(0) = c;
  
//  while ((ITM_STIM_U32(0) & 1) == 0);
//  ITM_STIM_U32(0) = c;
 
#endif
}
 
/**
 * SWO_PrintString()
 *
 * @brief Print a string via SWO.
 * @param *s The string to be printed.
 *
 */
void SWO_PrintString(const char *s) {
  if ((CoreDebug->DHCSR & CoreDebug_DHCSR_C_DEBUGEN_Msk)==CoreDebug_DHCSR_C_DEBUGEN_Msk) {
    // Print out characters until \0
    while (*s) {
      SWO_PrintChar(*s++);
    }
  }
}
