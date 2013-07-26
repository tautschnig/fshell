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

/*! \file fshell2/command/command_processing.cpp
 * \brief TODO
 *
 * $Id$
 * \author Michael Tautschnig <tautschnig@forsyte.de>
 * \date   Thu Apr  2 17:29:47 CEST 2009 
*/

#include <fshell2/command/command_processing.hpp>
#include <fshell2/config/annotations.hpp>
#include <fshell2/exception/command_processing_error.hpp>

#if FSHELL2_DEBUG__LEVEL__ > -1
#  include <diagnostics/basic_exceptions/violated_invariance.hpp>
#  include <diagnostics/basic_exceptions/invalid_argument.hpp>
#endif

#include <util/config.h>
#include <util/arith_tools.h>
#include <util/std_expr.h>
#include <util/std_code.h>
#include <util/replace_symbol.h>
#include <ansi-c/c_types.h>
#include <ansi-c/ansi_c_typecheck.h>
#include <langapi/language_ui.h>
#include <goto-programs/goto_convert_functions.h>
#include <pointer-analysis/add_failed_symbols.h>
#include <goto-programs/remove_function_pointers.h>
#include <goto-programs/goto_inline.h>
#include <goto-programs/loop_ids.h>
#include <analyses/goto_check.h>

#include <cerrno>
#include <fstream>
#include <sys/stat.h>
#include <cstdlib>

/* parser */
#define yyFlexLexer CMDFlexLexer
#include <FlexLexer.h>

extern int CMDparse(CMDFlexLexer *,
		::fshell2::command::Command_Processing::parsed_command_t &, char **,
		int *, ::std::list< ::std::pair< char*, char* > > &,::std::list< char* > &);
// extern int yydebug;
/* end parser */

#include <fshell2/command/grammar.hpp>

#define TMP_EXEC_FILE "tempSUT"
#define TMP_FOLDER "./temp/"
//TODO WARNING same define in parseoptions.cpp

FSHELL2_NAMESPACE_BEGIN;
FSHELL2_COMMAND_NAMESPACE_BEGIN;

::std::ostream & operator<<(::std::ostream & os, Command_Processing::status_t const& s) {
	switch (s) {
		case Command_Processing::NO_CONTROL_COMMAND:
			os << "NO_CONTROL_COMMAND";
			break;
		case Command_Processing::HELP:
			os << "HELP";
			break;
		case Command_Processing::QUIT:
			os << "QUIT";
			break;
		case Command_Processing::DONE:
			os << "DONE";
			break;
	}

	return os;
}

// cleanup char* stuff
class Cleanup {
	public:
		Cleanup(char ** a, ::std::list< ::std::pair< char*, char* > > & l);
		~Cleanup();
	private:
		char ** m_a;
		::std::list< ::std::pair< char*, char* > > & m_l;
};

Cleanup::Cleanup(char ** a, ::std::list< ::std::pair< char*, char* > > & l) :
	m_a(a), m_l(l) {
}

Cleanup::~Cleanup() {
	free(*m_a);

	while (!m_l.empty()) {
		free(m_l.front().first);
		m_l.front().first = 0;
		free(m_l.front().second);
		m_l.front().second = 0;
		m_l.erase(m_l.begin());
	}
}

Command_Processing::Command_Processing(::optionst & opts, ::goto_functionst & gf) :
	m_opts(opts), m_gf(gf), m_finalized(m_opts.get_int_option("max-argc") == 0),
	m_remove_zero_init(false) {
	if (::config.main.empty()) ::config.main = "main";
	m_opts.set_option(F2_O_LIMIT, 0);
	m_opts.set_option(F2_O_MULTIPLE_COVERAGE, 1);

	m_opts.set_option(STANDARD_CBMC_MODE, true);
	m_opts.set_option(FUNC_TO_COVER, "main");
	m_opts.set_option(TC_CONDITIONS_FILENAME, CONDITIONS_FILE);
	m_opts.set_option(TC_TRACE_FILENAME, "");
	m_opts.set_option(TEST_SUITE_FOLDER, TMP_FOLDER);
	m_opts.set_option(AUTOGENERATE_TESTSUITE,false);
	m_opts.set_option(MAX_PATHS,20);
	m_opts.set_option(PATH_DEPTH,10);


}

::std::ostream & Command_Processing::help(::std::ostream & os) {
	os << "Control commands:" << ::std::endl << COMMAND_HELP
		<< "Comments start with `//' and end at the end of the line" << ::std::endl;
	return os;
}
	
::std::ostream & Command_Processing::print_file_contents(::std::ostream & os, char const * name) const {
	FSHELL2_AUDIT_ASSERT(::diagnostics::Invalid_Argument, name != 0);
	// file might not correspond to what has been parsed, check file date
	::std::map< ::std::string, time_t >::const_iterator entry(m_parse_time.find(name));
	if (m_parse_time.end() == entry) {
		os << "WARNING: parse time of " << name
			<< " unknown, internal data may be inconsistent with file on disk!" << ::std::endl;
	} else {
		// try to obtain the file modification date
		struct stat info;
		FSHELL2_PROD_CHECK1(::fshell2::Command_Processing_Error, 0 == ::stat(name, &info),
				::std::string("Failed to stat() file ") + name);
		FSHELL2_PROD_CHECK1(::fshell2::Command_Processing_Error, info.st_mtime <= entry->second,
				::std::string("File ") + name + " changed on disk");
	}
	// read the file using an ifstream
	::std::ifstream fs(name);
	// make sure the file could be opened
	FSHELL2_PROD_CHECK1(::fshell2::Command_Processing_Error, fs.is_open(),
			::std::string("Failed to open file ") + name);
	int lc(0);
	::std::string line;
	while(!::std::getline(fs, line).eof())
	{
		os << ++lc << "\t " << line << ::std::endl;
	}
	return os;
}
	
void Command_Processing::add_sourcecode(::language_uit & manager, char const * file,
		::std::list< ::std::pair< char*, char* > > const& defines) {
	// defines -- keep a marker of global defines
	::std::list< ::std::string >::iterator last_global(::config.ansi_c.defines.end());
	if (!::config.ansi_c.defines.empty()) --last_global;
	// add the specific defines given with the ADD_SOURCE command
	if (!defines.empty())
		for (::std::list< ::std::pair<char*,char*> >::const_iterator iter(defines.begin());
				iter != defines.end(); ++iter)
			// CBMC internally does -D' and the closing ', thus we
			// generate define_ident=definition
			config.ansi_c.defines.push_back(
					::std::string(iter->first) + "=" + iter->second);
	// keep a private copy of previously loaded files
	::language_filest::filemapt prev_files;
	prev_files.swap(manager.language_files.filemap);
	// store the parse time to warn the user in case a modified file
	// is to be printed
	m_parse_time[file] = ::std::time(0);
	// attempt to parse the file
	// we don't even try to catch any exception thrown by CBMC here,
	// these would be fatal anyway
	bool err(manager.parse(file));
	// reset defines
	if (::config.ansi_c.defines.end() != last_global) {
		++last_global;
		::config.ansi_c.defines.erase(last_global, ::config.ansi_c.defines.end());
	}
	if (err) {
		manager.language_files.filemap.swap(prev_files);
		FSHELL2_PROD_CHECK1(::fshell2::Command_Processing_Error, false,
				::std::string("Failed to parse ") + file);
	}
	if (manager.typecheck()) {
		manager.language_files.filemap.swap(prev_files);
		FSHELL2_PROD_CHECK1(::fshell2::Command_Processing_Error, false,
				::std::string("Failed to typecheck ") + file);
	}
	// build the full list of loaded files
	manager.language_files.filemap.insert(prev_files.begin(), prev_files.end());
	// reset entry routine
	if (m_finalized) manager.symbol_table.remove(ID_main);
	m_finalized = false;
}
/*
 * takes the first test case from the list of possible test cases
 * sets it as current test case and removes it from the list
 *
 * return 0 if successful and -1 if test-case-list was empty
 */
int Command_Processing::set_to_next_testcase(void)
{
	::std::string testcase;
	if (!open_tc_names.empty())
	{
		testcase = open_tc_names.front();
		::std::cerr << "USING TEST CASE: " << testcase << ::std::endl;

		open_tc_names.erase(open_tc_names.begin());

		for (::std::vector< ::std::string>::iterator it = open_tc_names.begin(); it != open_tc_names.end(); ++it)
			::std::cerr << "POSSIBLE other test case in the list: " << *it << ::std::endl;

		testcase += ".log";
		m_opts.set_option(TC_TRACE_FILENAME,testcase);
		return(0);
	}

	::std::cerr << "Found no test case which can be used, please select manually or generate new one" << ::std::endl;

	return (-1); //FAIL
}

/*
 * takes a string as parameter and checks, if a test case having this name exists
 * i.e., that an entry with this name exists in the testcases.xml
 *
 * returns 0 if it does not yet exist and 1 if a test case with this name was found
 */
int Command_Processing::check_if_tc_name_exists(::std::string tc_name)
{

	::std::string strXML;
	::std::ifstream testcasesXML;

	strXML = m_opts.get_option(TEST_SUITE_FOLDER)+"testcases.xml";
	testcasesXML.open(strXML.c_str());


	while (testcasesXML.good())
	{
	  getline(testcasesXML,strXML);

	  strXML = strXML.substr(strXML.find("ID=\"")+4);
	  strXML = strXML.substr(0,strXML.find("\""));

      if (!tc_name.compare(strXML))
	  {
	  	testcasesXML.close();
	    return 1;
      }
	}

	return 0;

}

/*
 * Parses the conditions.xml file and stores the test cases where either the true or the false branch is not yet covered into a test case list
 * then calls set_to_next_testcase
 *
 * return value is the resulting value of set_to_next_testcase
 */
int Command_Processing::update_testcase_list(void)
{
	static const char XML_CONDITION_BEGIN[] = "<condition ID=\"";
	static const char XML_CONDITION_END[] = "</condition>";
    static const char XML_TESTS_TRUE_END[] = "</tests_true>";
	static const char XML_TESTS_FALSE_END[] = "</tests_false>";
	static const char XML_TESTCASE_BEFORE_ID[] = "<testcase ID=\"";
	static const char XML_TWO_SPACES[] = "  ";

	::std::string str;
	::std::string searchStr;
	::std::string myXMLstr;
	::std::string xmlLine;
	::std::string dstDir;
	::std::string true_tc;
	::std::string false_tc;
	::std::string function_to_cover;



	::std::ifstream conditionsXML;

	function_to_cover = m_opts.get_option(FUNC_TO_COVER);
	//::std::cerr << "LOOKING FOR TESTCASE to cover function: " << function_to_cover << ::std::endl;
	dstDir = m_opts.get_option(TEST_SUITE_FOLDER);
	myXMLstr = dstDir+"conditions.xml";
	str = "\" function=\""+function_to_cover+"\"";


	conditionsXML.open(myXMLstr.c_str());
	while (conditionsXML.good())
	{
		getline(conditionsXML,xmlLine);

		::std::vector< ::std::string> true_tc_v;
		::std::vector< ::std::string> false_tc_v;
		::std::vector< ::std::string>::iterator it;

		if ((xmlLine.find(XML_CONDITION_BEGIN) != ::std::string::npos) && (xmlLine.find(str.c_str()) != ::std::string::npos))
		{
			// here we found a condition in the function we want to cover, now check if true and false test cases exist
			true_tc = "";
			false_tc = "";
			searchStr = XML_TWO_SPACES;
			searchStr += XML_TESTS_TRUE_END;
			while (conditionsXML.good() && xmlLine.compare(searchStr.c_str()))
			{
				getline(conditionsXML,xmlLine);
				if (xmlLine.find(XML_TESTCASE_BEFORE_ID) != ::std::string::npos)
				{
					// store testcase ID, which is a testcase for true
					true_tc = xmlLine.substr(xmlLine.find("\"")+1);
					true_tc = true_tc.substr(0,true_tc.find("\""));
					true_tc_v.push_back(true_tc);
				}
			}
			searchStr = XML_TWO_SPACES;
			searchStr += XML_TESTS_FALSE_END;
			while (conditionsXML.good() && xmlLine.compare(searchStr.c_str()))
			{
				getline(conditionsXML,xmlLine);
				if (xmlLine.find(XML_TESTCASE_BEFORE_ID) != ::std::string::npos)
				{
					// store testcase ID, which is a testcase for true
					false_tc = xmlLine.substr(xmlLine.find("\"")+1);
					false_tc = false_tc.substr(0,false_tc.find("\""));
					false_tc_v.push_back(false_tc);
				}
			}

			open_tc_names.clear();

			if (true_tc_v.empty())
			{
				while (!false_tc_v.empty())
				{
					it = false_tc_v.begin();
					false_tc = *it;
					false_tc_v.erase(it);

					it = open_tc_names.begin();

					while (!open_tc_names.empty() && (it != open_tc_names.end()) && false_tc.compare(*it))
						++it;

					if (it == open_tc_names.end())
						open_tc_names.push_back(false_tc);
				}
			}

			if (false_tc_v.empty())
			{
				while (!true_tc_v.empty())
				{
					it = true_tc_v.begin();
					true_tc = *it;
					true_tc_v.erase(it);

					it = open_tc_names.begin();

					while (!open_tc_names.empty() && (it != open_tc_names.end()) && true_tc.compare(*it))
						++it;

					if (it == open_tc_names.end())
						open_tc_names.push_back(true_tc);
				}
			}

		}

	}


	conditionsXML.close();

  return set_to_next_testcase();
}

/*
 * parses the coverage log of a single test case (which is given as parameter) and integrates this test case into the testcase.xml file
 */
void Command_Processing::integrateInXML(::std::string tcFilename)
{
  static const char XML_TESTS_TRUE_BEGIN[] = "<tests_true>";
  static const char XML_TESTS_TRUE_END[] = "</tests_true>";
  static const char XML_TESTS_FALSE_BEGIN[] = "<tests_false>";
  static const char XML_TESTS_FALSE_END[] = "</tests_false>";
  static const char XML_CONDITION_END[] = "</condition>";
  static const char XML_TESTCASE_BEFORE_ID[] = "<testcase ID=\"";
  static const char XML_TESTCASE_AFTER_ID[] = "\" />";
  static const char XML_TWO_SPACES[] = "  ";

  ::std::string callString;
  ::std::string str;
  ::std::string oldXMLstr;
  ::std::string newXMLstr;
  ::std::string covLine;
  ::std::string xmlLine;
  ::std::string dstDir;
  int pos;
  bool trueVal;
  bool foundCondition;
  bool foundTC;

  dstDir = m_opts.get_option(TEST_SUITE_FOLDER);

  oldXMLstr = dstDir+"conditions.xml";
  ::std::ifstream oldConditionsXML;
  newXMLstr = dstDir+"conditions_temp.xml";
  ::std::ofstream newConditionsXML;
  callString = dstDir+tcFilename+".log";
  ::std::ifstream testcase(callString.c_str());

  while (testcase.good())
  {
	  getline(testcase,covLine);
	 // os << covLine << ::std::endl;

	  if ((pos = covLine.find("/> t")) != ::std::string::npos)
		  trueVal = true;
	  else if ((pos = covLine.find("/> f")) != ::std::string::npos)
		  trueVal = false;
	  else continue;

	  covLine.replace(pos,4,">");
	 // os << "line: " << covLine << ::std::endl;
	 // oldConditionsXML.seekg(0,oldConditionsXML.beg);

	  oldConditionsXML.open(oldXMLstr.c_str());
	  newConditionsXML.open(newXMLstr.c_str());
	  foundCondition = false;

	  while (oldConditionsXML.good())
	  {
		  getline(oldConditionsXML,xmlLine);
		  if (xmlLine.length() == 0)
			  continue;

		  newConditionsXML << xmlLine << "\n";

		  if (covLine.compare(xmlLine))
		  {
			  // lines are not the same, write to file and goto next line
			  continue;
		  }
		  else
		  {
			  foundCondition = true;

			  callString = XML_TWO_SPACES;
			  callString += XML_TWO_SPACES;
			  callString += XML_TESTCASE_BEFORE_ID;
			  callString += tcFilename;
			  callString += XML_TESTCASE_AFTER_ID;

			  foundTC = false;

			  str = XML_TWO_SPACES;
			  str += XML_TESTS_TRUE_END;

			  getline(oldConditionsXML,xmlLine); //this should be line: XML_TWO_SPACES << XML_TESTS_TRUE_BEGIN which we have read above
			  while (oldConditionsXML.good() && str.compare(xmlLine))
			  {
				  newConditionsXML << xmlLine << "\n";

				  getline(oldConditionsXML,xmlLine);

				  if (!callString.compare(xmlLine))
					  foundTC = true;
			  }
			  if (!oldConditionsXML.good())
				  assert(false);

			  if (trueVal && !foundTC)
				  newConditionsXML << XML_TWO_SPACES << XML_TWO_SPACES << XML_TESTCASE_BEFORE_ID << tcFilename << XML_TESTCASE_AFTER_ID << "\n";

			  newConditionsXML << xmlLine << "\n"; //this should be line: XML_TWO_SPACES << XML_TESTS_TRUE_END

			  foundTC = false;
			  str = XML_TWO_SPACES;
			  str += XML_TESTS_FALSE_END;

			  getline(oldConditionsXML,xmlLine);//this should be line: XML_TWO_SPACES << XML_TESTS_FALSE_BEGIN which we have read above
			  while (oldConditionsXML.good() && str.compare(xmlLine))
			  {
				  newConditionsXML << xmlLine << "\n";

				  getline(oldConditionsXML,xmlLine);

				  if (!callString.compare(xmlLine))
					  foundTC = true;
			  }
			  if (!oldConditionsXML.good())
			  	  assert(false);

			  if (!trueVal&& !foundTC)
				  newConditionsXML << XML_TWO_SPACES << XML_TWO_SPACES << XML_TESTCASE_BEFORE_ID << tcFilename << XML_TESTCASE_AFTER_ID << "\n";

			  newConditionsXML << xmlLine << "\n"; //this should be line: XML_TWO_SPACES << XML_TESTS_FALSE_END

			  callString = XML_TWO_SPACES;
			  callString += XML_TESTS_FALSE_END;
			  while (oldConditionsXML.good() && callString.compare(xmlLine))
			  {
				  getline(oldConditionsXML,xmlLine);
				  newConditionsXML << xmlLine << "\n";
			  }

			  //os << "SAME " << xmlLine << ::std::endl;
		  }

	  }
	  if (!foundCondition)
	  {
		  // ADD new condition to xml file
		  //os << "new: " << covLine << ::std::endl;
		  newConditionsXML << covLine << "\n";
		  newConditionsXML << XML_TWO_SPACES << XML_TESTS_TRUE_BEGIN << "\n";
		  if (trueVal)
			  newConditionsXML << XML_TWO_SPACES << XML_TWO_SPACES << XML_TESTCASE_BEFORE_ID << tcFilename << XML_TESTCASE_AFTER_ID << "\n";
		  newConditionsXML << XML_TWO_SPACES << XML_TESTS_TRUE_END << "\n";
		  newConditionsXML << XML_TWO_SPACES << XML_TESTS_FALSE_BEGIN << "\n";
		  if (!trueVal)
			  newConditionsXML << XML_TWO_SPACES << XML_TWO_SPACES << XML_TESTCASE_BEFORE_ID << tcFilename << XML_TESTCASE_AFTER_ID << "\n";
		  newConditionsXML << XML_TWO_SPACES << XML_TESTS_FALSE_END << "\n";
		  newConditionsXML << XML_CONDITION_END << "\n";
	  }
	  else
		  foundCondition = false;

	  oldConditionsXML.close();
	  newConditionsXML.close();
	  remove(oldXMLstr.c_str());
	  rename(newXMLstr.c_str(),oldXMLstr.c_str());
	  //os << "--------" << ::std::endl;
  }
}
/*
 * takes the name of a test case and the shell-parameter-string and stores it to the file testcases.xml
 */
int Command_Processing::integrateTestCaseNameInXML(::std::string tcName, std::string paramStr)
{
  static const char XML_TESTCASE_BEFORE_ID[] = "<testcase ID=\"";
  static const char XML_TESTCASE_AFTER_ID[] = "\">";
  static const char XML_TESTCASE_END[] ="</testcase>";
  static const char XML_TWO_SPACES[] = "  ";

  ::std::string str;
  ::std::string oldXMLstr;
  ::std::string newXMLstr;
  ::std::string xmlLine;
  ::std::string dstDir;

  if (check_if_tc_name_exists(tcName) == 1) //test case name already exists
	  return FAIL;

  dstDir = m_opts.get_option(TEST_SUITE_FOLDER);

  oldXMLstr = dstDir+"testcases.xml";
  ::std::ifstream oldTestcasesXML;
  newXMLstr = dstDir+"testcases_temp.xml";
  ::std::ofstream newTestcasesXML;

  str = XML_TESTCASE_BEFORE_ID;
  str += tcName;
  str += XML_TESTCASE_AFTER_ID;
  str += paramStr;
  str += XML_TESTCASE_END;

  oldTestcasesXML.open(oldXMLstr.c_str());
  newTestcasesXML.open(newXMLstr.c_str());

  while (oldTestcasesXML.good())
  {
	getline(oldTestcasesXML,xmlLine);

	newTestcasesXML << xmlLine << "\n";

	if (!str.compare(xmlLine))
	{
		oldTestcasesXML.close();
		newTestcasesXML.close();
		remove(newXMLstr.c_str());
	    return FAIL;
	}
  }

  // if we come here, we did not find the TC in the file

  newTestcasesXML << XML_TESTCASE_BEFORE_ID << tcName << XML_TESTCASE_AFTER_ID << paramStr << XML_TESTCASE_END;

  oldTestcasesXML.close();
  newTestcasesXML.close();
  remove(oldXMLstr.c_str());
  rename(newXMLstr.c_str(),oldXMLstr.c_str());

  return DONE;
}

/*
 * execute a single testcase, with the name and shell-parameter
 */
int Command_Processing::run_testcase(::std::string tc_name, ::std::string tc_parameter)
{
	::std::string singleName;
	::std::string callString;
	::std::string str;
	::std::string destDir;
	::std::string covFile;
	::std::string condFile;

	//::std::ifstream readFile;

	covFile = COVERAGE_FILE;//m_opts.get_option(TC_PATH_FILENAME);
	condFile = m_opts.get_option(TC_CONDITIONS_FILENAME);
	destDir = m_opts.get_option(TEST_SUITE_FOLDER);
	singleName = TMP_EXEC_FILE;


    if (integrateTestCaseNameInXML(tc_name,tc_parameter) != DONE)
    	return FAIL;

	remove(covFile.c_str());

	callString = destDir+condFile;
	rename(callString.c_str(),condFile.c_str());

	callString = TMP_FOLDER;
	callString += singleName+"instr.gcc.out "+tc_parameter;
	system(callString.c_str());
	::std::cerr << "Running testcase with parameter: " << tc_parameter << ::std::endl;

	callString = tc_name;
	if (callString.length() != 0)
	  str = tc_name;
	int pos;
	while ((pos = str.find(" ")) != ::std::string::npos)
		str.replace(pos,1,"_");

	//callString = "mv ./"+covFile+" "+destDir+str+".log";
	//system(callString.c_str());
	callString = destDir+str+".log";
	rename(covFile.c_str(),callString.c_str());

	integrateInXML(str);

	//callString = "cp ./"+condFile+" "+destDir+condFile;
	//system(callString.c_str());
	callString = destDir+condFile;
	rename(condFile.c_str(),callString.c_str());

	update_testcase_list();

	return DONE;
}

Command_Processing::status_t Command_Processing::process(::language_uit & manager,
		::std::ostream & os, char const * cmd) {
	while (0 != *cmd && ::std::isspace(*cmd)) ++cmd;
	if (0 == *cmd || ('/' == *cmd && '/' == *(cmd + 1))) return DONE;
		
	::std::string singleName;
	::std::string callString;
	::std::string destDir;
	::std::string str;
	::std::string covFile;
	::std::string condFile;

	::std::ifstream readFile;

	covFile = COVERAGE_FILE;//m_opts.get_option(TC_PATH_FILENAME);
	condFile = m_opts.get_option(TC_CONDITIONS_FILENAME);
	destDir = m_opts.get_option(TEST_SUITE_FOLDER);
	singleName = TMP_EXEC_FILE;

	// new lexer
	CMDFlexLexer lexer;
	// put the input into a stream
	::std::istringstream is(cmd);
	// set the stream as the input to the parser
	lexer.switch_streams(&is, &os);
	// reset errno, readline for some reason sets this to EINVAL
	errno = 0;
	// information returned by parser
	parsed_command_t parsed_cmd(FAIL);
	char * arg(0);
	int numeric_arg(-1);
	::std::list< ::std::pair< char*, char* > > defines;
	::std::list< char* > runParameter;
	Cleanup cleanup(&arg, defines);
	// yyparse returns 0 iff there was no error
	if (0 != CMDparse(&lexer, parsed_cmd, &arg, &numeric_arg, defines,runParameter)) return NO_CONTROL_COMMAND;

	// parsing succeeded, what has to be done?
	switch (parsed_cmd) {
		case FAIL:
			FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, false);
			return NO_CONTROL_COMMAND;
		case CMD_HELP:
			return HELP;
		case CMD_QUIT:
			return QUIT;
		case CMD_ADD_SOURCECODE:
			FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, arg != 0);
			add_sourcecode(manager, arg, defines);
			return DONE;
		case CMD_SHOW_FILENAMES:
			for(::language_filest::filemapt::const_iterator iter(manager.language_files.filemap.begin());
					iter != manager.language_files.filemap.end(); ++iter)
				os << iter->first << ::std::endl;
			return DONE;
		case CMD_SHOW_SOURCECODE:
			FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, arg != 0);
			print_file_contents(os, arg);
			return DONE;
		case CMD_SHOW_SOURCECODE_ALL:
			for(::language_filest::filemapt::const_iterator iter(manager.language_files.filemap.begin());
					iter != manager.language_files.filemap.end(); ++iter)
				print_file_contents(os, iter->first.c_str());
			return DONE;
		case CMD_SET_ENTRY:
			FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, arg != 0);
			{
				::config.main = arg;
				if (m_finalized) manager.symbol_table.remove(ID_main);
				m_finalized = false;
			}
			return DONE;
		case CMD_SET_LIMIT_COUNT:
			FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, numeric_arg >= 0);
			FSHELL2_PROD_CHECK1(::fshell2::Command_Processing_Error, numeric_arg > 0,
					"Limit must be greater than 0");
			m_opts.set_option(F2_O_LIMIT, numeric_arg);
			return DONE;
		case CMD_SET_NO_ZERO_INIT:
			m_remove_zero_init = true;	
			m_finalized = false;
			return DONE;
		case CMD_SET_ABSTRACT:
			FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, arg != 0);
			{
				::symbol_tablet::symbolst::iterator sym_entry(
						manager.symbol_table.symbols.find(::std::string("c::") + arg));
				FSHELL2_PROD_CHECK1(::fshell2::Command_Processing_Error,
						sym_entry != manager.symbol_table.symbols.end(),
						::std::string("Function ") + arg + " not found");
				sym_entry->second.value.make_nil();
				m_finalized = false;
			}
			return DONE;
		case CMD_SET_MULTIPLE_COVERAGE:
			FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, numeric_arg >= 0);
			FSHELL2_PROD_CHECK1(::fshell2::Command_Processing_Error, numeric_arg > 1,
					"Multiplicity must be greater than 1");
			m_opts.set_option(F2_O_MULTIPLE_COVERAGE, numeric_arg);
			return DONE;
		/*###### BEGIN frEDIT ####*/
		case CMD_SET_FUNC_TO_COVER:
		    FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, arg != 0);
			m_opts.set_option(FUNC_TO_COVER,arg);
			//os << "done" << ::std::endl;
			update_testcase_list();
			return DONE;
		case CMD_SET_TC_CONDITIONS_FILENAME:
		    FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, arg != 0);
			m_opts.set_option(TC_CONDITIONS_FILENAME,arg);
			//os << "done" << ::std::endl;
			return DONE;
		case CMD_SET_TC_TRACE_FILENAME:
		    FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, arg != 0);
			m_opts.set_option(TC_TRACE_FILENAME,arg);
			//os << "done" << ::std::endl;
			return DONE;
		case CMD_SET_STANDARD_CBMC_MODE:
			{
			m_opts.set_option(STANDARD_CBMC_MODE,true);
			m_opts.set_option(INTERNAL_COVERAGE_CHECK, m_opts.get_bool_option(ORIG_INTERNAL_COVERAGE_CHECK));
			m_opts.set_option(SAT_COVERAGE_CHECK, m_opts.get_bool_option(ORIG_SAT_COVERAGE_CHECK));
			return DONE;
			}
		case CMD_SET_PATHWALK_CBMC_MODE:
			{
			m_opts.set_option(STANDARD_CBMC_MODE,false);
			m_opts.set_option(INTERNAL_COVERAGE_CHECK, false);
			m_opts.set_option(SAT_COVERAGE_CHECK, true);

			update_testcase_list();
			return DONE;
			}
		case CMD_SET_AUTOGENERATE:
		{
			m_opts.set_option(AUTOGENERATE_TESTSUITE,true);
			return DONE;
		}
		case CMD_SET_MAX_PATHS:
		{
			FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, numeric_arg >= 0);
			FSHELL2_PROD_CHECK1(::fshell2::Command_Processing_Error, numeric_arg >= 0,
					"MAX_PATHS must be greater than or equal to o");
			m_opts.set_option(MAX_PATHS, numeric_arg);
			return DONE;
		}
		case CMD_SET_PATH_DEPTH:
		{
			FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, numeric_arg >= 0);
			FSHELL2_PROD_CHECK1(::fshell2::Command_Processing_Error, numeric_arg > 1,
					"Path depth must be greater than 1");
			m_opts.set_option(PATH_DEPTH, numeric_arg);
			return DONE;
		}
		case CMD_SET_TESTSUITE_FOLDER:
		    FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, arg != 0);
			m_opts.set_option(TEST_SUITE_FOLDER,arg);
			//os << "done" << ::std::endl;
			return DONE;
		case CMD_SET_TESTCASE:
		    FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, arg != 0);
		    {

			callString = destDir+arg+".log";
			readFile.open(callString.c_str());

			if (readFile.good())
			{
				callString = arg;
				callString = callString + ".log";
				m_opts.set_option(TC_TRACE_FILENAME,callString.c_str());
			}
			else
				os << "test case not found" << ::std::endl;

			readFile.close();

			return DONE;
		    }
		case CMD_RUN:
		    FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, arg != 0);
		    {
				if (!runParameter.empty())
				{
					for (::std::list< char* >::const_iterator iter(runParameter.begin());
							iter != runParameter.end(); ++iter)
						str = str + " " + *iter;
				}
				if (run_testcase(arg, str) != DONE)
					os << "Test case name already used, aborting ..." << ::std::endl;
				return DONE;
		    }
		/*###### END frEDIT ####*/
	}
			
	FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, false);
	return NO_CONTROL_COMMAND;
}

void Command_Processing::remove_zero_init(::language_uit & manager) const {
	::symbol_tablet::symbolst::iterator init_iter(
	  manager.symbol_table.symbols.find("c::__CPROVER_initialize"));
	FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance,
						 init_iter != manager.symbol_table.symbols.end());

	// check all operands
	for (::exprt::operandst::iterator iter(init_iter->second.value.operands().begin());
		 iter != init_iter->second.value.operands().end();)
	{
		if (iter->get(ID_statement) == ID_assign &&
			iter->location().get_file() != "<built-in>") {
			FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, 2 == iter->operands().size());
			if (iter->op1().get_bool(ID_C_zero_initializer)) {
				iter = init_iter->second.value.operands().erase(iter);
			} else {
				++iter;
			}
		} else {
			++iter;
		}
	}
}

void Command_Processing::model_argv(::language_uit & manager) const {
	::symbol_tablet &symbol_table=manager.symbol_table;
	::namespacet const ns(symbol_table);

	FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, symbol_table.has_symbol("c::main"));
	locationt const& main_loc=symbol_table.symbols.find("c::main")->second.location;
	FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, main_loc.is_not_nil());

	/*
	unsigned next=0;
	unsigned i=0;
   	__CPROVER_assume(argc'<=max_argc);
   	while(i<argc' && i<max_argc)
   	{
    	unsigned len;
     	__CPROVER_assume(len<4096);
     	__CPROVER_assume(next+len<4096);
     	argv'[i]=argv''+next;
     	argv''[next+len]=0;
     	next=next+len+1;
		i=i+1;
   	}
	*/

	int max_argc=m_opts.get_int_option("max-argc");
	FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, max_argc > 0);

	::symbol_tablet::symbolst::iterator main_iter(
	  symbol_table.symbols.find("main"));
	FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance,
						 main_iter != symbol_table.symbols.end());

	// turn argv' into a bounded array
	{
		FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, symbol_table.has_symbol("c::argv'"));
		symbol_table.symbols.find("c::argv'")->second.type.set(ID_size, from_integer(max_argc+1, index_type()));

		replace_symbolt replace_argvp;

		::symbolt const& argvp_symbol(ns.lookup("c::argv'"));
		symbol_exprt argvp(argvp_symbol.name, argvp_symbol.type);
		replace_argvp.insert(argvp_symbol.name, argvp);

		bool replaced=replace_argvp.replace(main_iter->second.value);
		assert(!replaced);
	}

	// add argv'' to the symbol table
	if(!symbol_table.has_symbol("c::main::1::argv''"))
	{
		// POSIX guarantees that at least 4096 characters can be passed as
		// arguments
		::array_typet argv_type(char_type(), from_integer(42, index_type()));

		::symbolt argv_symbol;
		argv_symbol.pretty_name="argv''";
		argv_symbol.base_name="main::1::argv''";
		argv_symbol.name="c::main::1::argv''";
		argv_symbol.type=argv_type;
		argv_symbol.is_file_local=true;
		argv_symbol.is_type=false;
		argv_symbol.is_thread_local=false;
		argv_symbol.is_static_lifetime=false;
		argv_symbol.value.make_nil();
		argv_symbol.location=main_loc;

		symbol_table.add(argv_symbol);
	}

	// add counters/size
	::irep_idt locals[] = { "next", "i", "len'" };
	for(size_t i=0; i<sizeof(locals)/sizeof(::irep_idt); ++i)
	{
		::irep_idt name="c::main::1::"+id2string(locals[i]);
		if(symbol_table.has_symbol(name))
			continue;

		symbolt next_symbol;
		next_symbol.pretty_name=id2string(locals[i]);
		next_symbol.base_name="main::1::"+id2string(locals[i]);
		next_symbol.name=name;
		next_symbol.type=unsigned_short_int_type();
		next_symbol.is_file_local=true;
		next_symbol.is_type=false;
		next_symbol.is_thread_local=false;
		next_symbol.is_static_lifetime=false;
		next_symbol.is_lvalue=true;
		next_symbol.value.make_nil();
		next_symbol.location=main_loc;

		symbol_table.add(next_symbol);
	}

	/*
	char argv''[4096];
	*/
	::symbolt const& argvpp_symbol(ns.lookup("c::main::1::argv''"));
	symbol_exprt argvpp(argvpp_symbol.base_name, argvpp_symbol.type);
	code_declt argvpp_decl(argvpp);
	argvpp_decl.location()=main_loc;

	code_blockt code;

	/*
	unsigned next=0;
	unsigned i=0;
	*/
	for(int i=0; i<2; ++i)
	{
		::symbolt const& symbol(ns.lookup("c::main::1::"+id2string(locals[i])));
		::symbol_exprt symbol_expr(symbol.base_name, symbol.type);

		code_declt decl(symbol_expr);
		decl.location()=main_loc;
		code.move_to_operands(decl);

		code_assignt assign(symbol_expr, from_integer(0, unsigned_short_int_type()));
		assign.location()=main_loc;
		code.move_to_operands(assign);
	}

	/*
   	__CPROVER_assume(argc'<=max_argc);
	*/
	::symbolt const& argcp_symbol(ns.lookup("c::argc'"));
	symbol_exprt argcp("argc'", argcp_symbol.type); /* for some reason the base_name of c::argc' was set to argc */
	{
		binary_relation_exprt le(
		  argcp,
		  ID_le,
		  from_integer(max_argc, argcp_symbol.type));
		side_effect_expr_function_callt fc;
		fc.function()=symbol_exprt("__CPROVER_assume", code_typet());
		fc.arguments().push_back(le);
		code_expressiont assume(fc);
		assume.location()=main_loc;
		code.move_to_operands(assume);
	}

	/*
   	while(i<argc' && i<max_argc)
   	{
    	unsigned len;
     	__CPROVER_assume(len<4096);
     	__CPROVER_assume(next+len<4096);
     	argv'[i]=argv''+next;
     	argv''[next+len]=0;
     	next=next+len+1;
		i=i+1;
   	}
	*/
	{
		::symbolt const& next_symbol(ns.lookup("c::main::1::next"));
		::symbol_exprt next(next_symbol.base_name, next_symbol.type);
		::symbolt const& len_symbol(ns.lookup("c::main::1::len'"));
		::symbol_exprt len(len_symbol.base_name, len_symbol.type);
		::symbolt const& i_symbol(ns.lookup("c::main::1::i"));
		symbol_exprt i(i_symbol.base_name, i_symbol.type);

		::symbolt const& argvp_symbol(ns.lookup("c::argv'"));
		symbol_exprt argvp(argvp_symbol.base_name, argvp_symbol.type);

		code_whilet while_loop;
		while_loop.location()=main_loc;

		while_loop.cond()=and_exprt(
		  binary_relation_exprt(i, ID_lt, argcp),
		  binary_relation_exprt(i, ID_lt, from_integer(max_argc, i_symbol.type)));

		while_loop.body()=code_blockt();

		code_declt decl(len);
		decl.location()=main_loc;
		while_loop.body().move_to_operands(decl);

		exprt e4096=from_integer(42, len_symbol.type);
		binary_relation_exprt lt1(len, ID_lt, e4096);
		side_effect_expr_function_callt fc1;
		fc1.function()=symbol_exprt("__CPROVER_assume", code_typet());
		fc1.arguments().push_back(lt1);
		code_expressiont assume1(fc1);
		assume1.location()=main_loc;
		while_loop.body().move_to_operands(assume1);
		binary_relation_exprt lt2(plus_exprt(next, len), ID_lt, e4096);
		side_effect_expr_function_callt fc2;
		fc2.function()=symbol_exprt("__CPROVER_assume", code_typet());
		fc2.arguments().push_back(lt2);
		code_expressiont assume2(fc2);
		assume2.location()=main_loc;
		while_loop.body().move_to_operands(assume2);

		code_assignt assign1(
		  index_exprt(argvp, i),
		  plus_exprt(argvpp, next));
		assign1.location()=main_loc;
		while_loop.body().move_to_operands(assign1);

		code_assignt assign2(
		  index_exprt(argvpp, plus_exprt(next, len)),
		  from_integer(0, char_type()));
		assign2.location()=main_loc;
		while_loop.body().move_to_operands(assign2);

		code_assignt assign3(next, plus_exprt(plus_exprt(next, len), from_integer(1, unsigned_short_int_type())));
		assign3.location()=main_loc;
		while_loop.body().move_to_operands(assign3);

		code_assignt assign4(i, plus_exprt(i, from_integer(1, unsigned_short_int_type())));
		assign4.location()=main_loc;
		while_loop.body().move_to_operands(assign4);

		code.move_to_operands(while_loop);
	}

	ansi_c_parse_treet ansi_c_parse_tree;
	ansi_c_typecheckt ansi_c_typecheck(
	  ansi_c_parse_tree, symbol_table, "", manager.get_message_handler());
	try
	{
		side_effect_exprt expr(ID_statement_expression, empty_typet());
		expr.move_to_operands(code);

		ansi_c_typecheck.typecheck_expr(expr);
		code.swap(expr.op0());

		code_blockt dummyb;
		dummyb.move_to_operands(argvpp_decl);
		expr.op0()=dummyb;
		ansi_c_typecheck.typecheck_expr(expr);
		argvpp_decl.swap(to_code_block(to_code(expr.op0())).operands().front());
	}

	catch(int)
	{
		ansi_c_typecheck.error();
	}

	catch(const char *e)
	{
		ansi_c_typecheck.error(e);
	}

	catch(const std::string &e)
	{
		ansi_c_typecheck.error(e);
	}
	FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance,
						 !ansi_c_typecheck.get_error_found());

	// find main() call and insert code before
	for (::exprt::operandst::iterator iter(main_iter->second.value.operands().begin());
		 iter != main_iter->second.value.operands().end();
		 ++iter)
	{
		if (iter->get(ID_statement) == ID_function_call &&
			to_symbol_expr(to_code_function_call(to_code(*iter)).function()).get_identifier() == "c::main")
		{
			::exprt::operandst items;

			items.push_back(argvpp_decl);
			items.push_back(code);

			main_iter->second.value.operands().insert(iter, items.begin(), items.end());

			code.make_nil();
			break;
		}
	}
	FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, code.is_nil());
}

bool Command_Processing::finalize(::language_uit & manager) {
	FSHELL2_PROD_CHECK1(::fshell2::Command_Processing_Error,
			!manager.symbol_table.symbols.empty(),
			"No source files loaded!");
				
	::std::string entry("c::");
	entry += ::config.main;
	FSHELL2_PROD_CHECK1(::fshell2::Command_Processing_Error,
			manager.symbol_table.has_symbol(entry),
			::std::string("Could not find entry function " + ::config.main));
	
	if (m_finalized) return false;
	
	FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance,
			!manager.language_files.filemap.empty());
	m_finalized = ! manager.final();
	// this must never fail, given all the previous sanity checks
	FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, m_finalized);

	// remove 0-initialization of global variables, if requested by user
	if (m_remove_zero_init) remove_zero_init(manager);

	// create init code for argv, if max-argc was set
	if (m_opts.get_int_option("max-argc") > 0 &&
		entry=="c::main" &&
		manager.symbol_table.has_symbol("c::argc'"))
		model_argv(manager);

	// convert all symbols; iterators are unstable, copy symbol names first
	::std::vector< ::irep_idt > symbols;
	for(::symbol_tablet::symbolst::iterator iter(manager.symbol_table.symbols.begin());
			iter != manager.symbol_table.symbols.end(); ++iter)
		if(iter->second.type.id() == ID_code)
			symbols.push_back(iter->first);
	
	::goto_convert_functionst converter(manager.symbol_table, m_gf,
			manager.ui_message_handler);

	//::std::cerr << "WITH _1 d found? " << manager.symbol_table.has_symbol("c::main::$tmp::return_value_setMyVal$1_1") << ::std::endl;

	for(::std::vector< ::irep_idt >::const_iterator iter(symbols.begin());
			iter != symbols.end(); ++iter) {
		::goto_functionst::function_mapt::iterator fct(m_gf.function_map.find(*iter));
		if (m_gf.function_map.end() != fct)
			/* m_gf.function_map.erase(fct);
		converter.convert_function(*iter);*/
		{
			if (*iter!=ID_main)
				continue;
			m_gf.function_map.erase(fct);
		}
		converter.convert_function(*iter);
	}


	//::std::cerr << "WITH _1 e found? " << manager.symbol_table.has_symbol("c::main::$tmp::return_value_setMyVal$1_1") << ::std::endl;

	finalize_goto_program(manager);
	return true;
}

void Command_Processing::finalize_goto_program(::language_uit & manager) {

	manager.status("Function Pointer Removal");
	::remove_function_pointers(manager.symbol_table, m_gf, false);

	manager.status("Partial Inlining");
	// do partial inlining
	::namespacet ns(manager.symbol_table);
	::goto_partial_inline(m_gf, ns, manager.ui_message_handler);

	// add generic checks
	manager.status("Generic Property Instrumentation");
	::goto_check(ns, m_opts, m_gf);

	// add failed symbols
	::add_failed_symbols(manager.symbol_table);

	// recalculate numbers, etc.
	m_gf.update();

	// add loop ids
	m_gf.compute_loop_numbers();

	// show loops, if requested
	if (m_opts.get_bool_option("show-loops"))
		::show_loop_ids(manager.get_ui(), m_gf);
}

FSHELL2_COMMAND_NAMESPACE_END;
FSHELL2_NAMESPACE_END;

