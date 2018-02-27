/*
ESP32analogReadNonBlocking
v1.1
Terry Myers
github.com/terryjmyers/ESP32analogReadNonBlocking

Theory of operation:
A globally scoped Arbitration Token is passed in.
If the adc isn't busy and the arbitration token is 0 (ADC is free), this library will take ownership of the ADC by changing the valud of the token to its pin number,
     initiate a conversion, and immediatly return to allow your code to process
On each subsequent call of tick, it will check to see the adc conversion is done.  If so, it will initate another one if NumOfSamples > 1.
If the NumOfSamples has been reached (even if you juset set to 1), it will update ".counts" and release arbitration.
This will allow the next instance of this function to take ownership and use the ADC.
Ownership and use of the ADC will thusly happen in a "round robin" fashion in the order of code execution.

Notes:
1. The number of samples to take should be limited to <= 1,048,831 (2^32 / 4095) samples due to the uint32_t size of NumOfSamples
2. Change NumOfSamples on the fly after class instantiation by simply changing NumOfSamples.
3. Each ADC conversion only takes ~10us.  Use this to figure out how best to optimize the number of samples you need vs how many GPIO are trying to use a single ADC
*/

#ifndef ESP32
#error "ESP32analogReadNonBlocking.h library only valid for ESP32"
#endif // !ESP32
#include <Arduino.h>
#ifndef ESP32analogReadNonBlocking_H
#define ESP32analogReadNonBlocking_H

class ESP32analogReadNonBlocking
{
	public:
		ESP32analogReadNonBlocking(uint8_t pin, uint32_t NumOfSamplesToAverage) { // uint8_t pin = Valid GPIO pin using ADC1(GPIO34,35,36,37,38,and 39) or ADC2(GPIO4,12,13,14,15,25,26,and 27).  NumOfSamples to average.  *NOTE GPIO25, 26, and 27 cannot be used on ESP32-WROVER module
			NumOfSamples = NumOfSamplesToAverage;
			_pin = pin;
			//An initial useless adc read is performed because when the ESP32 boots up its ADC its "busy".
			//Doing this at least once, "releases" the ADC for the first instance of this library to take ownership and start working
			adcAttachPin(_pin);
			adcStart(_pin);
		};

		void tick(uint8_t &ArbitrationToken); //call this every loop
		uint32_t NumOfSamples; //Change the number of samples on the fly after the constructor if you need to
		uint16_t counts; //access the raw counts directly at any time
		bool newValueFlag; //Set true for one loop when ".counts" is updated with a new averaged number.  Can be used to update scaling and trigger other code only when nessesary

	private:

		uint8_t _pin;
		//Used for averaging
		uint32_t _adcSamplesTotal;
		uint32_t _adcCurrentSamples;
};
#endif