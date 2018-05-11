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
#ifndef PLAYBACK_THREAD_H
#define PLAYBACK_THREAD_H

#include "Clock.h"
#include "Duration.h"
#include "backend/rpi-gpio/FastGPIO.h"

#include <utility>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <vector>
#include <chrono>

namespace rts
{

class PlaybackThread
{
public:
	PlaybackThread(unsigned gpioNr);

	void start();
	void stop();

	void play(const std::vector<Duration> & samples);

private:
	void playbackLoop();

	FastGPIO m_gpioWriter;
	const unsigned m_gpioNr;

	std::mutex m_mutex;
	std::thread m_thread;
	bool m_running;
	bool m_playbackFinished;
	std::condition_variable m_playbackFinishedCondVar;

	std::vector<Duration> m_playbackBuffer;
	std::condition_variable m_playbackRequestCondVar;
};

} // namespace rts

#endif // PLAYBACK_THREAD_H
