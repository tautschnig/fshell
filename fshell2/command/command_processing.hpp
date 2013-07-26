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

#ifndef FSHELL2__COMMAND__COMMAND_PROCESSING_HPP
#define FSHELL2__COMMAND__COMMAND_PROCESSING_HPP

/*! \file fshell2/command/command_processing.hpp
 * \brief TODO
 *
 * $Id$
 * \author Michael Tautschnig <tautschnig@forsyte.de>
 * \date   Thu Apr  2 17:29:44 CEST 2009 
*/

#include <fshell2/config/config.hpp>

#include <iostream>
#include <map>
#include <ctime>
#include <list>

#include <util/options.h>
#include <goto-programs/goto_functions.h>

class language_uit;
class optionst;

#define F2_O_LIMIT "fshell::limit"
#define F2_O_MULTIPLE_COVERAGE "fshell::multiple_coverage"

#define STANDARD_CBMC_MODE "StandardCBMCmode"
#define FUNC_TO_COVER "FuncToCover"
#define TC_CONDITIONS_FILENAME "TestCaseConditionsFileName"
#define TC_TRACE_FILENAME "TestCaseTraceFileName"
#define TEST_SUITE_FOLDER "TestSuiteFolder"
#define INTERNAL_COVERAGE_CHECK "internal-coverage-check"
#define SAT_COVERAGE_CHECK "sat-coverage-check"
#define ORIG_INTERNAL_COVERAGE_CHECK "original-internal-coverage-check"
#define ORIG_SAT_COVERAGE_CHECK "original-sat-coverage-check"
#define AUTOGENERATE_TESTSUITE "autogenerate-testsuite"
#define MAX_PATHS "max-paths"
#define PATH_DEPTH "path-depth"

#define CONDITIONS_FILE "conditions.log"
#define COVERAGE_FILE "coverage.log"

FSHELL2_NAMESPACE_BEGIN;
FSHELL2_COMMAND_NAMESPACE_BEGIN;

/*! \brief TODO
*/
class Command_Processing
{
	/*! \copydoc doc_self
	*/
	typedef Command_Processing Self;

	public:
	typedef enum {
		FAIL,
		CMD_QUIT,
		CMD_HELP,
		CMD_ADD_SOURCECODE,
		CMD_SHOW_FILENAMES,
		CMD_SHOW_SOURCECODE,
		CMD_SHOW_SOURCECODE_ALL,
		CMD_SET_ENTRY,
		CMD_SET_LIMIT_COUNT,
		CMD_SET_NO_ZERO_INIT,
		CMD_SET_ABSTRACT,
		CMD_SET_MULTIPLE_COVERAGE,
		/*###### BEGIN frEDIT ####*/
		CMD_SET_FUNC_TO_COVER,
		CMD_SET_TC_CONDITIONS_FILENAME,
		CMD_SET_TC_TRACE_FILENAME,
		CMD_SET_STANDARD_CBMC_MODE,
		CMD_SET_PATHWALK_CBMC_MODE,
		CMD_SET_TESTSUITE_FOLDER,
		CMD_RUN,
		CMD_SET_TESTCASE,
		CMD_SET_AUTOGENERATE,
		CMD_SET_MAX_PATHS,
		CMD_SET_PATH_DEPTH
		/*###### END frEDIT ####*/
	} parsed_command_t;
	
	typedef enum {
		NO_CONTROL_COMMAND = FAIL,
		QUIT = CMD_QUIT,
		HELP = CMD_HELP,
		DONE
	} status_t;

	Command_Processing(::optionst & opts, ::goto_functionst & gf);

	status_t process(::language_uit & manager, ::std::ostream & os, char const * cmd);

	static ::std::ostream & help(::std::ostream & os);

	bool finalize(::language_uit & manager);
	void finalize_goto_program(::language_uit & manager);
	
	int run_testcase(::std::string tc_name, ::std::string tc_parameter);
	int set_to_next_testcase(void);

	int check_if_tc_name_exists(::std::string tc_name);

	private:
	::optionst & m_opts;
	::goto_functionst & m_gf;
	::std::map< ::std::string, time_t > m_parse_time;
	bool m_finalized;
	bool m_remove_zero_init;
	
	::std::vector< ::std::string> open_tc_names;

	int update_testcase_list(void);

	::std::ostream & print_file_contents(::std::ostream & os, char const * name) const;

	void add_sourcecode(::language_uit & manager, char const * file,
			::std::list< ::std::pair< char*, char* > > const& defines);
	void remove_zero_init(::language_uit & manager) const;
	void model_argv(::language_uit & manager) const;

	void integrateInXML(::std::string tcFilename);
	int integrateTestCaseNameInXML(::std::string tcName, std::string paramStr);
	/*! \copydoc copy_constructor
	*/
	Command_Processing( Self const& rhs );

	/*! \copydoc assignment_op
	 */
	Self& operator=( Self const& rhs );
};

::std::ostream & operator<<(::std::ostream & os, Command_Processing::status_t const& s);

FSHELL2_COMMAND_NAMESPACE_END;
FSHELL2_NAMESPACE_END;

#endif /* FSHELL2__COMMAND__COMMAND_PROCESSING_HPP */
