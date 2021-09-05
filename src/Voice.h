#pragma once

#include <jack/types.h>
#include "Wavetable.h"
#include "ADSR.h"

class Voice
{
public:
	Voice(double frequency, jack_nframes_t sample_rate, Wavetable &wavetable);
	void Process(jack_default_audio_sample_t *buffer, jack_nframes_t nframes);
	bool isActiveInCurrentBuffer();
	void on(unsigned char velocity);
	void off();
	static void setFrequencyModulation(double value);
	static void setSustainPedal(unsigned char value);

	inline static int attackLen = 150;
	inline static int decayLen = 150;
	inline static double sustainLev = 0.5;
	inline static int releaseLen = 600;
	inline static double freqMod = 1;
private:
	VoiceState state;
	double amplitude;
	int buffersInCurrentState;
	bool noteOn;
	inline static bool sustainPedalOn = false;
	ADSR ampAdsr;
	double phase;
	jack_default_audio_sample_t ramp_step;
	double baseFrequency;
	Wavetable &wavetable;
	jack_nframes_t sampleRate;
};
