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
#ifndef DURATION_TRACKER_H
#define DURATION_TRACKER_H

#include <optional>

#include "Clock.h"
#include "Transition.h"
#include "Duration.h"

template<typename Source>
class DurationTracker
{
public:
	DurationTracker(Source & source):
		m_source(source)
	{}

	std::optional<Duration> get()
	{
		if (!m_lastTransition)
		{
			m_lastTransition = m_source.get();

			if (!m_lastTransition)
				return std::nullopt; // no data
		}

		const std::optional<Transition> transition = m_source.get();
		if (!transition || transition->second == m_lastTransition->second /*a glitch*/)
			return std::nullopt;

		const Duration d(transition->first - m_lastTransition->first, m_lastTransition->second);
		m_lastTransition = transition;
		return d;
	}

private:
	Source & m_source;
	std::optional<Transition> m_lastTransition;
};

#endif // DURATION_TRACKER_H
