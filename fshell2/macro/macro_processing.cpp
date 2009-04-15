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

/*! \file fshell2/macro/macro_processing.cpp
 * \brief TODO
 *
 * $Id$
 * \author Michael Tautschnig <tautschnig@forsyte.de>
 * \date   Thu Apr  2 17:29:10 CEST 2009 
*/

#include <fshell2/macro/macro_processing.hpp>
#include <fshell2/config/annotations.hpp>

#include <diagnostics/basic_exceptions/violated_invariance.hpp>

#include <fstream>
#include <cstdio>
#include <sys/wait.h>
#include <unistd.h>
#include <vector>

#include <fshell2/exception/macro_processing_error.hpp>

FSHELL2_NAMESPACE_BEGIN;
FSHELL2_MACRO_NAMESPACE_BEGIN;
	
Macro_Processing::Macro_Processing() :
	m_has_defines(false),
	m_deffilename(::strdup("/tmp/defsXXXXXX.c")),
	m_checkfilename(::strdup("/tmp/defsXXXXXX.c")),
	m_file_index(0),
	m_cpp_argv(0)
{
	int ret(0);
	ret = ::mkstemps(m_deffilename, 2);
	FSHELL2_PROD_ASSERT1(::fshell2::Macro_Processing_Error, ret != -1,
			"Failed to create macro expansion file");
	FSHELL2_AUDIT_TRACE("Defines are written to " + *m_deffilename);
	::std::ofstream fs(m_deffilename);
	FSHELL2_PROD_ASSERT1(::fshell2::Macro_Processing_Error, fs.is_open(),
			"Failed to open macro expansion file");
	fs << "// FShell macro list" << ::std::endl;
	fs.close();
	
	ret = ::mkstemps(m_checkfilename, 2);
	FSHELL2_PROD_ASSERT1(::fshell2::Macro_Processing_Error, ret != -1,
			"Failed to create macro syntax check file");

	::std::vector< ::std::string > cpp_cmdline;
	cpp_cmdline.push_back(::std::string());
	char const * cpp_cmd(CPP_CMD);
	char const * iter(cpp_cmd);
	while (0 != *iter) {
		if (::std::isspace(*iter)) {
			++iter;
			cpp_cmdline.push_back(::std::string());
			continue;
		}
		cpp_cmdline.back().push_back(*iter);
		++iter;
	}
	cpp_cmdline.push_back("-dP");
	cpp_cmdline.push_back("-undef");

	m_cpp_argv = new char * [cpp_cmdline.size() + 2];
	// abusing m_file_index
	for (::std::vector< ::std::string >::const_iterator iter(cpp_cmdline.begin());
			iter != cpp_cmdline.end(); ++iter, ++m_file_index) {
		m_cpp_argv[m_file_index] = ::strdup(iter->c_str());
	}
	// the real file index
	m_cpp_argv[m_file_index] = 0;
	m_cpp_argv[m_file_index + 1] = 0;
}

Macro_Processing::~Macro_Processing() {
	::unlink(m_deffilename);
	::free(m_deffilename);
	
	::unlink(m_checkfilename);
	::free(m_checkfilename);

	while (m_file_index > 0) {
		--m_file_index;
		::free(m_cpp_argv[m_file_index]);
	}
	delete[] m_cpp_argv;
}

Macro_Processing & Macro_Processing::get_instance() {
	static Macro_Processing instance;
	return instance;
}
	
::std::ostream & Macro_Processing::help(::std::ostream & os) {
	os << "Macro definitions:" << ::std::endl;
	os << "<Macro> ::= `#define' bla bla bla TODO" << ::std::endl;
	return os;
}

int Macro_Processing::preprocess(char * filename, ::std::ostream & out) const {
	int channel[2];
	int ret(::pipe(channel));
	FSHELL2_PROD_ASSERT1(::diagnostics::Violated_Invariance, 0 == ret,
			"Failed to open pipe");
	pid_t child(::fork());
	FSHELL2_PROD_ASSERT1(::diagnostics::Violated_Invariance, child != -1,
			"Failed to fork"); 

	if(0 == child)
	{
		/* child */
		ret = ::close(1);
		FSHELL2_PROD_ASSERT1(::diagnostics::Violated_Invariance, 0 == ret,
				"Failed to close stdout");
		ret = ::dup(channel[1]);
		FSHELL2_PROD_ASSERT1(::diagnostics::Violated_Invariance, 0 == ret,
				"Failed to duplicate new stdout");
		ret = ::close(channel[0]);
		FSHELL2_PROD_ASSERT1(::diagnostics::Violated_Invariance, 0 == ret,
				"Failed to close parent side stdin");
		ret = ::close(channel[1]);
		FSHELL2_PROD_ASSERT1(::diagnostics::Violated_Invariance, 0 == ret,
				"Failed to close parent side stdout");

		FSHELL2_AUDIT_TRACE("starting " CPP_CMD);
		m_cpp_argv[m_file_index] = filename;
		::execvp(m_cpp_argv[0], m_cpp_argv);
		FSHELL2_PROD_ASSERT1(::diagnostics::Violated_Invariance, false,
				"Failed to start " CPP_CMD);
		// never reached
		return 1;
	}
	else
	{
		/* parent */
		ret = ::close(0);
		FSHELL2_PROD_ASSERT1(::diagnostics::Violated_Invariance, 0 == ret,
				"Failed to close stdin");
		ret = ::dup(channel[0]);
		FSHELL2_PROD_ASSERT1(::diagnostics::Violated_Invariance, 0 == ret,
				"Failed to duplicate new stdin");
		ret = ::close(channel[0]);
		FSHELL2_PROD_ASSERT1(::diagnostics::Violated_Invariance, 0 == ret,
				"Failed to close child side stdin");
		ret = ::close(channel[1]);
		FSHELL2_PROD_ASSERT1(::diagnostics::Violated_Invariance, 0 == ret,
				"Failed to close child side stdout");

		::std::string line;
		::std::cin.clear();
		FSHELL2_AUDIT_TRACE("reading input");
		while(!::std::getline(::std::cin, line).eof())
		{
			FSHELL2_AUDIT_TRACE("read " + line);
			if (line.empty() || *(line.begin()) == '#') continue;
			out << line << ::std::endl;
		}
		
		int status(-1);
		child = ::waitpid(child, &status, WNOHANG);
		FSHELL2_PROD_ASSERT1(::diagnostics::Violated_Invariance, -1 != child,
				"Failed to wait for child");

		return WIFEXITED(status)?WEXITSTATUS(status):1;
	}
}
	

::std::string Macro_Processing::expand(char const * cmd) {
	while (0 != *cmd && ::std::isspace(*cmd)) ++cmd;
	if (::strlen(cmd) > 7 && 0 == ::strncmp("#define", cmd, 7) && ::std::isspace(*(cmd + 7))) {
		// syntax check
		char ch;
		::std::ofstream fcheck(m_checkfilename, ::std::ios_base::trunc);
		::std::ifstream in(m_deffilename);
		while (!in.eof() && in.get(ch)) fcheck.put(ch);
		in.close();
		fcheck << cmd << ::std::endl;
		fcheck.close();
		::std::ostringstream os;
		int cpp_ret(preprocess(m_checkfilename, os));
		FSHELL2_PROD_CHECK1(::fshell2::Macro_Processing_Error, 0 == cpp_ret && os.str().empty(),
				"Failed to process macro definition");

		::std::ofstream fdef(m_deffilename, ::std::ios_base::out|::std::ios_base::app);
		fdef << cmd << ::std::endl;
		fdef.close();

		m_has_defines = true;
		return "";
	}
	
	if (!m_has_defines) return cmd;
		
	::std::ofstream fexpand(m_checkfilename, ::std::ios_base::trunc);
	fexpand << "#include \"" << m_deffilename << "\"" << ::std::endl;
	fexpand << cmd << ::std::endl;
	fexpand.close();
		
	::std::ostringstream os;
	int cpp_ret(preprocess(m_checkfilename, os));
	FSHELL2_PROD_CHECK1(::fshell2::Macro_Processing_Error, 0 == cpp_ret,
			"Failed to expand command");
	return os.str();
}

FSHELL2_MACRO_NAMESPACE_END;
FSHELL2_NAMESPACE_END;

