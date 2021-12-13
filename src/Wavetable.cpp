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

#include "Wavetable.h"

#include <cmath>
#include <iostream>

void Wavetable::init(const std::string& filename)
{
	std::ifstream file(filename, std::ios::in);
	if (!file.good()) {
		std::cerr << "ERROR: Couldn't find the requested wavetable file: " << filename;
		exit(1);
	}
	for (std::string line; std::getline(file, line); ) {
		float lowFrequency = std::stof(line);
		std::getline(file, line);
		float highFrequency = std::stof(line);
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

float Wavetable::returnSample(float frequency, float phase)
{
	int tableIndex = 0;
	if (frequency < tables[tableIndex].lowFrequency) {
		return 0.0f;
	}
	while (tables[tableIndex].highFrequency < frequency) {
		tableIndex++;
	}
	// TODO: make sure phase is in the range <0; 1>
	float sampleNumber = phase * (float) tables[tableIndex].samples.size();
	int lowerSampleNumber = static_cast<int>(floorf(sampleNumber));
	int upperSampleNumber = lowerSampleNumber + 1;
	float lowerSample = tables[tableIndex].samples[lowerSampleNumber];
	float upperSample = tables[tableIndex].samples[upperSampleNumber];
	return std::lerp(lowerSample, upperSample, (float) sampleNumber - (float) lowerSampleNumber);
}

Wavetable::Wavetable(const std::string &filename)
{
	init(filename);
}

Wavetable::Wavetable()
= default;

Wavetable::Data::Data(float lowFrequency, float highFrequency, int n_samples)
	: lowFrequency(lowFrequency), highFrequency(highFrequency)
{
	samples.reserve(n_samples + 1);
}
