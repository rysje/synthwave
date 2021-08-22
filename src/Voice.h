#pragma once

#include <jack/types.h>
#include "Wavetable.h"

enum class VoiceState
{
	Inactive,
	Attack,
	Decay,
	Sustain,
	Release
};

class Voice
{
public:
	Voice(double frequency, jack_nframes_t sample_rate, Wavetable &wavetable);
	void Process(jack_default_audio_sample_t *buffer, jack_nframes_t nframes);
	bool isActiveInCurrentBuffer();
	void on();
	void off();
	static void setFrequencyModulation(double value);
	static void setSustainPedal(char value);

	inline static int attackLen;
	inline static int decayLen;
	inline static int sustainLev;
	inline static int releaseLen;
	inline static double freqMod = 1;
private:
	VoiceState state;
	bool noteOn;
	inline static bool sustainPedalOn = false;
	double phase;
	jack_default_audio_sample_t ramp_step;
	double baseFrequency;
	Wavetable &wavetable;
	jack_nframes_t sampleRate;
};
