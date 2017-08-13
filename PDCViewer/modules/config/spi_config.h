/**
 * ----------------------------------------------------------------------------
 *
 * "THE ANY BEVERAGE-WARE LICENSE" (Revision 42 - based on beer-ware license):
 * <dev@layer128.net> wrote this file. As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a be(ve)er(age) in return. (I don't
 * like beer much.)
 *
 * Matthias Kleemann
 *
 * ----------------------------------------------------------------------------
 *
 * \file spi_config.h
 *
 * \date Created: 28.11.2011 18:16:39
 * \author Matthias Kleemann
 **/



#ifndef SPI_CONFIG_H_
#define SPI_CONFIG_H_

#ifndef SPI_PRESCALER
/**
 * @brief SPI_PRESCALER must have a value which is 2^n (n = 1..7)
 *
 * This is used to determine SPI speed in dependence from main clock. The
 * speed results as follows: Fspi = Fosc/SPI_PRESCALER
 */
#define SPI_PRESCALER      8
#endif


/***************************************************************************/
/* PORT PINS FOR SPI - here ATmega8                                        */
/***************************************************************************/

//! SPI clock pin
#define P_SCK                 B,5
//! Master In Slave Out
#define P_MISO                B,4
//! Master Out Slave In
#define P_MOSI                B,3


#endif /* SPI_CONFIG_H_ */