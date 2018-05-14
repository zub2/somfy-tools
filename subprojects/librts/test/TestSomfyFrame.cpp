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
#include <vector>
#include <ostream>
#include <iostream>
#include <iomanip>

#include "SomfyFrame.h"

using namespace rts;

namespace
{
	const std::vector<uint8_t> TEST_FRAME = {
		0xa8, 0xef, 0xe8, 0x5b, 0x68, 0x01, 0x44
	};

	const uint8_t TEST_FRAME_KEY = 0xa8;
	const SomfyFrame::Action TEST_FRAME_CTRL = SomfyFrame::Action::down;
	const uint16_t TEST_FRAME_ROLLING_CODE = 0x07b3;
	const uint32_t TEST_FRAME_ADDRESS = 0x336945;
}

namespace rts
{
	std::ostream & operator<< (std::ostream & os, SomfyFrame::Action action)
	{
		typedef SomfyFrame::Action Action;
		switch (action)
		{
		case Action::my:
			os << "my";
			break;
		case Action::up:
			os << "up";
			break;
		case Action::my_up:
			os << "my_up";
			break;
		case Action::down:
			os << "down";
			break;
		case Action::my_down:
			os << "my_down";
			break;
		case Action::up_down:
			os << "up_down";
			break;
		case Action::prog:
			os << "prog";
			break;
		case Action::sun_flag:
			os << "sun_flag";
			break;
		case Action::flag:
			os << "flag";
			break;
		default:
			os << "UNKNOWN[" << static_cast<uint16_t>(action) << "]";
			break;
		}
		return os;
	}
}

BOOST_AUTO_TEST_CASE(TestSomfyFrame_decode)
{
	const SomfyFrame frame = SomfyFrame::fromBytes(TEST_FRAME);

	BOOST_TEST(frame.getKey() == TEST_FRAME_KEY);
	BOOST_TEST(frame.getCtrl() == TEST_FRAME_CTRL);
	BOOST_TEST(frame.getRollingCode() == TEST_FRAME_ROLLING_CODE);
	BOOST_TEST(frame.getAddress() = TEST_FRAME_ADDRESS);
}

BOOST_AUTO_TEST_CASE(TestSomfyFrame_encode)
{
	SomfyFrame frame;

	frame.setKey(TEST_FRAME_KEY);
	frame.setCtrl(TEST_FRAME_CTRL);
	frame.setRollingCode(TEST_FRAME_ROLLING_CODE);
	frame.setAddress(TEST_FRAME_ADDRESS);

	const std::vector<uint8_t> data = frame.getBytes();

	BOOST_TEST(data == TEST_FRAME);
}
