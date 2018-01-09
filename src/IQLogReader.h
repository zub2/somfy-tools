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
#ifndef IQ_LOG_READER_H
#define IQ_LOG_READER_H

#include <string>
#include <fstream>
#include <cstdint>
#include <complex>
#include <optional>

class IQLogReader
{
public:
	IQLogReader(const std::string & fileName);

	std::optional<std::complex<float>> getNextSample();

public:
	std::optional<float> getNextFloat();
	std::optional<uint8_t> readUint8();

	std::ifstream m_stream;
};

#endif // IQ_LOG_READER_H
