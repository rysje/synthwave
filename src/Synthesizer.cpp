#include "Synthesizer.h"

#include <cmath>
#include <iostream>

Synthesizer::Synthesizer()
{
	wavetable.init("saw.synthwave");
	voices.reserve(128);
	isActive.reserve(128);
	time.reserve(128);
	for (int i = 0; i < 128; i++) {
		voices.emplace_back(new Voice((440.0 / 32.0) * pow(2, (((double)i - 9.0) / 12.0)), 48000, wavetable));
		isActive.emplace_back(false);
		time.emplace_back(0);
	}
}

void Synthesizer::setMIDINoteEvents(const std::vector<jack_midi_event_t> &midiEventsList)
{
	for (jack_midi_event_t midiEvent : midiEventsList) {
		unsigned char noteNumber = *(midiEvent.buffer + 1);
		isActive[noteNumber] = !isActive[noteNumber];
		time[*(midiEvent.buffer + 1)] = midiEvent.time;
	}
}

int Synthesizer::Process(jack_default_audio_sample_t *buffer, jack_nframes_t nframes)
{
	for (int i = 0; i < nframes; i++) {
		buffer[i] = 0;
	}
// 	for (int i = 0; i < 128; i++) {
// 		if (isActive[i]) {
// 			if (time[i]) { //active from time[i]
// 				std::cout << "New note" << i << std::endl;
// 				voices[i]->Process(buffer, time[i], nframes, nframes);
// 				time[i] = 0;
// 			} else { //active through whole buffer
// 				voices[i]->Process(buffer, 0, nframes, nframes);
// 			}
// 		} else {
// 			if (time[i]) { //active until time[i]
// 				voices[i]->Process(buffer, 0, time[i], nframes);
// 				time[i] = 0;
// 			} // else inactive
// 		}
// 	}
	for (int i = 0; i < 128; i++) {
		if (isActive[i]) {
			voices[i]->Process(buffer, nframes);
		}
	}
	return 0;
}



