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

#pragma once

#include <jack/midiport.h>
#include <jack/jack.h>
#include <vector>

#include "Voice.h"
#include "Wavetable.h"

class Synthesizer
{
public:
	Synthesizer(jack_client_t* client, const char* exePath);
	void setMidiEvents(std::vector<jack_midi_event_t> *midiEventsList);
	int Process(jack_default_audio_sample_t* buffer, jack_nframes_t nframes);
private:
	float LFO();
	void processMidiEvents(jack_nframes_t begin, jack_nframes_t offset);
	static inline float convertMidiValueToExpRange(unsigned char midiValue, float lowerLimit, float upperLimit);
	std::vector<jack_midi_event_t>* midiEventsList{};
	float lfoFrequency;
	float lfoDepth;
	float lfoPhase;
	jack_nframes_t sampleRate;
	jack_nframes_t bufferSize;
	jack_nframes_t controlBufferSize;
	jack_nframes_t controlBuffersPerAudioBuffer;
	std::vector<Voice*> voices;
	std::vector<Wavetable> wavetables;
	Wavetable wavetable;
	unsigned long numberOfWavetables;
};
