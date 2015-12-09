###############################################################################
## [The "BSD license"]
## Copyright (c) 2012-2013 Marcel Schaible
## All rights reserved.
##
## Redistribution and use in source and binary forms, with or without
## modification, are permitted provided that the following conditions
## are met:
##
## 1. Redistributions of source code must retain the above copyright
##    notice, this list of conditions and the following disclaimer.
## 2. Redistributions in binary form must reproduce the above copyright
##    notice, this list of conditions and the following disclaimer in the
##    documentation and/or other materials provided with the distribution.
## 3. The name of the author may not be used to endorse or promote products
##    derived from this software without specific prior written permission.
##
## THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
## IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
## OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
## IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
## INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
## NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
## DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
## THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
## (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
## THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
###############################################################################

# common definitionss
CC	     = gcc
PRJCFLAGS    = -g
LD	     = gcc
LDFLAGS	     =
AR	     = ar
ARFLAGS      =
RANLIB	     = ranlib
#RM	     = rm
ECHO	     = echo
SHELL	     = /bin/sh
# ANTLR is obsolete -> use $(CONFIG_COMPILER_ANTLR) from menuconfig tool
#ANTLR        = /usr/local/lib/antlr-4.5-complete.jar
#ANTLR       = /opt/antlr/antlr.jar

################################
# Where to install the compiler:
################################
#
# obsolete --> use $(CONFIG_INSTALL_Target) from menuconfig tool
#
#ifndef OPENPEARL_INSTALL_DIR
#       INSTALL_DIR  = /usr/local/bin
#else
#       INSTALL_DIR = $(OPENPEARL_INSTALL_DIR)
#endif

#.SILENT :
