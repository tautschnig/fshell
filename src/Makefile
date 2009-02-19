DIRS = ansi-c big-int cbmc cprover csp ebmc hoare \
       infrules intrep propdec pvs separate smvlang util verilog \
       vhdl langapi netlist cpp pascal symex clock-mc clocklang \
       satqe satmc satabs explain specc xmllang promela \
       goto-programs bplang simplifylang vcegar hw-cbmc vsynth \
       boppo cvclang pointer-analysis cogent propsolve \
       goto-symex ipp prover trans bv_refinement mcp goto-cc \
       smtlang php cemc interpolation scoot

all: cprover.dir cbmc.dir goto-cc.dir

maintainer-clean distclean: clean

include config.inc
include common

###############################################################################

util.dir: big-int.dir

languages: util.dir langapi.dir \
           ansi-c.dir \
           xmllang.dir

cogent.dir: ansi-c.dir util.dir propdec.dir propsolve.dir

cprover.dir: languages

cbmc.dir: languages propdec.dir goto-symex.dir propsolve.dir \
          pointer-analysis.dir goto-programs.dir goto-symex.dir

cemc.dir: cbmc.dir

scoot.dir: satabs.dir languages

goto-cc.dir: languages pointer-analysis.dir goto-programs.dir

hw-cbmc.dir: cbmc.dir trans.dir

boppo.dir: langapi.dir bplang.dir promela.dir util.dir propsolve.dir \
           propdec.dir languages

explain.dir: cbmc.dir

satabs.dir: languages goto-symex.dir goto-programs.dir \
            pointer-analysis.dir propdec.dir propsolve.dir trans.dir

vcegar.dir: languages satqe.dir propdec.dir ebmc.dir

vsynth.dir: langapi.dir verilog.dir util.dir vhdl.dir netlist.dir

ebmc.dir: propdec.dir languages propsolve.dir trans.dir

satmc.dir: propdec.dir smvlang.dir util.dir verilog.dir languages \
           intrep.dir vhdl.dir netlist.dir trans.dir satqe.dir propsolve.dir

clock-mc.dir: propdec.dir smvlang.dir propsolve.dir \
              util.dir verilog.dir langapi.dir \
              intrep.dir vhdl.dir netlist.dir ebmc.dir \
              clocklang.dir

ifdef MODULE_INTERPOLATION
ebmc.dir: interpolation.dir
interpolation.dir: propdec.dir propsolve.dir langapi.dir util.dir
endif

ifdef MODULE_PROVER
satabs.dir: prover.dir
ebmc.dir: prover.dir
endif

ifdef MODULE_IPP
satabs.dir: ipp.dir
endif

ifdef MODULE_BV_REFINEMENT
cbmc.dir: bv_refinement.dir
endif

ifdef MODULE_SATQE
satabs.dir: satqe.dir
all: satmc.dir
endif

ifdef MODULE_SMTLANG
languages: smtlang.dir
endif

ifdef MODULE_CPP
languages: cpp.dir
endif

ifdef MODULE_PHP
languages: php.dir
endif

ifdef MODULE_CSP
languages: csp.dir
endif

ifdef MODULE_PVS
languages: pvs.dir
endif

ifdef MODULE_VERILOG
languages: verilog.dir
endif

ifdef MODULE_VHDL
languages: vhdl.dir
endif

ifdef MODULE_NETLIST
languages: netlist.dir
endif

ifdef MODULE_SPECC
languages: specc.dir
endif

ifdef MODULE_PASCAL
languages: pascal.dir
endif

ifdef MODULE_SIMPLIFYLANG
languages: simplifylang.dir
endif

ifdef MODULE_HWCBMC
satabs: hw-cbmc.dir
all: hw-cbmc.dir
endif

ifdef MODULE_CEMC
all: cemc.dir
endif

$(patsubst %, %.dir, $(DIRS)):
	## Entering $(basename $@)
	$(MAKE) $(MAKEARGS) -C $(basename $@)

clean: $(patsubst %, %_clean, $(DIRS))
	rm -f *.o $(shell find . -name "*.d")

dep: $(patsubst %, %_dep, $(DIRS))

$(patsubst %, %_clean, $(DIRS)):
	if [ -e $(patsubst %_clean, %, $@)/. ] ; then \
		$(MAKE) $(MAKEARGS) -C $(patsubst %_clean, %, $@) clean ; \
	fi

$(patsubst %, %_dep, $(DIRS)):
	if [ -e $(patsubst %_dep, %, $@)/. ] ; then \
		$(MAKE) $(MAKEARGS) -C $(patsubst %_dep, %, $@) dep ; \
	fi
