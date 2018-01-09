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
#include "GPIOLogWriter.h"

#include <stdexcept>

GPIOLogWriter::GPIOLogWriter(const std::string & fileName)
{
	m_stream.open(fileName, std::ios_base::out | std::ios_base::binary);
	if (!m_stream.good())
		throw std::runtime_error(std::string("Can't write to ") + fileName);
}

GPIOLogWriter::~GPIOLogWriter()
{
	if (m_lastDuration)
		writeDuration(m_lastDuration->first);
}

void GPIOLogWriter::write(const Duration & duration)
{
	if (!m_lastDuration)
	{
		// initial value at the beginning of the file
		writeUint64(duration.second);
		m_lastDuration = duration;
	}
	else if (m_lastDuration->second != duration.second)
	{
		// there is a transition
		writeDuration(m_lastDuration->first);
		m_lastDuration = duration;
	}
	else
	{
		// not really a transition, just sum up
		m_lastDuration->first += duration.first;
	}
}

void GPIOLogWriter::writeUint64(uint64_t value)
{
	m_stream.write(reinterpret_cast<char*>(&value), sizeof(value));
	if (m_stream.bad())
		throw std::runtime_error("can't write to output file");
}

void GPIOLogWriter::writeDuration(const Clock::duration & d)
{
	writeUint64(std::chrono::duration_cast<std::chrono::microseconds>(d).count());
}
