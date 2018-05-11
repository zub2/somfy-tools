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
#include "SomfyFrame.h"
#include "SomfyFrameTransmitter.h"

#include <iostream>
#include <vector>
#include <algorithm>
#include <iterator>
#include <type_traits>
#include <sstream>
#include <limits>

#include <boost/program_options.hpp>
#include <boost/any.hpp>

using namespace std::literals;

namespace
{
	constexpr size_t DEFAULT_REPEAT_FRAMES = 1;

	void play(unsigned gpioNr, uint8_t key, rts::SomfyFrame::Action ctrl, uint16_t rollingCode, uint32_t address,
		size_t nRepeatFrames, bool verbose, bool dryRun, const std::string & logFile)
	{
		if (verbose)
			std::cout << "Will transmit using GPIO #" << gpioNr << std::endl;
		SomfyFrameTransmitter transmitter(gpioNr, verbose, dryRun, logFile);

		// send 1 normal and 1 repeat frame
		const rts::SomfyFrame frame(key, ctrl, rollingCode, address);
		transmitter.send(frame, nRepeatFrames);
	}

	const std::map<std::string, rts::SomfyFrame::Action> ACTION_NAMES =
	{
		{ "up", rts::SomfyFrame::Action::up },
		{ "down", rts::SomfyFrame::Action::down },
		{ "my", rts::SomfyFrame::Action::my },
		{ "my+down", rts::SomfyFrame::Action::my_down },
		{ "up+down", rts::SomfyFrame::Action::up_down },
		{ "flag", rts::SomfyFrame::Action::flag },
		{ "sun+flag", rts::SomfyFrame::Action::sun_flag },
		{ "prog", rts::SomfyFrame::Action::prog }
	};
}

// defined in namespace rts because of Argument-dependent lookup
namespace rts
{
	std::istream & operator>>(std::istream & in, SomfyFrame::Action & action)
	{
		std::string token;
		in >> token;

		auto it = ACTION_NAMES.find(token);
		if (it != ACTION_NAMES.end())
			action = it->second;
		else
			in.setstate(std::ios_base::failbit);

		return in;
	}
}

// a simple wrapper around an integral type used solely to let boost::program_options
// parse the input value ourselves - to enable hex input (0x...)
template<typename T>
struct Number
{
	static_assert(std::is_integral<T>::value && std::is_unsigned<T>::value, "T must be an unsigned integral type!");
	T value;
};

template<typename T>
std::istream & operator>>(std::istream & in, Number<T> & n)
{
	std::string token;
	in >> token;

	static_assert(std::numeric_limits<T>::max() <= std::numeric_limits<unsigned long long>::max(),
		"T must fit in unsigned long long");

	size_t numConverted = 0;
	n.value = static_cast<T>(std::stoull(token, &numConverted, 0));

	if (numConverted != token.size())
		in.setstate(std::ios_base::failbit);

	return in;
}

int main(int argc, char * argv[])
{
	try
	{
		unsigned gpioNr;
		Number<uint8_t> key;
		rts::SomfyFrame::Action ctrl;
		Number<uint16_t> rollingCode;
		Number<uint32_t> address;
		Number<uint32_t> nRepeatFrames = { DEFAULT_REPEAT_FRAMES };
		std::string logFile;

		std::string allActions;
		for (const auto & actionItem : ACTION_NAMES)
		{
			if (!allActions.empty())
				allActions += ", ";
			allActions += actionItem.first;
		}

		boost::program_options::options_description argDescription("Available options");
		argDescription.add_options()
			("gpio-nr,n", boost::program_options::value(&gpioNr)->required(),
				"The GPIO number to use.")
			("key,k", boost::program_options::value(&key)->required(),
				"Key value in the packet.")
			("control,c", boost::program_options::value(&ctrl)->required(),
				(std::string("Control code - what operation shall be done. One of ") + allActions + ".").c_str())
			("rolling-code,r", boost::program_options::value(&rollingCode)->required(),
				"Rolling code.")
			("address,a", boost::program_options::value(&address)->required(),
				"Address.")
			("repeat-frames,R", boost::program_options::value(&nRepeatFrames),
				(std::string("Number of repeat frames. Default: ") + std::to_string(DEFAULT_REPEAT_FRAMES)).c_str())
			("verbose,v",
				"Be berbose, print out what it being transmitted.")
			("dry-run,D",
				"Do everything except for the actual transmission.")
			("log-file,f", boost::program_options::value(&logFile),
				"Write transmitted data to a log file.")
			("help,h", "print this help")
		;

		boost::program_options::variables_map variablesMap;
		boost::program_options::store(boost::program_options::parse_command_line(argc, argv, argDescription), variablesMap);

		if (variablesMap.count("help"))
		{
			std::cout << argDescription << "\n";
			return 1;
		}

		bool verbose = variablesMap.count("verbose") != 0;
		bool dryRun = variablesMap.count("dry-run") != 0;

		boost::program_options::notify(variablesMap);

		play(gpioNr, key.value, ctrl, rollingCode.value, address.value, nRepeatFrames.value, verbose, dryRun, logFile);
	}
	catch (const boost::program_options::error & e)
	{
		std::cerr << e.what() << "\n";
		return 1;
	}

	return 0;
}
