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

#include <jack/types.h>
#include <stk/BiQuad.h>
#include <stk/ADSR.h>
#include "Wavetable.h"

class Voice
{
public:
	Voice(float frequency, jack_nframes_t sample_rate, Wavetable& wavetable);
	void Process(jack_default_audio_sample_t* buffer, jack_nframes_t nframes);
	bool isActiveInCurrentBuffer();
	void on(float velocity);
	void off();
	static void setFrequencyModulation(float value);
	static void setSustainPedal(unsigned char value);
	void setAttackLength(float value);
	void setDecayLength(float value);
	void setSustainLevel(float value);
	void setReleaseLength(float value);
	static void setFilterFrequencyMultiplier(float value);
	static void setFilterResonance(float value);
	static void setFilterModulation(float value);
	static void setFilterEnvelopeFactor(float value);

private:
	void updateFilter();
	float attackLength = 0.02;
	float decayLength = 0.08;
	float sustainLevel = 0.6;
	float releaseLength = 0.2;
	inline static float freqMod = 1;
	float amplitude;
	stk::BiQuad biquad;
	stk::ADSR adsr;
	bool noteOn;
	inline static bool sustainPedalOn = false;
	float phase;
	float ramp_step;
	float baseFrequency;
	inline static float filterFrequencyMultiplier = 5.0f;
	inline static float filterResonance = 2.0f;
	inline static float filterModulation = 0.0f;
	inline static float filterEnvelopeFactor = 0.0f;
	Wavetable &wavetable;
	jack_nframes_t sampleRate;
};
