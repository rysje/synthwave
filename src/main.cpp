#include <iostream>
#include <csignal>
#include <chrono>
#include <thread>

#include <jack/jack.h>
#include <jack/midiport.h>

#include "Synthesizer.h"

jack_client_t* client;
jack_port_t* midi_in;
jack_port_t* audio_out;

Synthesizer* g_synthesizer;

void signal_handler(int sig) 
{
	jack_client_close(client);
	exit(0);
}

int process(jack_nframes_t nframes, void* arg)
{
	void *midi_in_buf = jack_port_get_buffer(midi_in, nframes);
	jack_midi_event_t midi_event_in;
	jack_nframes_t event_count = jack_midi_get_event_count(midi_in_buf);
	std::vector<jack_midi_event_t> midiEventsList;
	// TODO: remove those two
	std::vector<jack_midi_event_t> midiNotesList;
	std::vector<jack_midi_event_t> pitchBendEventsList;
	auto* buffer = (jack_default_audio_sample_t*) jack_port_get_buffer(audio_out, nframes);
	for (int i = 0; i < event_count; i++) {
		jack_midi_event_get(&midi_event_in, midi_in_buf, i);
		midiEventsList.push_back(midi_event_in);
	}
	g_synthesizer->setMidiEvents(&midiEventsList);
	g_synthesizer->Process(buffer, nframes);

	return 0;
}

void connect_audio_ports()
{
	const char **available_audio_ports = jack_get_ports(
		client, nullptr, JACK_DEFAULT_AUDIO_TYPE,
		JackPortIsInput|JackPortIsPhysical|JackPortIsTerminal);

	for (int i = 0; available_audio_ports[i] != nullptr; i++) {
		std::cout << available_audio_ports[i] <<std::endl;
	}
	jack_connect(client, jack_port_name(audio_out), available_audio_ports[0]);
	jack_connect(client, jack_port_name(audio_out), available_audio_ports[1]);
}

void connect_midi_ports()
{
	const char **available_midi_ports = jack_get_ports(
		client, nullptr, JACK_DEFAULT_MIDI_TYPE,
		JackPortIsOutput|JackPortIsPhysical|JackPortIsTerminal);

	for (int i = 0; available_midi_ports[i] != nullptr; i++) {
		std::cout << available_midi_ports[i] <<std::endl;
	}
	jack_connect(client, available_midi_ports[1], jack_port_name(midi_in));
}

int main(int argc, char** argv)
{
	g_synthesizer = new Synthesizer();
	
	if ((client = jack_client_open("synthwave", JackNullOption, nullptr)) == nullptr) {
		std::cerr << "JACK server not running";
	}
	jack_set_process_callback(client, process, nullptr);
	
	midi_in = jack_port_register(client, "midi_in", JACK_DEFAULT_MIDI_TYPE, JackPortIsInput, 0);
	audio_out = jack_port_register(client, "audio_out", JACK_DEFAULT_AUDIO_TYPE, JackPortIsOutput, 0);
	
	if (jack_activate(client)) {
		std::cerr << "Cannot activate client";
		return 1;
	}

	signal(SIGTERM, signal_handler);
	signal(SIGINT, signal_handler);
	
	while (true) {
		connect_audio_ports();
		connect_midi_ports();
		std::this_thread::sleep_for(std::chrono::milliseconds(5000));
	}
}


