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
#ifndef SOMFY_FRAME_TRANSMITTER_H
#define SOMFY_FRAME_TRANSMITTER_H

#include "rts/SomfyFrame.h"
#include "rts/SomfyFrameType.h"
#include "rts/backend/rpi-gpio/PlaybackThread.h"
#include "rts/DurationBuffer.h"
#include "GPIOLogWriter.h"

#include <string>
#include <optional>

class SomfyFrameTransmitter
{
public:
	SomfyFrameTransmitter(unsigned gpioNr, bool verbose = false, bool dryRun = false, const std::string & debugLog = std::string());
	~SomfyFrameTransmitter();

	void send(const rts::SomfyFrame & frame, size_t repeatFrameCount);

private:
	void appendFrame(rts::DurationBuffer & buffer, rts::SomfyFrameType frameType, const std::vector<rts::Duration> & frameSamples);
	std::vector<rts::Duration> getEncodedFramePayload(const rts::SomfyFrame & frame);

	bool m_verbose;
	std::optional<rts::PlaybackThread> m_playbackThread;
	std::optional<GPIOLogWriter> m_debugLogWriter;

	static const rts::Duration INTER_FRAME_GAP;
};

#endif // SOMFY_FRAME_TRANSMITTER_H
