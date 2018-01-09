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
#ifndef GPIO_LOG_WRITER_H
#define GPIO_LOG_WRITER_H

#include <string>
#include <fstream>
#include <vector>
#include <cstdint>
#include <optional>

#include "Duration.h"

class GPIOLogWriter
{
public:
	GPIOLogWriter(const std::string & fileName);
	~GPIOLogWriter();

	void write(const Duration & duration);

public:
	void writeUint64(uint64_t value);
	void writeDuration(const Clock::duration & d);

	std::ofstream m_stream;
	std::optional<Duration> m_lastDuration;
};

#endif // GPIO_LOG_WRITER_H
