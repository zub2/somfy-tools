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
#include "backend/rpi-gpio/FastGPIO.h"

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>

#include <cerrno>
#include <system_error>
#include <stdexcept>
#include <fstream>

#include <boost/format.hpp>

namespace rts
{

namespace
{
	/*
	 * https://www.raspberrypi.org/app/uploads/2012/02/BCM2835-ARM-Peripherals.pdf
	 * 1.2 Address map
	 *
	 * The IO peripherals live on bus addresses 0x7Ennnnnn. For the RPi 1 this corresponds
	 * to physical addresses 0x20nnnnnn. On the RPi 2 and 3 the corresponding physical
	 * addresses are 0x3Fnnnnnn. See https://www.raspberrypi.org/forums/viewtopic.php?t=142439
	 *
	 * The physical address of the start of the IO region (i.e. 0x20000000 on RPi 1 or 0x3F000000
	 * on RPi 2 and 3) can be read from /proc/device-tree/soc/ranges - see:
	 * * https://raspberrypi.stackexchange.com/questions/27268/do-the-gpio-registers-have-the-same-addresses-on-the-new-raspberry-pi-2
	 * * https://github.com/raspberrypi/userland/commit/3b81b91c18ff19f97033e146a9f3262ca631f0e9
	 * (implemented by bcm_host_get_peripheral_address())
	 */

	/**
	 * @brief Return the starting physical address of the I/O peripherals area.
	 *
	 * This is known to be 0x20000000 on RPi1 or 0x3F000000 on RPi2 and 3. But the value
	 * can also be read from the device tree config and that feels more generic (i.e.
	 * if it's ever changed in some other revision, this code should work so long as
	 * the DT config is updated (and compatible)).
	 *
	 * @return Physical address of the start of the I/O peripherals area.
	 */
	uintptr_t getPhysIOMemBase()
	{
		// the start address is a 32bit BE unsigned integer at offset 4 in /proc/device-tree/soc/ranges
		constexpr size_t IO_MEM_BASE_OFFSET = 4;
		const char * DT_SOC_RANGES = "/proc/device-tree/soc/ranges";

		std::ifstream f(DT_SOC_RANGES);
		if (!f.is_open())
			throw std::runtime_error(std::string("Can't open ") + DT_SOC_RANGES);

		f.seekg(IO_MEM_BASE_OFFSET);
		uint8_t bytes[4];
		if (!f.good() || f.read(reinterpret_cast<char*>(bytes), sizeof(bytes)).bad())
			throw std::runtime_error(std::string("Can't read ") + DT_SOC_RANGES);

		// the value is stored as big endian
		return static_cast<uintptr_t>(bytes[0]) << 24 |	bytes[1] << 16 | bytes[2] << 8 | bytes[3];
	}

	/**
	 * @brief Convert a bus addess inside the I/O peripherals area to a physical address.
	 *
	 * The conversion only works for addresses inside the I/O peripherals area.
	 *
	 * @param ioBusAddr The address to convert. Must lie in the I/O peripherals area.
	 * @param physIOMemBase Physical address of the start of the I/O peripherals area.
	 * @return The resulting physical address.
	 */
	constexpr uintptr_t ioBusToPhys(uintptr_t ioBusAddr, uintptr_t physIOMemBase)
	{
		constexpr uintptr_t IO_BUS_ADDR_BASE = 0x7E000000;
		return ioBusAddr - IO_BUS_ADDR_BASE + physIOMemBase;
	}

	// https://www.raspberrypi.org/app/uploads/2012/02/BCM2835-ARM-Peripherals.pdf
	// 6.1 Register View
	constexpr uintptr_t IO_GPIO_BASE_BUS_ADDR = 0x7E200000;

	constexpr size_t MAP_LENGTH = 4*1024;
}

FastGPIO::FastGPIO()
{
	// see man 4 mem
	int memFd = open("/dev/mem", O_RDWR|O_SYNC);
	if (memFd < 0)
	{
		throw std::system_error(errno, std::generic_category(), "can't open /dev/mem");
	}

	const uintptr_t gpioBase = ioBusToPhys(IO_GPIO_BASE_BUS_ADDR, getPhysIOMemBase());

	// from man mmap: offset must be a multiple of the page size as returned by sysconf(_SC_PAGE_SIZE)
	// in reality gpioBase will always be correctly aligned...
	const long pageSize = sysconf(_SC_PAGESIZE);
	if (gpioBase % pageSize != 0)
	{
		// ... so this can never happen, so don't bother implementing code that would handle it
		throw std::runtime_error((boost::format("The impossible has happened: GPIO base address "
			"(0x%1$x) is not divisible by page size (0x%2$x)") % gpioBase % pageSize).str());
	}

	m_gpioMem = static_cast<volatile uint32_t*>(mmap(nullptr, MAP_LENGTH, PROT_READ|PROT_WRITE, MAP_SHARED, memFd, gpioBase));
	int errCode = errno;
	close(memFd);

	if (m_gpioMem == MAP_FAILED)
	{
		throw std::system_error(errCode, std::generic_category(), "can't mmap /dev/mem");
	}
}

FastGPIO::~FastGPIO()
{
	munmap(const_cast<uint32_t*>(m_gpioMem), MAP_LENGTH);
}

} // namespace rts
