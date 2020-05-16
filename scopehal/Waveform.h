/***********************************************************************************************************************
*                                                                                                                      *
* ANTIKERNEL v0.1                                                                                                      *
*                                                                                                                      *
* Copyright (c) 2012-2020 Andrew D. Zonenberg                                                                          *
* All rights reserved.                                                                                                 *
*                                                                                                                      *
* Redistribution and use in source and binary forms, with or without modification, are permitted provided that the     *
* following conditions are met:                                                                                        *
*                                                                                                                      *
*    * Redistributions of source code must retain the above copyright notice, this list of conditions, and the         *
*      following disclaimer.                                                                                           *
*                                                                                                                      *
*    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the       *
*      following disclaimer in the documentation and/or other materials provided with the distribution.                *
*                                                                                                                      *
*    * Neither the name of the author nor the names of any contributors may be used to endorse or promote products     *
*      derived from this software without specific prior written permission.                                           *
*                                                                                                                      *
* THIS SOFTWARE IS PROVIDED BY THE AUTHORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED   *
* TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL *
* THE AUTHORS BE HELD LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES        *
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR       *
* BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT *
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE       *
* POSSIBILITY OF SUCH DAMAGE.                                                                                          *
*                                                                                                                      *
***********************************************************************************************************************/

/**
	@file
	@author Andrew D. Zonenberg
	@brief Declaration of Waveform
 */

#ifndef Waveform_h
#define Waveform_h

#include <vector>

/**
	@brief Wrapper around a primitive data type that has an empty default constructor.

	Can be seamlessly casted to that type. This allows STL data structures to be created with explicitly uninitialized
	members via resize() and avoids a nasty memset that wastes a lot of time.
*/
template<class T>
class EmptyConstructorWrapper
{
public:
	EmptyConstructorWrapper()
	{}

	EmptyConstructorWrapper(const T& rhs)
	: m_value(rhs)
	{}

	operator T&()
	{ return m_value; }

	T& operator=(const T& rhs)
	{
		m_value = rhs;
		return *this;
	}

	T m_value;
};

/**
	@brief Base class for all Waveform specializations

	One waveform contains a time-series of sample objects as well as scale information etc. The samples may
	or may not be at regular intervals depending on whether the Oscilloscope uses RLE compression.

	The WaveformBase contains all metadata, but the actual samples are stored in a derived class member.
 */
class WaveformBase
{
public:
	WaveformBase()
	{
		m_triggerPhase = 0;
		m_startTimestamp = 0;
		m_startPicoseconds = 0;
	}

	//empty virtual destructor in case any derived classes need one
	virtual ~WaveformBase()
	{}

	/**
		@brief The time scale, in picoseconds per timestep, used by this channel.

		This is used as a scaling factor for individual sample time values as well as to compute the maximum zoom value
		for the time axis.
	 */
	int64_t m_timescale;

	/**
		@brief Start time of the acquisition, rounded to nearest second
	 */
	time_t	m_startTimestamp;

	/**
		@brief Fractional start time of the acquisition (picoseconds since m_startTimestamp)
	 */
	int64_t m_startPicoseconds;

	/**
		@brief Phase offset, in picoseconds, from the trigger to the sampling clock.
	 */
	double m_triggerPhase;

	///@brief Start timestamps of each sample
	std::vector<EmptyConstructorWrapper<int64_t>> m_offsets;

	///@brief Durations of each sample
	std::vector<EmptyConstructorWrapper<int64_t>> m_durations;

	virtual void clear()
	{
		m_offsets.clear();
		m_durations.clear();
	}

	virtual void Resize(size_t size)
	{
		m_offsets.resize(size);
		m_durations.resize(size);
	}
};

/**
	@brief A waveform that contains actual data
 */
template<class S>
class Waveform : public WaveformBase
{
public:

	///@brief Sample data
	std::vector<S> m_samples;

	virtual void Resize(size_t size)
	{
		m_offsets.resize(size);
		m_durations.resize(size);
		m_samples.resize(size);
	}

	virtual void clear()
	{
		m_offsets.clear();
		m_durations.clear();
		m_samples.clear();
	}
};

//we need this to avoid problems with the bitfield packing used by vector<bool>
typedef Waveform<EmptyConstructorWrapper<bool> >	DigitalWaveform;

typedef Waveform< std::vector<bool> > 	DigitalBusWaveform;
typedef Waveform<float>					AnalogWaveform;
typedef Waveform<char>					AsciiWaveform;

#endif
