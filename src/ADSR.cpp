#include <cmath>
#include "ADSR.h"

double ADSR::value(VoiceState state, int bufferNumber) const
{
	switch (state) {
		case VoiceState::Inactive:
			return 0.0;
		case VoiceState::Attack:
			return std::lerp(startVal, attackVal, (double) bufferNumber / (double) attackLen);
		case VoiceState::Decay:
			return std::lerp(attackVal, sustainVal, (double) bufferNumber / (double) decayLen);
		case VoiceState::Sustain:
			return sustainVal;
		case VoiceState::Release:
			return std::lerp(sustainVal, finalVal, (double) bufferNumber / (double) releaseLen);
	}
	return 0.0;
}

ADSR::ADSR(double startVal, int attackLen, double attackVal, int decayLen, double sustainVal, int releaseLen,
           double finalVal) : startVal(startVal), attackLen(attackLen), attackVal(attackVal), decayLen(decayLen),
           sustainVal(sustainVal), releaseLen(releaseLen), finalVal(finalVal)
{

}

ADSR::ADSR() :
	startVal(0.0),
	attackLen(150),
	attackVal(1.0),
	decayLen(150),
	sustainVal(0.5),
	releaseLen(600),
	finalVal(0.0)
{

}
