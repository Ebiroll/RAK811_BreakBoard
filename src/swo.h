
#include <stdint.h> 
#ifdef TRACE_SWO
void SWO_Init(uint32_t portBits, uint32_t cpuCoreFreqHz);
void SWO_PrintString(const char *s);
void SWO_PrintChar(char c);
#else
void SWO_Init(uint32_t portBits, uint32_t cpuCoreFreqHz);
void SWO_PrintString(const char *s);
void SWO_PrintChar(char c);
#endif

