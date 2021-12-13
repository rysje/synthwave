/*
    Copyright 2021 Ryszard Jezierski

    This file is part of synthwave.
    synthwave is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    synthwave is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with synthwave.  If not, see <https://www.gnu.org/licenses/>.
*/

#include "Synthesizer.h"

#include <cmath>
#include <algorithm>
#include <filesystem>
#include <iostream>

#include <jack/jack.h>

Synthesizer::Synthesizer(jack_client_t* client, const char* exePath) : controlBuffersPerAudioBuffer(4), lfoDepth(0.0f),
	lfoPhase(0.0f), lfoFrequency(6.0f)
{
	// find all files in wavetables subdirectory
	std::filesystem::path exeDir = std::filesystem::path(exePath).parent_path();
	std::vector<std::filesystem::path> wavetable_files;
	std::copy(std::filesystem::directory_iterator(exeDir/"wavetables"), std::filesystem::directory_iterator(), std::back_inserter(wavetable_files));
	std::sort(wavetable_files.begin(), wavetable_files.end());
	// create wavetable from every .synthwave file
	for (const auto& filename : wavetable_files) {
		if (filename.extension().string() == ".synthwave") {
			std::cout << "Found wavetable file: " << filename << std::endl;
			wavetables.emplace_back(Wavetable{filename});
		}
	}
	wavetable = wavetables[0];
	numberOfWavetables = wavetables.size();
	// initialize buffers' size
	sampleRate = jack_get_sample_rate(client);
	bufferSize = jack_get_buffer_size(client);
	controlBufferSize = bufferSize / controlBuffersPerAudioBuffer;
	// initialize voices
	voices.reserve(128);
	for (int i = 0; i < 128; i++) {
		voices.emplace_back(new Voice((442.0f / 32.0f) * powf(2, (((float)i - 9.0f) / 12.0f)), sampleRate, wavetable));
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
			float value = convertMidiValueToExpRange(midiEvent.buffer[2], 0.01f, 1.0f);
			voices[noteNumber]->on(value);
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
		// program change
		else if ((midiEvent.buffer[0] & 0xf0) == 0xc0) {
			auto wavetableNumber = static_cast<unsigned int>(midiEvent.buffer[1]);
			if (wavetableNumber >= numberOfWavetables) {
				continue;
			}
			else {
				wavetable = wavetables[wavetableNumber];
			}
		}
		// control messages
		else if ((midiEvent.buffer[0] & 0xf0) == 0xB0) {
			// sustain pedal
			if (midiEvent.buffer[1] == 0x40) {
				Voice::setSustainPedal(midiEvent.buffer[2]);
			}
			// attack length (MIDI CC name: Attack Time)
			else if (midiEvent.buffer[1] == 73) {
				float value = convertMidiValueToExpRange(midiEvent.buffer[2], 0.01f, 1.0f);
				for (auto voice: voices) {
					voice->setAttackLength(value);
				}
			}
			// decay length (MIDI CC name: Sound Controller 6)
			else if (midiEvent.buffer[1] == 75) {
				float value = convertMidiValueToExpRange(midiEvent.buffer[2], 0.01f, 1.0f);
				for (auto voice: voices) {
					voice->setDecayLength(value);
				}
			}
			// sustain level (MIDI CC name: Sound Controller 7)
			else if (midiEvent.buffer[1] == 76) {
				float value = convertMidiValueToExpRange(midiEvent.buffer[2], 0.1f, 1.0f);
				for (auto voice: voices) {
					voice->setSustainLevel(value);
				}
			}
			// release length (MIDI CC name: Release Time)
			else if (midiEvent.buffer[1] == 72) {
				float value = convertMidiValueToExpRange(midiEvent.buffer[2], 0.1f, 2.0f);
				for (auto voice: voices) {
					voice->setReleaseLength(value);
				}
			}
			// filter cutoff (MIDI CC name: Brightness)
			else if (midiEvent.buffer[1] == 74) {
				float value = convertMidiValueToExpRange(midiEvent.buffer[2], 1.5f, 50.0f);
				Voice::setFilterFrequencyMultiplier(value);
			}
			// filter resonance (MIDI CC name: Timber/Harmonic Intensity)
			else if (midiEvent.buffer[1] == 71) {
				float value = convertMidiValueToExpRange(midiEvent.buffer[2], 1.0f, 10.0f);
				Voice::setFilterResonance(value);
			}
			// LFO frequency (MIDI CC name: Sound Controller 8)
			else if (midiEvent.buffer[1] == 77) {
				lfoFrequency = convertMidiValueToExpRange(midiEvent.buffer[2], 0.5f, 10.0f);
			}
			// LFO depth (MIDI CC name: Modulation wheel
			else if (midiEvent.buffer[1] == 1) {
				lfoDepth = static_cast<float>(midiEvent.buffer[2]) / 508.0f;
			}
			// filter envelope factor (MIDI CC name: Sound Controller 9)
			else if (midiEvent.buffer[1] == 78) {
				Voice::setFilterEnvelopeFactor(static_cast<float>(midiEvent.buffer[2]) / 63.5f);
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
		Voice::setFilterModulation(LFO());
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

float Synthesizer::LFO()
{
	float ramp_step = lfoFrequency / (static_cast<float>(sampleRate) / static_cast<float>(controlBufferSize));
	lfoPhase += ramp_step;
	lfoPhase = (lfoPhase > 1.0f) ? lfoPhase - 1.0f : lfoPhase;
	// get the last wavetable which should be a sine wave
	return lfoDepth * wavetables[numberOfWavetables - 1].returnSample(lfoFrequency, lfoPhase);
}
