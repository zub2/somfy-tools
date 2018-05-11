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
#ifndef RECORDING_THREAD_H
#define RECORDING_THREAD_H

#include "FastGPIO.h"
#include "../../Clock.h"
#include "../../Transition.h"

#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <vector>
#include <chrono>
#include <optional>

#include <boost/circular_buffer.hpp>

namespace rts
{

class RecordingThread
{
public:
	RecordingThread(unsigned gpioNr, size_t bufferSize, const Clock::duration & samplePeriod);

	void start();
	void stop();

	std::optional<Transition> get();

private:
	void recordingLoop();
	void put(const Clock::time_point & tp, bool value);
	Transition * advanceBufferPtr(Transition *ptr);

	const FastGPIO m_gpioReader;
	const unsigned m_gpioNr;
	const Clock::duration m_samplePeriod;

	// temporary buffer used solely by the reading thread
	boost::circular_buffer<Transition> m_readBuffer;

	// buffer shared between the reading and recording threads
	std::vector<Transition> m_buffer;
	std::atomic<Transition*> m_writePtr;
	std::atomic<Transition*> m_readPtr;

	std::mutex m_mutex;
	std::thread m_thread;

	bool m_running;
	std::atomic<bool> m_stop;
	std::condition_variable m_runningCondVar;
};

} // namespace rts

#endif // RECORDING_THREAD_H
