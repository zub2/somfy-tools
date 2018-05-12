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
#ifndef RTS_DURATION_BUFFER_H
#define RTS_DURATION_BUFFER_H

#include "Duration.h"
#include <vector>

namespace rts
{

class DurationBuffer
{
public:
	typedef Duration value_type;

	DurationBuffer(size_t capacity = 0)
	{
		m_buffer.reserve(0);
	}

	DurationBuffer & operator<< (const Duration & d)
	{
		if (!m_buffer.empty() && m_buffer.back().second == d.second)
			m_buffer.back().first += d.first;
		else
			m_buffer.push_back(d);

		return *this;
	}

	void push_back(const Duration & d)
	{
		*this << d;
	}

	const std::vector<Duration> & get() const
	{
		return m_buffer;
	}

private:
	std::vector<Duration> m_buffer;
};

} // namespace rts

#endif // RTS_DURATION_BUFFER_H
