/*
 *  (c) 2018 Microchip Technology Inc. and its subsidiaries.
 *
 *  Subject to your compliance with these terms, you may use Microchip
 *  software and any derivatives exclusively with Microchip products. It is
 *  your responsibility to comply with third party
 *  license terms applicable to your use of third party software (including
 *  open source software) that may accompany Microchip software.
 *
 *  THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
 *  EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY
 *  IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS
 *  FOR A PARTICULAR PURPOSE.
 *
 *  IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE,
 *  INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND
 *  WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP
 *  HAS BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO
 *  THE FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL
 *  CLAIMS IN ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT
 *  OF FEES, IF ANY, THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS
 *  SOFTWARE.
 */

#ifndef MCC_H
#define MCC_H

#if defined _16F1503
#define _XTAL_FREQ 500000
#define PORT_LED A5

#elif defined _12F1612
#define _XTAL_FREQ 500000
#define PORT_LED A5

#elif defined _16F18313
#define _XTAL_FREQ 1000000
#define PORT_LED A5

#elif defined _16F18857
#define _XTAL_FREQ 1000000
#define PORT_LED A0

#elif defined _18F2550
#define _XTAL_FREQ 4000000
#define PORT_LED A0

#else  // MCU SPECIFIC
#error unsupported MCU

#endif  // MCU SPECIFIC

#define INPUT   1
#define OUTPUT  0

#define HIGH    1
#define LOW     0

#define ANALOG      1
#define DIGITAL     0

#define PULL_UP_ENABLED      1
#define PULL_UP_DISABLED     0

#define IO_CAT(x, y) IO_CAT_(x, y)
#define IO_CAT_(x, y) x ## y
#define IO_CAT3(a, b, c) IO_CAT3_(a, b, c)
#define IO_CAT3_(a, b, c) a ## b ## c
#define IO_CAT4(a, b, c, d) IO_CAT4_(a, b, c, d)
#define IO_CAT4_(a, b, c, d) a ## b ## c ## d

#define IO_NAME(P) IO_CAT(IO_NAME_, P)

// get/set IO aliases
#define IO_TRIS(P)              IO_CAT4(TRIS, IO_NAME(P), bits.TRIS, P)
#define IO_LAT(P)               IO_CAT4(LAT,  IO_NAME(P), bits.LAT,  P)
#define IO_PORT(P)              IO_CAT4(PORT, IO_NAME(P), bits.R,    P)
#define IO_WPU(P)               IO_CAT4(WPU,  IO_NAME(P), bits.WPU,  P)
#define IO_OD(P)                IO_CAT4(ODCON,IO_NAME(P), bits.OD,   P)

/**
 * @Param
    none
 * @Returns
    none
 * @Description
    Initializes the device to the default states configured in the
 *                  MCC GUI
 * @Example
    SYSTEM_Initialize(void);
 */
void SYSTEM_Initialize(void);

/**
 * @Param
    none
 * @Returns
    none
 * @Description
    Interrupt on Change Handling routine
 * @Example
    PIN_MANAGER_IOC();
 */
void PIN_MANAGER_IOC(void);

#define IO_NAME_A0 A
#define IO_NAME_A1 A
#define IO_NAME_A2 A
#define IO_NAME_A3 A
#define IO_NAME_A4 A
#define IO_NAME_A5 A
#define IO_NAME_A6 A
#define IO_NAME_A7 A

#define IO_NAME_B0 B
#define IO_NAME_B1 B
#define IO_NAME_B2 B
#define IO_NAME_B3 B
#define IO_NAME_B4 B
#define IO_NAME_B5 B
#define IO_NAME_B6 B
#define IO_NAME_B7 B

#define IO_NAME_C0 C
#define IO_NAME_C1 C
#define IO_NAME_C2 C
#define IO_NAME_C3 C
#define IO_NAME_C4 C
#define IO_NAME_C5 C
#define IO_NAME_C6 C
#define IO_NAME_C7 C

#define IO_NAME_D0 D
#define IO_NAME_D1 D
#define IO_NAME_D2 D
#define IO_NAME_D3 D
#define IO_NAME_D4 D
#define IO_NAME_D5 D
#define IO_NAME_D6 D
#define IO_NAME_D7 D

#define IO_NAME_E0 E
#define IO_NAME_E1 E
#define IO_NAME_E2 E
#define IO_NAME_E3 E
#define IO_NAME_E4 E
#define IO_NAME_E5 E
#define IO_NAME_E6 E
#define IO_NAME_E7 E

#endif	// MCC_H
