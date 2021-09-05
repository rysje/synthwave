#pragma once

enum class VoiceState
{
	Inactive,
	Attack,
	Decay,
	Sustain,
	Release
};

struct ADSR
{
	ADSR();
	ADSR(double startVal, int attackLen, double attackVal, int decayLen, double sustainVal, int releaseLen,
	     double finalVal);
	double value(VoiceState state, int bufferNumber) const;
	double startVal = 0.0;
	int attackLen = 150;
	double attackVal = 1.0;
	int decayLen = 150;
	double sustainVal = 0.5;
	int releaseLen = 600;
	double finalVal = 0.0;
};
