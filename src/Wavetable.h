#pragma once

#include <string>
#include <sstream>
#include <fstream>
#include <vector>

class Wavetable
{
public:
	struct Data
	{
		float lowFrequency;
		float highFrequency;
		std::vector<float> samples;
		Data(float lowFrequency, float highFrequency, int n_samples);
	};
	std::vector<Data> tables;

	void init(const std::string& filename);
	Wavetable();
	Wavetable(const std::string& filename);
	float returnSample(float frequency, float phase);
};