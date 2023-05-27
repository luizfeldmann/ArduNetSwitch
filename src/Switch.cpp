#include "Switch.h"
#include <Arduino.h>
#include "persistent.h"

// Timer calculation: (4s pulse)
// 4s * 16 MHz / 1024 prescaler = 4s * 15625/s = 62 500 clock cycles
// Preload = 0xFFFF (16-bit max) minus required cycles = 0x0B 0xDC
static constexpr unsigned char c_preloadHigh = 0x0B;
static constexpr unsigned char c_preloadLow  = 0xDC;

//! Pin where switch relay is connected
static constexpr unsigned char SWITCH_PIN = 8;

//! Inverts the current logical state of the pin
static inline void ToggleSwitch()
{
    digitalWrite(SWITCH_PIN, !digitalRead(SWITCH_PIN));
}

//! On-Init setup
void ConfigureSwitch()
{
    // Set pin as output
    pinMode(SWITCH_PIN, OUTPUT);

    // Initial state
    digitalWrite(SWITCH_PIN, Persist_GetSwitchInitialState());

    // Configure Timer 1 for 'Pulse' feature
    TCCR1A = 0;             // Normal mode - count up until overflow
    TCCR1B = 0;             // No clock - stop timer
    TIMSK1 = (1 << TOIE1);  // Enable overflow interrupt
}

//! Perform requested command from HTTP
void OperateSwitch(ESwitchCommand eCmd)
{
    switch (eCmd)
    {
    default:
    case ESwitchCommand::None:
        break;

    case ESwitchCommand::On:
        digitalWrite(SWITCH_PIN, HIGH);
        break;

    case ESwitchCommand::Off:
        digitalWrite(SWITCH_PIN, LOW);
        break;

    case ESwitchCommand::Toggle:
        ToggleSwitch();
        break;

    case ESwitchCommand::Pulse:
        // First edge of pulse
        ToggleSwitch();
        // Timer will do second edge of pulse on ISR
        TCNT1H = c_preloadHigh; // Preload counter 
        TCNT1L = c_preloadLow;  // (L & H nibbles)
        TCCR1B = (1 << CS12) | (1 << CS10);   // Start timer (set clock source)
        break;
    }
}

ISR(TIMER1_OVF_vect)
{
    // Stop the timer - this is single-shot
    TCCR1B = 0;

    // Second edge of pulse
    ToggleSwitch();
}