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
#ifndef RTS_GPIO_FRAME_TRANSMITTER_H
#define RTS_GPIO_FRAME_TRANSMITTER_H

#include <vector>
#include <functional>
#include <optional>

#include "IFrameTransmitter.h"
#include "Duration.h"
#include "DurationBuffer.h"
#include "SomfyFrame.h"
#include "SomfyFrameType.h"
#include "backend/rpi-gpio/PlaybackThread.h"

namespace rts
{

class GPIOFrameTransmitter: public IFrameTransmitter
{
public:
	explicit GPIOFrameTransmitter(unsigned gpioNr, bool dryRun, std::function<void(const std::string &)> debugLogger,
		std::function<void(const std::vector<Duration>&)> durationLogger);
	virtual ~GPIOFrameTransmitter();

	virtual void send(const SomfyFrame & frame, size_t repeatFrameCount) override;

private:
	void appendFrame(DurationBuffer & buffer, SomfyFrameType frameType, const std::vector<Duration> & frameSamples);
	std::vector<Duration> getEncodedFramePayload(const SomfyFrame & frame);

	std::optional<PlaybackThread> m_playbackThread;
	std::function<void(const std::string &)> m_debugLogger;
	std::function<void(const std::vector<Duration>&)> m_durationLogger;

	static const rts::Duration INTER_FRAME_GAP;
};

}

#endif // RTS_GPIO_FRAME_TRANSMITTER_H
