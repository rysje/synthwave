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