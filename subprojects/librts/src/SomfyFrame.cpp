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
#include "SomfyFrame.h"

namespace rts
{

// this is based on https://pushstack.wordpress.com/somfy-rts-protocol/
SomfyFrame::SomfyFrame():
	SomfyFrame(0, Action::my, 0, 0)
{}

SomfyFrame::SomfyFrame(uint8_t key, Action ctrl, uint16_t rollingCode, uint32_t address):
	m_key(key),
	m_ctrl(ctrl),
	m_rollingCode(rollingCode),
	m_address(address)
{
	// address is encoded in 3 bytes, so it can only hold values 0x000000 .. 0xffffff
	if (address > UINT32_C(0xffffff))
		throw std::runtime_error("Address must be lower than 0x1000000.");
}

SomfyFrame SomfyFrame::fromBytes(std::vector<uint8_t> bytes)
{
	if (bytes.size() != FRAME_SIZE)
		throw std::runtime_error("invalid frame size");

	deobfuscate(bytes);
	const uint8_t key = bytes[0];
	const Action ctrl = static_cast<Action>(bytes[1] >> 4);
	const uint16_t rollingCode = (static_cast<uint16_t>(bytes[2]) << 8) | bytes[3];
	const uint32_t address = (static_cast<uint32_t>(bytes[4]) << 16) |
		(static_cast<uint32_t>(bytes[5]) << 8) | bytes[6];

	const uint8_t c = checksum(bytes);
	if (c != 0)
		throw WrongFrameChecksumException("invalid checksum!");

	return SomfyFrame(key, ctrl, rollingCode, address);
}

std::vector<uint8_t> SomfyFrame::getBytes() const
{
	std::vector<uint8_t> frame;
	frame.resize(FRAME_SIZE);

	frame[0] = m_key;
	frame[1] = static_cast<uint8_t>(m_ctrl) << 4;
	frame[2] = static_cast<uint8_t>(m_rollingCode >> CHAR_BIT);
	frame[3] = static_cast<uint8_t>(m_rollingCode);
	frame[4] = static_cast<uint8_t>(m_address >> 2*CHAR_BIT);
	frame[5] = static_cast<uint8_t>(m_address >> 1*CHAR_BIT);
	frame[6] = static_cast<uint8_t>(m_address >> 0*CHAR_BIT);

	frame[1] |= checksum(frame);
	obfuscate(frame);
	return frame;
}

void SomfyFrame::deobfuscate(std::vector<uint8_t> & bytes)
{
	for (size_t i = bytes.size() - 1; i > 0; i--)
		bytes[i] ^= bytes[i-1];
}

void SomfyFrame::obfuscate(std::vector<uint8_t> & bytes)
{
	for (size_t i = 1; i < bytes.size(); i++)
		bytes[i] ^= bytes[i-1];
}

uint8_t SomfyFrame::checksum(const std::vector<uint8_t> & data)
{
	uint8_t c = 0;
	for (uint8_t d : data)
		c = c ^ d ^ (d >> 4);

	return c & 0xf;
}

} // namespace rts
