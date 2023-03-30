# EECS373-DASA
## Road Map
- [ ] Rotor control
- [ ] IR
- [ ] Speaker
- [ ] LED 
- [ ] Object detection
- [ ] Central control: object avoidance

## Motor control
Forward, backward, left turn, right turn, stand-still rotate
### Components
1. Nucleo L4R5ZIP
2. 12VDC, 200rpm, 0.78Kg-cm GHM-16 (w/ rear shaft)
3. L298 H-Bridge heavy duty w/breakout
### Implement
1. Nucleo control H-bridge logic input (direction of spin)
2. Nucleo control motor speed through PWM via H-bridge enable pin (turning on and off to control spinning speed)
3. Connect Vs on H-bridge

## IR sensor
https://learn.adafruit.com/ir-breakbeam-sensors/overview
1. The receiver is open collector transistor output which means that a pull up resistor is needed.