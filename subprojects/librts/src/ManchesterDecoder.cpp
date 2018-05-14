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
#include "ManchesterDecoder.h"

#include <iostream>
#include <chrono>

#include "Clock.h"

namespace rts
{

using namespace std::literals;

const Clock::duration ManchesterDecoder::LONG_SHORT_THRESHOLD = 962us;

bool ManchesterDecoder::newTransition(const Duration & duration)
{
	const Clock::duration d = duration.first;
	const bool value = duration.second;

	if (!m_started)
	{
		// the first duration has to be a half symbol - always
		if (d > LONG_SHORT_THRESHOLD)
		{
			std::cerr << "DECODER error: invalid first transition (half-symbol expected)" << std::endl;
			return false;
		}

		m_bits.push_back(!value);
		m_started = true;
	}
	else if (m_waitingHalfSym)
	{
		if (d > LONG_SHORT_THRESHOLD)
		{
			std::cerr << "DECODER error: expecting short transition" << std::endl;
			return false;
		}

		m_bits.push_back(!value);
		m_waitingHalfSym = false;
	}
	else if (d > LONG_SHORT_THRESHOLD)
	{
		// full-width symbol -> new bit = !prev bit
		m_bits.push_back(!value);
	}
	else // duration <= LONG_SHORT_THRESHOLD
	{
		// half-width symbol
		m_waitingHalfSym = true;
	}

	return true;
}

} // namespace rts
