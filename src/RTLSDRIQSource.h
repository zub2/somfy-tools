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
#ifndef RTLSDR_IQ_SOURCE
#define RTLSDR_IQ_SOURCE

#include <cstdint>
#include <thread>
#include <condition_variable>
#include <optional>
#include <complex>
#include <vector>
#include <memory>
#include <string>

#include <fstream>

#include <boost/circular_buffer.hpp>

#include "RTLSDRDevice.h"
#include "RTLSDRBufferReader.h"

class RTLSDRIQSource
{
public:
	RTLSDRIQSource(RTLSDRDevice & rtlSdrDevice, size_t bufferSize, size_t bufferCount,
		const std::string & logName);

	void start();
	void stop();

	std::optional<std::complex<float>> getNextSample();

private:
	typedef std::vector<uint8_t> TBuffer;

	void recordingLoop();
	void readBuffer(TBuffer & buffer);

	RTLSDRDevice & m_rtlSDRDevice;

	/***
	 * Mutex protecting m_recordedBuffers, m_freeBuffers and m_running.
	 * Locking order must be: 1) m_mutex, 2) m_recordingMutex.
	 */
	std::mutex m_recordingMutex;
	std::condition_variable m_freeBuffersCondVar;
	std::vector<std::unique_ptr<TBuffer>> m_freeBuffers;
	std::condition_variable m_recordedBuffersCondVar;
	boost::circular_buffer<std::unique_ptr<TBuffer>> m_recordedBuffers;
	bool m_running;

	/***
	 * Mutex protecting all members except for m_recordedBuffers, m_freeBuffers and m_running.
	 * Locking order must be: 1) m_mutex, 2) m_recordingMutex.
	 */
	std::mutex m_mutex;

	std::unique_ptr<TBuffer> m_currentBuffer;

	RTLSDRBufferReader m_bufferReader;
	size_t m_readOffset;

	std::thread m_thread;

	std::ofstream m_log;
};

#endif // RTLSDR_IQ_SOURCE
