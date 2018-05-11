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
#ifndef OOK_DECODER_H
#define OOK_DECODER_H

#include <optional>
#include <cstddef>

#include "../../Clock.h"
#include "../../Transition.h"
#include "Filter.h"

namespace rts
{

// convert from IQ signal to transitions
template<typename IQSource>
class OOKDecoder
{
public:
	OOKDecoder(IQSource & iqSource, size_t sampleRate):
		m_iqSource(iqSource),
		m_samplePeriod(1.0f / static_cast<float>(sampleRate)),
		m_numSamples(0),
		/*
		 * Butterworth low-pass filter with cut-off frequency 100 kHz (assuming sample rate 2.6 MHz)
		 * calculated by Octave as:
		 * Fcutoff = 1e5; % 100 kHz... this is also used by rtl_433
		 * fs = 2.6e6; % sample rate this tool uses
		 * [B, A] = butter(1, Fcutoff / (fs/2));
		 */
		m_butterworthLowPass(/*A*/ {1.0f, -0.78345f}, /*B*/ {0.10828, 0.10828})
	{}

	std::optional<Transition> get()
	{
		// minDuration = 50e-6;
		// minSamples = minDuration*fs; % fs = 2.6e6
		constexpr size_t minSamples = 130;
		size_t count = 0;
		bool startValue;
		size_t startIndex;

		// find a sequence of at least minSamples samples with the same value
		std::optional<bool> b = getNextSample();
		while (b)
		{
			if (count > 0)
			{
				if (*b == startValue)
				{
					count++;
					if (count == minSamples)
					{
						// hooray!
						m_lastValue = startValue;
						return makeTransition(startIndex, startValue);
					}
					// else: continue search
				}
				else
				{
					// early transition - reset
					count = 1;
					startValue = *b;
				}
			}
			else
			{
				if (*b != m_lastValue)
				{
					count = 1;
					startValue = *b;
					startIndex = m_numSamples - 1;
				}
			}

			b = getNextSample();
		}

		return std::nullopt;
	}

private:
	std::optional<bool> getNextSample()
	{
		std::optional<std::complex<float>> s = m_iqSource.getNextSample();
		if (!s)
		{
			// end of data
			return std::nullopt;
		}

		// keep track of samples count and thus of time elapsed
		m_numSamples++;

		// filter
		const float f = m_butterworthLowPass << std::abs(*s);

		// the range of the signal is 0 .. sqrt(2) (abs(1+i)), place
		// the threshold in the middle
		constexpr float threshold = /*sqrt(2)*/ 1.4142135623730950488f / 2;

		return f >= threshold;
	}

	Transition makeTransition(size_t sampleIndex, bool newValue) const
	{
		const Clock::time_point tp = Clock::time_point() +
			std::chrono::duration_cast<Clock::duration>(std::chrono::duration<float>(sampleIndex * m_samplePeriod));
		return Transition(tp, newValue);
	}

	IQSource & m_iqSource;
	const float m_samplePeriod;
	size_t m_numSamples;
	Filter m_butterworthLowPass;
	std::optional<bool> m_lastValue;
};

} // namespace rts

#endif // OOK_DECODER_H
