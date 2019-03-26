/*
Get some initial cutdown circuit stuff working in here.
The cutdown circuit requires a HIGH and a LOW to work, I imagine.
Need to clarify this thing with the cutdown team, and hopefully test it out
once on the Particle board.


The documentation for the pin stuff can be found here:

https://docs.particle.io/reference/device-os/firmware/photon
Device OS API > Input/Output > pinMode()
Device OS API > Low Level Input/Output > pinSetFast(), pinResetFast()
*/


// WARNING
// Pin 12 is the pin that was being used from Adam's cutdown code, found
// here:
// https://github.com/VIP-LES/Cutdown-Circuit/blob/master/test-cutdown-timer.ino
// Ensure that this pin isn't being used by other devices, because if you don't,
// that device may get fried.
int CUTDOWN_PIN = 12;

// Takes 8 seconds to ensure cutdown?
int CUTDOWN_TIME_MILLIS = 8000;


void setup() {
	// The cutdown pin need to be configured first to let it act as a power
	// source.
	pinMode(CUTDOWN_PIN, OUTPUT);
	Particle.function("cutcirc2", activate_cutdown);
}


void loop() {

}

// This is a cheap cutdown circuit method, since it simply activates, waits, and
// deactivates. I think there is an issue here: this can't be triggered through
// an nterrupt, since the delay function blocks (I think it blocks).
void activate_cutdown() {
	// Set the cutdown pin to HIGH for some time, and then set it to low again.
	pinSetFast(CUTDOWN_PIN);
	delay(CUTDOWN_TIME_MILLIS);
	pinResetFast(CUTDOWN_PIN);
}