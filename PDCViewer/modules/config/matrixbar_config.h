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
 * \file matrixbar_config.h
 *
 * \date Created: 04.02.2012 21:18:18
 * \author Matthias Kleemann
 *
 **/



#ifndef MATRIXBAR_CONFIG_H_
#define MATRIXBAR_CONFIG_H_

/***************************************************************************/
/* GENERAL CONFIGURATION                                                   */
/***************************************************************************/

/**
 * \brief number of rows used
 *
 * This value should correspond with the setup in P_MATRIXBAR_ROW. At least
 * it should be no larger than the definitions of port/pin entries in it. The
 * pins themselves are not meant here.
 *
 * \see P_MATRIXBAR_ROW
 */
#define MATRIXBAR_NUM_ROWS    2

/**
 * \brief number of columns used
 *
 * This value should correspond with the setup in P_MATRIXBAR_COL. At least
 * it should be no larger than the definitions of port/pin entries in it. The
 * pins themselves are not meant here.
 *
 * \see P_MATRIXBAR_COL
 */
#define MATRIXBAR_NUM_COLS    1

/**
 * \brief maximum value which causes all bargraph pins to be high
 *
 * The algorithm used will result in the following match:
 * 0..MATRIXBAR_MAX_VALUE -> 0..MATRIXBAR_RANGE-1
 */
#define MATRIXBAR_MAX_VALUE   254

/**
 * \brief reverse bargraph (MAX -> 0; 0 -> MATRIXBAR_RANGE-1)
 *
 * Sometimes it is wanted to set all pins when the value is 0, but clear
 * them when the value is near maximum. This is used e.g. in park distance
 * control units to visualize the measured values as a warning to the user.
 *
 * Comment this defintion to avoid using this feature.
 */
#define MATRIXBAR_REVERSE

/**
 * \brief invert bits, e.g. to show value left
 *
 * This means, the bargraph reacts like normal, but the 1s and 0s are
 * inverted.
 *
 * Comment this defintion to avoid using this feature.
 */
#ifdef __DOXYGEN__
   #define MATRIXBAR_INVERTED
#else
//#define MATRIXBAR_INVERTED
#endif

/***************************************************************************/
/* PORT/PIN DEFINITIONS                                                    */
/***************************************************************************/

/**
 * \def P_MATRIXBAR_ROW
 * \brief the row pins of the bargraph matrix
 *
 * The row pins are defined with as many ports may be used. The mask is used
 * to determine the pins of the port to be used. The logic behind uses the
 * first defined port as the lowest value part and starting with the LSB to
 * show the lowest bar value.
 *
 * Example:
 *
 * \code
 * #define P_MATRIXBAR_ROW       {&DDR(C), &PORT(C), 0x3F}, \
 *                               {&DDR(D), &PORT(D), 0xE0}, \
 *                               {&DDR(B), &PORT(B), 0x03}
 * \endcode
 *
 * The bargraph now has 11 pins to a row to show a value. The value
 * '0000 0100 1011 1011' will be split like follows:
 *
 * \code
 * Bit:
 * | 1 | 1 | 1 | 1 | 1 | 1 | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 0 |
 * | 5 | 4 | 3 | 2 | 1 | 0 | 9 | 8 | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
 *
 * Port/Pin Setup:
 * |     not used      |
 *                     |   PB  |
 *                     | 1   0 |    PD     |
 *                             | 7   6   5 |           PC          |
 *                                         | 5   4   3   2   1   0 |
 *
 * Value:
 * | 0   0   0   0   0   1   0   0   1   0   1   1   1   0   1   1 |
 * \endcode
 */
#define P_MATRIXBAR_ROW       {&DDR(C), &PORT(C), 0x3F}, \
                              {&DDR(D), &PORT(D), 0x1B}

/**
 * \def P_MATRIXBAR_COL
 * \brief the columns pins of the bargraph matrix
 *
 * The columns will be switched by used SW - most likely in sequence. The
 * set bits are used from LSB to MSB and in order of the definitions. See
 * P_MATRIXBAR_ROW for details.
 *
 * \see P_MATRIXBAR_ROW
 */
#define P_MATRIXBAR_COL       {&DDR(D), &PORT(D), 0x60}

/**
 * \def P_MATRIXBAR_COL_INVERTED
 * \brief define if column pins are inverted
 *
 * To comment/not use this define use a HIGH level for a column pin. Set this
 * definition and the columns pins are set LOW active.
 */
#ifdef __DOXYGEN__
   #define P_MATRIXBAR_COL_INVERTED
#else
//#define P_MATRIXBAR_COL_INVERTED
#endif

#endif
