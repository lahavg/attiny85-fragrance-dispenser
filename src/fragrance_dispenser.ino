// ATtiny85 Fragrance Dispenser - 8MHz Internal Clock
// Simple & Reliable Code with Power Saving

#include <avr/sleep.h>
#include <avr/power.h>
#include <avr/wdt.h>
#include <avr/interrupt.h>

// Define bit manipulation macros
#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif
#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif

/* 
=== LED Indications Explanation ===
1. Current light condition (reading from sensor):
   - 1 blink = bright/day
   - 2 blinks = dark/night

2. Long LED (2 seconds) = separator between checks

3. Time mode check:
   - 1 blink = 15 minutes (switch at 0/LOW)
   - 2 blinks = 30 minutes (switch at 1/HIGH)

4. Long LED (2 seconds) = separator between checks

5. Operation mode check:
   - 1 blink = day and night (switch at 0/LOW)
   - 2 blinks = day only (switch at 1/HIGH)

== Wait 15 seconds then first activation ==

=== SWITCH LOGIC ===
MODE_SWITCH_PIN (time): LOW=15min, HIGH=30min
LIGHT_SWITCH_PIN (operation): LOW=day&night, HIGH=day only
*/

#define SERVO_PIN 0           // PB0 - Servo motor
#define LED_PIN 4             // PB4 - Status LED  
#define MODE_SWITCH_PIN 1     // PB1 - LOW = 15min, HIGH = 30min
#define LIGHT_SWITCH_PIN 2    // PB2 - LOW = day only, HIGH = always work
#define LIGHT_SENSOR_PIN 3    // PB3 - Light sensor (A3)

bool skip30MinCycle = false;  // For 30min mode - alternate cycles

void setup_watchdog(int ii) {
  // 0=16ms, 1=32ms, 2=64ms, 3=128ms, 4=250ms, 5=500ms
  // 6=1 sec, 7=2 sec, 8=4 sec, 9=8sec
  uint8_t bb;
  if (ii > 9) ii = 9;
  bb = ii & 7;
  if (ii > 7) bb |= (1<<5);
  bb |= (1<<WDCE);
  
  MCUSR &= ~(1<<WDRF);
  // Start timed sequence
  WDTCR |= (1<<WDCE) | (1<<WDE);
  // Set new watchdog timeout value
  WDTCR = bb;
  WDTCR |= _BV(WDIE);
}

// System wakes up when watchdog is timed out
void system_sleep() {
  cbi(ADCSRA, ADEN);                    // Switch ADC OFF
  setup_watchdog(9);                    // Approximately 8 seconds sleep
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);  // Sleep mode is set here
  sleep_enable();
  sei();                                // Enable interrupts so WDT can wake us up
  sleep_mode();                         // System sleeps here
  sleep_disable();                      // System continues execution here when WDT timed out
  sbi(ADCSRA, ADEN);                   // Switch ADC back ON
}

// Watchdog Interrupt Service - is executed when watchdog timed out
ISR(WDT_vect) {
  // Must be present, otherwise ATtiny reboots instead of returning from sleep!
}

void setup() {
  // Configure pins
  pinMode(SERVO_PIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);
  pinMode(MODE_SWITCH_PIN, INPUT_PULLUP);
  pinMode(LIGHT_SWITCH_PIN, INPUT_PULLUP);
  
  // Startup indication sequence
  startupIndication();
  
  // Wait 15 seconds between indication and first activation - REGULAR DELAY
  delay(15000); // Regular delay for first time
  
  // First activation after startup
  activateFragranceDispenser();
}

void loop() {
  // Sleep for multiple 8-second periods to get 15 minutes (112 cycles = ~896 seconds ≈ 15 min)
  for (int i = 0; i < 112; i++) { // 112 cycles for 15 minutes
    system_sleep(); // 8 second sleep cycles
  }
  
  // Check if 30min mode is enabled
  bool is30MinMode = digitalRead(MODE_SWITCH_PIN) == HIGH;
  
  if (is30MinMode) {
    // 30min mode: alternate cycles (skip every other time)
    if (skip30MinCycle) {
      skip30MinCycle = false;
      return; // Skip this cycle
    }
    skip30MinCycle = true;
  }
  
  // Check light condition before activation
  if (shouldActivateBasedOnLight()) {
    activateFragranceDispenser();
  }
}

void startupIndication() {
  delay(500); // Calm start
  
  // 1. Current light condition - READ FROM SENSOR
  delay(1);
  
  int currentLight = analogRead(A3);
  if (currentLight >= 500) { // Back to 500
    // Bright - day - 1 blink
    slowBlink(1);
  } else {
    // Dark - night - 2 blinks
    slowBlink(2);
  }
  
  delay(1500); // Long pause
  
  // 2. Long LED on
  digitalWrite(LED_PIN, HIGH);
  delay(2000); // 2 seconds long
  digitalWrite(LED_PIN, LOW);
  
  delay(1500); // Long pause
  
  // 3. Time mode indication  
  if (digitalRead(MODE_SWITCH_PIN) == HIGH) {
    // 30min mode - 2 blinks
    slowBlink(2);
  } else {
    // 15min mode - 1 blink
    slowBlink(1);
  }
  
  delay(1500); // Long pause
  
  // 4. Long LED on  
  digitalWrite(LED_PIN, HIGH);
  delay(2000); // 2 seconds long
  digitalWrite(LED_PIN, LOW);
  
  delay(1500); // Long pause
  
  // 5. Operation mode indication (switch setting)
  if (digitalRead(LIGHT_SWITCH_PIN) == LOW) {
    // Day and night mode (switch at 0) - 1 blink
    slowBlink(1);
  } else {
    // Day only mode (switch at 1) - 2 blinks
    slowBlink(2);
  }
  
  delay(2000); // Final pause
}

void slowBlink(int times) {
  for (int i = 0; i < times; i++) {
    digitalWrite(LED_PIN, HIGH);
    delay(500); // Half second on
    digitalWrite(LED_PIN, LOW);
    delay(500); // Half second off
    if (i < times - 1) {
      delay(300); // Extra pause between blinks
    }
  }
}

bool shouldActivateBasedOnLight() {
  // Check light switch setting
  bool dayOnlyMode = digitalRead(LIGHT_SWITCH_PIN) == HIGH; // HIGH = day only
  
  if (!dayOnlyMode) {
    // Day and night mode (switch at LOW) - ignore light level
    return true;
  }
  
  // Day only mode (switch at HIGH) - check light level
  delay(1); // Small delay for ADC to stabilize
  
  int lightLevel = analogRead(A3);
  
  return (lightLevel >= 500); // Activate only if bright enough
}

void activateFragranceDispenser() {
  // 3 faster LED blinks
  for (int i = 0; i < 3; i++) {
    digitalWrite(LED_PIN, HIGH);
    delay(100);
    digitalWrite(LED_PIN, LOW);
    delay(100);
  }
  
  // Regular delay - no sleep mode before servo
  delay(5000); // 5 seconds regular delay
  
  // Activate servo - one press and return
  activateServo();
}

void activateServo() {
  // Servo control: from -180° to +180° and back to -180°
  
  // Move to press position (+180 degrees)
  for (int i = 0; i < 30; i++) {
    digitalWrite(SERVO_PIN, HIGH);
    delayMicroseconds(2000); // 2ms = +180 degrees (press)
    digitalWrite(SERVO_PIN, LOW);
    delay(18); // Complete 20ms cycle
  }
  
  delay(500); // Hold press
  
  // Return to start position (-180 degrees)  
  for (int i = 0; i < 30; i++) {
    digitalWrite(SERVO_PIN, HIGH);
    delayMicroseconds(500); // 0.5ms = -180 degrees (extended range)
    digitalWrite(SERVO_PIN, LOW);
    delay(19); // Complete 20ms cycle
  }
}
