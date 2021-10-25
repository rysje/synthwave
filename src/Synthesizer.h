#pragma once

#include <jack/midiport.h>
#include <vector>

#include "Voice.h"
#include "Wavetable.h"

class Synthesizer
{
public:
	Synthesizer();
	void setMidiEvents(std::vector<jack_midi_event_t> *midiEventsList);
	int Process(jack_default_audio_sample_t *buffer, jack_nframes_t nframes);
private:
	void processMidiEvents(jack_nframes_t begin, jack_nframes_t offset);
	std::vector<jack_midi_event_t> *midiEventsList{};
	jack_nframes_t controlBufferSize;
	jack_nframes_t controlBuffersPerAudioBuffer;
	std::vector<Voice*> voices;
	Wavetable wavetable;
};
