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
#ifndef RTLSDR_BUFFER_READER_H
#define RTLSDR_BUFFER_READER_H

#include <complex>

class RTLSDRBufferReader
{
public:
	RTLSDRBufferReader():
		m_buffer(nullptr),
		m_size(0)
	{}

	void reset()
	{
		reset(nullptr, 0);
	}

	void reset(const uint8_t * buffer, size_t size)
	{
		m_buffer = buffer;
		m_size = size / 2;

		// TODO: can rtl-sdr return an odd-sized buffer?
		if (size % 2 != 0)
			throw std::runtime_error("size of data buffer must be even");
	}

	size_t size() const
	{
		return m_size;
	}

	std::complex<float> at(size_t index) const
	{
		if (index > m_size)
			throw std::runtime_error("index out of bounds");

		return std::complex<float>(normalize(m_buffer[2*index]), normalize(m_buffer[2*index + 1]));
	}

private:
	static float normalize(uint8_t sample)
	{
		// Octave's uint8=>double conversion in audioread() seems to
		// do it like this. That is, it maps [0,255] to [-1, 127/128]
		// (with 128 being mapped to 0).
		return static_cast<float>(sample)/128.0f - 1.0f;
	}

	const uint8_t * m_buffer;
	size_t m_size; // number of pairs (i, q) in the buffer
};

#endif // RTLSDR_BUFFER_READER_H
