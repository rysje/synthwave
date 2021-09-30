#include <iostream>
#include "Voice.h"

Voice::Voice(double frequency, jack_nframes_t sample_rate, Wavetable &wavetable)
	: phase(0), baseFrequency(frequency), wavetable(wavetable), noteOn(false),
	sampleRate(sample_rate), state(VoiceState::Inactive)
{
	ramp_step = frequency / sampleRate;
	std::cout << frequency << "\t" <<ramp_step << std::endl;
	double biquadFreq = frequency * 2;
	if (biquadFreq > sample_rate) {
		biquadFreq = sample_rate / 2.0;
	}
	biquad.setResonance(biquadFreq, 0.9, true);
	ampAdsr.setAttackRate(1.0 / ( 0.1 * sampleRate));
	ampAdsr.setDecayRate(1.0 / ( 0.3 * sampleRate));
	ampAdsr.setSustainLevel(0.8);
	ampAdsr.setReleaseRate(1.0 / ( 0.5 * sampleRate));
}

void Voice::Process(jack_default_audio_sample_t* buffer, jack_nframes_t nframes)
{
	if (state == VoiceState::Sustain) {
		double frequency = baseFrequency * freqMod;
		ramp_step = frequency / sampleRate;
		for (int i = 0; i < nframes; i++) {
//			buffer[i] += 0.2f * wavetable.returnSample(frequency, phase);
//			buffer[i] *= ampAdsr.tick();
//			buffer[i] = biquad.tick(buffer[i]);
			jack_default_audio_sample_t sample = 0.2f * wavetable.returnSample(frequency, phase);
			sample *= ampAdsr.tick();
			sample = biquad.tick(sample);
			buffer[i] += sample;
			phase += ramp_step;
			phase = (phase > 1.0) ? phase - 1.0 : phase;
		}
	}
}

bool Voice::isActiveInCurrentBuffer()
{
	bool active = false;
	if (!sustainPedalOn && !noteOn) {
		state = VoiceState::Inactive;
	}
	if (state != VoiceState::Inactive) {
		active = true;
	}
	return active;
}

void Voice::on()
{
	noteOn = true;
	ampAdsr.keyOn();
	state = VoiceState::Sustain;
}

void Voice::off()
{
	ampAdsr.keyOff();
	noteOn = false;
	if (!sustainPedalOn) {
		state = VoiceState::Inactive;
	}
}

void Voice::setFrequencyModulation(double value)
{
	freqMod = value;
}

void Voice::setSustainPedal(char value)
{
	if (value == 0) {
		sustainPedalOn = false;
	} else {
		sustainPedalOn = true;
	}
}



