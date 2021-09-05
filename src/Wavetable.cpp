#include "Wavetable.h"

#include <cmath>

void Wavetable::init(const std::string& filename)
{
	std::ifstream file(filename, std::ios::in);
	for (std::string line; std::getline(file, line); ) {
		double lowFrequency = std::stod(line);
		std::getline(file, line);
		double highFrequency = std::stod(line);
		std::getline(file, line);
		int n_samples = std::stoi(line);
		Data table(lowFrequency, highFrequency, n_samples);
		for (int i = 0; i < n_samples; i++) {
			std::getline(file, line);
			table.samples.emplace_back(std::stod(line));
		}
		// additional element at the end to simplify interpolation
		table.samples.emplace_back(table.samples[0]);
		tables.emplace_back(table);
	}
}

double Wavetable::returnSample(double frequency, double phase)
{
	int tableIndex = 0;
	if (frequency < tables[tableIndex].lowFrequency) {
		return 0.0;
	}
	while (tables[tableIndex].highFrequency < frequency) {
		tableIndex++;
	}
	// TODO: make sure phase is in the range <0; 1>
	double sampleNumber = phase * (double) tables[tableIndex].samples.size();
	int lowerSampleNumber = static_cast<int>(floor(sampleNumber));
	int upperSampleNumber = lowerSampleNumber + 1;
	double lowerSample = tables[tableIndex].samples[lowerSampleNumber];
	double upperSample = tables[tableIndex].samples[upperSampleNumber];
	return std::lerp(lowerSample, upperSample, sampleNumber - lowerSampleNumber);
	//return lowerSample + (sampleNumber - lowerSampleNumber) * (upperSample - lowerSample);

}

Wavetable::Data::Data(double lowFrequency, double highFrequency, int n_samples)
	: lowFrequency(lowFrequency), highFrequency(highFrequency)
{
	samples.reserve(n_samples + 1);
}
