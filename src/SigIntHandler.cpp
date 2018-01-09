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
#include "SigIntHandler.h"

#include <system_error>
#include <cerrno>

#include <signal.h>
#include <string.h>
#include <unistd.h>

#include <condition_variable>

namespace
{
	std::mutex intMutex;
	std::condition_variable intCondVar;
	bool gotInt = false;

	extern "C" void sigIntHandler(int)
	{
		std::lock_guard<std::mutex> g(intMutex);
		gotInt = true;
		intCondVar.notify_all();
	}
}

void installSigIntHandler()
{
	struct sigaction action;
	memset(&action, 0, sizeof(action));
	action.sa_handler = sigIntHandler;

	if (sigaction(SIGINT, &action, nullptr) != 0)
		throw std::system_error(errno, std::generic_category(), "can't install SIGINT handler");
}

bool gotSigInt()
{
	std::lock_guard<std::mutex> g(intMutex);
	return gotInt;
}

void waitForSigInt()
{
	std::unique_lock<std::mutex> g(intMutex);
	while (!gotInt)
		intCondVar.wait(g);
}
