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
#include "backend/rpi-gpio/PlaybackThread.h"
#include "../../ThreadPrio.h"

#include <sched.h>

PlaybackThread::PlaybackThread(unsigned gpioNr):
	m_gpioNr(gpioNr),
	m_running(false),
	m_playbackFinished(false)
{}

void PlaybackThread::start()
{
	std::lock_guard<std::mutex> g(m_mutex);

	if (m_running)
		throw std::runtime_error("playback thread already started");

	m_running = true;
	m_thread = std::thread(&PlaybackThread::playbackLoop, this);
	setThreadSchedulerAndPrio(m_thread, SCHED_FIFO);
}

void PlaybackThread::stop()
{
	bool join = false;

	{
		std::lock_guard<std::mutex> g(m_mutex);
		if (m_running)
		{
			m_running = false;
			m_playbackRequestCondVar.notify_all();
			join = true;
		}
	}

	if (join)
		m_thread.join();
}

void PlaybackThread::play(const std::vector<Duration> & samples)
{
	std::unique_lock<std::mutex> g(m_mutex);

	if (!m_running)
		throw std::runtime_error("PlaybackThread not running!");

	m_playbackBuffer = samples;
	m_playbackFinished = false;
	m_playbackRequestCondVar.notify_one();

	while (!m_playbackFinished)
	{
		m_playbackFinishedCondVar.wait(g);
	}
}

void PlaybackThread::playbackLoop()
{
	while (true)
	{
		std::unique_lock<std::mutex> g(m_mutex);
		while (m_playbackBuffer.empty() && m_running)
			m_playbackRequestCondVar.wait(g);

		if (!m_running)
			return;

		// play back...
		for (size_t i = 0; i < m_playbackBuffer.size(); i++)
		{
			const Duration & s = m_playbackBuffer[i];

			m_gpioWriter.write(m_gpioNr, s.second);
			std::this_thread::sleep_for(s.first);
		}

		m_playbackBuffer.clear();
		m_playbackFinished = true;
		m_playbackFinishedCondVar.notify_all();
	}
}
