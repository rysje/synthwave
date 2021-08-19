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
	void on(jack_nframes_t time);
	void off(jack_nframes_t time);
	static void setFrequencyModulation(jack_nframes_t time, double value);

	inline static jack_nframes_t controlBufferSize = 64;
	inline static jack_nframes_t controlBuffersPerAudioBuffer = 4;
	inline static int attackLen;
	inline static int decayLen;
	inline static int sustainLev;
	inline static int releaseLen;
	inline static std::vector<double> freqMod;
private:
	std::vector<VoiceState> state;
	bool noteOn;
	bool sustainPedalOn;
	double phase;
	jack_default_audio_sample_t ramp_step;
	double baseFrequency;
	Wavetable &wavetable;
	jack_nframes_t sampleRate;
};
