PPMDisplay
==========

Arduino Project To Display RC Servo PPM Data

Nothing much to the H/W setup. A standard Arduino with the common keypad shield, and wire a servo connector to Arduino pin 3. Can also be used with I2C display through #define statements.

Use the Up and Down buttons to scroll through the channels

Use the Left button to select a display of the number of channels captured, frame update rate, and two channels. Use the Right button to select display of four channels.

Use the Select button to reset the display.

Also works fine for display of a normal servo control signal.

Includes a servo drive function so that you can test your servos. You can send a minimum 1ms pulse, a center 1.5ms, or a maximum 2ms. If you connect a pot to the pin defined in the code by POT_PIN, such that it sends a voltage between 0V and 5V, then you can turn the pot to set the servo position, or use it to set the amplitude around 1.5ms as the servo is swept back and forth. You get to the servo output mode by pushing the right button twice, and select the output mode with the up and down buttons.




I have only tried it with positive pulses (normal servo ccontrol and the FrSky CPPM). Not sure if other manufacturers use negative pulses for their equivalent of CPPM. 

Uses code derived from "Read-any-PPM" (https://code.google.com/p/read-any-ppm/ArduPilotMega) released under GNU GPL v3. This code is released under the same conditions.

