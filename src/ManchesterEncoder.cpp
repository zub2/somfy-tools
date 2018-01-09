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
#include "ManchesterEncoder.h"

#include <chrono>

using namespace std::literals;

const Clock::duration ManchesterEncoder::HALF_SYMBOL_DURATION = 645us;

ManchesterEncoder & ManchesterEncoder::operator<<(bool b)
{
	appendHalfSymbol(!b);
	appendHalfSymbol(b);

	return *this;
}

const std::vector<Duration> & ManchesterEncoder::getDurations() const
{
	return m_buffer.get();
}

void ManchesterEncoder::appendHalfSymbol(bool b)
{
	m_buffer << Duration(HALF_SYMBOL_DURATION, b);
}
