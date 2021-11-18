#include "Synthesizer.h"

#include <cmath>
#include <algorithm>

#include <jack/jack.h>

Synthesizer::Synthesizer(jack_client_t* client) : controlBuffersPerAudioBuffer(4)
{
	wavetable.init("saw.synthwave");
	voices.reserve(128);
	auto sampleRate = jack_get_sample_rate(client);
	auto bufferSize = jack_get_buffer_size(client);
	controlBufferSize = bufferSize / controlBuffersPerAudioBuffer;
	for (int i = 0; i < 128; i++) {
		voices.emplace_back(new Voice((440.0f / 32.0f) * powf(2, (((float)i - 9.0f) / 12.0f)), sampleRate, wavetable));
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
		else if ( (midiEvent.buffer[0] & 0xf0) == 0x90) {
			unsigned char noteNumber = midiEvent.buffer[1];
			voices[noteNumber]->on(midiEvent.buffer[2]);
		}
		// pitch bend
		else if ((midiEvent.buffer[0] & 0xf0) == 0xe0) {
			int pitchBendValue;
			float pitchBendRange = 2.0f;
			float freqModValue;
			pitchBendValue = midiEvent.buffer[1];
			pitchBendValue += midiEvent.buffer[2] << 7;
			// convert to range <-1;1>  ==>  (pitchBendValue - 8192.0) / 8192
			// multiply range by the interval ==>  pitchBendRange / 12
			freqModValue = powf(2.0f, (((float) pitchBendValue - 8192.0f) * pitchBendRange) / (8192 * 12));
			Voice::setFrequencyModulation(freqModValue);
		}
		// control messages
		else if ((midiEvent.buffer[0] & 0xf0) == 0xB0) {
			// sustain pedal
			if (midiEvent.buffer[1] == 0x40) {
				Voice::setSustainPedal(midiEvent.buffer[2]);
			}
			// attack length
			else if (midiEvent.buffer[1] == 0x1f) {
				float value = convertMidiValueToExpRange(midiEvent.buffer[2], 0.01f, 1.0f);
				for (auto voice: voices) {
					voice->setAttackLength(value);
				}
			}
			// decay length
			else if (midiEvent.buffer[1] == 0x2) {
				float value = convertMidiValueToExpRange(midiEvent.buffer[2], 0.01f, 1.0f);
				for (auto voice: voices) {
					voice->setDecayLength(value);
				}
			}
			// sustain level
			else if (midiEvent.buffer[1] == 0x4) {
				float value = convertMidiValueToExpRange(midiEvent.buffer[2], 0.1f, 1.0f);
				for (auto voice: voices) {
					voice->setSustainLevel(value);
				}
			}
			// release length
			else if (midiEvent.buffer[1] == 0xa) {
				float value = convertMidiValueToExpRange(midiEvent.buffer[2], 0.1f, 2.0f);
				for (auto voice: voices) {
					voice->setReleaseLength(value);
				}
			}
			// filter cutoff
			else if (midiEvent.buffer[1] == 0x1) {
				float value = convertMidiValueToExpRange(midiEvent.buffer[2], 1.0f, 50.0f);
				for (auto voice: voices) {
					voice->setFilterFrequencyMultiplier(value);
				}
			}
			// filter resonance
			else if (midiEvent.buffer[1] == 0x5f) {
				float value = convertMidiValueToExpRange(midiEvent.buffer[2], 1.0f, 10.0f);
				for (auto voice: voices) {
					voice->setFilterResonance(value);
				}
			}
		}
	}
}

int Synthesizer::Process(jack_default_audio_sample_t* buffer, jack_nframes_t nframes)
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

float Synthesizer::convertMidiValueToExpRange(unsigned char midiValue, float lowerLimit, float upperLimit)
{
	float value = (float) midiValue / 127.0f;
	float arg = std::lerp(logf(lowerLimit), logf(upperLimit), value);
	return expf(arg);
}
