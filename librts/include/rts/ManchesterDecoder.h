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
#ifndef MANCHESTER_DECODER_H
#define MANCHESTER_DECODER_H

#include "Duration.h"

#include <cstddef>
#include <utility>
#include <vector>

namespace rts
{

class ManchesterDecoder
{
public:
	ManchesterDecoder(size_t expectedBits):
		m_started(false),
		m_waitingHalfSym(false)
	{
		m_bits.reserve(expectedBits);
	}

	bool newTransition(const Duration & duration);

	size_t getDecodedBitsCount() const
	{
		return m_bits.size();
	}

	const std::vector<bool> & getBits() const
	{
		return m_bits;
	}

	void reset()
	{
		m_started = false;
		m_waitingHalfSym = false;
		m_bits.clear();
	}

private:
	static const Clock::duration LONG_SHORT_THRESHOLD;
	bool m_started;
	bool m_waitingHalfSym;
	std::vector<bool> m_bits;
};

} // namespace rts

#endif // MANCHESTER_DECODER_H
