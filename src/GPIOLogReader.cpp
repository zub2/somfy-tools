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
#include "GPIOLogReader.h"

GPIOLogReader::GPIOLogReader(const std::string & fileName)
{
	m_stream.open(fileName, std::ios_base::in | std::ios_base::binary);
	if (!m_stream.good())
		throw std::runtime_error(std::string("Can't read ") + fileName);

	std::optional<std::uint64_t> initialState = readUint64();
	if (!initialState)
		throw std::runtime_error("can't read input file");

	m_state = *initialState != 0;
}

std::optional<rts::Duration> GPIOLogReader::get()
{
	std::optional<rts::Clock::duration> d = readDuration();
	if (!d)
		return std::nullopt;

	rts::Duration duration(*d, m_state);
	m_state = !m_state;

	return duration;
}

std::optional<std::uint64_t> GPIOLogReader::readUint64()
{
	std::uint64_t value;
	m_stream.read(reinterpret_cast<char*>(&value), sizeof(value));
	if (m_stream.eof())
		return std::nullopt;

	if (m_stream.bad() || m_stream.gcount() != sizeof(value))
		throw std::runtime_error("can't read input file");

	return value;
}

std::optional<rts::Clock::duration> GPIOLogReader::readDuration()
{
	std::optional<std::uint64_t> ts = readUint64();
	if (!ts)
		return std::nullopt;

	return std::chrono::microseconds(*ts);
}
