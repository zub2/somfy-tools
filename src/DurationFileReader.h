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
#ifndef DURATION_FILE_READER_H
#define DURATION_FILE_READER_H

#include "Duration.h"

#include <string>
#include <fstream>
#include <stdexcept>
#include <cctype>
#include <regex>
#include <optional>

class DurationFileReader
{
public:
	DurationFileReader(const std::string & f);
	std::optional<rts::Duration> get();

private:
	std::optional<rts::Duration> parseLine(const std::string & line);
	static bool parseBool(const std::string & s);

	std::ifstream m_stream;
	size_t m_line;

	static const std::regex LINE_REGEX;
};

#endif // DURATION_FILE_READER_H
