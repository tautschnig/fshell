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

/*! \file fshell2/util/statistics.t.cpp
 * \brief TODO
 *
 * $Id$
 * \author Michael Tautschnig <tautschnig@forsyte.de>
 * \date   Tue Aug 10 10:56:14 CEST 2010 
*/


#include <diagnostics/unittest.hpp>
#include <fshell2/config/config.hpp>
#include <fshell2/config/annotations.hpp>

#include <fshell2/util/statistics.hpp>

#include <cstdlib>
#include <unistd.h>

#include <cbmc/src/util/message.h>

#define TEST_COMPONENT_NAME Statistics
#define TEST_COMPONENT_NAMESPACE fshell2::statistics

FSHELL2_NAMESPACE_BEGIN;
FSHELL2_STATISTICS_NAMESPACE_BEGIN;

/** @cond */
TEST_NAMESPACE_BEGIN;
TEST_COMPONENT_TEST_NAMESPACE_BEGIN;
/** @endcond */
using namespace ::diagnostics::unittest;

#define get_stats_long(stat) \
	::std::ostringstream stat ## oss; \
	stat.print(stat ## oss); \
	TEST_CHECK(::std::string::npos != stat ## oss.str().rfind(':')); \
	long const stat ## _val(::strtol(stat ## oss.str().c_str() + stat ## oss.str().rfind(':') + 1, 0, 10)); \
	stat ## oss.str("")

#define get_stats_double(stat) \
	::std::ostringstream stat ## oss; \
	stat.print(stat ## oss); \
	TEST_CHECK(::std::string::npos != stat ## oss.str().rfind(':')); \
	double const stat ## _val(::strtod(stat ## oss.str().c_str() + stat ## oss.str().rfind(':') + 1, 0)); \
	stat ## oss.str("")

////////////////////////////////////////////////////////////////////////////////
/**
 * @test A test of Statistics
 *
 */
void test( Test_Data & data )
{
	Statistics stats;
	NEW_STAT(stats, Wallclock_Timer, wt, "Wallclock");
	wt.start_timer();
	NEW_STAT(stats, CPU_Timer, ct, "CPU Time");
	ct.start_timer();
	NEW_STAT(stats, Counter<int>, cnt_int, "int counter");
	NEW_STAT(stats, Counter<size_t>, cnt_size_t, "size_t counter");
	cnt_int.inc();
	cnt_int.inc();
	cnt_int.inc(-5);
	cnt_size_t.inc(7);
	::usleep(1500);
	ct.stop_timer();
	wt.stop_timer();

	{
		get_stats_long(cnt_int);
		TEST_ASSERT(-3 == cnt_int_val);
		get_stats_long(cnt_size_t);
		TEST_ASSERT(7 == cnt_size_t_val);
		get_stats_double(ct);
		get_stats_double(wt);
		TEST_ASSERT(ct_val < wt_val);
	}
	
	ct.reset();
	wt.reset();

	::std::ostringstream oss;
	::stream_message_handlert mh(oss);
	::messaget msg(mh);
	stats.print(msg);
	TEST_ASSERT(data.compare("stats_output1", oss.str()));

	cnt_int.reset();
	cnt_size_t.reset();

	{
		get_stats_long(cnt_int);
		TEST_ASSERT(0 == cnt_int_val);
		get_stats_long(cnt_size_t);
		TEST_ASSERT(0 == cnt_size_t_val);
	}
}

/** @cond */
TEST_COMPONENT_TEST_NAMESPACE_END;
TEST_NAMESPACE_END;
/** @endcond */

FSHELL2_STATISTICS_NAMESPACE_END;
FSHELL2_NAMESPACE_END;

TEST_SUITE_BEGIN;
TEST_NORMAL_CASE( &test, LEVEL_PROD );
TEST_SUITE_END;

STREAM_TEST_SYSTEM_MAIN;
