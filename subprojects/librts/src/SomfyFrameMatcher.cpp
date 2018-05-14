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
#include "SomfyFrameMatcher.h"
#include "SomfyFrameHeader.h"

#include <cmath>
#include <chrono>

#include <iostream>

namespace rts
{

SomfyFrameMatcher::SomfyFrameMatcher(double tolerance):
	m_tolerance(tolerance),
	m_matcherNormal(SOMFY_HEADER_NORMAL.durations, SOMFY_HEADER_NORMAL.count),
	m_matcherRepeat(SOMFY_HEADER_REPEAT.durations, SOMFY_HEADER_REPEAT.count)
{}

void SomfyFrameMatcher::reset()
{
	m_matcherNormal.reset();
	m_matcherRepeat.reset();
}

std::optional<SomfyFrameMatcher::FrameMatch> SomfyFrameMatcher::newTransition(const Duration & duration)
{
	std::optional<FrameMatch> frameMatch;
	std::optional<Clock::duration> remainingDuration = m_matcherNormal.newTransition(duration, m_tolerance);
	if (remainingDuration)
	{
		// normal frame was matched
		frameMatch = FrameMatch(SomfyFrameType::normal, *remainingDuration, duration.second);
	}
	else
	{
		// try repeat frame
		remainingDuration = m_matcherRepeat.newTransition(duration, m_tolerance);
		if (remainingDuration)
		{
			// repeat frame was matched
			frameMatch = FrameMatch(SomfyFrameType::repeat, *remainingDuration, duration.second);
		}
	}

	if (frameMatch)
	{
		m_matcherNormal.reset();
		m_matcherRepeat.reset();
	}

	return frameMatch;
}

SomfyFrameMatcher::SequenceMatcher::SequenceMatcher(const Duration * sequence, size_t sequenceSize):
	m_sequence(sequence),
	m_sequenceSize(sequenceSize),
	m_matchedCount(0)
{}

std::optional<Clock::duration> SomfyFrameMatcher::SequenceMatcher::newTransition(const Duration & duration, double tolerance)
{
	if (m_matchedCount == m_sequenceSize)
		throw std::runtime_error("SequenceMatcher::newTransition() called too many times w/o a reset!");

	const bool newState = duration.second;
	const bool expectedNewState = m_sequence[m_matchedCount].second;
	if (newState == expectedNewState &&
			((matchDuration(duration.first, m_sequence[m_matchedCount].first, tolerance) ||
			(m_matchedCount + 1 == m_sequenceSize && matchDurationAtLeast(duration.first, m_sequence[m_matchedCount].first, tolerance))))
		)
	{
		m_matchedCount++;

		// TODO: clean this up
		if (m_matchedCount == m_sequenceSize)
		{
			if (matchDuration(duration.first, m_sequence[m_matchedCount-1].first, tolerance))
				return Clock::duration::zero();
			else
				return std::max(duration.first - m_sequence[m_matchedCount-1].first, Clock::duration::zero());
		}
		return std::nullopt;
	}
	else
	{
		reset();
		return std::nullopt;
	}
}

bool SomfyFrameMatcher::SequenceMatcher::matchDuration(Clock::duration actual, Clock::duration expected, double tolerance)
{
	const double a = std::chrono::duration<double>(actual).count();
	const double e = std::chrono::duration<double>(expected).count();

	return fabs(a - e) < tolerance * e;
}

bool SomfyFrameMatcher::SequenceMatcher::matchDurationAtLeast(Clock::duration actual, Clock::duration expected, double tolerance)
{
	const double a = std::chrono::duration<double>(actual).count();
	const double e = std::chrono::duration<double>(expected).count();

	return a - e > - tolerance * e;
}

void SomfyFrameMatcher::SequenceMatcher::reset()
{
	m_matchedCount = 0;
}

} // namespace rts
