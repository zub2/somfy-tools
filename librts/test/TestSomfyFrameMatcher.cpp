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
#include <boost/test/unit_test.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/data/monomorphic.hpp>

#include <cstdint>
#include <vector>
#include <chrono>
#include <optional>
#include <ostream>
#include <functional>

#include "Clock.h"
#include "Duration.h"
#include "SomfyFrameType.h"
#include "SomfyFrameMatcher.h"
#include "TestUtils.h"

using namespace std::literals;
using namespace rts;

namespace
{
	const std::vector<Duration> IDEAL_FRAME_HEADER_NORMAL =
	{
		Duration(std::chrono::duration_cast<Clock::duration>(10.4ms), true),
		Duration(std::chrono::duration_cast<Clock::duration>(7.10ms), false),
		Duration(std::chrono::duration_cast<Clock::duration>(2.47ms), true),
		Duration(std::chrono::duration_cast<Clock::duration>(2.55ms), false),
		Duration(std::chrono::duration_cast<Clock::duration>(2.47ms), true),
		Duration(std::chrono::duration_cast<Clock::duration>(2.55ms), false),
		Duration(std::chrono::duration_cast<Clock::duration>(4.80ms), true),
		Duration(std::chrono::duration_cast<Clock::duration>(645us), false)
	};

	const std::vector<Duration> IDEAL_FRAME_HEADER_REPEAT =
	{
		Duration(std::chrono::duration_cast<Clock::duration>(2.47ms), true),
		Duration(std::chrono::duration_cast<Clock::duration>(2.55ms), false),
		Duration(std::chrono::duration_cast<Clock::duration>(2.47ms), true),
		Duration(std::chrono::duration_cast<Clock::duration>(2.55ms), false),
		Duration(std::chrono::duration_cast<Clock::duration>(2.47ms), true),
		Duration(std::chrono::duration_cast<Clock::duration>(2.55ms), false),
		Duration(std::chrono::duration_cast<Clock::duration>(2.47ms), true),
		Duration(std::chrono::duration_cast<Clock::duration>(2.55ms), false),
		Duration(std::chrono::duration_cast<Clock::duration>(2.47ms), true),
		Duration(std::chrono::duration_cast<Clock::duration>(2.55ms), false),
		Duration(std::chrono::duration_cast<Clock::duration>(2.47ms), true),
		Duration(std::chrono::duration_cast<Clock::duration>(2.55ms), false),
		Duration(std::chrono::duration_cast<Clock::duration>(2.47ms), true),
		Duration(std::chrono::duration_cast<Clock::duration>(2.55ms), false),
		Duration(std::chrono::duration_cast<Clock::duration>(4.80ms), true),
		Duration(std::chrono::duration_cast<Clock::duration>(645us), false)
	};

	constexpr Clock::duration FRAME_REMINDER = 645us;
	const std::vector<Duration> FRAME_HEADER_NORMAL_WITH_REMINDER =
	{
		Duration(std::chrono::duration_cast<Clock::duration>(10.4ms), true),
		Duration(std::chrono::duration_cast<Clock::duration>(7.10ms), false),
		Duration(std::chrono::duration_cast<Clock::duration>(2.47ms), true),
		Duration(std::chrono::duration_cast<Clock::duration>(2.55ms), false),
		Duration(std::chrono::duration_cast<Clock::duration>(2.47ms), true),
		Duration(std::chrono::duration_cast<Clock::duration>(2.55ms), false),
		Duration(std::chrono::duration_cast<Clock::duration>(4.80ms), true),
		Duration(std::chrono::duration_cast<Clock::duration>(645us + FRAME_REMINDER), false)
	};

	const std::vector<Duration> NO_FRAME =
	{
		Duration(std::chrono::duration_cast<Clock::duration>(4.0ms), true),
		Duration(std::chrono::duration_cast<Clock::duration>(1.10ms), false),
		Duration(std::chrono::duration_cast<Clock::duration>(4.1ms), true),
		Duration(std::chrono::duration_cast<Clock::duration>(1.2ms), false),
		Duration(std::chrono::duration_cast<Clock::duration>(1.10ms), true),
		Duration(std::chrono::duration_cast<Clock::duration>(4.1ms), false),
		Duration(std::chrono::duration_cast<Clock::duration>(1.2ms), true),
		Duration(std::chrono::duration_cast<Clock::duration>(1.10ms), false),
		Duration(std::chrono::duration_cast<Clock::duration>(4.1ms), true),
		Duration(std::chrono::duration_cast<Clock::duration>(1.2ms), false),
		Duration(std::chrono::duration_cast<Clock::duration>(1.9ms), true)
	};
}

namespace rts
{
	std::ostream & operator<< (std::ostream & os, SomfyFrameType frameType)
	{
		switch (frameType)
		{
		case SomfyFrameType::normal:
			os << "normal";
			break;

		case SomfyFrameType::repeat:
			os << "repeat";
			break;

		default:
			os << "[UNKNOWN]";
			break;
		}

		return os;
	}
}

void testMatch(const std::vector<Duration> & inputSequence, double tolerance,
	std::optional<SomfyFrameMatcher::FrameMatch> expectedMatch)
{
	SomfyFrameMatcher matcher(tolerance);

	for (size_t i = 0; i < inputSequence.size(); i++)
	{
		const bool isLast = i == inputSequence.size() - 1;
		const std::optional<SomfyFrameMatcher::FrameMatch> result = matcher.newTransition(inputSequence[i]);
		if (!isLast)
		{
			// frame should not be matched yet
			BOOST_TEST(!result.has_value());
		}
		else
		{
			// last sample, check that we got the expected result
			BOOST_TEST(result.has_value() == expectedMatch.has_value());
			if (result && expectedMatch)
			{
				BOOST_TEST(result->type == expectedMatch->type);
				BOOST_TEST(result->remainingDuration == expectedMatch->remainingDuration);
				BOOST_TEST(result->state == expectedMatch->state);
			}
		}
	}
}

BOOST_AUTO_TEST_CASE(TestSomfyFrameMatcher_idealNormal)
{
	SomfyFrameMatcher::FrameMatch match(
		SomfyFrameType::normal,
		Clock::duration::zero(),
		false
	);
	testMatch(IDEAL_FRAME_HEADER_NORMAL, 0.1, match);
}

BOOST_AUTO_TEST_CASE(TestSomfyFrameMatcher_idealRepeat)
{
	SomfyFrameMatcher::FrameMatch match(
		SomfyFrameType::repeat,
		Clock::duration::zero(),
		false
	);
	testMatch(IDEAL_FRAME_HEADER_REPEAT, 0.1, match);
}

BOOST_AUTO_TEST_CASE(TestSomfyFrameMatcher_normalWithReminder)
{
	SomfyFrameMatcher::FrameMatch match(
		SomfyFrameType::normal,
		FRAME_REMINDER,
		false
	);
	testMatch(FRAME_HEADER_NORMAL_WITH_REMINDER, 0.1, match);
}

BOOST_AUTO_TEST_CASE(TestSomfyFrameMatcher_noFrame)
{
	testMatch(NO_FRAME, 0.1, std::nullopt);
}
