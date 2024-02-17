/**
 *       Device            :  PIC18F2550
 */

#if defined _18F2550

#include <xc.h>
#include "mcc.h"

#pragma config PLLDIV   = 5
#pragma config CPUDIV   = OSC2_PLL3
#pragma config FOSC     = INTOSC_HS
#pragma config WDT      = OFF
#pragma config LVP      = OFF
#pragma config BOR      = OFF
#pragma config MCLRE    = OFF
#pragma config PWRT     = OFF
#pragma config PBADEN   = OFF
#pragma config WDT      = OFF

void SYSTEM_Initialize(void)
{
    OSCCONbits.IRCF = 6;  // 4MHz
    TRISA = 0xff;
    IO_TRIS(PORT_LED) = OUTPUT;
}

#endif	// _18F2550
