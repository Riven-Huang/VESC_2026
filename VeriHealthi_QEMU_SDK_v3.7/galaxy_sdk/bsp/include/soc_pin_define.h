/*
 * Copyright (c) 2026, VeriSilicon Holdings Co., Ltd. All rights reserved
 *
 * 1. Redistributing the source code of this software is only allowed after
 * receiving explicit, written permission from VeriSilicon. The copyright notice,
 * this list of conditions and the following disclaimer must be retained in all
 * source code distributions.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors
 * may be used to endorse or promote products derived from this software without
 * specific prior written permission.
 */

#ifndef SOC_PIN_DEFINE_H
#define SOC_PIN_DEFINE_H

/**
 * @addtogroup PEGASUS_PIN_DEFINE
 * Pin definition
 * @ingroup SOC_PEGASUS
 * @{
 */

/* Pinmux bit set */
#define SW_DS(x)  (x)        /**< Pad drive strength control*/
#define SW_IN     BIT(2)     /**< Pad input force on */
#define SW_ST     BIT(3)     /**< Pad schmitt trigger enable */
#define SW_PD     BIT(4)     /**< Pad pull down enable */
#define SW_PU     BIT(5)     /**< Pad pull up enable */
#define SW_OD     BIT(6)     /**< Pad open drain enable */
#define SW_MUX(x) ((x) << 7) /**< Pad mux select */
#define SW_IOKEEP BIT(12)    /**< IO keep enable */

/**
 * @brief Gpio pins
 */
typedef enum PinList {
    PIN_IRQ_DS0     = 0,
    PIN_GROUP0_BASE = PIN_IRQ_DS0,
    PIN_IRQ_DS1,
    PIN_IRQ_DS2,
    PIN_IRQ_DS3,
    PIN_IRQ_DS4,
    PIN_IRQ_DS5,
    PIN_IRQ_DS6,
    PIN_IRQ_DS7,
    PIN_IRQ_DS8,
    PIN_IRQ_DS9,
    PIN_IRQ_DS10,
    PIN_IRQ_DS11,
    PIN_IRQ_DS12,
    PIN_IRQ_DS13,
    PIN_IRQ_DS14,
    PIN_IRQ_DS15,
    PIN_NMI,
    PIN_GROUP1_BASE = PIN_NMI,
    PIN_SPI_CS0,
    PIN_SPI_MOSI0,
    PIN_SPI_MISO0,
    PIN_SPI_CLK0,
    PIN_I2S_BCLK,
    PIN_I2S_LRCK,
    PIN_I2S_DI,
    PIN_I2S_DO,
    PIN_UART_TXD0,
    PIN_GROUP2_BASE = PIN_UART_TXD0,
    PIN_UART_RXD0,
    PIN_UART_CTS0,
    PIN_UART_RTS0,
    PIN_UART_TXD1,
    PIN_UART_RXD1,
    PIN_UART_TXD2,
    PIN_UART_RXD2,
    PIN_I2C_SCL0,
    PIN_I2C_SDA0,
    PIN_I2C_SCL1,
    PIN_I2C_SDA1,
    PIN_CACREF,
    PIN_QSPI_SCLK,
    PIN_QSPI_MISO,
    PIN_QSPI_MOSI,
    PIN_QSPI_WP,
    PIN_QSPI_HOLD,
    PIN_QSPI_CS,
    PIN_MAX,
} PinList;

#define PIN_TO_PORT_ID(pin_id)                                   \
    ((pin_id) < PIN_GROUP1_BASE ? (pin_id) :                     \
     (pin_id) < PIN_GROUP2_BASE ? ((pin_id) - PIN_GROUP1_BASE) : \
     (pin_id) < PIN_CACREF      ? ((pin_id) - PIN_GROUP2_BASE) : \
     (pin_id) == PIN_CACREF     ? 9 :                            \
                                  0xff)

#define GET_GPIO_GROUP_ID(pin_id)     \
    ((pin_id) < PIN_GROUP1_BASE ? 0 : \
     (pin_id) < PIN_GROUP2_BASE ? 1 : \
     (pin_id) < PIN_CACREF      ? 2 : \
     (pin_id) == PIN_CACREF     ? 1 : \
                                  0)

/** @} */

#endif // SOC_PIN_DEFINE_H
