#include "Voice.h"

#include <cmath>
#include <iostream>


Voice::Voice(double frequency, jack_nframes_t sample_rate, Wavetable &wavetable)
	: phase(0), frequency(frequency), wavetable(wavetable)
{
	ramp_step = frequency / sample_rate;
	std::cout << frequency << "\t" <<ramp_step << std::endl;
}

// void Voice::Process(jack_default_audio_sample_t* buffer, jack_nframes_t start_sample, jack_nframes_t end_sample, jack_nframes_t nframes)
// {
// 	for (int i = start_sample; i < end_sample; i++) {
// 		phase += ramp_step;
// 		phase = (phase > 1.0) ? phase - 2.0 : phase;
// 		buffer[i] += 0.1*sin(2 * M_PI * phase);
// 	}
// }

void Voice::Process(jack_default_audio_sample_t* buffer, jack_nframes_t nframes)
{
	for (int i = 0; i < nframes; i++) {
		//buffer[i] += 0.2*sin(M_PI * phase);
		buffer[i] += 0.2 * wavetable.returnSample(frequency, phase);
		phase += ramp_step;
		phase = (phase > 1.0) ? phase - 1.0 : phase;
	}
}



