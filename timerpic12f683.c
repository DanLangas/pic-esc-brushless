//*****************************************************************
// Abstract: simple PWM timer for brushless ESC
// Description: developed for control line model airplanes with the
// on board throttle control with no RC
//*****************************************************************

// Microchip's free edition of the xc8 compiler
#include <xc.h>
#include <pic.h>

//*****************************************************************
// Device configuraton
#pragma config FOSC = INTOSCIO   // internal oscilator
#pragma config PWRTE = OFF       // no power up time
#pragma config WDTE = OFF        // no watchdog tmpfile
#pragma config MCLRE = OFF       // MCLRE as regular IO
#pragma config CP = OFF          // no code protection
#pragma config CPD = OFF         // no data public
#pragma config BOREN = OFF       // no brown-out detection
#pragma config IESO = OFF        // no escillator switch-over
#pragma config FCMEN = OFF       // no fail safe monitoring

//*****************************************************************
// User program defines

#define _XTAL_FREQ 8000000       // required for proper delay timing
#define START_SWITCH GPIObits.GP4 // 'start/stop' switch

#define PWM_OUT GPIObits.GP2     // CCP1/GP2 pin used for PWM
#define START_UP_DELAY_SEC 25    // motor startup delay

#define FLIGHT_DURATION_MINS 5    // 'minute' part of the flight duration
#define FLIGHT_DURATION_SECS 30  // additional flight seconds for landing approach
#define FLIGHT_POWER_PERCENT 70  // PWM for 70% of max power of brushless motor

#define LANDING_DURATION_SECS 10 // duration of the landing period
#define LANDING_POWER_PERCENT 50 // PWM for 50% of max power in landing

// when assuming a 50Hz PWM frequency, we deal with a period length of 20 msec,
// i.e. 20 000 usec
#define PWM_STANDBY_DUTY_USEC 1000
#define PWM_STANDBY_IDLE_USEC 19000

#define PWM_FLIGHT_DUTY_USEC (1000+(FLIGHT_POWER_PERCENT*10))
#define PWM_FLIGHT_IDLE_USEC (20000-PWM_FLIGHT_DUTY_USEC)

#define PWM_LANDING_DUTY_USEC (1000+(LANDING_POWER_PERCENT*10))
#define PWM_LANDING_IDLE_USEC (20000-PWM_LANDING_DUTY_USEC)

//*****************************************************************

void start_up_delay(void) {
  while (START_SWITCH != 0) __delay_ms(100);

   int num_pulses = 0;
   int num_secs = 0;

   // now wait the specified number of seconds before engaging the motor,
   // while still sending the idle PWM pulses to ESC
   while (1) {
    PWM_OUT = 1;
        __delay_us(PWM_STANDBY_DUTY_USEC);

        PWM_OUT = 0;
        __delay_us(PWM_STANDBY_IDLE_USEC);

        ++num_pulses;

        if (num_pulses == 50) {
        num_pulses = 0;
        num_secs++;
        }
           if (num_secs == START_UP_DELAY_SEC) break;
   }
}

//*****************************************************************

int flight_pwm(void) {
   int num_pulses = 0;
   int num_secs = 0;
   int num_mins = 0;

   // now wait the specified number of seconds before engaging the motor,
   // while still sending the idle PWM pulses to ESC
   while (1) {
    PWM_OUT = 1;
        __delay_us(PWM_FLIGHT_DUTY_USEC);

        PWM_OUT = 0;
        __delay_us(PWM_FLIGHT_IDLE_USEC);

        ++num_pulses;

        if (num_pulses == 50) {
        num_pulses = 0;
        num_secs++;
        }

        if (num_secs == 60) {
        num_secs = 0;
        num_mins++;
        }

        // terminate upon reaching the flight duration
        if (num_mins ==  FLIGHT_DURATION_MINS &&
            num_secs == FLIGHT_DURATION_SECS)
                {
                        // flight finished
                        return 0;
                }

                // also terminate upon pressing the start-button
                if (START_SWITCH == 0)
                {
                        // flight aborted
                        return 1;
                }
   }
}

//*****************************************************************

void landing_pwm(void) {
   int num_pulses = 0;
   int num_secs = 0;

   while (1) {
    PWM_OUT = 1;
        __delay_us(PWM_LANDING_DUTY_USEC);

        PWM_OUT = 0;
        __delay_us(PWM_LANDING_IDLE_USEC);

        ++num_pulses;

        if (num_pulses == 50) {
        num_pulses = 0;
        num_secs++;
        }
           if (num_secs == LANDING_DURATION_SECS) break;
   }
}

//*****************************************************************

int main(int argc, char **argv) {

// configuration
OSCCON = 0x71;
CMCON0 = 0x7;
TRISIO = 0x10;
OPTION_REGbits.nGPPU = 0;
WPUbits.WPU4 = 1;
ANSEL = 0x0;
IOC = 0x0;

while (OSCCONbits.HTS != 1) __delay_ms(50);

start:
  start_up_delay();

  // enjoy flying
  if (flight_pwm() == 0) {
          landing_pwm();
}
  __delay_ms(500);
  goto start;

  // actually dead
  return 0;
}
