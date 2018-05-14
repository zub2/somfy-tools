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
#include "SomfyFrameHeader.h"
#include "Clock.h"

#include <chrono>

namespace rts
{

using namespace std::literals;

namespace
{
	constexpr Duration PULSES_SOMFY_HEADER_NORMAL[] =
	{
		// wakeup
		Duration(10400us, true),
		Duration(7100us, false),

		// HW sync (1)
		Duration(2550us, true),
		Duration(2470us, false),

		// HW sync (2)
		Duration(2470us, true),
		Duration(2550us, false),

		// SW sync
		Duration(4800us, true),
		Duration(645us, false)
	};

	const Duration PULSES_SOMFY_HEADER_REPEAT[] =
	{
		// HW sync (1)
		Duration(2470us, true),
		Duration(2550us, false),

		// HW sync (2)
		Duration(2470us, true),
		Duration(2550us, false),

		// HW sync (3)
		Duration(2470us, true),
		Duration(2550us, false),

		// HW sync (4)
		Duration(2470us, true),
		Duration(2550us, false),

		// HW sync (5)
		Duration(2470us, true),
		Duration(2550us, false),

		// HW sync (6)
		Duration(2470us, true),
		Duration(2550us, false),

		// HW sync (7)
		Duration(2470us, true),
		Duration(2550us, false),

		// SW sync
		Duration(4800us, true),
		Duration(645us, false)
	};
}

const SomfyFrameHeader SOMFY_HEADER_NORMAL =
{
	PULSES_SOMFY_HEADER_NORMAL,
	sizeof(PULSES_SOMFY_HEADER_NORMAL)/sizeof(PULSES_SOMFY_HEADER_NORMAL[0])
};

const SomfyFrameHeader SOMFY_HEADER_REPEAT =
{
	PULSES_SOMFY_HEADER_REPEAT,
	sizeof(PULSES_SOMFY_HEADER_REPEAT)/sizeof(PULSES_SOMFY_HEADER_REPEAT[0])
};

} // namespace rts
