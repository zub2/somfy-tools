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
#include "SigIntHandler.h"
#include "Clock.h"
#include "backend/rpi-gpio/RecordingThread.h"
#include "DurationTracker.h"
#include "SomfyDecoder.h"
#include "GPIOLogReader.h"

#include <iostream>
#include <chrono>

#include <boost/program_options.hpp>

namespace
{
	constexpr unsigned DEFAULT_SAMPLE_PERIOD_US = 10;
	constexpr size_t DEFAULT_BUFFER_SIZE = 1000;
	constexpr double DEFAULT_TOLERANCE = 0.1;

	void decodeFromGPIO(unsigned gpioNr, size_t bufferSize, const Clock::duration & samplePeriod, double tolerance)
	{
		RecordingThread recorder(gpioNr, bufferSize, samplePeriod);
		DurationTracker<RecordingThread> durationTracker(recorder);
		SomfyDecoder decoder(durationTracker, tolerance);

		installSigIntHandler();

		recorder.start();

		std::thread stopThread([&recorder](){
			waitForSigInt();
			recorder.stop();
		});

		decoder.run();
		stopThread.join();
	}

	void decodeFromFile(const std::string & fileName, double tolerance)
	{
		GPIOLogReader reader(fileName);
		SomfyDecoder decoder(reader, tolerance);

		decoder.run();
	}
}

int main(int argc, char * argv[])
{
	try
	{
		unsigned gpioNr;
		unsigned samplePeriod = DEFAULT_SAMPLE_PERIOD_US;
		size_t bufferSize = DEFAULT_BUFFER_SIZE;
		double tolerance = DEFAULT_TOLERANCE;
		std::string inputFile;

		boost::program_options::options_description argDescription("Available options");
		argDescription.add_options()
			("gpio-nr,n", boost::program_options::value(&gpioNr),
				"The GPIO number to use.")
			("buffer-size,b", boost::program_options::value(&bufferSize),
				(std::string("Size of buffer (number of entries). Default: ") + std::to_string(DEFAULT_BUFFER_SIZE)).c_str())
			("sample-period,s", boost::program_options::value(&samplePeriod),
				(std::string("Sample period in µs. Default: ") + std::to_string(DEFAULT_SAMPLE_PERIOD_US)).c_str())
			("tolerance,t", boost::program_options::value(&tolerance),
				(std::string("Tolerance in measured timing. Default: ") + std::to_string(DEFAULT_TOLERANCE)).c_str())
			("input-file,f", boost::program_options::value(&inputFile),
				"GPIO log file to read instead of real GPIO.")
			("help,h", "print this help")
		;

		boost::program_options::variables_map variablesMap;
		boost::program_options::store(boost::program_options::parse_command_line(argc, argv, argDescription), variablesMap);

		if (variablesMap.count("help"))
		{
			std::cout << argDescription << "\n";
			return 1;
		}

		if (variablesMap.count("gpio-nr") == variablesMap.count("input-file"))
		{
			std::cout << "Exactly one of --gpio-nr (-n) and --input-file (-f) are required.\n";
			return 1;
		}

		boost::program_options::notify(variablesMap);

		if (variablesMap.count("gpio-nr"))
			decodeFromGPIO(gpioNr, bufferSize, std::chrono::microseconds(samplePeriod), tolerance);
		else
			decodeFromFile(inputFile, tolerance);
	}
	catch (const boost::program_options::error & e)
	{
		std::cerr << e.what() << "\n";
		return 1;
	}

	return 0;
}
