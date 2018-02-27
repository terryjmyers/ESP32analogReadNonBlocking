/*
ESP32analogReadNonBlocking
Terry Myers
github.com/terryjmyers/ESP32analogReadNonBlocking

Parameters
uint132_t NumOfSamples = Number of samples to average together for better smoothing.  Note that this should be limited to <= 1,048,831 (2^32 / 4095) samples due to the uint32_t size
unit8_t &ArbitrationToken = A global variable that each instance will use as a round robin token arbitrate the ADC

I.E. XXX.analogRead(100, ADC1ArbitrationToken).  This will take 100 samples ( minimum of ~1ms @ 10us per sample)

Theory of operation:
Pass in the Number of Samples to take and a globally scoped Arbitration Token.
If the adc isn't busy and the arbitration token is 0 (ADC is free), this library will change the token to the pin to "take ownership" of ADC1, initiate a conversion, and immediatly return to allow your code to process
Oneach subsequent call of analogRead, it will check to see the adc conversion is done.  If so, it will initate another one if NumOfSamples > 1.
If the NumOfSamples has been reached (even if you juset set to 1), it will update counts and release arbitration.
This will allow the next instance of this function to take ownership and use the ADC.
Ownership and use of the ADC will thusly happen in a "round robin" fashion in the order of code execution.

*/

#include <ESP32analogReadNonBlocking.h>

//instantiate as many as you need
	//These two are on ADC1 and will round robin with each other
		ESP32analogReadNonBlocking pin36(36, 100000);
		ESP32analogReadNonBlocking pin39(39, 100000);
	//These two are on ADC2 and will round robin with each other
		ESP32analogReadNonBlocking pin04(4, 100000);
		ESP32analogReadNonBlocking pin25(25, 100000);

	//Create one Aribtration token per ADC used in your project
	uint8_t ArbitrationToken1; //Use one Aribtration Token for anything on ADC1, GPIO: 34, 35, 36, 37, 38, 39
	uint8_t ArbitrationToken2; //Use one Arbitration Token for anything on ADC2, GPIO: 4, 12, 13, 14, 15, 25, 26, 27

	uint32_t loopcounter;//loop counter for example to show that the code is non-blocking
	uint32_t loopcounterSerialPrintTimer;

void setup() {
	Serial.begin(115200);	
}

void loop()
{	
	//Call the tick function every loop.  It handles the arbitration of the ADC, reading of the ADC, and sample averaging
	pin36.tick(ArbitrationToken1); 
	pin36.tick(ArbitrationToken1);
	pin04.tick(ArbitrationToken2);
	pin25.tick(ArbitrationToken2);

	/*
	That's all you need
	"pinXX.counts" will contain the ADC reading you can use in your code

	100,000 samples @ ~10us per sample gives a ~1sample per second rate which is useful for this example to not flood the serial output
	Each time the newValueFlag is true, print the raw averaged value to serial.
	You'll notice in the example below that GPIO36 and GPIO39 will take turns using ADC1, and GPIO04 and GPIO25 will take turns using ADC2
	Change the number of samples on the fly: pin36.NumOfSamples=10;
	*/
	if (pin36.newValueFlag) { Serial.print("GPIO36 raw counts = "); Serial.print(pin36.counts); }
	if (pin39.newValueFlag) { Serial.print("GPIO39 raw counts = "); Serial.print(pin39.counts); }
	if (pin04.newValueFlag) { Serial.print("GPIO04 raw counts = "); Serial.print(pin04.counts); }
	if (pin25.newValueFlag) { Serial.print("GPIO25 raw counts = "); Serial.print(pin25.counts); }
	
	//Once a second print how many loops the code has executed
		loopcounter++;
		if (millis() - loopcounterSerialPrintTimer > 1000) {
			Serial.print("Loops: "); 
			Serial.println(loopcounter);
			loopcounterSerialPrintTimer = millis();
		}
}
