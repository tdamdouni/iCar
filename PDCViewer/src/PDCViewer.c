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
 * \file PDCViewer.c
 *
 * \date Created: 09.08.2012 18:22:46
 * \author Matthias Kleemann
 **/


#include "leds/leds.h"
#include "can/can_mcp2515.h"
#include "timer/timer.h"
#include "matrixbar/matrixbar.h"
#include "PDCViewer.h"

#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <avr/cpufunc.h>

// === GLOBALS ===============================================================

/**
 * \brief testing w/o CAN
 */
//#define ___NO_CAN___

/**
 * \brief current state of FSM
 *
 * The state of the FSM is set and read from here.
 */
state_t fsmState       = INIT;

/**
 * \brief set current column
 */
uint8_t columnInUse    = 0;

/**
 * \brief trigger active column of display
 */
bool columnTrigger       = false;

/**
 * \brief used number of columns
 *
 * This value should match number of columns of matrixbar, but need not
 * necessarily. In this case it matches.
 */
#define NUM_OF_PDC_VALUES_SHOWN  2

//! store max value of left and right side
uint8_t pdcValueStored[NUM_OF_PDC_VALUES_SHOWN] = {PDC_OUT_OF_RANGE, PDC_OUT_OF_RANGE};

// === MAIN LOOP =============================================================

/**
 * \brief main loop
 *
 * The main loop consists of the FSM and calls all necessary init sequences
 * before entering it. Any error in the init process will result in
 * entering the error state.
 *
 * \returns  nothing, since it does not return
 **/
#ifndef __DOXYGEN__
int __attribute__((OS_main)) main(void)
#else
int main(void)
#endif
{

   initHardware();

#ifndef ___NO_CAN___
   if(true == initCAN())
   {
#endif
      while (1)
      {
         switch (fsmState)
         {
            case RUNNING:
            {
               run();
               break;
            }

            case WAKEUP:
            {
               wakeUp();
               fsmState = RUNNING;
               break;
            }

            case SLEEP_DETECTED:
            {
               sleepDetected();
               fsmState = SLEEPING;
               break;
            }

            case SLEEPING:
            {
               sleeping();
               // set state WAKEUP here, too avoid race conditions
               // with pending interrupt
               fsmState = WAKEUP;
               break;
            }

            default:
            {
               errorState();
               fsmState = ERROR;
               break;
            }
         }
      }
#ifndef ___NO_CAN___
   }
#endif

   // something went wrong here
   errorState();
}


// === FSM ===================================================================

/**
 * \brief Go to sleep mode. Deactivate CAN and set the sleep mode.
 *
 * Sleep trigger was detected (no CAN activity on master bus). All timers are
 * stopped. CAN controllers are put to sleep and AVR is  preparing for sleep
 * mode.
 */
void sleepDetected(void)
{
   // stop timer for now
   stopTimer1();
   stopTimer2();
   // leds off to save power
   led_all_off();
   matrixbar_clear();
   // all PDC values to default
   pdcValueStored[0] = PDC_OUT_OF_RANGE;
   pdcValueStored[1] = PDC_OUT_OF_RANGE;

#ifndef ___NO_CAN___
   // set CAN controller to sleep
   mcp2515_sleep(CAN_CHIP1, INT_SLEEP_WAKEUP_BY_CAN);
#endif
}

/**
 * \brief enter AVR sleep mode
 *
 * AVR enters sleep mode and also wakes up in this state, so some intial
 * steps to set wakeup interrupt need to be done here.
 *
 * The three \c _NOP(); instructions are a safety, since older AVRs may
 * skip the next couple of instructions after sleep mode.
 *
 * Also a precaution is the disabling of the wake up interrupt, to avoid
 * several interrupts to happen, if the signal lies too long on the
 * external interrupt pin.
 */
void sleeping(void)
{
   cli();

   // don't wake up with trigger set
   columnTrigger = false;

   // enable wakeup interrupt INT0
   GICR  |= EXTERNAL_INT0_ENABLE;

   // let's sleep...
   set_sleep_mode(SLEEP_MODE_PWR_DOWN);
   // sleep_mode() has a possible race condition in it, so splitting it
   sleep_enable();
   sei();
   sleep_cpu();
   sleep_disable();

   // just in case...
   _NOP();
   _NOP();
   _NOP();

   // disable interrupt: precaution, if signal lies too long on pin
   GICR  &= ~(EXTERNAL_INT0_ENABLE);
}

/**
 * \brief wake up CAN and reinitialize the timers
 *
 * Now the AVR has woken up. Timers needs to be restarted and the CAN
 * controllers will also need to enter their working mode.
 */
void wakeUp(void)
{
   cli();

#ifndef ___NO_CAN___
   // wakeup CAN bus
   mcp2515_wakeup(CAN_CHIP1, INT_SLEEP_WAKEUP_BY_CAN);
#endif

   // restart timers
   restartTimer1();
   restartTimer2();
   // set status LED to show run state
   led_on(statusLed);

   sei();
}

/**
 * \brief do all the work.
 */
void run(void)
{
#ifndef ___NO_CAN___
   can_t    msg;

   if (can_check_message_received(CAN_CHIP1))
   {
      // try to read message
      if (can_get_message(CAN_CHIP1, &msg))
      {
         // reset timer counter, since there is activity on master CAN bus
         setTimer1Count(0);

         // fetch information from CAN
         if ((PDC_CAN_ID == msg.msgId) && (0 == msg.header.rtr))
         {
            // fetch only rear sensors
            // left
            pdcValueStored[0] = (msg.data[2] < msg.data[6]) ? msg.data[2] : msg.data[6];
            // right
            pdcValueStored[1] = (msg.data[3] < msg.data[7]) ? msg.data[3] : msg.data[7];
         }
      }
   }
#else
   // testing w/o CAN

   // reset timer counter
   setTimer1Count(0);
#endif

   if(true == columnTrigger)
   {
      columnTrigger = false;
      // trigger value presentation in matrixbar
      matrixbar_reset_col(columnInUse % NUM_OF_PDC_VALUES_SHOWN);
      ++columnInUse;
      matrixbar_set(pdcValueStored[columnInUse % NUM_OF_PDC_VALUES_SHOWN]);
      matrixbar_set_col(columnInUse % NUM_OF_PDC_VALUES_SHOWN);
   }
}

/**
 * \brief Error state
 *
 * Call this when an illegal state is reached. Only some status LEDs will
 * blink to show the error, but the system stops to work.
 */
void errorState(void)
{
   led_toggle(statusLed);
   _delay_ms(500);
}


// === ISR ===================================================================

/**
 * \brief interrupt service routine for Timer1 capture
 *
 * Timer1 input capture interrupt (~15s 4MHz@1024 prescale factor)
 **/
ISR(TIMER1_CAPT_vect)
{
   fsmState = SLEEP_DETECTED;
}

/**
 * \brief interrupt service routine for Timer2 capture
 *
 * Timer2 input compare interrupt (~5ms 4MHz@1024 prescale factor) is used
 * to trigger the multiplexing of the display (bargraph) sides. At ~5ms the
 * flickering shouldn't be so obvious.
 **/
ISR(TIMER2_COMP_vect)
{
   columnTrigger = true;
}

/**
 * \brief interrupt service routine for external interrupt 0
 *
 * External Interrupt0 handler to wake up from CAN activity
 **/
ISR(INT0_vect)
{
   // does not need to do anything, but needs to be there
}


// === HELPERS ===============================================================


/**
 * \brief Initialize Hardware
 *
 * Setting up the peripherals to the AVR and the wake-up interrupt
 * trigger.
 *
 * * \ref page_timers to trigger events
 *
 * * \ref page_spi to communicate to the MCP2515
 *
 * * \ref page_matrixbar to give user feedback
 *
 */
void initHardware(void)
{
   // set timer for bussleep detection
   initTimer1(TimerCompare);
   // set timer for PDC off detection
   initTimer2(TimerCompare);

#ifndef ___NO_CAN___
   // initialize the hardware SPI with default values set in spi/spi_config.h
   spi_pin_init();
   spi_master_init();
#endif

   // init matrix bargraph
   matrixbar_init();
   // init status LED and switch to on
   led_init();
   led_on(statusLed);

   // set wakeup interrupt trigger on low level
   MCUCR |= EXTERNAL_INT0_TRIGGER;

#ifdef ___NO_CAN___
   // It's not done in initCAN()!
   fsmState = RUNNING;
#endif
}

/**
 * \brief Initialize the CAN controllers
 *
 * Calls can_init_mcp2515 for each attached CAN controller and setting up
 * bit rate. If an error occurs some status LEDs will indicate it.
 *
 * See chapter \ref page_can_bus for further details.
 *
 * @return true if all is ok. Otherwise false is returned.
 */
bool initCAN(void)
{
   bool retVal = can_init_mcp2515(CAN_CHIP1, CAN_BITRATE_100_KBPS, LISTEN_ONLY_MODE);
   /*
    * SIDH -> bits 3..10 of CAN ID @ bits 0..7
    * SIDL -> bits 0..2  of CAN ID @ bits 5..7
    */
   uint8_t filterVals[MAX_LENGTH_OF_FILTER_SETUP] = {((PDC_CAN_ID >> 3) & 0xFF), // SIDH
                                                     ((PDC_CAN_ID << 5) & 0xE0), // SIDL
                                                     0xFF,                       // EID8
                                                     0xFF};                      // EID0
   if(true == retVal)
   {
      // set filters to currently used can message, ignore anything else
      set_mode_mcp2515(CAN_CHIP1, CONFIG_MODE);
      // masks
      setFilters(CAN_CHIP1, RXM0SIDH, filterVals);
      setFilters(CAN_CHIP1, RXM1SIDH, filterVals);
      // filters
      setFilters(CAN_CHIP1, RXF0SIDH, filterVals);
      setFilters(CAN_CHIP1, RXF1SIDH, filterVals);
      /* not yet...
      setFilters(CAN_CHIP1, RXF2SIDH, filterVals);
      setFilters(CAN_CHIP1, RXF3SIDH, filterVals);
      setFilters(CAN_CHIP1, RXF4SIDH, filterVals);
      setFilters(CAN_CHIP1, RXF5SIDH, filterVals);
      */
      // back to normal
      set_mode_mcp2515(CAN_CHIP1, LISTEN_ONLY_MODE);
   }
   // If an error roccurs, the main loop is not started, so it's ok to set
   // the state here.
   fsmState = RUNNING;
   return retVal;
}

