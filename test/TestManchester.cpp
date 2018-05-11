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

#include <cstdint>
#include <chrono>
#include <ostream>

#include "Clock.h"
#include "Duration.h"
#include "ManchesterDecoder.h"
#include "ManchesterEncoder.h"
#include "TestUtils.h"

using namespace std::literals;

namespace
{
	const Clock::duration HALF_SYMBOL = 645us;
	const Clock::duration FULL_SYMBOL = 2 * HALF_SYMBOL;

	const Duration TRANSITIONS[] =
	{
		// 0
		{ HALF_SYMBOL, true },
		{ HALF_SYMBOL, false },
		// 0
		{ HALF_SYMBOL, true },
		{ HALF_SYMBOL, false },
		// 0
		{ HALF_SYMBOL, true },
		{ FULL_SYMBOL, false }, // 1
		{ FULL_SYMBOL, true },  // 0
		{ FULL_SYMBOL, false }, // 1
		{ HALF_SYMBOL, true },
		// 1
		{ HALF_SYMBOL, false },
		{ HALF_SYMBOL, true },
		// 1
		{ HALF_SYMBOL, false },
		{ HALF_SYMBOL, true }
	};

	const size_t N_TRANSITIONS = sizeof(TRANSITIONS)/sizeof(TRANSITIONS[0]);

	const bool BITS[] =
	{
		false,
		false,
		false,
		true,
		false,
		true,
		true,
		true
	};

	const size_t N_BITS = sizeof(BITS)/sizeof(BITS[0]);
}

BOOST_AUTO_TEST_CASE(TestManchester_decode)
{
	ManchesterDecoder decoder(N_BITS);

	for (size_t i = 0; i < N_TRANSITIONS; i++)
		BOOST_TEST(decoder.newTransition(TRANSITIONS[i]));

	const std::vector<bool> bits = decoder.getBits();

	BOOST_TEST(bits.size() == N_BITS);
	for (size_t i = 0; i < N_BITS; i++)
		BOOST_TEST(bits[i] == BITS[i]);
}

BOOST_AUTO_TEST_CASE(TestManchester_encode)
{
	ManchesterEncoder encoder;

	for (size_t i = 0; i < N_BITS; i++)
		encoder << BITS[i];

	const std::vector<Duration> transitions = encoder.getDurations();

	BOOST_TEST(transitions.size() == N_TRANSITIONS);
	for (size_t i = 0; i < N_TRANSITIONS; i++)
	{
		BOOST_TEST(transitions[i].first == TRANSITIONS[i].first);
		BOOST_TEST(transitions[i].second == TRANSITIONS[i].second);
	}
}
