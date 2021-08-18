#include "Voice.h"

#include <iostream>


Voice::Voice(double frequency, jack_nframes_t sample_rate, Wavetable &wavetable)
	: phase(0), frequency(frequency), wavetable(wavetable), controlBufferSize(64), controlBuffersPerAudioBuffer(4),
	noteOn(false), sustainPedalOn(false)
{
	state.reserve(4);
	for (int i = 0; i < controlBuffersPerAudioBuffer; i++) {
		state.emplace_back(VoiceState::Inactive);
	}
	ramp_step = frequency / sample_rate;
	std::cout << frequency << "\t" <<ramp_step << std::endl;
}

void Voice::Process(jack_default_audio_sample_t* buffer, jack_nframes_t nframes)
{
	if (noteOn || sustainPedalOn) {
		for (int i = 0; i < controlBuffersPerAudioBuffer; i++) {
			state[i] = VoiceState::Sustain;
		}
	}
	for (int i = 0; i < controlBuffersPerAudioBuffer; i++) {
		if (state[i] == VoiceState::Sustain) {
			for (int j = 0; j < controlBufferSize; j++) {
				buffer[i * controlBufferSize + j] += 0.2 * wavetable.returnSample(frequency, phase);
				phase += ramp_step;
				phase = (phase > 1.0) ? phase - 1.0 : phase;
			}
		}
	}
	if (!noteOn && !sustainPedalOn) {
		for (int i = 0; i < controlBuffersPerAudioBuffer; i++) {
			state[i] = VoiceState::Inactive;
		}
	}
}

bool Voice::isActiveInCurrentBuffer()
{
	bool active = false;
	for (VoiceState s : state) {
		if (s != VoiceState::Inactive) {
			active = true;
		}
	}
	return active || noteOn || sustainPedalOn;
}

void Voice::on(jack_nframes_t time)
{
	noteOn = true;
	for (int i = 0; i < controlBuffersPerAudioBuffer; i++) {
		if (time > controlBufferSize * i) {
			state[i] = VoiceState::Sustain;
		}
	}
}

void Voice::off(jack_nframes_t time)
{
	noteOn = false;
	for (int i = 0; i < controlBuffersPerAudioBuffer; i++) {
		if (time > controlBufferSize * i) {
			state[i] = VoiceState::Inactive;
		}
	}
}



