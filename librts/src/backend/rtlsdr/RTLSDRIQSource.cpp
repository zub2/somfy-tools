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
#include "backend/rtlsdr/RTLSDRIQSource.h"

#include <iostream>
#include <assert.h>

namespace rts
{

RTLSDRIQSource::RTLSDRIQSource(RTLSDRDevice & rtlSdrDevice, size_t bufferSize, size_t bufferCount,
		const std::string & logName):
	m_rtlSDRDevice(rtlSdrDevice),
	m_freeBuffers(bufferCount),
	m_recordedBuffers(bufferCount),
	m_running(false),
	m_readOffset(0)
{
	for (size_t i = 0; i < m_freeBuffers.size(); i++)
		m_freeBuffers[i].reset(new TBuffer(bufferSize));

	if (!logName.empty())
	{
		m_log.open(logName);
		if (!m_log.is_open())
			throw std::runtime_error(std::string("can't open log file '") + logName + "'");

		std::cout << "Writing raw IQ log into '" << logName << "'."  << std::endl;
	}
}

void RTLSDRIQSource::start()
{
	std::lock_guard<std::mutex> g(m_mutex);

	if (m_running)
		throw std::runtime_error("recording thread already started");

	m_running = true;
	m_thread = std::thread(&RTLSDRIQSource::recordingLoop, this);
}

void RTLSDRIQSource::stop()
{
	std::lock_guard<std::mutex> g(m_mutex);
	bool join = false;

	{
		std::lock_guard<std::mutex> gRecording(m_recordingMutex);
		if (m_running)
		{
			m_running = false;
			m_freeBuffersCondVar.notify_all();
			join = true;
		}
	}

	if (join)
		m_thread.join();
}

std::optional<std::complex<float>> RTLSDRIQSource::getNextSample()
{
	std::lock_guard<std::mutex> g(m_mutex);

	if (!m_currentBuffer)
	{
		{
			std::unique_lock<std::mutex> gRecording(m_recordingMutex);
			while (m_recordedBuffers.empty() && m_running)
				m_recordedBuffersCondVar.wait(gRecording);

			if (m_recordedBuffers.empty())
				return std::nullopt; // stopped and there is no more data

			m_currentBuffer = std::move(m_recordedBuffers.front());
			assert(m_currentBuffer);
			assert(!m_currentBuffer->empty());

			m_recordedBuffers.pop_front();
		}

		m_bufferReader.reset(m_currentBuffer->data(), m_currentBuffer->size());
		m_readOffset = 0;

		if (m_log.is_open())
			m_log.write(reinterpret_cast<const char*>(m_currentBuffer->data()), sizeof(m_currentBuffer->at(0)) * m_currentBuffer->size());
	}

	const std::optional<std::complex<float>> sample = m_bufferReader.at(m_readOffset++);
	if (m_readOffset == m_bufferReader.size())
	{
		m_bufferReader.reset();

		std::lock_guard<std::mutex> gRecording(m_recordingMutex);
		m_freeBuffers.push_back(std::move(m_currentBuffer));
		m_freeBuffersCondVar.notify_one();
	}

	return sample;
}

void RTLSDRIQSource::recordingLoop()
{
	while (true)
	{
		std::unique_ptr<TBuffer> buffer;
		{
			// wait for a free buffer to become available
			std::unique_lock<std::mutex> gRecording(m_recordingMutex);
			while (m_freeBuffers.empty() && m_running)
				m_freeBuffersCondVar.wait(gRecording);

			if (!m_running)
				break;

			buffer = std::move(m_freeBuffers.back());
			m_freeBuffers.pop_back();
		}

		assert(buffer != nullptr);
		readBuffer(*buffer);

		{
			std::unique_lock<std::mutex> gRecording(m_recordingMutex);

			// don't place empty buffers in recorded buffers
			if (!buffer->empty())
			{
				m_recordedBuffers.push_back(std::move(buffer));
				m_recordedBuffersCondVar.notify_one();
			}
			else
			{
				// got no data... place buffer back in free buffers vector
				m_freeBuffers.push_back(std::move(buffer));
			}
		}
	}
}

void RTLSDRIQSource::readBuffer(TBuffer & buffer)
{
	buffer.resize(buffer.capacity());
	size_t wasRead = m_rtlSDRDevice.readSync(buffer.data(), buffer.size());
	if (wasRead != buffer.size())
		std::cout << "Lost data!" << std::endl;

	buffer.resize(wasRead);
}

} // namespace rts
