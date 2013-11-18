PPMDisplay
==========

Arduino Project To Display RC Servo PPM Data

Nothing much to the H/W setup. A standard Arduino with the common keypad shield, and wire a servo connector to Arduino pin 3.

Use the Up and Down buttons to scroll through the channels

Use the Left button to select a display of the number of channels captured, frame update rate, and two channels. Use the Right button to select display of four channels.

Use the Select button to reset the display.

Also works fine for display of a normal servo control signal.

I have only tried it with positive pulses (normal servo ccontrol and the FrSky CPPM). Not sure if other manufacturers use negative pulses for their equivalent of CPPM. 

Uses code derived from "Read-any-PPM" (https://code.google.com/p/read-any-ppm/ArduPilotMega) released under GNU GPL v3. This code is released under the same conditions.
