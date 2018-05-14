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
#ifndef RTS_SOMFY_DECODER_H
#define RTS_SOMFY_DECODER_H

#include <iostream>
#include <iomanip>
#include <sstream>
#include <optional>

#include "Duration.h"
#include "ManchesterDecoder.h"
#include "SomfyFrameMatcher.h"
#include "SomfyFrameType.h"
#include "SomfyFrame.h"

namespace rts
{

template<typename Source>
class SomfyDecoder
{
private:
	enum class State
	{
		SearchingForFrame,
		ReadingPayload
	};

public:
	SomfyDecoder(Source & s, double tolerance):
		m_source(s),
		m_tolerance(tolerance)
	{}

	void run()
	{
		SomfyFrameMatcher matcher(m_tolerance);
		ManchesterDecoder decoder(SomfyFrame::FRAME_SIZE * CHAR_BIT);

		State state = State::SearchingForFrame;
		SomfyFrameMatcher::FrameMatch frameMatch;

		std::optional<Duration> duration = m_source.get();
		while (duration)
		{
			switch (state)
			{
			case State::SearchingForFrame:
				if (auto f = matcher.newTransition(*duration))
				{
					frameMatch = *f;
					onFrameDetected(frameMatch.type);
					state = State::ReadingPayload;
					decoder.reset();

					// pass reminder of pulse to the next state (ReadingPayload)
					if (frameMatch.remainingDuration > Clock::duration::zero())
						duration = Duration(frameMatch.remainingDuration, frameMatch.state);
					else
						duration.reset();
				}
				else
					duration.reset();
				break;

			case State::ReadingPayload:
				if (decoder.newTransition(*duration))
				{
					if (decoder.getBits().size() == SomfyFrame::FRAME_SIZE * CHAR_BIT)
					{
						onFrameDecoded(frameMatch.type, decoder.getBits());
						state = State::SearchingForFrame;
					}
					// else: go on
				}
				else
				{
					onFrameDecodeError(decoder.getBits());
					state = State::SearchingForFrame;
				}
				duration.reset();
				break;
			}

			if (!duration)
				duration = m_source.get();
		}
	}

private:
	void onFrameDetected(SomfyFrameType frameType)
	{
		std::cout << ">>> GOT SOMFY FRAME [type=";
		switch (frameType)
		{
		case SomfyFrameType::normal:
			std::cout << "normal";
			break;
		case SomfyFrameType::repeat:
			std::cout << "repeat";
			break;
		}
		std::cout << "] <<<" << std::endl;
		std::cout.flush();
	}

	void onFrameDecoded(SomfyFrameType frameType, const std::vector<bool> & bits)
	{
		std::cout << "got all bits!" << std::endl;
		std::cout << "decoded bits: " << stringifyBits(bits) << std::endl;

		std::vector<uint8_t> bytes = bitsToBytes(bits);
		std::cout << "decoded bytes: " << stringifyBytes(bytes) << std::endl;

		try
		{
			SomfyFrame frame = SomfyFrame::fromBytes(std::move(bytes));
			dumpFrame(frame);
		}
		catch (const WrongFrameChecksumException &)
		{
			std::cout << "Wrong frame checksum. :-(" << std::endl;
		}
		std::cout.flush();
	}

	void dumpFrame(const SomfyFrame & frame)
	{
		std::cout << "key: 0x" << std::hex << std::setw(2) << static_cast<uint16_t>(frame.getKey()) << std::endl;
		std::cout << "code: 0x" << std::hex << static_cast<uint16_t>(frame.getCtrl())
			<< " [" << getButtonName(frame.getCtrl()) << "]" << std::endl;
		std::cout << "rolling code: 0x" << std::hex << std::setw(4) << std::setfill('0') << frame.getRollingCode() << std::endl;
		std::cout << "address: 0x" << std::hex << std::setw(6) << frame.getAddress() << std::endl;
	}

	std::string getButtonName(SomfyFrame::Action code)
	{
		typedef SomfyFrame::Action Action;

		switch (code)
		{
		case Action::my:
			return "My";

		case Action::up:
			return "Up";

		case Action::my_up:
			return "My + Up";

		case Action::down:
			return "Down";

		case Action::my_down:
			return "My + Down";

		case Action::up_down:
			return "Up + Down";

		case Action::prog:
			return "Prog";

		case Action::sun_flag:
			return "Sun + Flag";

		case Action::flag:
			return "Flag";

		default:
			return "unknown";
		}
	}

	void onFrameDecodeError(const std::vector<bool> & bits)
	{
		std::cout << "decoding failed after " << bits.size() << " bits!" << std::endl;
		std::cout << "decoded bits: " << stringifyBits(bits) << std::endl;
		std::cout.flush();
	}

	std::string stringifyBits(const std::vector<bool> & bits)
	{
		std::stringstream s;

		for (size_t i = 0; i < bits.size(); i++)
		{
			if (i != 0 && i % CHAR_BIT == 0)
				s.put('|');
			else if (i != 0 && i % (CHAR_BIT/2) == 0)
				s.put('.');
			s.put("01"[bits[i]]);
		}
		return s.str();
	}

	std::vector<uint8_t> bitsToBytes(const std::vector<bool> & bits)
	{
		if (bits.size() % CHAR_BIT != 0)
			throw std::runtime_error("Bit count not a multiple of byte size!");

		std::vector<uint8_t> bytes;
		bytes.reserve(bits.size() / CHAR_BIT);
		for (size_t i = 0; i < bits.size(); i++)
		{
			if (i % CHAR_BIT == 0)
				bytes.push_back(0);

			if (bits[i])
			{
				const uint8_t bitWeight = 1 << (CHAR_BIT - 1 - (i % CHAR_BIT));
				bytes.back() |= bitWeight;
			}
		}

		return bytes;
	}

	std::string stringifyBytes(const std::vector<uint8_t> & bytes)
	{
		std::stringstream s;

		for (size_t i = 0; i < bytes.size(); i++)
		{
			if (i != 0)
				s.put('|');
			s << std::hex << std::setfill('0') << std::setw(2) << static_cast<uint16_t>(bytes[i]); // uint8_t is printed out as a char
		}
		return s.str();
	}

	Source & m_source;
	const double m_tolerance;
};

} // namespace rts

#endif // RTS_SOMFY_DECODER_H
