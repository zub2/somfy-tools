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
#ifndef SOMFY_FRAME_HEADER_H
#define SOMFY_FRAME_HEADER_H

#include "Clock.h"
#include "Duration.h"

namespace rts
{

struct SomfyFrameHeader
{
	const Duration * durations;
	size_t count;
};

extern const SomfyFrameHeader SOMFY_HEADER_NORMAL;
extern const SomfyFrameHeader SOMFY_HEADER_REPEAT;

} // namespace rts

#endif // SOMFY_FRAME_HEADER_H
