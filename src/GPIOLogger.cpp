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
#include "Clock.h"
#include "Transition.h"
#include "GPIOLogWriter.h"
#include "RecordingThread.h"
#include "DurationTracker.h"

#include <iostream>
#include <fstream>
#include <stdexcept>
#include <chrono>
#include <sstream>
#include <cstdlib>
#include <optional>

#include <boost/program_options.hpp>

using std::chrono::steady_clock;

namespace
{
	constexpr unsigned DEFAULT_SAMPLE_PERIOD_US = 10;
	const std::string DEFAULT_FILENAME = "gpio_log.bin";
	constexpr unsigned DEFAULT_DURATION_S = 5;
	constexpr size_t DEFAULT_BUFFER_SIZE = 1000;

	void record(unsigned gpioNr, const Clock::duration & recordingDuration, size_t bufferSize,
		const Clock::duration & samplePeriod, const std::string & outputFileName)
	{
		GPIOLogWriter writer(outputFileName);
		RecordingThread recorder(gpioNr, bufferSize, samplePeriod);

		std::cout << "Starting recording... " << std::flush;

		const steady_clock::time_point end = steady_clock::now() + recordingDuration;
		recorder.start();

		std::thread stopThread([&end, &recorder](){
			std::this_thread::sleep_until(end);
			recorder.stop();
		});

		DurationTracker<RecordingThread> durationTracker(recorder);
		std::optional<Duration> duration = durationTracker.get();
		while (duration)
		{
			writer.write(*duration);
			duration = durationTracker.get();
		}

		stopThread.join();

		std::cout << "done" << std::endl;
	}
}

int main(int argc, char * argv[])
{
	try
	{
		unsigned gpioNr;
		unsigned recordingDuration = DEFAULT_DURATION_S;
		size_t bufferSize = DEFAULT_BUFFER_SIZE;
		unsigned samplePeriod = DEFAULT_SAMPLE_PERIOD_US;
		std::string outputFileName = DEFAULT_FILENAME;

		boost::program_options::options_description argDescription("Available options");
		argDescription.add_options()
			("gpio-nr,n", boost::program_options::value(&gpioNr)->required(),
				"The GPIO number to use.")
			("duration,d", boost::program_options::value(&recordingDuration),
				(std::string("Number of seconds to keep recording. Default: ") + std::to_string(DEFAULT_DURATION_S)).c_str())
			("buffer-size,b", boost::program_options::value(&bufferSize),
				(std::string("Size of buffer (number of entries). Default: ") + std::to_string(DEFAULT_BUFFER_SIZE)).c_str())
			("sample-period,s", boost::program_options::value(&samplePeriod),
				(std::string("Sample rate in µs. Default: ") + std::to_string(DEFAULT_SAMPLE_PERIOD_US)).c_str())
			("file,f", boost::program_options::value(&outputFileName),
				(std::string("Name of the out file. Default: ") + DEFAULT_FILENAME).c_str())
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

		record(gpioNr, std::chrono::seconds(recordingDuration), bufferSize, std::chrono::microseconds(samplePeriod), outputFileName);
	}
	catch (const boost::program_options::error & e)
	{
		std::cerr << e.what() << "\n";
		return 1;
	}

	return 0;
}
