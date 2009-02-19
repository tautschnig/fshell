/* -*- Mode: C++; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 noexpandtab: */

/* LICENSE TBD */

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
 * $Id: config.hpp 1384 2009-02-18 12:33:41Z michaelz $
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
 * @namespace fortas @anchor fshell2_namespace
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
