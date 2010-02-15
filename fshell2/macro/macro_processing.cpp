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

#include <cstring>
#include <fstream>
#include <cstdio>
#include <sys/wait.h>
#include <unistd.h>
#include <vector>
#include <cstdlib>

#include <fshell2/exception/macro_processing_error.hpp>

#include <cbmc/src/util/tempfile.h>

FSHELL2_NAMESPACE_BEGIN;
FSHELL2_MACRO_NAMESPACE_BEGIN;
	
Macro_Processing::Macro_Processing() :
	m_has_defines(false),
	m_deffilename(::get_temporary_file("tmp.defs", ".c")),
	m_checkfilename(::get_temporary_file("tmp.check", ".c")),
#if defined(_WIN32) && !defined(__MINGW32__)
	m_cpp_cmdline("CL /nologo /E")
#else
	m_cpp_cmdline(CPP_CMD " -dP -undef -Werror")
#endif
{
	FSHELL2_AUDIT_TRACE("Defines are written to " + m_deffilename);
	::std::ofstream fs(m_deffilename.c_str());
	FSHELL2_PROD_ASSERT1(::diagnostics::Violated_Invariance, fs.is_open(),
			"Failed to open macro expansion file");
	fs << "// FShell macro list" << ::std::endl;
	fs.close();
	
	::std::ofstream fs2(m_checkfilename.c_str());
	FSHELL2_PROD_ASSERT1(::diagnostics::Violated_Invariance, fs2.is_open(),
			"Failed to open macro expansion file");
	fs2 << "// FShell macro test list" << ::std::endl;
	fs2.close();

	m_cpp_cmdline += " " + m_checkfilename;
}

Macro_Processing::~Macro_Processing() {
	::unlink(m_deffilename.c_str());
	::unlink(m_checkfilename.c_str());
}

::std::ostream & Macro_Processing::help(::std::ostream & os) {
	os << "Macro definitions:" << ::std::endl;
	os << "<Macro> ::= `#define' <Macro Decl> <Some Def or Empty>" << ::std::endl;
	os << "<Macro Decl> ::= <Identifier>" << ::std::endl;
	os << "               | <Identifier> `(' <Macro Args> `)'" << ::std::endl;
	os << "<Macro Args> ::= <Identifier>"  << ::std::endl;
	os << "               | <Macro Args> `,' <Identifier>" << ::std::endl;
	return os;
}

int Macro_Processing::preprocess(::std::ostream & os) const {
	::std::string const stderr_file(::get_temporary_file("tmp.stderr", ".fs"));
	::std::string cmd(m_cpp_cmdline);
	cmd += " 2> " + stderr_file;
	
	int status(-1);
	FILE * out(0);
	char line[1024];

#ifdef _WIN32
	::std::string const stdout_file(::get_temporary_file("tmp.stdout", ".fs"));
	cmd += " > " + stdout_file;
	
	status = ::system(cmd.c_str());
	out = ::fopen(stdout_file.c_str(), "r");
#else
	out = ::popen(cmd.c_str(), "r");
#endif
	
	FSHELL2_PROD_ASSERT1(::diagnostics::Violated_Invariance, 0 != out,
			"Failed to open stdout reader");
	::std::ostringstream out_stream;
	while (0 != ::fgets(line, 1024, out)) out_stream << line;

#ifdef _WIN32
	::unlink(stdout_file.c_str());
#else
	status = ::pclose(out);
#endif
	
	FILE * err(::fopen(stderr_file.c_str(), "r"));
	FSHELL2_PROD_ASSERT1(::diagnostics::Violated_Invariance, 0 != err,
			"Failed to open stderr reader");
	::std::ostringstream err_stream;
	while (0 != ::fgets(line, 1024, err)) err_stream << line;
	::unlink(stderr_file.c_str());
	
	if (!err_stream.str().empty()) {
		::std::string::size_type err_start(err_stream.str().find("error:"));
		os << (err_start == ::std::string::npos?err_stream.str():err_stream.str().substr(err_start));
		
		return 1;
	}
	
	::std::istringstream is(out_stream.str());
	::std::string line_str;
	FSHELL2_AUDIT_TRACE("reading input");
	while(!::std::getline(is, line_str).eof())
	{
		FSHELL2_AUDIT_TRACE("read " + line_str);
		if (line_str.empty() || *(line_str.begin()) == '#') continue;
		os << line_str << ::std::endl;
	}

	return status;
}
	

::std::string Macro_Processing::expand(char const * cmd) {
	while (0 != *cmd && ::std::isspace(*cmd)) ++cmd;
	if ('#' == *cmd) {
		if (::strlen(cmd) > 7 &&
				'd' == ::std::tolower(*(cmd + 1)) &&
				'e' == ::std::tolower(*(cmd + 2)) &&
				'f' == ::std::tolower(*(cmd + 3)) &&
				'i' == ::std::tolower(*(cmd + 4)) &&
				'n' == ::std::tolower(*(cmd + 5)) &&
				'e' == ::std::tolower(*(cmd + 6)) &&
				::std::isspace(*(cmd + 7))) {
			// syntax check
			char ch;
			::std::ofstream fcheck(m_checkfilename.c_str(), ::std::ios_base::trunc);
			::std::ifstream in(m_deffilename.c_str());
			while (!in.eof() && in.get(ch)) fcheck.put(ch);
			in.close();
			fcheck << "#define" << (cmd + 7) << ::std::endl;
			fcheck.close();
			::std::ostringstream os;
			int cpp_ret(preprocess(os));
			FSHELL2_PROD_CHECK1(::fshell2::Macro_Processing_Error, 0 == cpp_ret,
					"Failed to process macro definition, " + os.str());
			FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, os.str().empty());

			::std::ofstream fdef(m_deffilename.c_str(), ::std::ios_base::out|::std::ios_base::app);
			fdef << "#define" << (cmd + 7) << ::std::endl;
			fdef.close();

			m_has_defines = true;
			return "";
		} else {
			// get rid of other preprocessor commands early and have subsequent
			// parsers deal with that
			return cmd;
		}
	}
	
	if (!m_has_defines) return cmd;
		
	::std::ofstream fexpand(m_checkfilename.c_str(), ::std::ios_base::trunc);
	fexpand << "#include \"" << m_deffilename << "\"" << ::std::endl;
	fexpand << cmd << ::std::endl;
	fexpand.close();
		
	::std::ostringstream os;
	int cpp_ret(preprocess(os));
	FSHELL2_PROD_CHECK1(::fshell2::Macro_Processing_Error, 0 == cpp_ret,
			"Failed to expand command");
	// drop the final newline for consistency
	::std::istringstream is(os.str());
	::std::string line("");
	::std::getline(is, line);
	FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, is.ignore().eof());
	return line;
}

FSHELL2_MACRO_NAMESPACE_END;
FSHELL2_NAMESPACE_END;

