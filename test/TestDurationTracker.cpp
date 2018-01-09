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

#include <chrono>
#include <vector>
#include <optional>

#include "Clock.h"
#include "Transition.h"
#include "DurationTracker.h"
#include "Duration.h"

using namespace std::literals;

namespace
{
	class TransitionSource
	{
	public:
		TransitionSource(std::vector<Transition> && transitions):
			m_transitions(std::move(transitions)),
			m_current(m_transitions.begin())
		{}

		std::optional<Transition> get()
		{
			if (m_current != m_transitions.end())
				return *m_current++;
			else
				return std::nullopt;
		}

	private:
		std::vector<Transition> m_transitions;
		std::vector<Transition>::const_iterator m_current;
	};
}

BOOST_AUTO_TEST_CASE(TestDurationTracker_empty)
{
	TransitionSource source({});
	DurationTracker<TransitionSource> d(source);

	// no duration returned from first transition
	BOOST_TEST(!d.get().has_value());
}

BOOST_AUTO_TEST_CASE(TestDurationTracker_twoTransitions)
{
	const Clock::duration delta = 200us;
	const Clock::time_point t1 = Clock::now();
	const Clock::time_point t2 = t1 + delta;

	TransitionSource source({
		Transition(t1, true),
		Transition(t2, false),
	});
	DurationTracker<TransitionSource> d(source);

	const std::optional<Duration> r = d.get();
	BOOST_TEST(r.has_value());
	BOOST_TEST(r->first.count() == delta.count()); // count() to make BOOST_TEST happy
	BOOST_TEST(r->second == true);
}
