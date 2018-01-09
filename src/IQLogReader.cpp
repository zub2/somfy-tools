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
#include "IQLogReader.h"

IQLogReader::IQLogReader(const std::string & fileName)
{
	m_stream.open(fileName, std::ios_base::in | std::ios_base::binary);
	if (!m_stream.good())
		throw std::runtime_error(std::string("Can't read ") + fileName);
}

std::optional<std::complex<float>> IQLogReader::getNextSample()
{
	const std::optional<float> i = getNextFloat();
	if (!i)
		return std::nullopt;

	const std::optional<float> q = getNextFloat();
	if (!q)
		throw std::runtime_error("unexpected end of input file");

	return std::complex<float>(*i, *q);
}

std::optional<float> IQLogReader::getNextFloat()
{
	std::optional<uint8_t> b = readUint8();
	if (!b)
		return std::nullopt;

	return static_cast<float>(*b)/128.0f - 1.0f;
}

std::optional<uint8_t> IQLogReader::readUint8()
{
	uint8_t value;
	m_stream.read(reinterpret_cast<char*>(&value), sizeof(value));
	if (m_stream.eof())
		return std::nullopt;

	if (m_stream.bad() || m_stream.gcount() != sizeof(value))
		throw std::runtime_error("can't read input file");

	return value;
}
