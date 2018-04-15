# M5Widgets
Widgets for the M5Stack

This is based on the [alas now defunct!] [Phoenard project](https://github.com/Phoenard/Phoenard). The Phoenard had a touchscreen, whereas the M5Stack has a "dumb" screen. So instead of copy/pasting and adapting the code, I am selecting the juicy, non-touch bits, and adapting them to the M5, while simplifying a bit.

# Installation

As I am not integrating this to the M5Stack library, it has become simpler. Drop the folder into the Arduino/libraries folder and restart the Arduino IDE.

In your code, add:

    #include "utility/M5Widget.h"

You have now access to the M5Widgets. Yay!

There are a few examples in the examples folder, I will add more soon.
