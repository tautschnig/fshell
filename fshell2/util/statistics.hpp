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

#ifndef FSHELL2__UTIL__STATISTICS_HPP
#define FSHELL2__UTIL__STATISTICS_HPP

/*! \file fshell2/util/statistics.hpp
 * \brief TODO
 *
 * $Id$
 * \author Michael Tautschnig <tautschnig@forsyte.de>
 * \date   Mon Aug  9 18:23:44 CEST 2010 
*/

#include <fshell2/config/config.hpp>

#include <string>
#include <vector>

#include <util/timer.h>

class messaget;

FSHELL2_NAMESPACE_BEGIN;
FSHELL2_STATISTICS_NAMESPACE_BEGIN;

/*! \brief TODO
*/
class Statistics_Interface
{
	/*! \copydoc doc_self
	*/
	typedef Statistics_Interface Self;

	public:
	Statistics_Interface(::std::string const& name);

	virtual void reset() = 0;
	virtual void print(::std::ostream & os) const = 0;

	virtual ~Statistics_Interface();

	private:
	::std::string const m_name;

	/*! \copydoc copy_constructor
	*/
	Statistics_Interface( Self const& rhs );

	/*! \copydoc assignment_op
	 */
	Self& operator=( Self const& rhs );
};

/*! \brief TODO
*/
class Statistics
{
	/*! \copydoc doc_self
	*/
	typedef Statistics Self;

	public:
	Statistics();

	~Statistics();

	template <typename T>
		T & new_stat(::std::string const& name);
	void print(::messaget & mh) const;

	private:
	::std::vector< Statistics_Interface * > m_stats;

	/*! \copydoc copy_constructor
	*/
	Statistics( Self const& rhs );

	/*! \copydoc assignment_op
	 */
	Self& operator=( Self const& rhs );
};
	
template <typename T>
T & Statistics::new_stat(::std::string const& name)
{
	T * s(new T(name));
	m_stats.push_back(s);
	return *s;
}

#define NEW_STAT(coll, STAT, var, name) \
	::fshell2::statistics:: STAT & var(coll.new_stat< ::fshell2::statistics:: STAT >(name))

/*! \brief TODO
*/
template <typename T>
class Counter : public Statistics_Interface
{
	/*! \copydoc doc_self
	*/
	typedef Counter<T> Self;

	public:
	virtual void reset();
	virtual void print(::std::ostream & os) const;

	void inc();
	void inc(T const val);

	private:
	T m_value;
	
	template <typename TT> friend TT & Statistics::new_stat(::std::string const&);
	friend Statistics::~Statistics();
	
	Counter(::std::string const& name);
	virtual ~Counter();

	/*! \copydoc copy_constructor
	*/
	Counter( Self const& rhs );

	/*! \copydoc assignment_op
	 */
	Self& operator=( Self const& rhs );
};

template <typename T>
Counter<T>::Counter(::std::string const& name) :
	Statistics_Interface(name),
	m_value(0)
{
}

template <typename T>
Counter<T>::~Counter()
{
}

template <typename T>
void Counter<T>::reset()
{
	m_value = 0;
}

template <typename T>
void Counter<T>::print(::std::ostream & os) const
{
	Statistics_Interface::print(os);
	os << m_value;
}

template <typename T>
void Counter<T>::inc()
{
	inc(1);
}

template <typename T>
void Counter<T>::inc(T const val)
{
	m_value += val;
}

/*! \brief TODO
*/
class CPU_Timer : public Statistics_Interface
{
	/*! \copydoc doc_self
	*/
	typedef CPU_Timer Self;

	public:
	virtual void reset();
	virtual void print(::std::ostream & os) const;

	void start_timer();
	void stop_timer();

	private:
	bool m_timer_running;
	double m_time;
	
	friend Self & Statistics::new_stat<Self>(::std::string const&);
	friend Statistics::~Statistics();

	CPU_Timer(::std::string const& name);
	virtual ~CPU_Timer();

	/*! \copydoc copy_constructor
	*/
	CPU_Timer( Self const& rhs );

	/*! \copydoc assignment_op
	 */
	Self& operator=( Self const& rhs );
};

/*! \brief TODO
*/
class Wallclock_Timer : public Statistics_Interface
{
	/*! \copydoc doc_self
	*/
	typedef Wallclock_Timer Self;

	public:
	virtual void reset();
	virtual void print(::std::ostream & os) const;

	void start_timer();
	void stop_timer();

	private:
	::timert m_timer;
	
	friend Self & Statistics::new_stat<Self>(::std::string const&);
	friend Statistics::~Statistics();
	
	Wallclock_Timer(::std::string const& name);
	virtual ~Wallclock_Timer();

	/*! \copydoc copy_constructor
	*/
	Wallclock_Timer( Self const& rhs );

	/*! \copydoc assignment_op
	 */
	Self& operator=( Self const& rhs );
};

/*! \brief TODO
*/
class Peak_Memory_Usage : public Statistics_Interface
{
	/*! \copydoc doc_self
	*/
	typedef Peak_Memory_Usage Self;

	public:
	virtual void reset();
	virtual void print(::std::ostream & os) const;

	void current();

	private:
	double m_peak_memory_usage;
	
	friend Self & Statistics::new_stat<Self>(::std::string const&);
	friend Statistics::~Statistics();

	Peak_Memory_Usage(::std::string const& name);
	virtual ~Peak_Memory_Usage();

	/*! \copydoc copy_constructor
	*/
	Peak_Memory_Usage( Self const& rhs );

	/*! \copydoc assignment_op
	 */
	Self& operator=( Self const& rhs );
};

FSHELL2_STATISTICS_NAMESPACE_END;
FSHELL2_NAMESPACE_END;

#endif /* FSHELL2__UTIL__STATISTICS_HPP */
