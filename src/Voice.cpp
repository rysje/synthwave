#include <iostream>
#include "Voice.h"

Voice::Voice(double frequency, jack_nframes_t sample_rate, Wavetable &wavetable)
	: phase(0), baseFrequency(frequency), wavetable(wavetable), noteOn(false),
	sampleRate(sample_rate), state(VoiceState::Inactive), amplitude(1.0), buffersInCurrentState(0), ampAdsr{0.0, attackLen, 1.0, decayLen, sustainLev, releaseLen, 0.0}
{
	ramp_step = frequency / sampleRate;
	std::cout << frequency << "\t" <<ramp_step << std::endl;
}

void Voice::Process(jack_default_audio_sample_t* buffer, jack_nframes_t nframes)
{
	if (state != VoiceState::Inactive) {
		double envVal = ampAdsr.value(state, buffersInCurrentState);
		double frequency = baseFrequency * freqMod;
		for (int i = 0; i < nframes; i++) {
			ramp_step = frequency / sampleRate;
			buffer[i] += envVal * amplitude * 0.9f * wavetable.returnSample(frequency, phase);
			phase += ramp_step;
			phase = (phase > 1.0) ? phase - 1.0 : phase;
		}
		buffersInCurrentState++;
		switch (state) {
			case VoiceState::Attack:
				if (buffersInCurrentState >= attackLen) {
					state = VoiceState::Decay;
					buffersInCurrentState = 0;
					break;
				}
			case VoiceState::Decay:
				if (buffersInCurrentState >= decayLen) {
					state = VoiceState::Sustain;
					buffersInCurrentState = 0;
					break;
				}
			case VoiceState::Release:
				if (buffersInCurrentState >= releaseLen) {
					state = VoiceState::Inactive;
					buffersInCurrentState = 0;
					break;
				}
		}
	}
}

bool Voice::isActiveInCurrentBuffer()
{
	bool active = false;
	if (!sustainPedalOn && !noteOn && (state != VoiceState::Release)) {
		state = VoiceState::Inactive;
	}
	if (state != VoiceState::Inactive) {
		active = true;
	}
	return active;
}

void Voice::on(unsigned char velocity)
{
	buffersInCurrentState = 0;
	std::cout << (int) velocity << '\n';
	noteOn = true;
	state = VoiceState::Attack;
	amplitude = (double) velocity / 127.0;
}

void Voice::off()
{
	noteOn = false;
	if (!sustainPedalOn) {
		state = VoiceState::Release;
	}
}

void Voice::setFrequencyModulation(double value)
{
	freqMod = value;
}

void Voice::setSustainPedal(unsigned char value)
{
	if (value == 0) {
		sustainPedalOn = false;
	} else {
		sustainPedalOn = true;
	}
}



