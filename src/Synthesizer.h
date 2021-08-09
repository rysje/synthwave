#pragma once

#include <jack/midiport.h>
#include <vector>

#include "Voice.h"
#include "Wavetable.h"

class Synthesizer
{
public:
	Synthesizer();
	void setMIDINoteEvents(const std::vector< jack_midi_event_t > &midiEventsList);
	/* probably std::vector<jack_midi_data_t> as an argument to both methods,
	 * as we may need to define more than one value per buffer
	void setPitchWheelValues();
	void setSustainPedalValues();*/
	int Process(jack_default_audio_sample_t *buffer, jack_nframes_t nframes);
private:
	std::vector<Voice*> voices;
	std::vector<bool> isActive;
	std::vector<jack_nframes_t> time;
	Wavetable wavetable;
};
