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

#ifndef FSHELL2__CONFIG__ANNOTATIONS_HPP
#define FSHELL2__CONFIG__ANNOTATIONS_HPP

/**
 * @file fshell2/config/annotations.hpp
 *
 * $Id: annotations.hpp 331 2007-12-04 18:53:18Z tautschn $
 *
 * @author Christian Schallhart
 *
 * @brief [LEVEL: beta] Conditional Macros for annotating a
 * library. This file is meant as a template for a library specific
 * interface. 
 *
 */

#ifndef FSHELL2_DEBUG__LEVEL__
#  define FSHELL2_DEBUG__LEVEL__ -1
#endif

#ifndef DEBUG__LEVEL__
#  define DEBUG__LEVEL__ FSHELL2_DEBUG__LEVEL__
#endif

#if FSHELL2_DEBUG__LEVEL__ >= 0
#  include <diagnostics/macros/check_annotation.hpp>
#  include <diagnostics/macros/assert_annotation.hpp>
#  include <diagnostics/macros/block_annotation.hpp>
#  include <diagnostics/macros/invariance_annotation.hpp>
#  include <diagnostics/macros/exceptionless_block_annotation.hpp>
#  include <diagnostics/macros/relation_modifier.hpp>
#else
# define DIAGNOSTICS_RELATION(ARG1, ARG2, ARG3, ARG4, ARG5) ((void)0)
#endif


#ifndef FSHELL2_BASE_EXCEPTION_TYPE
#  define FSHELL2_BASE_EXCEPTION_TYPE ::std::exception
#  include <exception>
#  define FSHELL2_BASE_EXCEPTION_STR_WHAT(EX) (::std::string("EXCEPTION=\"") + "\" WHAT=\"" + (EX).what() + "\"")
#endif


#ifndef FSHELL2_HANDLE_TEST_EXCEPTION_EXPLICITLY
#  define FSHELL2_HANDLE_TEST_EXCEPTION_EXPLICITLY 1
#endif


#ifndef FSHELL2_FAILED_ASSERT_ACTION
#  define FSHELL2_FAILED_ASSERT_ACTION(LEVEL,NAME,COND) throw NAME(#COND)
#endif

#ifndef FSHELL2_FAILED_ASSERT_ACTION1
#  define FSHELL2_FAILED_ASSERT_ACTION1(LEVEL,NAME,COND,EXCEP_WHAT) throw NAME(EXCEP_WHAT)
#endif

#ifndef FSHELL2_FAILED_CHECK_ACTION
#  define FSHELL2_FAILED_CHECK_ACTION(LEVEL,NAME,COND) throw NAME(#COND)
#endif

#ifndef FSHELL2_FAILED_CHECK_ACTION1
#  define FSHELL2_FAILED_CHECK_ACTION1(LEVEL,NAME,COND,EXCEP_WHAT) throw NAME(EXCEP_WHAT)
#endif


#ifndef FSHELL2_NR_WHAT_DEFAULT
#  define FSHELL2_NR_WHAT_DEFAULT 0
#endif 

////////////////////////////////////////////////////////////////////////////////

// derived config

#if FSHELL2_DEBUG__LEVEL__ >= 0
#if FSHELL2_HANDLE_TEST_EXCEPTION_EXPLICITLY == 1
#  include <diagnostics/unittest/test_exception.hpp>
#endif
#endif

#ifndef FSHELL2_BASE_EXCEPTION_STR_WHAT
#  warning FSHELL2_BASE_EXCEPTION_STR_WHAT not defined
#endif

////////////////////////////////////////////////////////////////////////////////

// AUDIT

#if FSHELL2_DEBUG__LEVEL__ > 1

#  define FSHELL2_AUDIT_TRACE(STR_WHAT) \
    DIAGNOSTICS_BASE_LOG(::diagnostics::LEVEL_AUDIT,::diagnostics::TYPE_TRACE,FSHELL2_NR_WHAT_DEFAULT,(STR_WHAT))
#  define FSHELL2_AUDIT_BINARY_TRACE(STR_WHAT) \
    DIAGNOSTICS_BASE_LOG(::diagnostics::LEVEL_AUDIT,::diagnostics::TYPE_TRACE_BINARY,FSHELL2_NR_WHAT_DEFAULT,(STR_WHAT))

#  define FSHELL2_AUDIT_TRACE1(NR_WHAT,STR_WHAT) \
    DIAGNOSTICS_BASE_LOG(::diagnostics::LEVEL_AUDIT,::diagnostics::TYPE_TRACE,(NR_WHAT),(STR_WHAT))
#  define FSHELL2_AUDIT_BINARY_TRACE1(NR_WHAT,STR_WHAT) \
    DIAGNOSTICS_BASE_LOG(::diagnostics::LEVEL_AUDIT,::diagnostics::TYPE_TRACE_BINARY,(NR_WHAT),(STR_WHAT))


#  define FSHELL2_AUDIT_BLOCK_ENTER(STR_WHAT)  \
    DIAGNOSTICS_BASE_BLOCK_ENTER(::diagnostics::LEVEL_AUDIT, \
                                 ::diagnostics::TYPE_BLOCK_ENTER, \
                                 ::diagnostics::TYPE_BLOCK_EXIT,FSHELL2_NR_WHAT_DEFAULT,(STR_WHAT))
#  define FSHELL2_AUDIT_BLOCK_EXIT DIAGNOSTICS_BASE_BLOCK_EXIT
#  define FSHELL2_AUDIT_BLOCK_GUARD(STR_WHAT)  \
    DIAGNOSTICS_BASE_BLOCK_GUARD(::diagnostics::LEVEL_AUDIT, \
                                 ::diagnostics::TYPE_BLOCK_ENTER, \
                                 ::diagnostics::TYPE_BLOCK_EXIT,FSHELL2_NR_WHAT_DEFAULT,(STR_WHAT))

#  define FSHELL2_AUDIT_PROCEDURE_ENTER(STR_WHAT) \
    DIAGNOSTICS_BASE_BLOCK_ENTER(::diagnostics::LEVEL_AUDIT, \
                                 ::diagnostics::TYPE_PROCEDURE_ENTER, \
                                 ::diagnostics::TYPE_PROCEDURE_EXIT,FSHELL2_NR_WHAT_DEFAULT, \
                                 ::std::string("PROCEDURE=\"") + DIAGNOSTICS_FUNC_NAME + "\" " + (STR_WHAT))
#  define FSHELL2_AUDIT_PROCEDURE_EXIT DIAGNOSTICS_BASE_BLOCK_EXIT
#  define FSHELL2_AUDIT_PROCEDURE_GUARD(STR_WHAT) \
    DIAGNOSTICS_BASE_BLOCK_GUARD(::diagnostics::LEVEL_AUDIT, \
                                 ::diagnostics::TYPE_PROCEDURE_ENTER, \
                                 ::diagnostics::TYPE_PROCEDURE_EXIT,FSHELL2_NR_WHAT_DEFAULT, \
                                 ::std::string("PROCEDURE=\"") + DIAGNOSTICS_FUNC_NAME + "\" " + (STR_WHAT))

#  define FSHELL2_AUDIT_METHOD_ENTER(STR_WHAT) \
    DIAGNOSTICS_BASE_BLOCK_ENTER(::diagnostics::LEVEL_AUDIT, \
                                 ::diagnostics::TYPE_METHOD_ENTER, \
                                 ::diagnostics::TYPE_METHOD_EXIT,FSHELL2_NR_WHAT_DEFAULT, \
                                 ::std::string("METHOD=\"") + DIAGNOSTICS_FUNC_NAME + "\" " + (STR_WHAT))
#  define FSHELL2_AUDIT_METHOD_EXIT DIAGNOSTICS_BASE_BLOCK_EXIT
#  define FSHELL2_AUDIT_METHOD_GUARD(STR_WHAT) \
    DIAGNOSTICS_BASE_BLOCK_GUARD(::diagnostics::LEVEL_AUDIT, \
                                 ::diagnostics::TYPE_METHOD_ENTER, \
                                 ::diagnostics::TYPE_METHOD_EXIT,FSHELL2_NR_WHAT_DEFAULT, \
                                 ::std::string("METHOD=\"") + DIAGNOSTICS_FUNC_NAME + "\" " + (STR_WHAT))

#  define FSHELL2_AUDIT_ASSERT(NAME,COND) \
    DIAGNOSTICS_BASE_ASSERT(::diagnostics::LEVEL_AUDIT,\
                            FSHELL2_NR_WHAT_DEFAULT,\
                            "CONDITION=\"" #COND "\" EXCEPTION=\"" #NAME "\"",\
                            (COND),\
                            FSHELL2_FAILED_ASSERT_ACTION(::diagnostics::LEVEL_AUDIT,NAME,COND))
#  define FSHELL2_AUDIT_ASSERT1(NAME,COND,EXCEP_WHAT) \
    DIAGNOSTICS_BASE_ASSERT(::diagnostics::LEVEL_AUDIT,\
                            FSHELL2_NR_WHAT_DEFAULT,\
                            ::std::string("CONDITION=\"" #COND "\" EXCEPTION=\"" #NAME "\" WHAT=\"") + (EXCEP_WHAT) + "\"",\
                            (COND),\
                            FSHELL2_FAILED_ASSERT_ACTION1(::diagnostics::LEVEL_AUDIT,NAME,COND,EXCEP_WHAT))
#  define FSHELL2_AUDIT_ASSERT_LOG(NAME,COND) \
    DIAGNOSTICS_BASE_ASSERT(::diagnostics::LEVEL_AUDIT,\
                            FSHELL2_NR_WHAT_DEFAULT,\
                            "CONDITION=\"" #COND "\" EXCEPTION=\"" #NAME "\"",\
                            (COND),\
                            ((void)0))
#  define FSHELL2_AUDIT_ASSERT_LOG1(NAME,COND,EXCEP_WHAT) \
    DIAGNOSTICS_BASE_ASSERT(::diagnostics::LEVEL_AUDIT,\
                            FSHELL2_NR_WHAT_DEFAULT,\
                            ::std::string("CONDITION=\"" #COND "\" EXCEPTION=\"" #NAME "\" WHAT=\"") + (EXCEP_WHAT) + "\"",\
                            (COND),\
                            ((void)0))
#  define FSHELL2_AUDIT_CHECK(NAME,COND) \
    DIAGNOSTICS_BASE_CHECK(::diagnostics::LEVEL_AUDIT,\
                           FSHELL2_NR_WHAT_DEFAULT,\
                           "CONDITION=\"" #COND "\" EXCEPTION=\"" #NAME "\"",\
                           (COND),\
                           FSHELL2_FAILED_CHECK_ACTION(::diagnostics::LEVEL_AUDIT,NAME,COND))
#  define FSHELL2_AUDIT_CHECK1(NAME,COND,STR_WHAT) \
    DIAGNOSTICS_BASE_CHECK(::diagnostics::LEVEL_AUDIT,\
                           FSHELL2_NR_WHAT_DEFAULT,\
                           ::std::string("CONDITION=\"" #COND "\" EXCEPTION=\"" #NAME "\" WHAT=\"") + (STR_WHAT) + "\"",\
                           (COND),\
                           FSHELL2_FAILED_CHECK_ACTION1(::diagnostics::LEVEL_AUDIT,NAME,COND,STR_WHAT))

#  define FSHELL2_AUDIT_CLASS_THROWING_INVARIANCE_ENTER DIAGNOSTICS_BASE_CLASS_THROWING_INVARIANCE_ENTER
#  define FSHELL2_AUDIT_CLASS_THROWING_INVARIANCE_EXIT  \
    DIAGNOSTICS_BASE_CLASS_THROWING_INVARIANCE_EXIT(::diagnostics::LEVEL_AUDIT,\
                                           FSHELL2_NR_WHAT_DEFAULT, \
                                           FSHELL2_HANDLE_TEST_EXCEPTION_EXPLICITLY,\
                                           FSHELL2_BASE_EXCEPTION_TYPE,\
                                           FSHELL2_BASE_EXCEPTION_STR_WHAT)

#  define FSHELL2_AUDIT_CLASS_INVARIANCE_GUARD DIAGNOSTICS_BASE_CLASS_INVARIANCE_GUARD
#  define FSHELL2_AUDIT_CLASS_INVARIANCE_ENTER DIAGNOSTICS_BASE_CLASS_INVARIANCE_ENTER
#  define FSHELL2_AUDIT_CLASS_INVARIANCE_EXIT  DIAGNOSTICS_BASE_CLASS_INVARIANCE_EXIT

#  define FSHELL2_AUDIT_CLASS_INVARIANCE_ASSERT  DIAGNOSTICS_BASE_CLASS_INVARIANCE_ASSERT

#  define FSHELL2_AUDIT_EXCEPTIONLESS_BLOCK_ENTER  DIAGNOSTICS_BASE_EXCEPTIONLESS_BLOCK_ENTER
#  define FSHELL2_AUDIT_EXCEPTIONLESS_BLOCK_EXIT  \
    DIAGNOSTICS_BASE_EXCEPTIONLESS_BLOCK_EXIT(::diagnostics::LEVEL_AUDIT,\
                                              FSHELL2_NR_WHAT_DEFAULT, \
                                              true,\
                                              FSHELL2_HANDLE_TEST_EXCEPTION_EXPLICITLY,\
                                              FSHELL2_BASE_EXCEPTION_TYPE,\
                                              FSHELL2_BASE_EXCEPTION_STR_WHAT)
#  define FSHELL2_AUDIT_EXEC(CMD) CMD
#  define FSHELL2_AUDIT_EXEC_ENTER do { ((void)0)
#  define FSHELL2_AUDIT_EXEC_EXIT  } while(false)

#else

#  define FSHELL2_AUDIT_TRACE(STR_WHAT)        ((void)0)
#  define FSHELL2_AUDIT_BINARY_TRACE(STR_WHAT) ((void)0)
#  define FSHELL2_AUDIT_TRACE1(NR_WHAT,STR_WHAT)        ((void)0)
#  define FSHELL2_AUDIT_BINARY_TRACE1(NR_WHAT,STR_WHAT) ((void)0)

#  define FSHELL2_AUDIT_BLOCK_ENTER(STR_WHAT) do {
#  define FSHELL2_AUDIT_BLOCK_EXIT            } while(false)
#  define FSHELL2_AUDIT_BLOCK_GUARD(STR_WHAT) ((void)0)

#  define FSHELL2_AUDIT_PROCEDURE_ENTER(STR_WHAT) do {
#  define FSHELL2_AUDIT_PROCEDURE_EXIT            } while(false)
#  define FSHELL2_AUDIT_PROCEDURE_GUARD(STR_WHAT) ((void)0)

#  define FSHELL2_AUDIT_METHOD_ENTER(STR_WHAT) do {
#  define FSHELL2_AUDIT_METHOD_EXIT            } while(false)
#  define FSHELL2_AUDIT_METHOD_GUARD(STR_WHAT) ((void)0)

#  define FSHELL2_AUDIT_ASSERT(NAME,COND)           ((void)0)
#  define FSHELL2_AUDIT_ASSERT1(NAME,COND,STR_WHAT) ((void)0)
#  define FSHELL2_AUDIT_ASSERT_LOG(NAME,COND)           ((void)0)
#  define FSHELL2_AUDIT_ASSERT_LOG1(NAME,COND,STR_WHAT) ((void)0)
#  define FSHELL2_AUDIT_CHECK(NAME,COND)            ((void)0)
#  define FSHELL2_AUDIT_CHECK1(NAME,COND,STR_WHAT)  ((void)0)

#  define FSHELL2_AUDIT_CLASS_THROWING_INVARIANCE_ENTER do {
#  define FSHELL2_AUDIT_CLASS_THROWING_INVARIANCE_EXIT  } while(false)

#  define FSHELL2_AUDIT_CLASS_INVARIANCE_GUARD ((void)0)
#  define FSHELL2_AUDIT_CLASS_INVARIANCE_ENTER do {
#  define FSHELL2_AUDIT_CLASS_INVARIANCE_EXIT  } while(false)

#  define FSHELL2_AUDIT_CLASS_INVARIANCE_ASSERT  ((void)0)


#  define FSHELL2_AUDIT_EXCEPTIONLESS_BLOCK_ENTER do {
#  define FSHELL2_AUDIT_EXCEPTIONLESS_BLOCK_EXIT  } while(false)

#  define FSHELL2_AUDIT_EXEC(CMD) ((void)0)
#  define FSHELL2_AUDIT_EXEC_ENTER do { if(false) do { ((void)0)
#  define FSHELL2_AUDIT_EXEC_EXIT  } while(false); } while(false)

#endif


////////////////////////////////////////////////////////////////////////////////

#if FSHELL2_DEBUG__LEVEL__ > 0

#  define FSHELL2_DEBUG_TRACE(STR_WHAT) \
    DIAGNOSTICS_BASE_LOG(::diagnostics::LEVEL_DEBUG,::diagnostics::TYPE_TRACE,FSHELL2_NR_WHAT_DEFAULT,(STR_WHAT))
#  define FSHELL2_DEBUG_BINARY_TRACE(STR_WHAT) \
    DIAGNOSTICS_BASE_LOG(::diagnostics::LEVEL_DEBUG,::diagnostics::TYPE_TRACE_BINARY,FSHELL2_NR_WHAT_DEFAULT,(STR_WHAT))

#  define FSHELL2_DEBUG_BLOCK_ENTER(STR_WHAT)  \
    DIAGNOSTICS_BASE_BLOCK_ENTER(::diagnostics::LEVEL_DEBUG, \
                                 ::diagnostics::TYPE_BLOCK_ENTER, \
                                 ::diagnostics::TYPE_BLOCK_EXIT,FSHELL2_NR_WHAT_DEFAULT,(STR_WHAT))
#  define FSHELL2_DEBUG_BLOCK_EXIT DIAGNOSTICS_BASE_BLOCK_EXIT
#  define FSHELL2_DEBUG_BLOCK_GUARD(STR_WHAT)  \
    DIAGNOSTICS_BASE_BLOCK_GUARD(::diagnostics::LEVEL_DEBUG, \
                                 ::diagnostics::TYPE_BLOCK_ENTER, \
                                 ::diagnostics::TYPE_BLOCK_EXIT,FSHELL2_NR_WHAT_DEFAULT,(STR_WHAT))

#  define FSHELL2_DEBUG_PROCEDURE_ENTER(STR_WHAT) \
    DIAGNOSTICS_BASE_BLOCK_ENTER(::diagnostics::LEVEL_DEBUG, \
                                 ::diagnostics::TYPE_PROCEDURE_ENTER, \
                                 ::diagnostics::TYPE_PROCEDURE_EXIT,FSHELL2_NR_WHAT_DEFAULT, \
                                 ::std::string("PROCEDURE=\"" DIAGNOSTICS_FUNC_NAME "\" ") + (STR_WHAT))
#  define FSHELL2_DEBUG_PROCEDURE_EXIT DIAGNOSTICS_BASE_BLOCK_EXIT
#  define FSHELL2_DEBUG_PROCEDURE_GUARD(STR_WHAT) \
    DIAGNOSTICS_BASE_BLOCK_GUARD(::diagnostics::LEVEL_DEBUG, \
                                 ::diagnostics::TYPE_PROCEDURE_ENTER, \
                                 ::diagnostics::TYPE_PROCEDURE_EXIT,FSHELL2_NR_WHAT_DEFAULT, \
                                 ::std::string("PROCEDURE=\"" DIAGNOSTICS_FUNC_NAME "\" ") + (STR_WHAT))

#  define FSHELL2_DEBUG_METHOD_ENTER(STR_WHAT) \
    DIAGNOSTICS_BASE_BLOCK_ENTER(::diagnostics::LEVEL_DEBUG, \
                                 ::diagnostics::TYPE_METHOD_ENTER, \
                                 ::diagnostics::TYPE_METHOD_EXIT,FSHELL2_NR_WHAT_DEFAULT, \
                                 ::std::string("METHOD=\"" DIAGNOSTICS_FUNC_NAME "\" ") + (STR_WHAT))
#  define FSHELL2_DEBUG_METHOD_EXIT DIAGNOSTICS_BASE_BLOCK_EXIT
#  define FSHELL2_DEBUG_METHOD_GUARD(STR_WHAT) \
    DIAGNOSTICS_BASE_BLOCK_GUARD(::diagnostics::LEVEL_DEBUG, \
                                 ::diagnostics::TYPE_METHOD_ENTER, \
                                 ::diagnostics::TYPE_METHOD_EXIT,FSHELL2_NR_WHAT_DEFAULT, \
                                 ::std::string("METHOD=\"" DIAGNOSTICS_FUNC_NAME "\" ") + (STR_WHAT))

#  define FSHELL2_DEBUG_ASSERT(NAME,COND) \
    DIAGNOSTICS_BASE_ASSERT(::diagnostics::LEVEL_DEBUG,\
                            FSHELL2_NR_WHAT_DEFAULT,\
                            "CONDITION=\"" #COND "\" EXCEPTION=\"" #NAME "\"",\
                            (COND),\
                            FSHELL2_FAILED_ASSERT_ACTION(::diagnostics::LEVEL_DEBUG,NAME,COND))
#  define FSHELL2_DEBUG_ASSERT1(NAME,COND,STR_WHAT) \
    DIAGNOSTICS_BASE_ASSERT(::diagnostics::LEVEL_DEBUG,\
                            FSHELL2_NR_WHAT_DEFAULT,\
                            ::std::string("CONDITION=\"" #COND "\" EXCEPTION=\"" #NAME "\" WHAT=\"") + (STR_WHAT) + "\"",\
                            (COND),\
                            FSHELL2_FAILED_ASSERT_ACTION1(::diagnostics::LEVEL_DEBUG,NAME,COND,STR_WHAT))
#  define FSHELL2_DEBUG_CHECK(NAME,COND) \
    DIAGNOSTICS_BASE_CHECK(::diagnostics::LEVEL_DEBUG,\
                           FSHELL2_NR_WHAT_DEFAULT,\
                           "CONDITION=\"" #COND "\" EXCEPTION=\"" #NAME "\"",\
                           (COND),\
                           FSHELL2_FAILED_CHECK_ACTION(::diagnostics::LEVEL_DEBUG,NAME,COND))
#  define FSHELL2_DEBUG_CHECK1(NAME,COND,STR_WHAT) \
    DIAGNOSTICS_BASE_CHECK(::diagnostics::LEVEL_DEBUG,\
                           FSHELL2_NR_WHAT_DEFAULT,\
                           ::std::string("CONDITION=\"" #COND "\" EXCEPTION=\"" #NAME "\" WHAT=\"") + (STR_WHAT) + "\"",\
                           (COND),\
                           FSHELL2_FAILED_CHECK_ACTION1(::diagnostics::LEVEL_DEBUG,NAME,COND,STR_WHAT))

#  define FSHELL2_DEBUG_CLASS_THROWING_INVARIANCE_ENTER DIAGNOSTICS_BASE_CLASS_THROWING_INVARIANCE_ENTER
#  define FSHELL2_DEBUG_CLASS_THROWING_INVARIANCE_EXIT  \
    DIAGNOSTICS_BASE_CLASS_THROWING_INVARIANCE_EXIT(::diagnostics::LEVEL_DEBUG,\
                                           FSHELL2_NR_WHAT_DEFAULT, \
                                           FSHELL2_HANDLE_TEST_EXCEPTION_EXPLICITLY,\
                                           FSHELL2_BASE_EXCEPTION_TYPE,\
                                           FSHELL2_BASE_EXCEPTION_STR_WHAT)

#  define FSHELL2_DEBUG_CLASS_INVARIANCE_GUARD DIAGNOSTICS_BASE_CLASS_INVARIANCE_GUARD
#  define FSHELL2_DEBUG_CLASS_INVARIANCE_ENTER DIAGNOSTICS_BASE_CLASS_INVARIANCE_ENTER
#  define FSHELL2_DEBUG_CLASS_INVARIANCE_EXIT  DIAGNOSTICS_BASE_CLASS_INVARIANCE_EXIT

#  define FSHELL2_DEBUG_CLASS_INVARIANCE_ASSERT DIAGNOSTICS_BASE_CLASS_INVARIANCE_ASSERT


#  define FSHELL2_DEBUG_EXCEPTIONLESS_BLOCK_ENTER  DIAGNOSTICS_BASE_EXCEPTIONLESS_BLOCK_ENTER
#  define FSHELL2_DEBUG_EXCEPTIONLESS_BLOCK_EXIT  \
    DIAGNOSTICS_BASE_EXCEPTIONLESS_BLOCK_EXIT(::diagnostics::LEVEL_DEBUG,\
                                              FSHELL2_NR_WHAT_DEFAULT, \
                                              true,\
                                              FSHELL2_HANDLE_TEST_EXCEPTION_EXPLICITLY,\
                                              FSHELL2_BASE_EXCEPTION_TYPE,\
                                              FSHELL2_BASE_EXCEPTION_STR_WHAT)

#  define FSHELL2_DEBUG_EXEC(CMD) CMD
#  define FSHELL2_DEBUG_EXEC_ENTER do { ((void)0)
#  define FSHELL2_DEBUG_EXEC_EXIT  } while(false)

#else

#  define FSHELL2_DEBUG_TRACE(STR_WHAT)        ((void)0)
#  define FSHELL2_DEBUG_BINARY_TRACE(STR_WHAT) ((void)0)

#  define FSHELL2_DEBUG_BLOCK_ENTER(STR_WHAT) do {
#  define FSHELL2_DEBUG_BLOCK_EXIT            } while(false)
#  define FSHELL2_DEBUG_BLOCK_GUARD(STR_WHAT) ((void)0)

#  define FSHELL2_DEBUG_PROCEDURE_ENTER(STR_WHAT) do {
#  define FSHELL2_DEBUG_PROCEDURE_EXIT            } while(false)
#  define FSHELL2_DEBUG_PROCEDURE_GUARD(STR_WHAT) ((void)0)

#  define FSHELL2_DEBUG_METHOD_ENTER(STR_WHAT) do {
#  define FSHELL2_DEBUG_METHOD_EXIT            } while(false)
#  define FSHELL2_DEBUG_METHOD_GUARD(STR_WHAT) ((void)0)

#  define FSHELL2_DEBUG_ASSERT(NAME,COND)           ((void)0)
#  define FSHELL2_DEBUG_ASSERT1(NAME,COND,STR_WHAT) ((void)0)
#  define FSHELL2_DEBUG_CHECK(NAME,COND)            ((void)0)
#  define FSHELL2_DEBUG_CHECK1(NAME,COND,STR_WHAT)  ((void)0)

#  define FSHELL2_DEBUG_CLASS_THROWING_INVARIANCE_ENTER do {
#  define FSHELL2_DEBUG_CLASS_THROWING_INVARIANCE_EXIT  } while(false)

#  define FSHELL2_DEBUG_CLASS_INVARIANCE_GUARD ((void)0)
#  define FSHELL2_DEBUG_CLASS_INVARIANCE_ENTER do {
#  define FSHELL2_DEBUG_CLASS_INVARIANCE_EXIT  } while(false)

#  define FSHELL2_DEBUG_CLASS_INVARIANCE_ASSERT  ((void)0)


#  define FSHELL2_DEBUG_EXCEPTIONLESS_BLOCK_ENTER do {
#  define FSHELL2_DEBUG_EXCEPTIONLESS_BLOCK_EXIT  } while(false)

#  define FSHELL2_DEBUG_EXEC(CMD) ((void)0)
#  define FSHELL2_DEBUG_EXEC_ENTER do { if(false) do { ((void)0)
#  define FSHELL2_DEBUG_EXEC_EXIT  } while(false); } while(false)

#endif


////////////////////////////////////////////////////////////////////////////////

#if FSHELL2_DEBUG__LEVEL__ >= 0

#define FSHELL2_PROD_TRACE(STR_WHAT) \
    DIAGNOSTICS_BASE_LOG(::diagnostics::LEVEL_PROD,::diagnostics::TYPE_TRACE,FSHELL2_NR_WHAT_DEFAULT,(STR_WHAT))
#define FSHELL2_PROD_BINARY_TRACE(STR_WHAT) \
    DIAGNOSTICS_BASE_LOG(::diagnostics::LEVEL_PROD,::diagnostics::TYPE_TRACE_BINARY,FSHELL2_NR_WHAT_DEFAULT,(STR_WHAT))

#define FSHELL2_PROD_TRACE1(NR_WHAT,STR_WHAT) \
    DIAGNOSTICS_BASE_LOG(::diagnostics::LEVEL_PROD,::diagnostics::TYPE_TRACE,(NR_WHAT),(STR_WHAT))
#define FSHELL2_PROD_BINARY_TRACE1(NR_WHAT,STR_WHAT) \
    DIAGNOSTICS_BASE_LOG(::diagnostics::LEVEL_PROD,::diagnostics::TYPE_TRACE_BINARY,(NR_WHAT),(STR_WHAT))


#define FSHELL2_PROD_BLOCK_ENTER(STR_WHAT)  \
    DIAGNOSTICS_BASE_BLOCK_ENTER(::diagnostics::LEVEL_PROD, \
                                 ::diagnostics::TYPE_BLOCK_ENTER, \
                                 ::diagnostics::TYPE_BLOCK_EXIT,FSHELL2_NR_WHAT_DEFAULT,(STR_WHAT))
#define FSHELL2_PROD_BLOCK_EXIT DIAGNOSTICS_BASE_BLOCK_EXIT
#define FSHELL2_PROD_BLOCK_GUARD(STR_WHAT)  \
    DIAGNOSTICS_BASE_BLOCK_GUARD(::diagnostics::LEVEL_PROD, \
                                 ::diagnostics::TYPE_BLOCK_ENTER, \
                                 ::diagnostics::TYPE_BLOCK_EXIT,FSHELL2_NR_WHAT_DEFAULT,(STR_WHAT))

#define FSHELL2_PROD_PROCEDURE_ENTER(STR_WHAT) \
    DIAGNOSTICS_BASE_BLOCK_ENTER(::diagnostics::LEVEL_PROD, \
                                 ::diagnostics::TYPE_PROCEDURE_ENTER, \
                                 ::diagnostics::TYPE_PROCEDURE_EXIT,FSHELL2_NR_WHAT_DEFAULT, \
                                 ::std::string("PROCEDURE=\"" DIAGNOSTICS_FUNC_NAME "\" ") + (STR_WHAT))
#define FSHELL2_PROD_PROCEDURE_EXIT DIAGNOSTICS_BASE_BLOCK_EXIT
#define FSHELL2_PROD_PROCEDURE_GUARD(STR_WHAT) \
    DIAGNOSTICS_BASE_BLOCK_GUARD(::diagnostics::LEVEL_PROD, \
                                 ::diagnostics::TYPE_PROCEDURE_ENTER, \
                                 ::diagnostics::TYPE_PROCEDURE_EXIT,FSHELL2_NR_WHAT_DEFAULT, \
                                 ::std::string("PROCEDURE=\"" DIAGNOSTICS_FUNC_NAME "\" ") + (STR_WHAT))

#define FSHELL2_PROD_METHOD_ENTER(STR_WHAT) \
    DIAGNOSTICS_BASE_BLOCK_ENTER(::diagnostics::LEVEL_PROD, \
                                 ::diagnostics::TYPE_METHOD_ENTER, \
                                 ::diagnostics::TYPE_METHOD_EXIT,FSHELL2_NR_WHAT_DEFAULT, \
                                 ::std::string("METHOD=\"" DIAGNOSTICS_FUNC_NAME "\" ") + (STR_WHAT))
#define FSHELL2_PROD_METHOD_EXIT DIAGNOSTICS_BASE_BLOCK_EXIT
#define FSHELL2_PROD_METHOD_GUARD(STR_WHAT) \
    DIAGNOSTICS_BASE_BLOCK_GUARD(::diagnostics::LEVEL_PROD, \
                                 ::diagnostics::TYPE_METHOD_ENTER, \
                                 ::diagnostics::TYPE_METHOD_EXIT,FSHELL2_NR_WHAT_DEFAULT, \
                                 ::std::string("METHOD=\"" DIAGNOSTICS_FUNC_NAME "\" ") + (STR_WHAT))

#define FSHELL2_PROD_ASSERT(NAME,COND) \
    DIAGNOSTICS_BASE_ASSERT(::diagnostics::LEVEL_PROD,\
                            FSHELL2_NR_WHAT_DEFAULT,\
                            "CONDITION=\"" #COND "\" EXCEPTION=\"" #NAME "\"",\
                            (COND),\
                            FSHELL2_FAILED_ASSERT_ACTION(::diagnostics::LEVEL_PROD,NAME,COND))
#define FSHELL2_PROD_ASSERT1(NAME,COND,STR_WHAT) \
    DIAGNOSTICS_BASE_ASSERT(::diagnostics::LEVEL_PROD,\
                            FSHELL2_NR_WHAT_DEFAULT,\
                            ::std::string("CONDITION=\"" #COND "\" EXCEPTION=\"" #NAME "\" WHAT=\"") + (STR_WHAT) + "\"",\
                            (COND),\
                            FSHELL2_FAILED_ASSERT_ACTION1(::diagnostics::LEVEL_PROD,NAME,COND,STR_WHAT))

#define FSHELL2_PROD_CLASS_THROWING_INVARIANCE_ENTER DIAGNOSTICS_BASE_CLASS_THROWING_INVARIANCE_ENTER
#define FSHELL2_PROD_CLASS_THROWING_INVARIANCE_EXIT  \
    DIAGNOSTICS_BASE_CLASS_THROWING_INVARIANCE_EXIT(::diagnostics::LEVEL_PROD,\
                                           FSHELL2_NR_WHAT_DEFAULT, \
                                           FSHELL2_HANDLE_TEST_EXCEPTION_EXPLICITLY,\
                                           FSHELL2_BASE_EXCEPTION_TYPE,\
                                           FSHELL2_BASE_EXCEPTION_STR_WHAT)

#define FSHELL2_PROD_CLASS_INVARIANCE_GUARD DIAGNOSTICS_BASE_CLASS_INVARIANCE_GUARD
#define FSHELL2_PROD_CLASS_INVARIANCE_ENTER DIAGNOSTICS_BASE_CLASS_INVARIANCE_ENTER
#define FSHELL2_PROD_CLASS_INVARIANCE_EXIT  DIAGNOSTICS_BASE_CLASS_INVARIANCE_EXIT

#define FSHELL2_PROD_CLASS_INVARIANCE_ASSERT  DIAGNOSTICS_BASE_CLASS_INVARIANCE_ASSERT

#define FSHELL2_PROD_EXCEPTIONLESS_BLOCK_ENTER  DIAGNOSTICS_BASE_EXCEPTIONLESS_BLOCK_ENTER
#define FSHELL2_PROD_EXCEPTIONLESS_BLOCK_EXIT  \
    DIAGNOSTICS_BASE_EXCEPTIONLESS_BLOCK_EXIT(::diagnostics::LEVEL_PROD,\
                                              FSHELL2_NR_WHAT_DEFAULT, \
                                              true,\
                                              FSHELL2_HANDLE_TEST_EXCEPTION_EXPLICITLY,\
                                              FSHELL2_BASE_EXCEPTION_TYPE,\
                                              FSHELL2_BASE_EXCEPTION_STR_WHAT)

#else

#  define FSHELL2_PROD_TRACE(STR_WHAT)        ((void)0)
#  define FSHELL2_PROD_BINARY_TRACE(STR_WHAT) ((void)0)

#  define FSHELL2_PROD_BLOCK_ENTER(STR_WHAT) do {
#  define FSHELL2_PROD_BLOCK_EXIT            } while(false)
#  define FSHELL2_PROD_BLOCK_GUARD(STR_WHAT) ((void)0)

#  define FSHELL2_PROD_PROCEDURE_ENTER(STR_WHAT) do {
#  define FSHELL2_PROD_PROCEDURE_EXIT            } while(false)
#  define FSHELL2_PROD_PROCEDURE_GUARD(STR_WHAT) ((void)0)

#  define FSHELL2_PROD_METHOD_ENTER(STR_WHAT) do {
#  define FSHELL2_PROD_METHOD_EXIT            } while(false)
#  define FSHELL2_PROD_METHOD_GUARD(STR_WHAT) ((void)0)

#  define FSHELL2_PROD_ASSERT(NAME,COND)           if( ! ( COND ) ) throw NAME ( #COND )
#  define FSHELL2_PROD_ASSERT1(NAME,COND,STR_WHAT) if( ! ( COND ) ) throw NAME ( STR_WHAT )

#  define FSHELL2_PROD_CLASS_THROWING_INVARIANCE_ENTER do {
#  define FSHELL2_PROD_CLASS_THROWING_INVARIANCE_EXIT  } while(false)

#  define FSHELL2_PROD_CLASS_INVARIANCE_GUARD ((void)0)
#  define FSHELL2_PROD_CLASS_INVARIANCE_ENTER do {
#  define FSHELL2_PROD_CLASS_INVARIANCE_EXIT  } while(false)

#  define FSHELL2_PROD_CLASS_INVARIANCE_ASSERT  ((void)0)


#  define FSHELL2_PROD_EXCEPTIONLESS_BLOCK_ENTER do {
#  define FSHELL2_PROD_EXCEPTIONLESS_BLOCK_EXIT  } while(false)

#endif


#if FSHELL2_DEBUG__LEVEL__ > 0

#  define FSHELL2_PROD_CHECK(NAME,COND) \
    DIAGNOSTICS_BASE_CHECK(::diagnostics::LEVEL_PROD,\
                           FSHELL2_NR_WHAT_DEFAULT,\
                           "CONDITION=\"" #COND "\" EXCEPTION=\"" #NAME "\"",\
                           (COND),\
                           FSHELL2_FAILED_CHECK_ACTION(::diagnostics::LEVEL_PROD,NAME,COND))
#  define FSHELL2_PROD_CHECK1(NAME,COND,STR_WHAT) \
    DIAGNOSTICS_BASE_CHECK(::diagnostics::LEVEL_PROD,\
                           FSHELL2_NR_WHAT_DEFAULT,\
                           ::std::string("CONDITION=\"" #COND "\" EXCEPTION=\"" #NAME "\" WHAT=\"") + (STR_WHAT) + "\"",\
                           (COND),\
                           FSHELL2_FAILED_CHECK_ACTION1(::diagnostics::LEVEL_PROD,NAME,COND,STR_WHAT))

#else

#  define FSHELL2_PROD_CHECK(NAME,COND) \
    do { if(!(COND)) { FSHELL2_FAILED_CHECK_ACTION(::diagnostics::LEVEL_PROD,NAME,COND); } } while(false)
#  define FSHELL2_PROD_CHECK1(NAME,COND,STR_WHAT) \
    do { if(!(COND)) { FSHELL2_FAILED_CHECK_ACTION1(::diagnostics::LEVEL_PROD,NAME,COND,STR_WHAT); } } while(false)

#endif

#define FSHELL2_METHOD_GUARD(STR_WHAT) FSHELL2_AUDIT_METHOD_GUARD(STR_WHAT); FSHELL2_AUDIT_CLASS_INVARIANCE_GUARD 


#define FSHELL2_AUDIT_ASSERT_RELATION(NAME,OP1,REL,OP2) \
       DIAGNOSTICS_RELATION(FSHELL2_AUDIT_ASSERT1,NAME,OP1,REL,OP2)
#define FSHELL2_AUDIT_CHECK_RELATION(NAME,OP1,REL,OP2) \
       DIAGNOSTICS_RELATION(FSHELL2_AUDIT_CHECK1,NAME,OP1,REL,OP2)

#define FSHELL2_DEBUG_ASSERT_RELATION(NAME,OP1,REL,OP2) \
       DIAGNOSTICS_RELATION(FSHELL2_DEBUG_ASSERT1,NAME,OP1,REL,OP2)
#define FSHELL2_DEBUG_CHECK_RELATION(NAME,OP1,REL,OP2) \
       DIAGNOSTICS_RELATION(FSHELL2_DEBUG_CHECK1,NAME,OP1,REL,OP2)

#define FSHELL2_PROD_ASSERT_RELATION(NAME,OP1,REL,OP2) \
       DIAGNOSTICS_RELATION(FSHELL2_PROD_ASSERT1,NAME,OP1,REL,OP2)
#define FSHELL2_PROD_CHECK_RELATION(NAME,OP1,REL,OP2) \
       DIAGNOSTICS_RELATION(FSHELL2_PROD_CHECK1,NAME,OP1,REL,OP2)


#endif /* FSHELL2__CONFIG__ANNOTATIONS_HPP */
