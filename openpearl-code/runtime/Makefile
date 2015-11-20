#/*
# [The "BSD license"]
# Copyright (c) 2015 Rainer Mueller
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
#
# 1. Redistributions of source code must retain the above copyright
#    notice, this list of conditions and the following disclaimer.
# 2. Redistributions in binary form must reproduce the above copyright
#    notice, this list of conditions and the following disclaimer in the
#    documentation and/or other materials provided with the distribution.
# 3. The name of the author may not be used to endorse or promote products
#    derived from this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
# IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
# OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
# IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
# INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
# NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
# DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
# THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
# THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#*/

# pearl runtime system

targets :=

include ../configuration/.config

ifeq ($(CONFIG_LINUX),y) 
  phony += linux
  targets += linux
  $(warning targets = $(targets))
endif

# helper commands include composer, deleted white spaces
IC_DIR = ./includeComposer

.PHONY: clean depend help all $(targets)

all: $(targets)

linux:
	(cd linux; make;)

help:
	@echo "-----------------"
	@echo "make all         : build the pearl libraries for all selected targets"
	@echo "make clean       : remove all generated files"
	@echo "make install     : install to /usr/local/lib and /usr/local/include"
	@echo "-----------------"
	@echo

install: all
	(cd $(IC_DIR); make;)
	$(foreach tgt, $(targets),(cd $(tgt); make install;))

#---------------------------------------------------------
clean: 
	(cd $(IC_DIR); make clean;)
	$(foreach tgt, $(targets),(cd $(tgt); make clean;))

