This is the code for the Infini-pan (tm). A continuous rotation pan/tilt system for use with iNav.

Infini-pan (tm) uses a stepper motor to allow continuous rotation. This code translates an SBUS signal from iNav into steps for the stepper driver. It translates a stick position on your transmitter into a rate of rotation of the motor.

# iNav 3.0+ required.

# Disclaimers
- This code is written for an ESP32 running at 240Mhz. I don't think it'll work the same on any slower microcontrollers, especially a 16Mhz Arduino, due to the speed at which the stepping interrupt needs to run.
- You'll need to manually put in the channel numbers you're using. Each channel is as transmitted by iNav (by servo output number), not by transmitter channel.
- The code is probably really buggy, and sometimes goes crazy between iNav booting up and you moving a stick. I haven't worked out why.
- Pull requests are welcome to fix all of the above.
- Mounting the motor/slip ring/camera is left as an exercise for the reader.

# You will need
1. Stepper motor: https://www.banggood.com/28BYJ-48-5V-4-Phase-DC-Gear-Stepper-Motor-DIY-Kit-p-1553617.html
2. Motor driver: https://www.banggood.com/ULN2003-Stepper-Motor-Driver-Board-Module-for-5V-4-phase-5-line-28BYJ-48-Motor-Geekcreit-for-Arduino-products-that-work-with-official-Arduino-boards-p-1532830.html
3. Slip ring: https://www.aliexpress.com/item/4000338803341.html

If you're not going to use a tilt servo, get a slip ring with 3 conductors (vid, gnd, v+). If you want tilt you can get away with 5, 6 conductors is easiest.

# iNav Setup

1. Set `servo_protocol` to `SBUS_PWM`
2. Choose a serial port and set the configuration for it like so: `serial 2 4194304 115200 115200 0 115200` (for serial 3)
3. Connect the sbus pin on your arduino to the tx pin of your chosen serial port.
