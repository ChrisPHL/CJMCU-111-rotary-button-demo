Demo making use of the "JMCU-111" rotary button.
A schematic of it can be found at
https://easyeda.com/hogo20/CJMCU-111-rotary-encoder-schematic.

The Interrupt Service Routine assumes to be called on every FALLING and RISING edge of
button's inputs Ga and Gb, even the bouncing ones.
It also is prepared to catch push button events of the CJMCU-111 rotary button. This does work if
resistor at push button line is bridged (0 Ohm).
By doing this hack there is a chance to do a rotary button push by turning CCW the wheel but
turning it back CW when in the middle of the way of the actual rotary tick.
If you want to prevent this possiblilty you can exchange the 3k Ohm resister with about 500 to
1500 Ohm instead and connect Ga to an analog input of the Arduino. You can then distinct the cause
for the low level of Ga within ISR by calling analogRead(Ax) where x is the number of the analog
input pin used.

Overall performance is pretty stable though. :-)

