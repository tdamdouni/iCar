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
 * \file PDCViewer.h
 *
 * \date Created: 09.08.2012 22:23:25
 * \author Matthias Kleemann
 **/


#ifndef PDCVIEWER_H_
#define PDCVIEWER_H_

// === DEFINITIONS ===========================================================

/**
 * \brief INT0 trigger definition
 *
 * \code
 * ISC01 ISC00 Description
 *     0     0 The low level of INT0 generates an interrupt request
 *     0     1 Any logical change on INT0 generates an interrupt request
 *     1     0 The falling edge of INT0 generates an interrupt request
 *     1     1 The rising edge of INT0 generates an interrupt request
 * \endcode
 */
#define EXTERNAL_INT0_TRIGGER    0

/**
 * \brief setup for enabling the INT0 interrupt
 */
#define EXTERNAL_INT0_ENABLE     (1 << INT0)

/**
 * \brief maximum PDC value (nothing in range)
 */
#define PDC_OUT_OF_RANGE         255

/**
 * \brief define CAN id for PDC data
 *
 * Length of CAN message is 8 bytes. The 8 bytes are the values of different
 * PDC sensors. The range is 0..254 in cm distance to any object near the
 * sensor and 255 for "no object in range".
 *
 * \code
 * byte  sensor
 * 0     front left
 * 1     front right
 * 2     rear left
 * 3     rear right
 * 4     front mid left
 * 5     front mid right
 * 6     rear mid left
 * 7     rear mid right
 * \endcode
 */
#define PDC_CAN_ID               0x54B

// === TYPE DEFINITIONS ======================================================

/**
 * \brief defines all states of the FSM
 */
typedef enum
{
   //! initialize all hardware
   INIT           = 0,
   //! do all the work
   RUNNING        = 1,
   //! prepare sleep mode (AVR and CAN)
   SLEEP_DETECTED = 2,
   //! sleeping
   SLEEPING       = 3,
   //! wake up (AVR and CAN)
   WAKEUP         = 4,
   //! an error occurred, stop working
   ERROR          = 5
} state_t;


// === FSM ===================================================================

/**
 * \brief Go to sleep mode. Deactivate CAN and set the sleep mode.
 *
 * Sleep trigger was detected (no CAN activity on master bus). All timers are
 * stopped. CAN controllers are put to sleep and AVR is  preparing for sleep
 * mode.
 */
void sleepDetected(void);


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
void sleeping(void);

/**
 * \brief wake up CAN and reinitialize the timers
 *
 * Now the AVR has woken up. Timers needs to be restarted and the CAN
 * controllers will also need to enter their working mode.
 */
void wakeUp(void);

/**
 * \brief do all the work.
 */
void run(void);

/**
 * \brief Error state
 *
 * Call this when an illegal state is reached. Only some status LEDs will
 * blink to show the error, but the system stops to work.
 */
void errorState(void);


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
void initHardware(void);

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
bool initCAN(void);

#endif /* PDCVIEWER_H_ */
