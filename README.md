Keg Monitoring System
============
Sick and tired of kicking homebrew kegs, and not wanting to afford the expensive flow monitor based commercial solutions,
I instead developed a weight based keg monitoring system using inexpensive load cells found in postage meters.

This is the code from Ryan McKelly, rocket scientist and master of bacon, using techniques gleamed from multiple open source projects:

Adafruit's Sous-Viduino:         
https://learn.adafruit.com/sous-vide-powered-by-arduino-the-sous-viduino

Adafruit's RGB LCD Shield:       
https://learn.adafruit.com/rgb-lcd-shield

Adafruit's Data Logger Shield:   
https://learn.adafruit.com/adafruit-data-logger-shield

The data logging shield will record the setting to a more robust memory than the EEPROM (I did not want to limit to 100k writes)
and will also enable eventual usage / tracking functionality for the keg monitor.  The RGB shield will provide a simple way
to develop a settings and config menu (tare weight, initial configuration, etc).  The Sous-Viduino provides a good example of
how to configure a simple menu for a 16x2 character LCD.