#include "ESP32analogReadNonBlocking.h"

void ESP32analogReadNonBlocking::tick(uint8_t &ArbitrationToken) {
	//&unit8_t &ArbitrationToken = A global variable that this routine will assign to handle arbitration between itself and other instances of this routine
	newValueFlag = false;
	if (adcBusy(_pin) || (ArbitrationToken != _pin && ArbitrationToken > 0)) return; //no point in doing anything if the adc is already doing something or someone else has ownership

	if (ArbitrationToken == 0) { //adc isn't busy, nor is it arbitrated, now's your chance!
		adcAttachPin(_pin);
		adcStart(_pin);
		ArbitrationToken = _pin; //take ownership of the token by setting it to your pin number
		_adcSamplesTotal = 0; //reset number of samples and total for averaging
		_adcCurrentSamples = 0;
		return; //no reason to continue, check for completion on the next loop
	}
	else if (ArbitrationToken == _pin) { //you have ownership and the adc is done.  This really could have been just an else statement, but for completeness...
		
		_adcSamplesTotal += adcEnd(_pin); //get the adc results and add them to a total for averaging
		_adcCurrentSamples++; //increment the number of samples taken for averaging

		if (_adcCurrentSamples >= NumOfSamples) { //you have taken the requested number of samples, take the average and release arbitration
			counts = _adcSamplesTotal / _adcCurrentSamples; //take the average
			ArbitrationToken = 0; //release arbitration
			newValueFlag = true;
		}
		else { //you have more samples to take, start the adc again
			adcStart(_pin);
		}
	}

}