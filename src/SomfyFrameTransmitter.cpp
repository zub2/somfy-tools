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
#include "SomfyFrameTransmitter.h"
#include "rts/SomfyFrameHeader.h"
#include "rts/ManchesterEncoder.h"

#include <cassert>
#include <chrono>
#include <iostream>
#include <iomanip>
#include <stdexcept>

using namespace std::literals;

const rts::Duration SomfyFrameTransmitter::INTER_FRAME_GAP(27555us, false);

SomfyFrameTransmitter::SomfyFrameTransmitter(unsigned gpioNr, bool verbose, bool dryRun, const std::string & debugLog):
	m_verbose(verbose)
{
	if (!debugLog.empty())
	{
		m_debugLogWriter.emplace(debugLog);
		if (m_verbose)
			std::cout << "Writing transmitted data into log file: " << debugLog << std::endl;
	}
	if (!dryRun)
	{
		m_playbackThread.emplace(gpioNr);
		m_playbackThread->start();
	}
}

SomfyFrameTransmitter::~SomfyFrameTransmitter()
{
	if (m_playbackThread)
		m_playbackThread->stop();
}

void SomfyFrameTransmitter::send(const rts::SomfyFrame & frame, size_t repeatFrameCount)
{
	// prepare frame payload
	const std::vector<rts::Duration> frameSamples = getEncodedFramePayload(frame);

	rts::DurationBuffer buffer;

	buffer << INTER_FRAME_GAP;
	appendFrame(buffer, rts::SomfyFrameType::normal, frameSamples);

	for (size_t i = 0; i < repeatFrameCount; i++)
	{
		buffer << INTER_FRAME_GAP;
		appendFrame(buffer, rts::SomfyFrameType::repeat, frameSamples);
	}

	// add inter-frame gap (even if there is no further frame coming,
	// it makes sense to ensure there is a pause)
	buffer << INTER_FRAME_GAP;

	const std::vector<rts::Duration> & durationVector = buffer.get();

	if (m_verbose)
	{
		std::cout << "Whole data (header + payload) + inter frame gap + repeat frames, manchester-encoded:\n";
		for (const rts::Duration & d : durationVector)
			std::cout << std::chrono::duration_cast<std::chrono::microseconds>(d.first).count() << "µs " << d.second << "\n";
		std::cout << std::endl;
	}

	if (m_debugLogWriter)
	{
		for (const rts::Duration & duration : durationVector)
			m_debugLogWriter->write(duration);
	}

	if (m_playbackThread)
		m_playbackThread->play(durationVector);
	else
		std::cout << "Not transmitting because dry run mode is enabled." << std::endl;
}

void SomfyFrameTransmitter::appendFrame(rts::DurationBuffer & buffer, rts::SomfyFrameType frameType, const std::vector<rts::Duration> & frameSamples)
{
	// header
	const rts::SomfyFrameHeader * header;
	switch (frameType)
	{
	case rts::SomfyFrameType::normal:
		header = &rts::SOMFY_HEADER_NORMAL;
		break;
	case rts::SomfyFrameType::repeat:
		header = &rts::SOMFY_HEADER_REPEAT;
		break;
	default:
		throw std::runtime_error("unknown frame type");
	}

	assert(header != nullptr);

	// header
	std::copy(header->durations, header->durations + header->count, std::back_inserter(buffer));

	// payload
	std::copy(frameSamples.begin(), frameSamples.end(), std::back_inserter(buffer));
}

std::vector<rts::Duration> SomfyFrameTransmitter::getEncodedFramePayload(const rts::SomfyFrame & frame)
{
	const std::vector<uint8_t> & bytes = frame.getBytes();

	if (m_verbose)
	{
		std::cout << "Obfuscated packet:" << std::hex;
		for (size_t i = 0; i < bytes.size(); i++)
		{
			if (i != 0)
				std::cout << ',';
			std::cout << " 0x" << std::setw(2) << std::setfill('0') << static_cast<unsigned int>(bytes[i]);
		}
		std::cout << std::endl << std::dec;
	}

	rts::ManchesterEncoder encoder;
	for (uint8_t byte : bytes)
	{
		for (size_t i = 0; i < CHAR_BIT; i++)
		{
			encoder << ((byte & 0x80) != 0); // MSB
			byte = byte << 1;
		}
	}

	return encoder.getDurations();
}
