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
#ifndef RTS_FRAME_TRANSMITTER_FACTORY_H
#define RTS_FRAME_TRANSMITTER_FACTORY_H

#include <memory>
#include <functional>
#include <string>

#include "Duration.h"
#include "IFrameTransmitter.h"

namespace rts
{

struct GPIOConfiguration
{
	unsigned gpioNr;
	bool dryRun;
	std::function<void(const std::string &)> debugLogger;
	std::function<void(const std::vector<Duration>&)> durationLogger;
};

struct FrameTransmitterFactory
{
	static std::shared_ptr<IFrameTransmitter> create(const GPIOConfiguration & configuration);
	static std::shared_ptr<IFrameTransmitter> create(GPIOConfiguration && configuration);
};

}

#endif // RTS_FRAME_TRANSMITTER_FACTORY_H
