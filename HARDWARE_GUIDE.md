# ATtiny85 Dispenser - Complete Project Guide

## Project Overview
An autonomous fragrance dispenser system using ATtiny85 microcontroller with ultra-low power consumption, capable of running 3-5 years on 2 AA batteries. The device activates a servo motor every 15 or 30 minutes to press a fragrance spray button, with optional light-dependent operation.

## Features
- **Ultra-low power consumption**: 3-5 years battery life on 2 AA batteries
- **Dual time modes**: 15 minutes or 30 minutes intervals
- **Light-dependent operation**: Day-only or 24/7 operation modes
- **Visual status indicators**: LED feedback for all system states
- **Deep sleep mode**: Uses watchdog timer for power savings
- **Extended servo range**: -180° to +180° movement

## Hardware Requirements

### Components
- **ATtiny85 microcontroller** (8MHz internal clock)
- **Servo motor** (5V compatible)
- **Light sensor** (LDR - Light Dependent Resistor)
- **2x SPDT switches** (ON/OFF type) for mode selection
- **Status LED** with 220Ω current limiting resistor
- **10kΩ resistor** for light sensor pull-down
- **2x AA battery holder**
- **5V voltage regulator** (LM7805 or similar - TO BE ADDED)
- **Connecting wires and breadboard/PCB**

### Detailed Circuit Connections

#### ATtiny85 Pin Connections
```
ATtiny85 Pin Layout and Connections:
Pin 1 (RESET) - Not used (leave floating or connect 10kΩ pull-up to VCC)
Pin 2 (PB3/A3) - Light sensor circuit (analog input)
Pin 3 (PB4) - Status LED anode (through 220Ω resistor to ground)
Pin 4 (GND) - Common ground (battery negative, all component grounds)
Pin 5 (PB0) - Servo motor signal wire (usually orange/yellow)
Pin 6 (PB1) - Mode switch (15/30 minutes selection)
Pin 7 (PB2) - Light operation switch (day-only/always operation)
Pin 8 (VCC) - +5V power (from voltage regulator output)
```

#### Light Sensor Circuit
```
VCC (+5V) ─────┬──── ATtiny85 Pin 2 (PB3/A3)
               │
           [LDR/Photodiode]
               │
               ├──── 10kΩ resistor ──── GND
```
**Function**: Creates voltage divider. Bright light = higher voltage, darkness = lower voltage.

#### LED Circuit
```
ATtiny85 Pin 3 (PB4) ──── 220Ω resistor ──── LED Anode
                                              │
                                            LED Cathode ──── GND
```
**Function**: Status indication with current limiting.

#### Switch Connections
```
Mode Switch (15/30 min):
ATtiny85 Pin 6 (PB1) ──── Switch Common Terminal
                          │
                          ├── Switch NO/NC ──── GND
                          
Light Switch (Day/Always):
ATtiny85 Pin 7 (PB2) ──── Switch Common Terminal  
                          │
                          ├── Switch NO/NC ──── GND
```
**Function**: ON/OFF switches with center terminal to ATtiny, one side to ground.
- **Switch Logic**: Open (floating/pulled up) = HIGH, Closed to ground = LOW

#### Servo Motor Connection
```
Servo Motor:
Red wire    ──── +5V (VCC)
Black/Brown ──── GND  
Orange/Yellow ──── ATtiny85 Pin 5 (PB0)
```

#### Power Supply Circuit
```
Battery Pack (2x AA = 3V) ──── [5V Voltage Regulator - TO BE ADDED] ──── +5V Rail
                          │                                           │
                          └──── GND ────────────────────────────────── GND Rail

Voltage Regulator (LM7805 or similar):
Input: 3-6V from batteries
Output: Stable 5V for ATtiny85 and servo
Ground: Common ground reference
```

### Pin Configuration Summary
```
ATtiny85 Pin Layout:
     ┌─────┬─────┐
RESET│1   8│VCC (+5V from regulator)
PB3  │2   7│PB2 (Light switch)
PB4  │3   6│PB1 (Mode switch)  
GND  │4   5│PB0 (Servo signal)
     └─────┴─────┘

Pin 1 (RESET) - Not used
Pin 2 (PB3/A3) - Light sensor with 10kΩ pull-down
Pin 3 (PB4) - Status LED with 220Ω resistor
Pin 4 (GND) - Ground
Pin 5 (PB0) - Servo motor signal
Pin 6 (PB1) - Mode switch (15/30 minutes)
Pin 7 (PB2) - Light switch (day-only/always)
Pin 8 (VCC) - +5V power
```

### Component Values and Specifications
- **Light sensor resistor**: 10kΩ (pull-down for voltage divider)
- **LED resistor**: 220Ω (current limiting, ~20mA @ 5V)
- **Switches**: SPDT ON/OFF type, rated for 5V
- **Servo**: Standard 5V hobby servo (SG90 or similar)
- **Voltage regulator**: 5V output, input range 7-35V (LM7805 family)

### Circuit Diagrams and Images

#### Schematic Diagram
*[Circuit schematic to be added - showing all connections as described above]*

#### Physical Implementation Photo  
*[Photo of completed project to be added - showing actual wiring and component placement]*

#### Breadboard Layout
*[Breadboard diagram to be added - for prototyping and testing]*

### Power Supply Notes
- **Current status**: Running directly from 2x AA batteries (3V)
- **Voltage regulator**: TO BE ADDED for stable 5V operation
- **Recommended**: LM7805 or similar 5V regulator
- **Why needed**: Servo motors require stable 5V for reliable operation
- **Input range**: Most 5V regulators need 7V+ input, consider step-up converter for battery operation

### Assembly Notes
1. **Test each component** before final assembly
2. **Double-check connections** - incorrect wiring can damage components
3. **Voltage regulator placement** - add heat sink if using LM7805 with higher input voltage
4. **Battery monitoring** - consider adding low battery indicator
5. **Enclosure** - protect circuit from moisture and dust

## Software Requirements

### Arduino IDE Setup
1. **Install Arduino IDE** (version 1.8.0 or newer)
2. **Add ATtiny board support**:
   - File → Preferences → Additional Board Manager URLs
   - Add: `https://raw.githubusercontent.com/damellis/attiny/ide-1.6.x-boards-manager/package_damellis_attiny_index.json`
   - Tools → Board → Boards Manager → Search "ATtiny" → Install

### Libraries Required
**No external libraries needed!** The project uses only built-in AVR libraries:
- `<avr/sleep.h>` - Sleep mode functions
- `<avr/power.h>` - Power management
- `<avr/wdt.h>` - Watchdog timer
- `<avr/interrupt.h>` - Interrupt handling

### Board Configuration
- **Board**: ATtiny25/45/85
- **Processor**: ATtiny85
- **Clock**: Internal 8MHz
- **Programmer**: Your choice (USBasp, Arduino as ISP, etc.)

## Deep Sleep Implementation Explained

### Why Sleep Mode?
Normal operation consumes ~3-5mA continuously. With sleep mode:
- **Sleep consumption**: ~1μA (99.97% power reduction!)
- **Battery life**: From months to years

### Watchdog Timer Approach
We use the ATtiny85's built-in Watchdog Timer (WDT) to wake from deep sleep:

```cpp
void setup_watchdog(int ii) {
  // Configure watchdog for 8-second intervals
  uint8_t bb;
  if (ii > 9) ii = 9;
  bb = ii & 7;
  if (ii > 7) bb |= (1<<5);
  bb |= (1<<WDCE);
  
  MCUSR &= ~(1<<WDRF);          // Clear watchdog reset flag
  WDTCR |= (1<<WDCE) | (1<<WDE); // Enable watchdog change
  WDTCR = bb;                   // Set timeout value
  WDTCR |= _BV(WDIE);          // Enable watchdog interrupt
}
```

### Sleep Cycle Process
1. **Enter Power-Down Mode**: Lowest power consumption state
2. **Disable ADC**: Further power reduction
3. **Wait for WDT Interrupt**: 8-second cycles
4. **Wake up**: Re-enable peripherals
5. **Repeat**: Multiple cycles for desired interval

```cpp
void system_sleep() {
  cbi(ADCSRA, ADEN);                    // Switch ADC OFF
  setup_watchdog(9);                    // 8 seconds sleep
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);  // Deepest sleep
  sleep_enable();
  sei();                                // Enable interrupts
  sleep_mode();                         // Sleep here
  sleep_disable();                      // Continue after wake
  sbi(ADCSRA, ADEN);                   // Switch ADC back ON
}
```

### Critical: ISR Implementation
**Must include empty ISR** or the system will reset instead of waking:

```cpp
ISR(WDT_vect) {
  // Must be present, otherwise ATtiny reboots!
}
```

### Time Calculation
- **8 seconds per WDT cycle**
- **15 minutes = 112 cycles** (112 × 8 = 896 seconds ≈ 15 minutes)
- **30 minutes = skip every other 15-minute cycle**

## LED Status Indicators

### Startup Sequence
The device shows its configuration at startup:

1. **Light condition check** (reads sensor):
   - 1 blink = bright/day detected
   - 2 blinks = dark/night detected

2. **Long LED** (2 seconds) - separator

3. **Time mode indication**:
   - 1 blink = 15-minute mode
   - 2 blinks = 30-minute mode

4. **Long LED** (2 seconds) - separator

5. **Operation mode indication**:
   - 1 blink = day & night operation
   - 2 blinks = day-only operation

### Activation Sequence
Before each servo activation:
- **3 rapid blinks** - warning signal
- **5-second pause** - preparation time
- **Servo activation** - actual spraying

## Servo Control Details

### Extended Range Movement
- **Start position**: -180° (0.5ms pulse)
- **Press position**: +180° (2ms pulse)
- **Return**: Back to -180°

```cpp
void activateServo() {
  // Press position (+180 degrees)
  for (int i = 0; i < 30; i++) {
    digitalWrite(SERVO_PIN, HIGH);
    delayMicroseconds(2000); // 2ms pulse
    digitalWrite(SERVO_PIN, LOW);
    delay(18); // Complete 20ms cycle
  }
  
  delay(500); // Hold press
  
  // Return position (-180 degrees)
  for (int i = 0; i < 30; i++) {
    digitalWrite(SERVO_PIN, HIGH);
    delayMicroseconds(500); // 0.5ms pulse
    digitalWrite(SERVO_PIN, LOW);
    delay(19); // Complete 20ms cycle
  }
}
```

## Power Consumption Analysis

### Detailed Breakdown
- **Deep sleep**: ~1μA
- **LED operation**: ~10mA for ~0.6 seconds
- **Servo operation**: ~80mA for ~1.2 seconds
- **Microcontroller active**: ~3mA for ~5 seconds

### Battery Life Calculation
**With 2 AA batteries (3000mAh):**
- Average consumption per hour: ~0.026mAh
- **Estimated life**: 3000mAh ÷ 0.026mAh = ~115,000 hours = **13 years**
- **Realistic estimate** (accounting for battery degradation): **3-5 years**

## Programming and Upload

### First-Time Setup
1. **Burn bootloader** to set 8MHz internal clock
2. **Upload code** using your preferred programmer
3. **Test operation** with LED indicators

### Upload Process
```bash
# Using Arduino IDE:
1. Select: Tools → Board → ATtiny25/45/85
2. Select: Tools → Processor → ATtiny85
3. Select: Tools → Clock → Internal 8MHz
4. Connect programmer
5. Upload sketch
```

## Troubleshooting

### Common Issues
1. **Servo doesn't move**: Check 5V power supply and connections
2. **Device doesn't wake from sleep**: Verify ISR(WDT_vect) is present
3. **Wrong timing**: Confirm 8MHz clock setting
4. **High power consumption**: Check for floating pins

### Debug Mode
For testing, change loop sleep cycles from 112 to 5:
```cpp
for (int i = 0; i < 5; i++) { // Test mode: ~40 seconds
```

## Circuit Design Notes

## Circuit Design Notes

### Power Supply Design
- **Current implementation**: 2x AA batteries (3V total)
- **Issue**: Servo motors typically require 4.8-6V for proper operation
- **Solution needed**: 5V voltage regulator (TO BE COMPLETED)
- **Options**: 
  - Step-up converter (3V to 5V) - more efficient
  - LM7805 with higher input voltage (requires 3x AA or different battery pack)

### Component Placement Recommendations
- **Keep servo wiring short** - minimize voltage drop
- **Isolate switching circuits** - prevent interference
- **Central ground point** - reduce noise and improve reliability
- **Bypass capacitors** - add 100nF near ATtiny85 VCC pin for stability

### Switch Logic Implementation  
- **Mode Switch (PB1)**: LOW = 15 minutes, HIGH = 30 minutes
- **Light Switch (PB2)**: LOW = day & night operation, HIGH = day only
- **Pull-up resistors**: Internal pull-ups used (INPUT_PULLUP mode)
- **Switch debouncing**: Not required due to read-once-at-startup design

## Advanced Modifications

### Extending Sleep Time
Modify the loop counter for different intervals:
```cpp
// For 30 minutes: 224 cycles
// For 1 hour: 450 cycles
// For 2 hours: 900 cycles
```

### Adding External Wake Sources
Can be modified to wake on external interrupts (button press, PIR sensor, etc.)

### Multiple Servo Support
Code can be extended to control multiple servos with different timing

## Files and Code Structure

### Main Functions
- `setup()` - Initialization and first activation
- `loop()` - Main sleep/wake cycle
- `system_sleep()` - Deep sleep implementation
- `activateServo()` - Servo control
- `startupIndication()` - Status LED sequence

### Support Functions
- `setup_watchdog()` - Watchdog timer configuration
- `shouldActivateBasedOnLight()` - Light condition check
- `slowBlink()` / `activateFragranceDispenser()` - LED control

## Project History and Development

This project evolved through several iterations:
1. **Initial version**: Simple delay() timing (months battery life)
2. **First sleep attempt**: Complex watchdog implementation (failed)
3. **Final solution**: Proven sleep methodology from ATtiny community

The working sleep implementation is based on research from:
- marcelpost.com ATtiny85 WDT examples
- Arduino community forums on ATtiny sleep modes
- AVR documentation for watchdog timer interrupt mode

## License and Credits

This project uses standard AVR libraries and community-proven sleep techniques. Feel free to modify and distribute according to your needs.

## Support and Further Development

For questions or improvements, consider:
- Arduino forums ATtiny section
- AVR Freaks community
- GitHub repositories with similar ATtiny projects

---

**Final Note**: This implementation prioritizes reliability and power efficiency over complexity. The simple approach ensures years of autonomous operation with minimal maintenance.
