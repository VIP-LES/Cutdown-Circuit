# Cutdown-Circuit
Any code related to the cutdown circuit.

## test-cutdown-timer
Assuming the cutdown circuit is set up properly with the GPS Ultimate Shield, this will activate the cutdown circuit after 15 seconds.

## Cutdown Circuit 2
Contains code to activate the cutdown circuit. Important notes:  
* The current cutdown pin is pin 12. If another device is currently using this pin, this will have to be changed.
* The cutdown activation duration is 8 seconds. Depending on how long the cutdown circuit needs to be activated to fry the nichrome wire, this value can and should be changed.
* This is a basic version of the cutdown code that can't be called with interrupts.