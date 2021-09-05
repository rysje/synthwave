#include "Synthesizer.h"

#include <cmath>
#include <algorithm>

Synthesizer::Synthesizer() : controlBufferSize(64), controlBuffersPerAudioBuffer(4)
{
	wavetable.init("saw.synthwave");
	voices.reserve(128);
	for (int i = 0; i < 128; i++) {
		voices.emplace_back(new Voice((440.0 / 32.0) * pow(2, (((double)i - 9.0) / 12.0)), 48000, wavetable));
	}
}

void Synthesizer::setMidiEvents(std::vector<jack_midi_event_t>* midiEventsList)
{
	this->midiEventsList = midiEventsList;
}

void Synthesizer::processMidiEvents(jack_nframes_t begin, jack_nframes_t offset)
{
	for (jack_midi_event_t midiEvent : *midiEventsList) {
		if (midiEvent.time < begin || midiEvent.time >= begin + offset) {
			continue;
		}
		// note off
		if ( (midiEvent.buffer[0] & 0xf0) == 0x80) {
			unsigned char noteNumber = midiEvent.buffer[1];
			voices[noteNumber]->off();
		}
		// note on
		if ( (midiEvent.buffer[0] & 0xf0) == 0x90) {
			unsigned char noteNumber = midiEvent.buffer[1];
			voices[noteNumber]->on(midiEvent.buffer[2]);
		}
		// pitch bend
		if ((midiEvent.buffer[0] & 0xf0) == 0xE0) {
			int pitchBendValue;
			double pitchBendRange = 2.0;
			double freqModValue;
			pitchBendValue = midiEvent.buffer[1];
			pitchBendValue += midiEvent.buffer[2] << 7;
			// convert to range <-1;1>  ==>  (pitchBendValue - 8192.0) / 8192
			// multiply range by the interval ==>  pitchBendRange / 12
			freqModValue = pow(2.0, (((double) pitchBendValue - 8192.0) * pitchBendRange) / (8192 * 12));
			Voice::setFrequencyModulation(freqModValue);
		}
		// sustain pedal
		if ((midiEvent.buffer[0] & 0xf0) == 0xB0 && midiEvent.buffer[1] == 0x40) {
			Voice::setSustainPedal(midiEvent.buffer[2]);
		}
	}
}

int Synthesizer::Process(jack_default_audio_sample_t *buffer, jack_nframes_t nframes)
{
	for (int i = 0; i < nframes; i++) {
		buffer[i] = 0;
	}
	for (int i = 0; i < controlBuffersPerAudioBuffer; i++) {
		processMidiEvents(i * controlBufferSize, controlBufferSize);
		for (int j = 0; j < 128; j++) {
			if (voices[j]->isActiveInCurrentBuffer()) {
				voices[j]->Process(buffer + i * controlBufferSize, controlBufferSize);
			}
		}
	}
	return 0;
}
