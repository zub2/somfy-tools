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
#ifdef HAVE_RTLSDR
	#include "SigIntHandler.h"
	#include "backend/rtlsdr/RTLSDRDevice.h"
	#include "backend/rtlsdr/RTLSDRBufferReader.h"
	#include "backend/rtlsdr/RTLSDRIQSource.h"
#endif // HAVE_RTLSDR

#include "IQLogReader.h"
#include "DurationTracker.h"
#include "SomfyDecoder.h"
#include "backend/rtlsdr/OOKDecoder.h"

#include <iostream>
#include <stdexcept>
#include <string>
#include <complex>
#include <vector>
#include <thread>
#include <optional>

#include <boost/program_options.hpp>

namespace
{
	constexpr uint32_t DEFAULT_RTLSDR_DEVICE_INDEX = 0;
	constexpr double DEFAULT_TOLERANCE = 0.1;
	constexpr uint32_t RTLSDR_SAMPLE_RATE = 2600000; // 2.6 MHz
#ifdef HAVE_RTLSDR
	constexpr uint32_t RTLSDR_FREQUENCY = 433420000; // 433.42 MHz
#endif

#ifdef HAVE_RTLSDR
	void decodeSdrDev(uint32_t deviceIndex, double tolerance, const size_t bufferSize, const size_t bufferCount,
		const std::string & logName)
	{
		RTLSDRDevice rtlSDRDevice(deviceIndex);

		rtlSDRDevice.setSampleRate(RTLSDR_SAMPLE_RATE);
		rtlSDRDevice.setFrequency(RTLSDR_FREQUENCY);

		std::optional<int> manualGain = -99; // TODO: make this a command line option
		if (manualGain)
		{
			rtlSDRDevice.setManualTunnerGainMode(true);
			rtlSDRDevice.setTunerGain(*manualGain);
		}

		RTLSDRIQSource rtlSDRIQSource(rtlSDRDevice, bufferSize, bufferCount, logName);
		OOKDecoder<RTLSDRIQSource> ookDecoder(rtlSDRIQSource, RTLSDR_SAMPLE_RATE);
		DurationTracker<OOKDecoder<RTLSDRIQSource>> durationTracker(ookDecoder);
		SomfyDecoder decoder(durationTracker, tolerance);

		rtlSDRIQSource.start();

		installSigIntHandler();

		std::thread stopThread([&rtlSDRIQSource](){
			waitForSigInt();
			rtlSDRIQSource.stop();
		});

		decoder.run();
		stopThread.join();
	}
#endif // HAVE_RTLSDR

	void decodeIqLog(const std::string & iqLogFileName, double tolerance)
	{
		IQLogReader iqLogReader(iqLogFileName);
		OOKDecoder<IQLogReader> ookDecoder(iqLogReader, RTLSDR_SAMPLE_RATE);
		DurationTracker<OOKDecoder<IQLogReader>> durationTracker(ookDecoder);
		SomfyDecoder decoder(durationTracker, tolerance);

		decoder.run();
	}
}

int main(int argc, char * argv[])
{
	try
	{
		uint32_t deviceIndex = DEFAULT_RTLSDR_DEVICE_INDEX;
		std::string inputFileName;
		double tolerance = DEFAULT_TOLERANCE;
#ifdef HAVE_RTLSDR
		// TODO: make these into command line options
		constexpr size_t bufferSize = 256*1024;
		constexpr size_t bufferCount = 5;
		std::string logName; // TODO
#endif

		boost::program_options::options_description argDescription("Available options");
		argDescription.add_options()
			("device-index,d", boost::program_options::value(&deviceIndex),
				(std::string("Index of the RTL SDR device to read samples from. Default: ") + std::to_string(DEFAULT_RTLSDR_DEVICE_INDEX)).c_str())
			("file,f", boost::program_options::value(&inputFileName),
				"Source file name. (IQ format sampled at 2.6 MHz.)"
#ifndef HAVE_RTLSDR
				" OPTION NOT AVAILABLE IN THIS BUILD (executable has been built without librtlsdr)!"
#endif
			)
			("tolerance,t", boost::program_options::value(&tolerance),
				(std::string("Tolerance in measured timing. Default: ") + std::to_string(DEFAULT_TOLERANCE)).c_str())
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

		if (variablesMap.count("device-index") && variablesMap.count("file"))
		{
			std::cerr << "Only one of -d (--device-index) and -f (--file) can be specified." << std::endl;
			return 1;
		}
		else if (variablesMap.count("device-index") == 0 && variablesMap.count("file") == 0)
		{
#ifdef HAVE_RTLSDR
			std::cerr << "Specify exactly one of -d (--device-index) or -f (--file) to select input." << std::endl;
#else
			std::cerr << "Specify -f (--file) to select input file." << std::endl;
#endif
			return 1;
		}
		else if (variablesMap.count("file"))
		{
			decodeIqLog(inputFileName, tolerance);
		}
		else
		{
#ifdef HAVE_RTLSDR
			decodeSdrDev(deviceIndex, tolerance, bufferSize, bufferCount, logName);
#else
			std::cerr << "This executable has been built without librtlsdr so reading from a rtlsdr device is not possible." << std::endl;
#endif
		}
	}
	catch (const boost::program_options::error & e)
	{
		std::cerr << e.what() << "\n";
		return 1;
	}

	return 0;
}
