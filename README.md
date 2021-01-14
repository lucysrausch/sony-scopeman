# Sony-Scopeman
Hardware Design files of a replacement mainboard for the Sony Watchman FD-10. This turns it into a bluetooth and WiFi-enabled vector display.

# Features
There are two modes which can be selected via the power switch. 
* In "Audio" mode, the ESP32 acts as a bluetooth speaker. Play back audio files from your smartphone or laptop to hear and see the soundwaves. You can change the size by adjusting the playback volume

Demo: https://twitter.com/FauthNiklas/status/1337467949318279170
* In "Video" mode, the ESP32 renders the result of the Lorenz Attractor equation. You can change the simulation speed using the "Tune" knob.

Demo: https://twitter.com/FauthNiklas/status/1339650704584171522

# Instructions

You'll need:
* Sony Watchman Model FD-10
* Pre-assembled replacemend mainboard (eg. from JLCPCB assembly)
* Two 803040 (1000mAh) LiIon batterys with JST-PH connector

![](/pictures/photo_2021-01-13_22-03-22.jpg)

Open the Sony Watchman and unscrew the original mainboard. Make sure to keep all the screws in a safe place.

![](/pictures/photo_2021-01-13_22-03-24.jpg)

Unsolder all wires from the mainboard.

![](/pictures/photo_2021-01-13_22-03-25.jpg)

Unsolder (and unscrew) these components from the mainboard. We'll need them for our replacement.

![](/pictures/photo_2021-01-13_22-03-26.jpg)
![](/pictures/photo_2021-01-13_22-03-27.jpg)

Install all unsoldered components on the replacement mainboard.

![](/pictures/photo_2021-01-13_22-03-28.jpg)

Unsolder the speaker cables (black and white) from the flyback PCB. Solder them to the replacement PCB (take care of polarity. White is closer to the ESP32).
Remove all other cables from the flyback PCB except for blue, yellow, black and purple.

![](/pictures/photo_2021-01-13_22-03-29.jpg)

Solder the blue (flyback signal) and yellow (blanking) to the new mainboard.
Do the same for purple (supply) and black (GND).

![](/pictures/photo_2021-01-13_22-03-31.jpg)

Next, connect the deflection coil wires (purple, white, red and blue). 

![](/pictures/photo_2021-01-13_22-03-32.jpg)

You can now plug in the batteries (make sure the watchman is turned off) and screw the mainboard back in place. You need two screws for this (mind the arrows on the back of the PCB).

![](/pictures/photo_2021-01-13_22-03-33.jpg)

Now you can add a slothole to the lower half of the case for the USB type C jack. You also need to brake away some plastics to make space for the new batteries. No worries, if done correctly this won't be visible from the outside and the battery cover will still close.

![](/pictures/photo_2021-01-13_22-03-34.jpg)

That's it! Before you screw the watchman back together and store the batteries behind the cover, you may want to adjust the two potentiometers on the mainboard to center the image. But first you need to flash the firmware. Turn on the watchman and connect it via USB. You can then flash the ESP32 using the Arduino IDE (make sure to select "Board: ESP Wrover Module").

You might also need to readjust the focus. This can be done from outside using the original focus knob (hidden behind a small hole at the side).

![](/pictures/photo_2021-01-13_22-03-35.jpg)

# Credits
Deflection coil drivers inspired by http://www.e-basteln.de/file/asteroids/Vectrex_XY_Driver.pdf

A2DP Arduino lib for ESP32: https://github.com/pschatzmann/ESP32-A2DP
