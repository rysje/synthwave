#pragma once

#include <jack/types.h>
#include <stk/BiQuad.h>
#include <stk/ADSR.h>
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
	Voice(float frequency, jack_nframes_t sample_rate, Wavetable &wavetable);
	void Process(jack_default_audio_sample_t* buffer, jack_nframes_t nframes);
	bool isActiveInCurrentBuffer();
	void on(unsigned char velocity);
	void off();
	static void setFrequencyModulation(float value);
	static void setSustainPedal(unsigned char value);
	void setAttackLength(float value);
	void setDecayLength(float value);
	void setSustainLevel(float value);
	void setReleaseLength(float value);
	void setFilterFrequencyMultiplier(float value);
	void setFilterResonance(float value);

private:
	void updateFilter();
	inline static float attackLength = 0.02;
	inline static float decayLength = 0.08;
	inline static float sustainLevel = 0.6;
	inline static float releaseLength = 0.2;
	inline static float freqMod = 1;
	float amplitude;
	stk::BiQuad biquad;
	stk::ADSR ampAdsr;
	VoiceState state;
	bool noteOn;
	inline static bool sustainPedalOn = false;
	float phase;
	float ramp_step;
	float baseFrequency;
	float filterFrequencyMultiplier;
	float filterResonance;
	Wavetable &wavetable;
	jack_nframes_t sampleRate;
};
