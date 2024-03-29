dnl Process this file with autoconf 2.52+ to produce a configure script.
dnl
dnl Copyright (C) 2001 Philipp Rumpf
dnl Copyright (C) 2004 Henrique de Moraes Holschuh <hmh@debian.org>
dnl
dnl This program is free software; you can redistribute it and/or modify
dnl it under the terms of the GNU General Public License as published by
dnl the Free Software Foundation; either version 2 of the License, or
dnl (at your option) any later version.
dnl
dnl This program is distributed in the hope that it will be useful,
dnl but WITHOUT ANY WARRANTY; without even the implied warranty of
dnl MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
dnl GNU General Public License for more details.
dnl
dnl You should have received a copy of the GNU General Public License
dnl along with this program; if not, write to the Free Software
dnl Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

AC_PREREQ([2.71])
AC_INIT([jup],[0.0.6],[Jeff Garzik <jgarzik@pobox.com>])
AC_CONFIG_SRCDIR([src/jup.cc])
AC_CONFIG_HEADERS([jup-config.h])
AC_CONFIG_AUX_DIR([build-aux])
AC_CONFIG_MACRO_DIR([build-aux/m4])
AX_SUBDIRS_CONFIGURE([univalue], [--disable-shared])

AC_CANONICAL_HOST

AM_INIT_AUTOMAKE([subdir-objects foreign])
AM_MAINTAINER_MODE([enable])

dnl Checks for programs
AC_PROG_CC
AC_PROG_CXX
AC_PROG_RANLIB
dnl Require C++11 compiler (no GNU extensions)
AX_CXX_COMPILE_STDCXX([11], [noext], [mandatory], [nodefault])

dnl Checks for header files.
dnl AC_CHECK_HEADERS(sys/ioctl.h unistd.h)

dnl Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_SIZE_T
AC_TYPE_PID_T

dnl -----------------------------
dnl Checks for required libraries
dnl -----------------------------
dnl AC_CHECK_LIB(gssrpc, gssrpc_svc_register, GSSRPC_LIBS=-lgssrpc, exit 1)

AC_CHECK_LIB(argp, argp_parse, ARGP_LIBS=-largp)

AC_LANG(C++)

dnl -------------------------------------
dnl Checks for optional library functions
dnl -------------------------------------
dnl AC_CHECK_FUNCS(fdatasync lseek64 srand48_r xdr_u_quad_t)

dnl -----------------
dnl Configure options
dnl -----------------

dnl --------------------------
dnl autoconf output generation
dnl --------------------------

dnl AC_SUBST(GSSRPC_LIBS)
dnl AC_SUBST(DB4_LIBS)
dnl AC_SUBST(EVENT_LIBS)
AC_SUBST(ARGP_LIBS)

AC_CONFIG_FILES([Makefile])
AC_OUTPUT

