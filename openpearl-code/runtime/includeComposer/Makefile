# Makefile for include composer
#  ic collects the include files to one file by
#     collecting all project dependent files via #include "..."
#     and performs the evaluation of the #ifndef/#define/#endif-constructs
#     The tool expect a well formed header file
#
# dws deletes white space characters (including empty lines)
#
all: ic dws

clean:
	rm -f lex.yy.*
	rm -f ic dws

ic: ic.l
	flex ic.l
	cc lex.yy.c -o ic -l fl


dws: dws.l
	flex dws.l
	cc lex.yy.c -o dws -l fl

