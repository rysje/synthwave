#include "Synthesizer.h"

#include <cmath>
#include <algorithm>

Synthesizer::Synthesizer()
{
	Voice::freqMod.resize(Voice::controlBuffersPerAudioBuffer);
	std::fill(Voice::freqMod.begin(), Voice::freqMod.end(), 1);
	wavetable.init("saw.synthwave");
	voices.reserve(128);
	for (int i = 0; i < 128; i++) {
		voices.emplace_back(new Voice((440.0 / 32.0) * pow(2, (((double)i - 9.0) / 12.0)), 48000, wavetable));
	}
}

void Synthesizer::setMIDINoteEvents(const std::vector<jack_midi_event_t> &midiEventsList)
{
	for (jack_midi_event_t midiEvent : midiEventsList) {
		unsigned char noteNumber = midiEvent.buffer[1];
		if ( (midiEvent.buffer[0] & 0xf0) == 0x80) {
			voices[noteNumber]->off(midiEvent.time);
		}
		if ( (midiEvent.buffer[0] & 0xf0) == 0x90) {
			voices[noteNumber]->on(midiEvent.time);
		}
	}
}

int Synthesizer::Process(jack_default_audio_sample_t *buffer, jack_nframes_t nframes)
{
	for (int i = 0; i < nframes; i++) {
		buffer[i] = 0;
	}
	for (int i = 0; i < 128; i++) {
		if (voices[i]->isActiveInCurrentBuffer()) {
			voices[i]->Process(buffer, nframes);
		}
	}
	return 0;
}

void Synthesizer::setPitchWheelValues(const std::vector<jack_midi_event_t> &pitchBendEventsList)
{
	int pitchBendValue;
	double pitchBendRange = 2.0;
	double freqModValue;
	for (jack_midi_event_t e : pitchBendEventsList) {
		pitchBendValue = e.buffer[1];
		pitchBendValue += e.buffer[2] << 7;
		// convert to range <-1;1>  ==>  (pitchBendValue - 8192.0) / 8192
		// multiply range by the interval ==>  pitchBendRange / 12
		freqModValue = pow(2.0, (((double) pitchBendValue - 8192.0) * pitchBendRange) / (8192 * 12));
		Voice::setFrequencyModulation(e.time, freqModValue);
	}
}



