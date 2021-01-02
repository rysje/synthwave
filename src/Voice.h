#pragma once

#include <jack/types.h>

class Voice
{
public:
	Voice(double frequency, jack_nframes_t sample_rate);
	//void Process(jack_default_audio_sample_t *buffer, jack_nframes_t start_sample, jack_nframes_t end_sample, jack_nframes_t nframes);
	void Process(jack_default_audio_sample_t *buffer, jack_nframes_t nframes);
private:
	double phase;
	jack_default_audio_sample_t ramp_step;
	double frequency;	

};
