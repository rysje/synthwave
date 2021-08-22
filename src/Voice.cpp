#include <iostream>
#include "Voice.h"

Voice::Voice(double frequency, jack_nframes_t sample_rate, Wavetable &wavetable)
	: phase(0), baseFrequency(frequency), wavetable(wavetable), noteOn(false), sustainPedalOn(false),
	sampleRate(sample_rate), state(VoiceState::Inactive)
{
	ramp_step = frequency / sampleRate;
	std::cout << frequency << "\t" <<ramp_step << std::endl;
}

void Voice::Process(jack_default_audio_sample_t* buffer, jack_nframes_t nframes)
{
	if (state == VoiceState::Sustain) {
		double frequency = baseFrequency * freqMod;
		for (int i = 0; i < nframes; i++) {
			ramp_step = frequency / sampleRate;
			buffer[i] += 0.2f * wavetable.returnSample(frequency, phase);
			phase += ramp_step;
			phase = (phase > 1.0) ? phase - 1.0 : phase;
		}
	}
}

bool Voice::isActiveInCurrentBuffer()
{
	bool active = false;
	if (state != VoiceState::Inactive) {
		active = true;
	}
	return active || noteOn || sustainPedalOn;
}

void Voice::on()
{
	state = VoiceState::Sustain;
}

void Voice::off()
{
	state = VoiceState::Inactive;
}

void Voice::setFrequencyModulation(double value)
{
	freqMod = value;
}



