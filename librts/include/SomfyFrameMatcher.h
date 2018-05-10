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
#ifndef SOMFY_FRAME_MATCHER
#define SOMFY_FRAME_MATCHER

#include <optional>
#include <cstddef>

#include "Clock.h"
#include "Duration.h"
#include "SomfyFrameType.h"

class SomfyFrameMatcher
{
public:
	struct FrameMatch
	{
		FrameMatch(SomfyFrameType type, const Clock::duration & remainingDuration, bool state):
			type(type),
			remainingDuration(remainingDuration),
			state(state)
		{}

		FrameMatch():
			type(SomfyFrameType::normal),
			remainingDuration(Clock::duration::zero()),
			state(false)
		{}

		SomfyFrameType type;
		Clock::duration remainingDuration;
		bool state;
	};

	SomfyFrameMatcher(double tolerance);

	std::optional<FrameMatch> newTransition(const Duration & duration);
	void reset();

private:
	class SequenceMatcher
	{
	public:
		SequenceMatcher(const Duration * sequence, size_t sequenceSize);
		std::optional<Clock::duration> newTransition(const Duration & duration, double tolerance);
		void reset();
	private:
		bool matchDuration(Clock::duration actual, Clock::duration expected, double tolerance);
		bool matchDurationAtLeast(Clock::duration actual, Clock::duration expected, double tolerance);
		const Duration *m_sequence;
		const size_t m_sequenceSize;
		size_t m_matchedCount;
	};

	SequenceMatcher m_matcherNormal;
	SequenceMatcher m_matcherRepeat;

	const double m_tolerance;
};

#endif // SOMFY_FRAME_MATCHER
