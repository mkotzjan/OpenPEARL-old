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

include ../Makefile.inc

##
## antlr generates out of SmallPearl.g4 the following files:
##
ANTLR_GENERATED_FILES = SmallPearl.tokens          \
                        SmallPearlBaseVisitor.java \
                        SmallPearlLexer.java       \
                        SmallPearlLexer.tokens     \
                        SmallPearlParser.java      \
                        SmallPearlVisitor.java

## ANTLR_FLAGS = -XdbgST
ANTLR_FLAGS = 

###############################################################################

.PHONY: gen javac 

help:
	@echo "      ---------------------------------------------------"
	@echo "      make help     - this output"
	@echo "      make all      - gen;javac;install in one pass"
	@echo "      make gen      - generate parser"
	@echo "      make javac    - build compiler"
	@echo "      make test     - run regression test suite"
	@echo "      make install  - install compiler and compile-script"
	@echo "      ---------------------------------------------------"

all: gen javac install

.cmd_ok:
	./check_cmds.sh

###############################################################################

gen ./check_cmds.sh $(ANTLR_GENERATED_FILES) : SmallPearl.g4 
	java -jar $(ANTLR) -no-listener -visitor -o org/smallpearl/compiler $(ANTLR_FLAGS) SmallPearl.g4

###############################################################################

javac:
	javac -cp .:$(ANTLR)  org/smallpearl/compiler/Compiler.java
	rm -f smallpearl_compiler.jar
	jar cf smallpearl_compiler.jar org/smallpearl/compiler/*.class \
		SmallPearlCpp.stg SystemPartExport.stg

##############################################################################

test:
	../regression.sh
    
###############################################################################

install:
	cp prl_c $(INSTALL_DIR)
	cp smallpearl_compiler.jar $(INSTALL_DIR)

###############################################################################

clean:
	$(RM) org/smallpearl/compiler/SmallPearl.tokens
	$(RM) org/smallpearl/compiler/SmallPearlBaseVisitor.java
	$(RM) org/smallpearl/compiler/SmallPearlLexer.java
	$(RM) org/smallpearl/compiler/SmallPearlLexer.tokens
	$(RM) org/smallpearl/compiler/SmallPearlParser.java
	$(RM) org/smallpearl/compiler/SmallPearlVisitor.java
	$(RM) org/smallpearl/compiler/*.class

###############################################################################

