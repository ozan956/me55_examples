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

#ifndef EXAMPLES_MAX32572_SPI_USECASE_SPI_CONFIG_H_
#define EXAMPLES_MAX32572_SPI_USECASE_SPI_CONFIG_H_

#include "max32572.h"
#include "spi_regs.h"
#include "spi.h"

/*** SPI Master Configuration ***/
#define SPIx_MASTER MXC_SPI1
#define SPI_BAUD_RATE 1000000// 1MHz

/*** SPI Slave Configuration ***/
#define SPIx_SLAVE MXC_SPI0
#define SPIx_IRQn SPI0_IRQn
#define SPIx_IRQHandler SPI0_IRQHandler

//
#define TEST_BUFF_SIZE 64

/*** Functions ***/
int spi_master_init(void);
int spi_master_send_rcv(unsigned char *src, unsigned int srcLen, unsigned char *dst);

int spi_slave_init(void);
int spi_slave_send(unsigned char *src, unsigned int srcLen);
int spi_slave_rcv(unsigned char *dst, unsigned int maxLen, unsigned int *len);

#endif // EXAMPLES_MAX32572_SPI_USECASE_SPI_CONFIG_H_
