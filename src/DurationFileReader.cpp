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
#include "DurationFileReader.h"

#include <cassert>
#include <chrono>

#include <boost/lexical_cast.hpp>

const std::regex DurationFileReader::LINE_REGEX("[[:space:]]*([^[:space:]]+)[[:space:]]+([^[:space:]]+)[[:space:]]*|[[:space:]]*");

DurationFileReader::DurationFileReader(const std::string & f):
	m_stream(f),
	m_line(0)
{
	if (!m_stream.is_open())
		throw std::runtime_error(std::string("can't open ") + f);
}

std::optional<rts::Duration> DurationFileReader::get()
{
	std::optional<rts::Duration> d;
	std::string line;

	while (!d && std::getline(m_stream, line))
	{
		m_line++;
		d = parseLine(line);
	}

	if (m_stream.eof())
		return std::nullopt;
	if (m_stream.fail())
		throw std::runtime_error("can't read input file");

	return d;
}

std::optional<rts::Duration> DurationFileReader::parseLine(const std::string & line)
{
	// skip comment - if any
	std::string::size_type end = line.find('#');
	if (end == std::string::npos)
		end = line.size();

	std::smatch match;
	if (!std::regex_match(line.begin(), line.begin() + end, match, LINE_REGEX))
		throw std::runtime_error(std::string("unexpected line format at line ") + std::to_string(m_line));

	assert(match.size() == 1 + 2);
	if (match[1].length() == 0)
	{
		assert(match[2].length() == 0);

		// just an empy line - no matches
		return std::nullopt;
	}

	const std::ssub_match & matchDuration = match[1];
	const std::ssub_match & matchValue = match[2];

	// duration in µs
	const rts::Clock::duration d =
			std::chrono::microseconds(boost::lexical_cast<std::chrono::microseconds::rep>(matchDuration));

	// value
	const bool v = parseBool(matchValue);

	return rts::Duration(d, v);
}

bool DurationFileReader::parseBool(const std::string & s)
{
	if (s == "1" || s == "true")
		return true;
	else if (s == "0" || s == "false")
		return false;

	throw std::runtime_error(std::string("can't parse '") + s + "' as bool. Use one of '1', 'true', '0' or 'false'.");
}
