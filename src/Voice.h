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
private:
	jack_nframes_t controlBufferSize;
	jack_nframes_t controlBuffersPerAudioBuffer;
	static int attackLen;
	static int decayLen;
	static int sustainLev;
	static int releaseLen;
	std::vector<VoiceState> state;
	bool noteOn;
	bool sustainPedalOn;
	double phase;
	jack_default_audio_sample_t ramp_step;
	double frequency;
	Wavetable &wavetable;
};
