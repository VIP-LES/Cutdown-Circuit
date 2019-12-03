// Takes 8 s to ensure cutdown:
int CUTDOWN_TIME_MILLIS = 8000;
Timer ctimer(CUTDOWN_TIME_MILLIS, deactivate_circ);


// Timed cutdown mechanism, cutdown after 3.5 hours.
int hours = 7 * 60 * 60 * 1000 / 2;
Timer hourtimer(hours, activate_hour);


void setup() {
  
  // The cutdown pins need to be configured first to let them
  // act as power sources. Connect the relay channels to the
  // Particle Board's pins as follows:
  // IN1 -> B0
  // IN2 -> B1
  // VCC -> VIN (top right of board, next to -il, i.e. GROUND)
  // GND -> -il (symbol for GROUND)
  
  hourtimer.reset();
  
  pinMode(B0, OUTPUT);
  pinMode(B1, OUTPUT);
  pinSetFast(B0);
  pinSetFast(B1);
  
  Particle.function("cutcirc2", activate_cutdown2);
  Particle.function("resethour", reset_hourtimer);
  
}

void loop() {
    locator.loop();
}


int reset_hourtimer(const char* msg) {
    hourtimer.reset();
    return 13;
}

void activate_hour() {
    activate_cutdown2(0);
    hourtimer.stop();
}

// Remote control activated cutdown. Hit via console.particle.io through
// the manual "call fuction" capability.
int activate_cutdown2(const char* msg) {
    ctimer.reset(); // starts if stopped
    pinResetFast(B0);
    pinResetFast(B1);
    return 11;
}

void deactivate_circ() {
    pinSetFast(B0);
    pinSetFast(B1);
    ctimer.stop();
}
