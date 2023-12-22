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

#include "spi_config.h"

/*******************************      DEFINES     ****************************/

/******************************* Type Definitions ****************************/

/*******************************     Variables    ****************************/

/******************************* Static Functions ****************************/

/******************************* Public Functions ****************************/
int spi_master_init(void)
{
    // Variable declarations
    int ret = 0;
    int masterMode = MXC_SPI_TYPE_CONTROLLER;
    int quadModeUsed = MXC_SPI_INTERFACE_STANDARD;
    int numSlaves = 1;
    int ssPolarity = 0;

    // SPI pins configuration
    mxc_spi_pins_t spi_pins = {
        .ss0 = false,   // TS0
        .ss1 = true,    // TS1
        .ss2 = false,   // TS2
        .ss3 = false,   // TS3
        .vddioh = true,
        .clock = true,
        .drvstr = MXC_GPIO_DRVSTR_0
    };

    // Initialize SPI hardware
    ret = MXC_SPI_Init(SPIx_MASTER, masterMode, quadModeUsed, numSlaves, ssPolarity,
                       SPI_BAUD_RATE, spi_pins);
    if (ret != 0) {
        return ret;
    }

    // SPI configuration
    mxc_spi_cfg_t cfg_master = {
        .spi = SPIx_MASTER,
        .clk_mode = MXC_SPI_CLKMODE_0, // CPOL: 0, CPHA: 0
        .frame_size = 8,
        .use_dma_tx = false,
        .use_dma_rx = false,
        .dma = MXC_DMA
    };

    ret = MXC_SPI_Config(&cfg_master);
    if (ret != 0) {
        return ret;
    }

    // Set additional SPI parameters
    MXC_SPI_SetDataSize(SPIx_MASTER, 8);
    MXC_SPI_SetWidth(SPIx_MASTER, SPI_WIDTH_STANDARD);

    // Return success or error code
    return ret;
}

int spi_master_send_rcv(unsigned char *src, unsigned int srcLen, unsigned char *dst)
{
    // Variable declaration
    int ret = 0;

    // Create a SPI transaction request structure
    mxc_spi_req_t req = {
        .spi = SPIx_MASTER,
        .ssIdx = 1,
        .txData = (uint8_t *)src,
        .txLen = srcLen,
        .rxData = (uint8_t *)dst,
        .rxLen = srcLen,
        .ssDeassert = 1,
        .completeCB = NULL
    };

    // Perform SPI master send and receive transaction
    ret = MXC_SPI_ControllerTransaction(&req);

    // Return success or error code
    return ret;
}
