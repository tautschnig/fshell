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

/*! \file fshell2/main/fshell2.cpp
 * \brief TODO
 *
 * $Id$
 * \author Michael Tautschnig <tautschnig@forsyte.de>
 * \date   Wed Apr 15 11:58:46 CEST 2009 
*/

#include <fshell2/main/fshell2.hpp>
#include <fshell2/config/annotations.hpp>

#include <memory>
#include <cstdlib>
#include <cerrno>

#include <readline/readline.h>
#include <readline/history.h>

#include <fshell2/command/command_processing.hpp>
#include <fshell2/exception/command_processing_error.hpp>
#include <fshell2/macro/macro_processing.hpp>
#include <fshell2/exception/macro_processing_error.hpp>
#include <fshell2/fql/parser/query_processing.hpp>
#include <fshell2/exception/query_processing_error.hpp>

namespace std {
template<>
auto_ptr<char>::~auto_ptr()
{
	::free(this->release());
}
} 

FSHELL2_NAMESPACE_BEGIN;

FShell2 & FShell2::get_instance() {
	static FShell2 instance;
	return instance;
}

FShell2::FShell2() {
	// try to read history from file, ignore errors
	::read_history(".fshell2_history"); errno = 0;
}

FShell2::~FShell2() {
	// write the history and truncate it to 200 lines
	::write_history(".fshell2_history"); 
	// not yet available on OS X, thus left out for now
	// ::history_truncate_lines(".fshell2_history", 200);
}

bool FShell2::process_line(::language_uit & manager, ::std::ostream & os, char const * line) {
	using ::fshell2::command::Command_Processing;
	using ::fshell2::macro::Macro_Processing;
	using ::fshell2::fql::Query_Processing;

	switch (Command_Processing::get_instance().process(manager, os, line)) {
		case Command_Processing::QUIT:
			return true;
		case Command_Processing::HELP:
			Command_Processing::help(os);
			os << ::std::endl;
			Macro_Processing::help(os);
			os << ::std::endl;
			Query_Processing::help(os);
			return false;
		case Command_Processing::DONE:
			return false;
		case Command_Processing::NO_CONTROL_COMMAND:
			{
				::std::string query(Macro_Processing::get_instance().expand(line));
				if (!query.empty()) {
					Query_Processing::get_instance().parse(os, query.c_str());
				}
			}
			//incomplete
			return false;
	}

	return true;
}

void FShell2::interactive(::language_uit & manager, ::std::ostream & os) {
	while (true) {
		// obtain the input from readline
		::std::auto_ptr<char> input(::readline("FShell2> "));
		::add_history(input.get());
		try {
			// process the input; returning true signals "quit"
			if (process_line(manager, os, input.get())) {
				os << "Bye." << ::std::endl;
				return;
			}
		} catch (::fshell2::Command_Processing_Error & e) {
			throw;
		} catch (::fshell2::Macro_Processing_Error & e) {
			throw;
		} catch (::fshell2::Query_Processing_Error & e) {
			throw;
		}
	}
}

FSHELL2_NAMESPACE_END;

