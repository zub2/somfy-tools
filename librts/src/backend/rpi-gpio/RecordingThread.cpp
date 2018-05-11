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
#include "backend/rpi-gpio/RecordingThread.h"
#include "../../ThreadPrio.h"

#include <sched.h>

#include <algorithm>
#include <iterator>

namespace rts
{

using namespace std::literals;

static_assert(std::atomic<Transition*>::is_always_lock_free,
	"A lock-free atomic pointer required for good performance.");

namespace
{
	constexpr std::chrono::steady_clock::duration GET_RETRY_TIME = 100ms;
}

RecordingThread::RecordingThread(unsigned gpioNr, size_t bufferSize, const Clock::duration & samplePeriod):
	m_gpioNr(gpioNr),
	m_samplePeriod(samplePeriod),
	// +1 to account for the fact that there's always at least 1 free element in between to differentiate empty and full
	m_buffer(bufferSize + 1),
	m_readBuffer(bufferSize),
	m_running(false),
	m_stop(false)
{
	if (bufferSize == 0)
		throw std::runtime_error("bufferSize must be positive");

	m_writePtr = m_readPtr = &m_buffer.front();
}

void RecordingThread::start()
{
	std::lock_guard<std::mutex> g(m_mutex);

	if (m_running)
		throw std::runtime_error("recording thread already started");

	m_stop.store(false, std::memory_order_relaxed);
	m_thread = std::thread(&RecordingThread::recordingLoop, this);

	setThreadSchedulerAndPrio(m_thread, SCHED_FIFO);

	m_running = true;
	m_runningCondVar.notify_all();
}

void RecordingThread::stop()
{
	std::lock_guard<std::mutex> g(m_mutex);

	if (m_running)
	{
		m_stop.store(true, std::memory_order_relaxed);
		m_thread.join();

		m_running = false;
		m_runningCondVar.notify_all();
	}
}

std::optional<Transition> RecordingThread::get()
{
	std::unique_lock<std::mutex> g(m_mutex);

	if (m_readBuffer.empty())
	{
		Transition * readPtr = m_readPtr.load(std::memory_order_acquire);
		Transition * writePtr = m_writePtr.load(std::memory_order_relaxed);
		while (readPtr == writePtr && m_running)
		{
			const auto waitUntil = std::chrono::steady_clock::now() + GET_RETRY_TIME;
			while (m_running)
			{
				if (m_runningCondVar.wait_until(g, waitUntil) == std::cv_status::timeout)
					break;
			}

			readPtr = m_readPtr.load(std::memory_order_acquire);
			writePtr = m_writePtr.load(std::memory_order_relaxed);
		}

		if (readPtr == writePtr)
			return std::nullopt;

		// read as much data as available
		// it's guaranteed to fit because m_readBuffer.empty() && m_readBuffer.capacity() == m_buffer.size() - 1
		// (but it's always guaranteed that there's at least 1 unused element in m_buffer)
		if (readPtr < writePtr)
			std::copy(readPtr, writePtr, std::back_inserter(m_readBuffer));
		else
		{
			std::copy(readPtr, &m_buffer.back() + 1, std::back_inserter(m_readBuffer));
			std::copy(&m_buffer.front(), writePtr, std::back_inserter(m_readBuffer));
		}

		// mark all as read
		m_readPtr.store(writePtr, std::memory_order_release);
	}

	// return previously received data
	const Transition transition = m_readBuffer.front();
	m_readBuffer.pop_front();
	return transition;
}

void RecordingThread::recordingLoop()
{
	// wait for m_started
	{
		std::unique_lock<std::mutex> g(m_mutex);
		while (!m_running)
			m_runningCondVar.wait(g);
	}

	// recording loop
	Clock::time_point t = Clock::now();
	bool state = m_gpioReader.read(m_gpioNr);
	put(t, state);

	while (!m_stop.load(std::memory_order_relaxed))
	{
		Clock::time_point now = Clock::now();
		bool newState = m_gpioReader.read(m_gpioNr);
		if (state != newState)
		{
			state = newState;
			t = now;
			put(t, state);
		}
		std::this_thread::sleep_until(now + m_samplePeriod);
	}
}

void RecordingThread::put(const Clock::time_point & tp, bool value)
{
	const Transition * const readPtr = m_readPtr.load(std::memory_order_relaxed);
	Transition * writePtr = m_writePtr.load(std::memory_order_acquire);
	Transition * advancedWritePtr = advanceBufferPtr(writePtr);
	if (advancedWritePtr != readPtr)
	{
		*writePtr = std::make_pair(tp, value);
		m_writePtr.store(advancedWritePtr, std::memory_order_release);
	}
	// else: drop - no space in buffer
}

Transition * RecordingThread::advanceBufferPtr(Transition *ptr)
{
	ptr++;
	if (ptr == &m_buffer.back() + 1)
		ptr = &m_buffer.front();

	return ptr;
}

} // namespace rts
