/**
 * @file        main.c
 * @brief       Secure Keyboard Example
 * @details
 */

/******************************************************************************
 *
 * Copyright (C) 2023 Analog Devices, Inc. All Rights Reserved.
 *
 * This software is proprietary and confidential to Analog Devices, Inc. and
 * its licensors.
 *
 ******************************************************************************
 *
 * Copyright (C) 2022-2023 Maxim Integrated Products, Inc., All rights Reserved.
 * 
 * This software is protected by copyright laws of the United States and
 * of foreign countries. This material may also be protected by patent laws
 * and technology transfer regulations of the United States and of foreign
 * countries. This software is furnished under a license agreement and/or a
 * nondisclosure agreement and may only be used or reproduced in accordance
 * with the terms of those agreements. Dissemination of this information to
 * any party or parties not specified in the license agreement and/or
 * nondisclosure agreement is expressly prohibited.
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL MAXIM INTEGRATED BE LIABLE FOR ANY CLAIM, DAMAGES
 * OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *
 * Except as contained in this notice, the name of Maxim Integrated
 * Products, Inc. shall not be used except as stated in the Maxim Integrated
 * Products, Inc. Branding Policy.
 *
 * The mere transfer of this software does not imply any licenses
 * of trade secrets, proprietary technology, copyrights, patents,
 * trademarks, maskwork rights, or any other form of intellectual
 * property whatsoever. Maxim Integrated Products, Inc. retains all
 * ownership rights.
 *
 ******************************************************************************/

/***** Includes *****/
#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include <mxc.h>

/***** Definitions *****/

/***** Globals *****/
mxc_skbd_keys_t keys = { 0, 0, 0, 0 };

volatile int key_pressed = 0;

/* keys mapping on the keyboard */
const char keyboard_map[16] = { 'C', 'D', 'E', 'F', 'B', '9', '6', '3',
                                '0', '8', '5', '2', 'A', '7', '4', '1' };
/***** Functions *****/

void keypadHandler()
{
    unsigned int status;

    /* Do what has to be done */
    MXC_SKBD_InterruptStatus(&status);

    if (MXC_F_SKBD_INTFL_PUSH & status) {
        MXC_SKBD_ReadKeys(&keys);
        key_pressed = 1;
        /* Clear interruption */
        MXC_SKBD_ClearInterruptStatus(MXC_F_SKBD_INTFL_PUSH);
    }

    return;
}

int main(void)
{
    mxc_skbd_config_t skb_cfg;
    uint16_t *key;
    uint8_t i, in, out;
    int result;

    printf("\n********** Secure Keyboard Example **********\n");

    skb_cfg.inputs = MXC_SKBD_KBDIO4 | MXC_SKBD_KBDIO5 | MXC_SKBD_KBDIO6 | MXC_SKBD_KBDIO7;
    skb_cfg.outputs = MXC_SKBD_KBDIO0 | MXC_SKBD_KBDIO1 | MXC_SKBD_KBDIO2 | MXC_SKBD_KBDIO3;
    skb_cfg.debounce = MXC_V_SKBD_CTRL1_DBTM_TIME10MS;
    skb_cfg.ioselect = 0;
    skb_cfg.irq_handler = (irq_handler_t)keypadHandler;
    skb_cfg.reg_erase = 1;

    MXC_SKBD_PreInit();

    if ((result = MXC_SKBD_Init(skb_cfg)) != E_NO_ERROR) {
        printf("Error in Initializing Secure Keyboard: %d", result);
        return E_UNINITIALIZED;
    }

    if ((result = MXC_SKBD_EnableInterruptEvents(MXC_SKBD_INTERRUPT_STATUS_PUSHIS)) != E_NO_ERROR) {
        printf("Error in Enabling Interrupt: %d", result);
        return E_UNINITIALIZED;
    }

    while (1) {
        if (key_pressed == 1) {
            key = &keys.key0;

            for (i = 0; i < 4; i++) {
                in = 0x0f & *key;
                out = (0xf0 & *key) >> 4;

                if (*key) {
                    printf("\n-Key Pressed: %c\n", keyboard_map[(in - 4) * 4 + out]);
                }

                *key = 0;
                key++;
            }

            key_pressed = 0;
        }
    }

    return 0;
}
