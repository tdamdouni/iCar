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
 * \file timer_config.h
 *
 * We are using an ATmega8 right now - values for other AVRs might differ.
 *
 * \date Created: 28.11.2011 18:17:28
 * \author Matthias Kleemann
 **/



#ifndef TIMER_CONFIG_H_
#define TIMER_CONFIG_H_

/**
 * \def TIMER0_PRESCALER
 * \brief TIMER0 prescaler settings
 *
 * \code
 * CS02 CS01 CS00 Description
 *    0    0    0 No clock source (Timer/Counter stopped)
 *    0    0    1 clkI/O/(No prescaling)
 *    0    1    0 clkI/O/8 (From prescaler)
 *    0    1    1 clkI/O/64 (From prescaler)
 *    1    0    0 clkI/O/256 (From prescaler)
 *    1    0    1 clkI/O/1024 (From prescaler)
 *    1    1    0 External clock source on T0 pin. Clock on falling edge
 *    1    1    1 External clock source on T0 pin. Clock on rising edge
 * \endcode
 *
 * Example sets TIMER0 with prescaler clkI/O/1024
 */
#ifdef __DOXYGEN__
   #define TIMER0_PRESCALER      (1 << CS02) | (1 << CS00)
#else
//#define TIMER0_PRESCALER      (1 << CS02) | (1 << CS00)
#endif

/**
 * \def TIMER1_PRESCALER
 * \brief TIMER1 prescaler settings
 *
 * \code
 * CS12 CS11 CS10 Description
 *    0    0    0 No clock source (Timer/Counter stopped)
 *    0    0    1 clkI/O/(No prescaling)
 *    0    1    0 clkI/O/8 (From prescaler)
 *    0    1    1 clkI/O/64 (From prescaler)
 *    1    0    0 clkI/O/256 (From prescaler)
 *    1    0    1 clkI/O/1024 (From prescaler)
 *    1    1    0 External clock source on T1 pin. Clock on falling edge
 *    1    1    1 External clock source on T1 pin. Clock on rising edge
 * \endcode
 *
 * TIMER1 with prescaler clkI/O/1024
 */
#define TIMER1_PRESCALER      (1 << CS12) | (1 << CS10)

/**
 * @brief Timer 1 Output Compare Value
 *
 * The value given calculates to approx. 15s (4MHz@1024 prescale value).
 */
#define TIMER1_COMPARE_VALUE  0xE4E1

/**
 * \def TIMER2_PRESCALER
 * \brief TIMER2 prescaler settings
 *
 * \code
 * CS22 CS21 CS20 Description
 *    0    0    0 No clock source (Timer/Counter stopped)
 *    0    0    1 clkT2S/(No prescaling)
 *    0    1    0 clkT2S/8 (From prescaler)
 *    0    1    1 clkT2S/32 (From prescaler)
 *    1    0    0 clkT2S/64 (From prescaler)
 *    1    0    1 clkT2S/128 (From prescaler)
 *    1    1    0 clkT2S/256 (From prescaler)
 *    1    1    1 clkT2S/1024 (From prescaler)
 * \endcode
 *
 * TIMER2 with prescaler clkI/O/1024
 */
#define TIMER2_PRESCALER      (1 << CS22) | (1 << CS21) | (1 << CS20)

/**
 * @brief Timer 2 Output Compare Value
 *
 * The value given calculates to approx. 5ms (4MHz@1024 prescale factor).
 * Correctly, it would be 5.12ms.
 *
 * Default value should be set to MAX (0xFF)
 */
#define TIMER2_COMPARE_VALUE  20

#endif

