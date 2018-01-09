/*
 * Copyright 2018 David Kozub <zub at linux.fjfi.cvut.cz>
 *
 * This file is part of somfy-tools.
 *
 * somfy-tools is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * somfy-tools is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with somfy-tools.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef FILTER_H
#define FILTER_H

#include <vector>
#include <stdexcept>

#include <boost/circular_buffer.hpp>

class Filter
{
public:
	Filter(std::vector<float> && a, std::vector<float> && b):
		m_a(std::move(a)),
		m_b(std::move(b)),
		m_pastOutputs(m_a.size(), 0.0f),
		m_pastInputs(m_b.size(), 0.0f)
	{
		if (m_a.empty() || m_b.empty())
			throw std::runtime_error("invalid filter parameters!");
	}

	float operator << (float f)
	{
		m_pastInputs.push_back(f);

		float y = 0.0f;

		/*
		 * This should work exactly like Octave's filter(), that is: solve y_k from:
		 *
		 *   sum_{i=0}^{N_a-1} a_i*y_{k-i} = sum_{i=0}^{N_b-1} b_i*x{k-i}
		 *
		 * assuming:
		 * * {x_0, ..., x_k} are the input samples (k being the latest, current, sample)
		 * * {y_0, ..., y_k} are the output samples
		 * * a_0, ..., a_{N_a-1} are the y coefficients,
		 * * b_0, ..., b_{N_b-1} are the x coefficients
		 *
		 * y_k can be expressed as:
		 *
		 *   y_k = 1/a_0 [ sum_{i=0}^{N_b-1} b_i*x{k-i} - sum_{i=1}^{N_a-1} a_i*y_{k-i} ]
		 *
		 * and this is what the code below calculates.
		 */

		// the x part
		for (size_t i = 0; i < m_b.size(); i++)
			y += m_pastInputs[i] * m_b[i];

		// the y part (with a -), skipping i == 0 obviously
		for (size_t i = 1; i < m_a.size(); i++)
			y -= m_pastOutputs[i] * m_a[i];

		// account for a_0
		y /= m_a[0];

		// store y_0 for next iteration
		m_pastOutputs.push_back(y);

		return y;
	}

private:
	const std::vector<float> m_a;
	const std::vector<float> m_b;

	boost::circular_buffer<float> m_pastOutputs; // Y
	boost::circular_buffer<float> m_pastInputs; // X
};

#endif // FILTER_H
