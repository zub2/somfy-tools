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
#include "backend/rtlsdr/RTLSDRDevice.h"

#include <stdexcept>
#include <string>

#include <vector>
#include <iostream>

namespace rts
{

RTLSDRDevice::RTLSDRDevice(uint32_t deviceIndex):
	m_device(nullptr),
	m_reading(false)
{
	if (rtlsdr_open(&m_device, deviceIndex) < 0)
		throw std::runtime_error(std::string("Can't open rdl_sdr device #") + std::to_string(deviceIndex));
}

RTLSDRDevice::~RTLSDRDevice()
{
	rtlsdr_close(m_device);
}

void RTLSDRDevice::setSampleRate(uint32_t sampleRate)
{
	if (rtlsdr_set_sample_rate(m_device, sampleRate) < 0)
		throw std::runtime_error(std::string("can't set sample rate to ") + std::to_string(sampleRate));
}

void RTLSDRDevice::setFrequency(uint32_t frequency)
{
	if (rtlsdr_set_center_freq(m_device, frequency))
		throw std::runtime_error(std::string("can't set frequency to ") + std::to_string(frequency));
}

void RTLSDRDevice::setFrequencyCorrection(int ppm)
{
	if (rtlsdr_set_freq_correction(m_device, ppm) < 0)
		throw std::runtime_error(std::string("can't set frequency correction to ") + std::to_string(ppm));
}

void RTLSDRDevice::setManualTunnerGainMode(bool enableManualGain)
{
	rtlsdr_set_tuner_gain_mode(m_device, static_cast<int>(enableManualGain));
}

std::vector<int> RTLSDRDevice::getTunnerGains()
{
	const int numGains = rtlsdr_get_tuner_gains(m_device, nullptr);
	std::vector<int> gains(numGains);
	rtlsdr_get_tuner_gains(m_device, gains.data());

	return gains;
}

void RTLSDRDevice::setTunerGain(int gain)
{
	rtlsdr_set_tuner_gain(m_device, gain);
}

size_t RTLSDRDevice::readSync(uint8_t * buffer, size_t size)
{
	if (!m_reading)
	{
		rtlsdr_reset_buffer(m_device);
		m_reading = true;
	}

	int wasRead = 0;
	if (rtlsdr_read_sync(m_device, buffer, size, &wasRead) < 0)
		throw std::runtime_error("rtlsdr_read_syn failed");

	return wasRead;
}

} // namespace rts
