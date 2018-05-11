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
#include "DurationBuffer.h"
#include "backend/rpi-gpio/PlaybackThread.h"

#include <iostream>

#include <boost/program_options.hpp>
#include <boost/lexical_cast.hpp>

void play(unsigned gpioNr, const std::string &inputFile)
{
	DurationFileReader reader(inputFile);
	rts::DurationBuffer buffer;

	std::optional<rts::Duration> d = reader.get();
	while (d)
	{
		buffer << *d;
		d = reader.get();
	}

	rts::PlaybackThread playbackThread(gpioNr);
	playbackThread.start();
	playbackThread.play(buffer.get());
	playbackThread.stop();
}

int main(int argc, char * argv[])
{
	try
	{
		unsigned gpioNr;
		std::string inputFile;

		boost::program_options::options_description argDescription("Available options");
		argDescription.add_options()
			("gpio-nr,n", boost::program_options::value(&gpioNr)->required(),
				"The GPIO number to use.")
			("file,f", boost::program_options::value(&inputFile)->required(),
				"The GPIO log file to play.")
			("help,h", "print this help")
		;

		boost::program_options::variables_map variablesMap;
		boost::program_options::store(boost::program_options::parse_command_line(argc, argv, argDescription), variablesMap);

		if (variablesMap.count("help"))
		{
			std::cout << argDescription << "\n";
			return 1;
		}

		boost::program_options::notify(variablesMap);

		play(gpioNr, inputFile);
	}
	catch (const boost::program_options::error & e)
	{
		std::cerr << e.what() << "\n";
		return 1;
	}

	return 0;
}
