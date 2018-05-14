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
#ifndef RTS_FAST_GPIO_H
#define RTS_FAST_GPIO_H

#include <cstdint>
#include <cstddef>
#include <iostream>

namespace rts
{

class FastGPIO
{
private:
	static constexpr size_t GPIO_READ_OFFSET = 13; // in multiples of sizeof(*m_gpioMem) = sizeof(uint32_t)
	static constexpr size_t GPIO_SET_OFFSET = 7;
	static constexpr size_t GPIO_CLR_OFFSET = 10;

public:
	FastGPIO();
	~FastGPIO();

	// https://elinux.org/RPi_GPIO_Code_Samples
	bool read(unsigned n) const
	{
		const uint32_t gpioStates = *(m_gpioMem + GPIO_READ_OFFSET);
		const uint32_t mask = UINT32_C(1) << n;
		return (gpioStates & mask) != 0;
	}

	void write(unsigned n, bool value)
	{
		if (value)
			*(m_gpioMem + GPIO_SET_OFFSET) = 1 << n;
		else
			*(m_gpioMem + GPIO_CLR_OFFSET) = 1 << n;
	}

private:
	volatile uint32_t *m_gpioMem;
};

} // namespace rts

#endif // RTS_FAST_GPIO_H
