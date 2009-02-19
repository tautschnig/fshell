CC     = gcc
CPLUS  = $(CXX)
LD     = ld
AR     = ar
YACC   = bison -y
YFLAGS = -v
LEX    = flex
#CC     = icc
#CPLUS  = icpc
#USE_AR = 1

# Linux etc.
GCCFLAGS = $(CXXFLAGS) -Wno-long-long
LINKFLAGS =

# use these for Cygwin:
# GCCFLAGS = -Wall -MMD -O2 -mno-cygwin
# LINKFLAGS = -mno-cygwin

# SAT-solvers we have
#CHAFF = ../../zChaff
#BOOLEFORCE = ../../booleforce-0.4
MINISAT = ../MiniSat-p_v1.14
#MINISAT2 = ../../minisat2
#SMVSAT =

LANGUAGES = ansi-c csp intrep pvs smvlang verilog vhdl netlist cpp specc \
            xmllang promela pascal bplang simplifylang cvclang csharp \
            smtlang nsf

# modules that we have
MODULE_PROVER = 1
#MODULE_IPP = 1
#MODULE_BV_REFINEMENT = 1
#MODULE_SATQE = 1
#MODULE_CPP = 1
MODULE_SMTLANG = 1
#MODULE_SPECC = 1
#MODULE_VERILOG = 1
#MODULE_VHDL = 1
#MODULE_PASCAL = 1
#MODULE_PHP = 1
#MODULE_SIMPLIFYLANG = 1
#MODULE_HW_CBMC = 1
