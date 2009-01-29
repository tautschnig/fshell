/*******************************************************************************
 * PACKAGE, PACKAGE_DESCRIPTION
 * Copyright (C) YEAR  AUTHORS
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *******************************************************************************/

#ifndef TEST__CONFIG_HPP
#define TEST__CONFIG_HPP

////////////////////////////////////////////////////////////////////////////////
/**
 * @file test/config.hpp
 *
 * @brief Definition of the namespace macro for unit tests
 *
 *
 * $Id: config.hpp 1 2006-11-14 21:48:14Z tautschn $
 *
 * @author Michael Tautschnig <tautschn@model.in.tum.de>
 * @date Thu Nov  3 17:23:48 CET 2005
*/

/** @cond  */ 
#ifndef DUMMY_FUNC
#define DUMMY_FUNC void dummy_for_semicolon_do_not_use()
#endif
/** @endcond */

////////////////////////////////////////////////////////////////////////////////
/**
 * @brief Use this macro to start the @ref test_namespace "test" namespace.
 *
 * Always put a ; after the macro, it includes some dummy code to keep the
 * compiler from warning about useless &ldquo;;&rdquo; (copied from diagnostics,
 * credits go to Christian Schallhart).
 *
 * @namespace test @anchor test_namespace
 *
 * Code for the unit test environment (not the tests itself) should be place
 * inside this namespace
 *
*/
#define TEST_NAMESPACE_BEGIN namespace test { DUMMY_FUNC

////////////////////////////////////////////////////////////////////////////////
/**
 * @brief Macro to end the @ref test_namespace "test" namespace.
 *
*/
#define TEST_NAMESPACE_END } DUMMY_FUNC

#endif /* TEST__CONFIG_HPP */
