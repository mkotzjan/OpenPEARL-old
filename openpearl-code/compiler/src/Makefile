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
include ../../configuration/.config

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

OUTPUT_DIR = ../out
##JAVAC_FLAGS = -Xlint:unchecked
JAVAC_FLAGS =

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
	java -jar $(CONFIG_COMPILER_ANTLR) -no-listener -visitor -o org/smallpearl/compiler $(ANTLR_FLAGS) SmallPearl.g4

###############################################################################

javac:
	test -d $(OUTPUT_DIR) || mkdir $(OUTPUT_DIR)
	javac -cp .:$(CONFIG_COMPILER_ANTLR) -d $(OUTPUT_DIR) $(JAVAC_FLAGS) org/smallpearl/compiler/Compiler.java
	rm -f smallpearl_compiler.jar
	cp SmallPearlCpp.stg $(OUTPUT_DIR)
	cp IMC.stg $(OUTPUT_DIR)
	cd $(OUTPUT_DIR) && jar cf  smallpearl_compiler.jar \
				    org/smallpearl/compiler/*.class \
	                            org/smallpearl/compiler/SymbolTable/*.class \
	                            org/smallpearl/compiler/SemanticAnalysis/*.class \
			            SmallPearlCpp.stg IMC.stg

##############################################################################

test:
	../regression.sh
    
###############################################################################

prepare:
	touch $(CONFIG_INSTALL_Target)/bin/smallpearl_compiler.jar 
	chmod 777 $(CONFIG_INSTALL_Target)/bin/smallpearl_compiler.jar 

install: gen javac prl_cc.inc
#	cp prl_c $(INSTALL_DIR)
#	cp smallpearl_compiler.jar $(INSTALL_DIR)
	cp $(OUTPUT_DIR)/smallpearl_compiler.jar $(CONFIG_INSTALL_Target)/bin

.PHONY: prl_cc.inc
prl_cc.inc:
	echo 'java -cp $(CONFIG_INSTALL_Target)/bin/smallpearl_compiler.jar:$(CONFIG_COMPILER_ANTLR)  org/smallpearl/compiler/Compiler $$fn.prl --output $$fn.cc' > ../prl_cc.inc

###############################################################################

clean:
	$(RM) org/smallpearl/compiler/SmallPearl.tokens
	$(RM) org/smallpearl/compiler/SmallPearlBaseVisitor.java
	$(RM) org/smallpearl/compiler/SmallPearlLexer.java
	$(RM) org/smallpearl/compiler/SmallPearlLexer.tokens
	$(RM) org/smallpearl/compiler/SmallPearlParser.java
	$(RM) org/smallpearl/compiler/SmallPearlVisitor.java
	$(RM) $(OUTPUT_DIR)/org/smallpearl/compiler/*.class
	$(RM) $(OUTPUT_DIR)/org/smallpearl/compiler/SymbolTable/*.class
	$(RM) $(OUTPUT_DIR)/org/smallpearl/compiler/SemanticAnalysis/*.class
	$(RM) $(OUTPUT_DIR)/*.jar
	$(RM) $(OUTPUT_DIR)/*.stg
	$(RM) ../prl_cc.inc

###############################################################################

