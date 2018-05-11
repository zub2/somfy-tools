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
#ifndef MANCHESTER_ENCODER_H
#define MANCHESTER_ENCODER_H

#include "Duration.h"
#include "DurationBuffer.h"

namespace rts
{

class ManchesterEncoder
{
public:
	ManchesterEncoder & operator<<(bool b);

	const std::vector<Duration> & getDurations() const;

private:
	void appendHalfSymbol(bool b);

	DurationBuffer m_buffer;

	static const Clock::duration HALF_SYMBOL_DURATION;
};

} // namespace rts

#endif // MANCHESTER_ENCODER_H
