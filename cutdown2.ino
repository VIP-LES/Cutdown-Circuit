//this is all we need to grab the data from the serial port and put it through the publisher
String content = "";
char character;


// Takes 8 seconds to ensure cutdown?
int CUTDOWN_TIME_MILLIS = 8000;
Timer ctimer(8000, deactivate_circ);

// alternate timer cutdown:
// https://docs.particle.io/reference/device-os/firmware/photon/#start-
int periodicity = 60000; // call every 60,000ms = 60s = 1 min
Timer timer(periodicity, time_to_cutdown);
int CUTDOWN_MINS_LIMIT = 180; // cutdown after 180 mins = 3 hrs

void setup() {
  Serial.begin(9600);   //this is the usb port at 9600
  
  
  // The cutdown pin need to be configured first to let it act as a power source.
  pinMode(B0, OUTPUT);
  pinMode(B1, OUTPUT);
  Particle.function("cutcirc2", activate_cutdown);
  
  timer.start();
}

void loop() {

}


void time_to_cutdown() {
    static int hit_cutdown = 0;
    static int plus = 1;
    
    hit_cutdown += plus;
    
    if (hit_cutdown >= CUTDOWN_MINS_LIMIT) {
        hit_cutdown = 0;
        plus = 0;
        activate_cutdown(NULL);
    }
}


void serialEvent()  //built in callback from serial
{
  while (Serial.available()) {
      character = Serial.read();//reads character by character
      content.concat(character);//dumps into a string that'll deal with the variable size on the backend
  }
  Particle.publish("data", content, PRIVATE); //We are publishing all in one topic to save a few bytes
  content = "";
}


// curl https://api.par373037383634/cutcirc2 -d access_token=b38d047a9da4e6b5e60f8a6f62814bdb0ee8267a

// This is a cheap cutdown circuit method, since it simply activates, waits, and
// deactivates. I think there is an issue here: this can't be triggered through
// an nterrupt, since the delay function blocks (I think it blocks).
int activate_cutdown(const char* msg) {
	// Set the cutdown pin to HIGH for some time, and then set it to low again.
	pinSetFast(B0);
	pinSetFast(B1);
	delay(CUTDOWN_TIME_MILLIS);
	pinResetFast(B0);
	pinResetFast(B1);
	return 13;
}


// an alternate cutdown circuit approach, will test later. For now, the above works.
int activate_cutdown2(const char* msg) {
    pinSetFast(B0);
    pinSetFast(B1);
    ctimer.reset(); // starts if stopped
    return 11;
}

void deactivate_circ() {
    pinResetFast(B0);
    pinResetFast(B1);
    ctimer.stop();
}