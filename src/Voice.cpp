#include <cmath>
#include <numbers>
#include "Voice.h"

Voice::Voice(float frequency, jack_nframes_t sample_rate, Wavetable& wavetable)
	: phase(0), baseFrequency(frequency), wavetable(wavetable), noteOn(false),
	sampleRate(sample_rate), amplitude(1.0)
{
	ramp_step = frequency / (float) sampleRate;
	ampAdsr.setAttackRate(1.0 / (attackLength * (float) sampleRate));
	ampAdsr.setDecayRate(1.0 / (decayLength * (float) sampleRate));
	ampAdsr.setSustainLevel(sustainLevel);
	ampAdsr.setReleaseRate(1.0 / (releaseLength * (float) sampleRate));
}

void Voice::Process(jack_default_audio_sample_t* buffer, jack_nframes_t nframes)
{
	updateFilter();
	float frequency = baseFrequency * freqMod;
	ramp_step = frequency / (float) sampleRate;
	for (int i = 0; i < nframes; i++) {
		float sample = amplitude * 0.3f * wavetable.returnSample(frequency, phase);
		sample *= (float) ampAdsr.tick();
		sample = (float) biquad.tick(sample);
		buffer[i] += sample;
		phase += ramp_step;
		phase = (phase > 1.0f) ? phase - 1.0f : phase;
	}
}

bool Voice::isActiveInCurrentBuffer()
{
	// jezeli nieaktywny to od razu zwraca false zeby nie zajmowac procka
	if (ampAdsr.getState() == stk::ADSR::IDLE) {
		return false;
	}
	// jezeli aktywny ale trzeba wyłączyć nute po pedale
	if (!noteOn && !sustainPedalOn) {
		ampAdsr.keyOff();
	}
	// jeżeli aktywny
	return noteOn || (ampAdsr.getState() != stk::ADSR::IDLE);
}

void Voice::on(float velocity)
{
	amplitude = velocity;
	noteOn = true;
	ampAdsr.keyOn();
}

void Voice::off()
{
	noteOn = false;
	if (!sustainPedalOn) {
		ampAdsr.keyOff();
	}
}

void Voice::setFrequencyModulation(float value)
{
	freqMod = value;
}

void Voice::setSustainPedal(unsigned char value)
{
	if (value == 0) {
		sustainPedalOn = false;
	} else {
		sustainPedalOn = true;
	}
}

void Voice::setAttackLength(float value)
{
	attackLength = value;
	ampAdsr.setAttackRate(1.0f / (attackLength * (float) sampleRate));
}

void Voice::setDecayLength(float value)
{
	decayLength = value;
	ampAdsr.setDecayRate(1.0f / (decayLength * (float) sampleRate));
}

void Voice::setSustainLevel(float value)
{
	sustainLevel = value;
	ampAdsr.setSustainLevel(sustainLevel);
}

void Voice::setReleaseLength(float value)
{
	releaseLength = value;
	ampAdsr.setReleaseRate(1.0f / (releaseLength * (float) sampleRate));
}

void Voice::updateFilter()
{
	//filterFrequencyMultiplier += 2.0f * static_cast<float>(ampAdsr.lastOut());
	float filterFrequency = baseFrequency * filterFrequencyMultiplier * (1 + filterModulation);
	if (filterFrequency > (float) sampleRate / 2.0f) {
		filterFrequency = (float) sampleRate / 2.0f - 1.0f;
	}
	float K = tanf((float) std::numbers::pi_v<float> * filterFrequency / (float) sampleRate);
	float norm = 1 / (1 + K / filterResonance + K * K);
	float a0 = K * K * norm;
	float a1 = 2 * a0;
	float a2 = a0;
	float b1 = 2 * (K * K - 1) * norm;
	float b2 = (1 - K / filterResonance + K * K) * norm;
	biquad.setCoefficients( a0, a1, a2, b1, b2);
}

void Voice::setFilterResonance(float value)
{
	filterResonance = value;
}

void Voice::setFilterFrequencyMultiplier(float value)
{
	filterFrequencyMultiplier = value;
}

void Voice::setFilterModulation(float value)
{
	filterModulation = value;
}
