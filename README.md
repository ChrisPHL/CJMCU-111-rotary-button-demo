# Interrupt driven CJMCU-111

## Introduction
Demo making use of the "CJMCU-111" rotary button by just  utilizing interrupts.
Yes, it's interrupt drivens so you'll never miss any input click! :-)
This code supports not only left/right rotation clicks but push button clicks also. (This need minor hardware change though.) 

## How to use
Connection to your Arduino is very simple: Connect the two pins (Ga, Gb) of the CJMCU-111 to interrupt supported pins of the Arduino device. Short Ga to another analog input pin of the Arduino if you need to avoid false pushes at all costs. Note: False pushes are very rare though.
Setup the pins used in the code.

A schematic of it can be found at
https://easyeda.com/hogo20/CJMCU-111-rotary-encoder-schematic.


## How does it work?
The Interrupt Service Routine assumes to be called on every FALLING and RISING edge of
button's inputs Ga and Gb, even the bouncing ones.
It also is prepared to catch push button events of the CJMCU-111 rotary button. This does work if
resistor at push button line is bridged (0 Ohm - In other words: Just short pins 4 and 1 of the rotary button.).
By doing this hack there is a chance to do a rotary button push by turning CCW the wheel but
turning it back CW when in the middle of the way of the actual rotary tick.
If you want to prevent this possiblilty you can bridge the 3k Ohm resister with 330 Ohm instead and connect Ga to an analog input of the Arduino. You can then distinct the cause for the low level of Ga within ISR in case kRotationStatusStartLorPush by calling analogRead(Ax) where x is the number of the analog
input pin used.

Overall performance is pretty stable though. :-)
