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
		double lowFrequency;
		double highFrequency;
		std::vector<double> samples;
		Data(double lowFrequency, double highFrequency, int n_samples);
	};
	std::vector<Data> tables;

	void init(const std::string& filename);
	double returnSample(double frequency, double phase);
};