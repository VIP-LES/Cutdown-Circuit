# Cutdown-Circuit
Any code related to the cutdown circuit.

## test-cutdown-timer
Assuming the cutdown circuit is set up properly with the GPS Ultimate Shield, this will activate the cutdown circuit after 15 seconds.

## Cutdown Circuit 2
Contains code to activate the cutdown circuit. Important notes:  
* The current cutdown pin is pin 12. If another device is currently using this pin, this will have to be changed.
* The cutdown activation duration is 8 seconds. Depending on how long the cutdown circuit needs to be activated to fry the nichrome wire, this value can and should be changed.
* This is a basic version of the cutdown code that can't be called with interrupts.

### Some more documentation, code explained:
* The cutdown circuit takes about 8 seconds to get sparked, and needs a voltage of 9V. The Particle board doesn't provide that voltage, but it does flick a pin that's connected to a relay switch. -- To alter time, change `CUTDOWN_TIME_MILLIS`.
* PIN Documentation can be found [here](https://docs.particle.io/reference/device-os/firmware/photon/#input-output).
* The pins need to be set to `OUTPUT` mode first. This is so they can be set either `HIGH` or `LOW`. This is done in `setup()`.
* The Particle board can be hit remotely, through an `HTTP POST` request. This can be done through either this command - `curl https://api.par373037383634/cutcirc2 -d access_token=xxxx`(the access_token can be found on the Particle Console), or through the "Function" option on the Particle Console.
* A "function" has been registered with the Particle board, to accept `HTTP POST` requests on the URL mentioned earlier.
* The function introduces a complication - code that is called through that listener CANNOT be blocking. Originally, the code had a `delay(CUTDOWN_TIME_MILLIS)`, but this was causing the system to crash (sometimes). Therefore, this approach can't be used to hold the PIN to `HIGH` for the 8 seconds needed for cutdown to happen.
* To get around this, we have a timer to go off. The timer document can be found [here](https://docs.particle.io/reference/device-os/firmware/photon/#software-timers). This timer is a software timer, and we have a listener for it.
* We use software timers to set the automatic cutdown as well, for 5 hours or so. To alter how long it takes, change `CUTDOWN_MINS_LIMIT`, from 300 (300 mins = 5 hours) to a new amount of time, in minutes.