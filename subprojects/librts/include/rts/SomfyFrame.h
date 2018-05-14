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
#ifndef RTS_SOMFY_FRAME_H
#define RTS_SOMFY_FRAME_H

#include <cstdint>
#include <climits>
#include <vector>
#include <stdexcept>

namespace rts
{

class WrongFrameChecksumException: public std::runtime_error
{
public:
	using std::runtime_error::runtime_error;
};

class SomfyFrame
{
public:
	enum class Action: uint8_t
	{
		my = 0x1,
		up = 0x2,
		my_up = 0x3,
		down = 0x4,
		my_down = 0x5,
		up_down = 0x6,
		prog = 0x8,
		sun_flag = 0x9,
		flag = 0xa
	};

	uint8_t getKey() const
	{
		return m_key;
	}

	void setKey(uint8_t key)
	{
		m_key = key;
	}

	Action getCtrl() const
	{
		return m_ctrl;
	}

	void setCtrl(Action ctrl)
	{
		m_ctrl= ctrl;
	}

	uint16_t getRollingCode() const
	{
		return m_rollingCode;
	}

	void setRollingCode(uint16_t rollingCode)
	{
		m_rollingCode = rollingCode;
	}

	uint32_t getAddress() const
	{
		return m_address;
	}

	void setAddress(uint32_t address)
	{
		m_address = address;
	}

	static constexpr size_t FRAME_SIZE = 7; // Somfy RTS frame size in bytes

	SomfyFrame();
	SomfyFrame(uint8_t key, Action ctrl, uint16_t rollingCode, uint32_t address);

	static SomfyFrame fromBytes(std::vector<uint8_t> bytes);
	std::vector<uint8_t> getBytes() const;

private:
	static void deobfuscate(std::vector<uint8_t> & bytes);
	static void obfuscate(std::vector<uint8_t> & bytes);
	static uint8_t checksum(const std::vector<uint8_t> & data);

	uint8_t m_key;
	Action m_ctrl;
	uint16_t m_rollingCode;
	uint32_t m_address;
};

} // namespace rts

#endif // RTS_SOMFY_FRAME_H
