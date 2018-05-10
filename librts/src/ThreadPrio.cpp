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
#include "ThreadPrio.h"

#include <system_error>
#include <cerrno>

#include <pthread.h>
#include <sched.h>

void setThreadSchedulerAndPrio(std::thread & thread, int schedAlgo)
{
	const pthread_t nativeHandle = thread.native_handle();

	const int maxPrio = sched_get_priority_max(schedAlgo);
	if (maxPrio == -1)
		throw std::system_error(errno, std::generic_category(), "sched_get_priority_max failed");

	const sched_param schedParam =
	{
		maxPrio
	};
	const int r = pthread_setschedparam(nativeHandle, schedAlgo, &schedParam);
	if (r != 0)
		throw std::system_error(errno, std::generic_category(), "pthread_setschedparam failed");
}
