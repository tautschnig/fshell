/* -*- Mode: C++; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 noexpandtab: */

/*******************************************************************************
 * FShell 2
 * Copyright 2009 Michael Tautschnig, tautschnig@forsyte.de
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *******************************************************************************/

/*! \file fshell2/util/statistics.cpp
 * \brief TODO
 *
 * $Id$
 * \author Michael Tautschnig <tautschnig@forsyte.de>
 * \date   Mon Aug  9 18:23:47 CEST 2010 
*/

#include <fshell2/util/statistics.hpp>
#include <fshell2/config/annotations.hpp>

#if FSHELL2_DEBUG__LEVEL__ > -1
#  include <diagnostics/basic_exceptions/invalid_protocol.hpp>
#endif

#include <sstream>

#include <util/message.h>
#include <minisat-2.2.0/utils/System.h>

FSHELL2_NAMESPACE_BEGIN;
FSHELL2_STATISTICS_NAMESPACE_BEGIN;

Statistics_Interface::Statistics_Interface(::std::string const& name) :
	m_name(name)
{
}

Statistics_Interface::~Statistics_Interface()
{
}

void Statistics_Interface::print(::std::ostream & os) const
{
	os << m_name << ": ";
}
	
Statistics::Statistics()
{
}

Statistics::~Statistics()
{
	for (::std::vector< Statistics_Interface * >::iterator iter(m_stats.begin());
			iter != m_stats.end(); ++iter)
		delete *iter;
}

void Statistics::print(::messaget & mh) const
{
	if (m_stats.empty()) return;

	::std::ostringstream oss;
	for (::std::vector< Statistics_Interface * >::const_iterator iter(m_stats.begin());
			iter != m_stats.end();) {
		(*iter)->print(oss);
		++iter;
		if (iter != m_stats.end()) oss << ::std::endl;
	}
	mh.print(0, oss.str());
}

CPU_Timer::CPU_Timer(::std::string const& name) :
	Statistics_Interface(name),
	m_timer_running(false),
	m_time(0)
{
}
	
CPU_Timer::~CPU_Timer()
{
}

void CPU_Timer::reset()
{
	m_timer_running = false;
	m_time = 0;
}
	
void CPU_Timer::print(::std::ostream & os) const
{
	FSHELL2_DEBUG_ASSERT(::diagnostics::Invalid_Protocol, !m_timer_running);
	Statistics_Interface::print(os);
	os << m_time;
}

void CPU_Timer::start_timer()
{
	FSHELL2_DEBUG_ASSERT(::diagnostics::Invalid_Protocol, !m_timer_running);
	m_timer_running = true;
	m_time -= ::Minisat::cpuTime();
}

void CPU_Timer::stop_timer()
{
	FSHELL2_DEBUG_ASSERT(::diagnostics::Invalid_Protocol, m_timer_running);
	m_timer_running = false;
	m_time += ::Minisat::cpuTime();
}

Wallclock_Timer::Wallclock_Timer(::std::string const& name) :
	Statistics_Interface(name)
{
}

Wallclock_Timer::~Wallclock_Timer()
{
}

void Wallclock_Timer::reset()
{
	m_timer.clear();
}

void Wallclock_Timer::print(::std::ostream & os) const
{
	Statistics_Interface::print(os);
	os << m_timer;
}

void Wallclock_Timer::start_timer()
{
	m_timer.start();
}

void Wallclock_Timer::stop_timer()
{
	m_timer.stop();
}

Peak_Memory_Usage::Peak_Memory_Usage(::std::string const& name) :
	Statistics_Interface(name),
	m_peak_memory_usage(0)
{
}
	
Peak_Memory_Usage::~Peak_Memory_Usage()
{
}

void Peak_Memory_Usage::reset()
{
	m_peak_memory_usage = 0;
}
	
void Peak_Memory_Usage::print(::std::ostream & os) const
{
	Statistics_Interface::print(os);
	os << m_peak_memory_usage;
}

void Peak_Memory_Usage::current()
{
	m_peak_memory_usage = ::Minisat::memUsedPeak();
}

FSHELL2_STATISTICS_NAMESPACE_END;
FSHELL2_NAMESPACE_END;

