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

/*! \file fshell2/main/main.cpp
 * \brief TODO
 *
 * $Id$
 * \author Michael Tautschnig <tautschnig@forsyte.de>
 * \date   Mon Aug  9 12:21:03 CEST 2010
*/


#include <fshell2/config/config.hpp>

/* diagnostics related stuff */
#if FSHELL2_DEBUG__LEVEL__ >= 2
#  include <diagnostics/configuration.hpp>
#  include <diagnostics/logger/file_logger.hpp>
// #  include <diagnostics/logger/stream_logger.hpp>
// #  include <diagnostics/logger/intending_file_logger.hpp>
#  include <diagnostics/frame/type.hpp>

DIAGNOSTICS_NAMESPACE_BEGIN;
void set_initial_loggers(::std::vector<Logger *> & loggers)
{
	loggers.push_back(new File_Logger("test/main.log"));
	// loggers.push_back(new Stream_Logger(::std::cout));
}

DIAGNOSTICS_NAMESPACE_END;

#else
#  if SHELL2_DEBUG__LEVEL__ > -1
#    include <diagnostics/configuration.hpp>
#    include <vector>

DIAGNOSTICS_NAMESPACE_BEGIN;

class Logger;

void set_initial_loggers(::std::vector<Logger *> & loggers)
{
}

DIAGNOSTICS_NAMESPACE_END;
#  endif
#endif
/* end diagnostics related stuff */

#include <fshell2/main/parseoptions.hpp>

int main(int argc, const char* argv[])
{
	::fshell2::Parseoptions fshell(argc, argv);
	int const ret(fshell.main());

#if WIN32 && _DEBUG
	system("PAUSE");        //some additional comfort for the vs users
#endif

	return ret;
}


