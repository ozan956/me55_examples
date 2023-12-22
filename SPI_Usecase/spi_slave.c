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

/*******************************      INCLUDES    ****************************/
#include <stdio.h>
#include <string.h>

#include "mxc_delay.h"

#include "spi_config.h"

/*******************************      DEFINES     ****************************/

/******************************* Type Definitions ****************************/

/*******************************     Variables    ****************************/
static unsigned char g_rxBuff[TEST_BUFF_SIZE];
static mxc_spi_req_t g_slave_req = { 0 };

static volatile int is_transaction_done = 0;

/******************************* Static Functions ****************************/

void SPIx_IRQHandler(void)
{
	MXC_SPI_RevA2_Handler(SPIx_SLAVE);
}

void spi_transaction_cb(void *req, int error_code)
{
    (void)error_code;
    (void)req;

    is_transaction_done = 1;
}

/******************************* Public Functions ****************************/
int spi_slave_init(void)
{
    int ret = 0;
    int masterMode = MXC_SPI_TYPE_SLAVE;
    int quadModeUsed = MXC_SPI_INTERFACE_STANDARD;
    int numSlaves = 1;
    int ssPolarity = 0b0000;


    // SPI pins configuration
    /* In target(slave) mode, SPIn_SS0 is a dedicated input that indicates
     * an external controller must start communication. Other target
     * select signals into the peripheral are ignored in target mode.*/
    mxc_spi_pins_t spi_pins = {
        .ss0 = true,    // TS0
        .ss1 = false,   // TS1
        .ss2 = false,   // TS2
        .ss3 = false,   // TS3
        .vddioh = true,
        .clock = true,
        .drvstr = MXC_GPIO_DRVSTR_0
    };

    // Initialize SPI hardware
    ret = MXC_SPI_Init(SPIx_SLAVE, masterMode, quadModeUsed, numSlaves, ssPolarity,
                       SPI_BAUD_RATE, spi_pins);
    if (ret != 0) {
        return ret;
    }

    // SPI configuration
    mxc_spi_cfg_t cfg_slave = {
        .spi = SPIx_SLAVE,
        .clk_mode = MXC_SPI_CLKMODE_0, // CPOL: 0, CPHA: 0
        .frame_size = 8,
        .use_dma_tx = false,
        .use_dma_rx = false,
        .dma = MXC_DMA
    };

    ret = MXC_SPI_Config(&cfg_slave);
    if (ret) {
        return ret;
    }

    MXC_SPI_SetDataSize(SPIx_SLAVE, 8);
    MXC_SPI_SetWidth(SPIx_SLAVE, SPI_WIDTH_STANDARD);

    return ret;
}

int spi_slave_send(unsigned char *src, unsigned int srcLen)
{
    int ret = 0;

    // Populate SPI transaction request structure
    g_slave_req.spi = SPIx_SLAVE;
    g_slave_req.ssIdx = 0;
    g_slave_req.ssDeassert = 1;
    g_slave_req.txData = (uint8_t *)src;
    g_slave_req.txLen = srcLen;
    g_slave_req.rxData = (uint8_t *)g_rxBuff; // Assuming g_rxBuff is a global buffer
    g_slave_req.rxLen = srcLen;
    g_slave_req.completeCB = spi_transaction_cb;

    // Set up NVIC for SPI IRQ
    NVIC_SetVector(SPIx_IRQn, (void*)SPIx_IRQHandler);
    NVIC_EnableIRQ(SPIx_IRQn);

    // Initiate asynchronous SPI transaction
    ret = MXC_SPI_TargetTransactionAsync(&g_slave_req);

    // Return success or error code
    return ret;
}

int spi_slave_rcv(unsigned char *dst, unsigned int maxLen, unsigned int *len)
{
    int ret = 0;
    int counter = 100;

    while (is_transaction_done == 0) {
        if (--counter == 0) {
            break;
        }
        MXC_Delay(MXC_DELAY_MSEC(1));
    }

    memcpy(dst, g_rxBuff, sizeof(g_rxBuff));
    *len = sizeof(g_rxBuff);

    return ret;
}
