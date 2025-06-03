# ATtiny85 Fragrance Dispenser

![Project Prototype](images/project_prototype.png)

A battery-powered fragrance dispenser that automatically sprays every 15 or 30 minutes. Runs for **3-5 years** on 2 AA batteries using intelligent sleep modes.

## Why This Project?

I needed an automatic air freshener but couldn't find one with long battery life and customizable timing. After testing different approaches, this design achieves years of operation instead of weeks.

## Key Features

- **3-5 year battery life** on 2 AA batteries
- **15 or 30 minute intervals** via switch
- **Day-only mode** using light sensor  
- **Status LED** shows settings at startup
- **Ultra-low power** watchdog sleep mode

## Quick Start

1. **Get the code**: Download `fragrance_dispenser.ino` from the `src/` folder
2. **Set up Arduino IDE**: Install ATtiny board support, select ATtiny85 @ 8MHz
3. **Build circuit**: Use the breadboard layout below or Fritzing files
4. **Upload and test**: Device will show status on LED, then start working

## Circuit Design

### Breadboard Layout
![Breadboard](schematics/ATtiny85_Fragrance_Dispenser_bb.png)

### PCB Version  
![PCB](schematics/ATtiny85_Fragrance_Dispenser_pcb.png)

## Pin Connections

```
ATtiny85 Pinout:
     ┌─────┬─────┐
RESET│1   8│+5V
LIGHT│2   7│DAY/NIGHT SWITCH
  LED│3   6│15/30MIN SWITCH  
  GND│4   5│SERVO
     └─────┴─────┘
```

- **Pin 2**: Light sensor (LDR + 10kΩ to ground)
- **Pin 3**: Status LED (+ 220Ω resistor)
- **Pin 5**: Servo signal wire
- **Pin 6**: Time switch (15/30 min)
- **Pin 7**: Mode switch (day only/always)

## How It Works

The device sleeps most of the time (consuming ~1μA) and wakes every 8 seconds using the watchdog timer. After the right number of wake cycles (112 for 15 minutes), it activates the servo to press the spray button.

**Power consumption**:
- Sleep: ~1μA (99.98% of the time)
- Active: ~15mA (0.02% of the time)
- Result: Years of battery life instead of weeks

## Status LED Sequence

At startup, the LED shows current settings:
1. **Light check**: 1 blink = day, 2 blinks = night
2. **Time mode**: 1 blink = 15min, 2 blinks = 30min  
3. **Operation**: 1 blink = always on, 2 blinks = day only

## Files Included

- `src/fragrance_dispenser.ino` - Main code
- `schematics/ATtiny85_Fragrance_Dispenser.fzz` - Fritzing project
- `schematics/ATtiny85_Fragrance_Dispenser_bb.png` - Breadboard layout
- `schematics/ATtiny85_Fragrance_Dispenser_pcb.png` - PCB design

## Troubleshooting

**Servo doesn't move**: Check 5V power supply
**Wrong timing**: Verify 8MHz clock setting in Arduino IDE  
**Won't wake from sleep**: Make sure ISR(WDT_vect) function is present

## License

MIT License - use and modify freely.
