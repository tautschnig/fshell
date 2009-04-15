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

#include <cbmc/src/langapi/language_ui.h>

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
		NO_CONTROL_COMMAND,
		BLANK,
		CMD_PROCESSED,
		CMD_QUIT,
		CMD_HELP,
		CMD_ADD_SOURCECODE,
		CMD_SHOW_FILENAMES,
		CMD_SHOW_SOURCECODE,
		CMD_SHOW_SOURCECODE_ALL,
		CMD_START
	} status_t;

	static Command_Processing & get_instance();

	status_t process(::language_uit & manager, ::std::ostream & os, char const * cmd);

	static ::std::ostream & help(::std::ostream & os);

	private:
	::std::map< ::std::string, time_t > m_parse_time;

	Command_Processing();
	
	::std::ostream & print_file_contents(::std::ostream & os, char const * name) const;

	/*! \copydoc copy_constructor
	*/
	Command_Processing( Self const& rhs );

	/*! \copydoc assignment_op
	 */
	Self& operator=( Self const& rhs );
};

FSHELL2_COMMAND_NAMESPACE_END;
FSHELL2_NAMESPACE_END;

#endif /* FSHELL2__COMMAND__COMMAND_PROCESSING_HPP */
