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

#ifndef FSHELL2__CONFIG__CONFIG_HPP
#define FSHELL2__CONFIG__CONFIG_HPP

////////////////////////////////////////////////////////////////////////////////
/**
 * @file fshell2/config/config.hpp
 *
 * @brief Definition of the namespace macros and general defines.
 *
 * This file must be included by all components.
 *
 *
 * $Id$
 *
 * @author Michael Tautschnig <tautschn@model.in.tum.de>
 * @date   Wed Sep 07 23:27:30 CEST 2005
*/

/** @cond  */ 
#ifndef DUMMY_FUNC
#define DUMMY_FUNC void dummy_for_semicolon_do_not_use()
#endif
/** @endcond */

////////////////////////////////////////////////////////////////////////////////
/**
 * @brief Use this macro to start the @ref fshell2_namespace "fshell2" 
 * namespace.
 *
 * Always put a ; after the macro, it includes some dummy code to keep the
 * compiler from warning about useless &ldquo;;&rdquo; (copied from diagnostics,
 * credits go to Christian Schallhart).
 *
 * @namespace fshell2 @anchor fshell2_namespace
 *
*/
#define FSHELL2_NAMESPACE_BEGIN namespace fshell2 { DUMMY_FUNC

////////////////////////////////////////////////////////////////////////////////
/**
 * @brief Macro to end the @ref fshell2_namespace "fshell2" namespace.
 *
*/
#define FSHELL2_NAMESPACE_END } DUMMY_FUNC

////////////////////////////////////////////////////////////////////////////////
/**
 * @brief Use this macro to start the @ref fshell2_command_namespace "command" 
 * namespace.
 *
 * @namespace fshell2::command @anchor fshell2_command_namespace
 *
*/
#define FSHELL2_COMMAND_NAMESPACE_BEGIN namespace command { DUMMY_FUNC

////////////////////////////////////////////////////////////////////////////////
/**
 * @brief Macro to end the @ref fshell2_command_namespace "command" namespace.
 *
*/
#define FSHELL2_COMMAND_NAMESPACE_END } DUMMY_FUNC

////////////////////////////////////////////////////////////////////////////////
/**
 * @brief Use this macro to start the @ref fshell2_macro_namespace "macro" 
 * namespace.
 *
 * @namespace fshell2::macro @anchor fshell2_macro_namespace
 *
*/
#define FSHELL2_MACRO_NAMESPACE_BEGIN namespace macro { DUMMY_FUNC

////////////////////////////////////////////////////////////////////////////////
/**
 * @brief Macro to end the @ref fshell2_macro_namespace "macro" namespace.
 *
*/
#define FSHELL2_MACRO_NAMESPACE_END } DUMMY_FUNC

////////////////////////////////////////////////////////////////////////////////
/**
 * @brief Use this macro to start the @ref fshell2_instrumentation_namespace "instrumentation" 
 * namespace.
 *
 * @namespace fshell2::instrumentation @anchor fshell2_instrumentation_namespace
 *
*/
#define FSHELL2_INSTRUMENTATION_NAMESPACE_BEGIN namespace instrumentation { DUMMY_FUNC

////////////////////////////////////////////////////////////////////////////////
/**
 * @brief instrumentation to end the @ref fshell2_instrumentation_namespace "instrumentation" namespace.
 *
*/
#define FSHELL2_INSTRUMENTATION_NAMESPACE_END } DUMMY_FUNC

////////////////////////////////////////////////////////////////////////////////
/**
 * @brief Use this macro to start the @ref fshell2_statistics_namespace "statistics" 
 * namespace.
 *
 * @namespace fshell2::statistics @anchor fshell2_statistics_namespace
 *
*/
#define FSHELL2_STATISTICS_NAMESPACE_BEGIN namespace statistics { DUMMY_FUNC

////////////////////////////////////////////////////////////////////////////////
/**
 * @brief statistics to end the @ref fshell2_statistics_namespace "statistics" namespace.
 *
*/
#define FSHELL2_STATISTICS_NAMESPACE_END } DUMMY_FUNC

////////////////////////////////////////////////////////////////////////////////
/**
 * @brief Use this macro to start the @ref fshell2_fql_namespace "fql" 
 * namespace.
 *
 * @namespace fshell2::fql @anchor fshell2_fql_namespace
 *
*/
#define FSHELL2_FQL_NAMESPACE_BEGIN namespace fql { DUMMY_FUNC

////////////////////////////////////////////////////////////////////////////////
/**
 * @brief Macro to end the @ref fshell2_fql_namespace "fql" namespace.
 *
*/
#define FSHELL2_FQL_NAMESPACE_END } DUMMY_FUNC

////////////////////////////////////////////////////////////////////////////////
/**
 * @brief Use this macro to start the @ref fshell2_fql_parser_namespace "parser" 
 * namespace.
 *
 * @namespace fshell2::fql::parser @anchor fshell2_fql_parser_namespace
 *
*/
#define FSHELL2_FQL_PARSER_NAMESPACE_BEGIN namespace parser { DUMMY_FUNC

////////////////////////////////////////////////////////////////////////////////
/**
 * @brief Macro to end the @ref fshell2_fql_parser_namespace "parser" namespace.
 *
*/
#define FSHELL2_FQL_PARSER_NAMESPACE_END } DUMMY_FUNC

////////////////////////////////////////////////////////////////////////////////
/**
 * @brief Macro for no throws
 *
 * Depending on the standard library, it is necessary to declare some methods
 * and procedures with a throw() (e.g. ::std::exception::what). Sometimes,
 * one can omit the throw(). 
 *
 * In general, we do not use throw declaration, since they cause code
 * to be emitted at each call of the method. Thus, if we can, we
 * define the following macro as empty -- if this does not work, we
 * define it as throw().
 */ 
#define FSHELL2_NO_THROW throw()


////////////////////////////////////////////////////////////////////////////////
/**
 * @mainpage Introduction to FShell2
 *
 * @section intro Introduction to FShell2
 *
 * The FShell2 test case generation engine
 *
 * &copy; 2009 Michael Tautschnig
 *
*/
   
////////////////////////////////////////////////////////////////////////////////
/**
 * @addtogroup copy_constructor 
 * 
 * @{
 * @brief Copy constructor. Not implemented.
 *
 * @param rhs Source object
 * @}
 *
 * @addtogroup assignment_op
 * 
 * @{
 * @brief Assignment operator. Not implemented.
 *
 * @param rhs Source object
 *
 * @return Reference to modified Variable object. 
 * @}
 *
 * @addtogroup doc_self
 *
 * @{
 * @brief Just a shorthand to save some typing
 * @}
 * 
*/

#endif /* FSHELL2__CONFIG__CONFIG_HPP */
