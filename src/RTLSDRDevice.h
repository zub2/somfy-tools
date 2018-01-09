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
#ifndef RTLSDR_DEVICE_H
#define RTLSDR_DEVICE_H

#include <cstddef>
#include <cstdint>
#include <vector>

#include <rtl-sdr.h>

class RTLSDRDevice
{
public:
	RTLSDRDevice(uint32_t deviceIndex);
	~RTLSDRDevice();

	void setSampleRate(uint32_t sampleRate);
	void setFrequency(uint32_t frequency);
	void setFrequencyCorrection(int ppm);

	void setManualTunnerGainMode(bool enableManualGain);
	void setTunerGain(int gain);

	std::vector<int> getTunnerGains();

	size_t readSync(uint8_t * buffer, size_t size);

private:
	rtlsdr_dev_t * m_device;
	bool m_reading;
};

#endif // RTLSDR_DEVICE_H
