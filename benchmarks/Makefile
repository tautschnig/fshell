top_builddir = ../
top_srcdir = ../
ifneq (,$(EXEEXT))
fshell = $(top_builddir)/fshell2/main/.libs/fshell$(EXEEXT)
else
fshell = $(top_builddir)/fshell2/main/fshell
endif
params = $(MORE_PARAMS)
testenv = $(top_srcdir)/test-harness-builder/simple/TestEnvGenerator.pl


ASE_SCALE_s = busybox-1.14.0_cat.c_BB busybox-1.14.0_cat.c_CC busybox-1.14.0_cat.c_BB^2 \
						  busybox-1.14.0_echo.c_BB busybox-1.14.0_echo.c_CC busybox-1.14.0_echo.c_BB^2 \
							busybox-1.14.0_nohup.c_BB busybox-1.14.0_nohup.c_CC busybox-1.14.0_nohup.c_BB^2 \
						  busybox-1.14.0_seq.c_BB busybox-1.14.0_seq.c_CC busybox-1.14.0_seq.c_BB^2 \
						  busybox-1.14.0_tee.c_BB busybox-1.14.0_tee.c_CC busybox-1.14.0_tee.c_BB^2 \
						  nt-drivers_kbfiltr.c_BB nt-drivers_kbfiltr.c_CC nt-drivers_kbfiltr.c_BB^2 \
							vfs-model_pseudo-vfs.c_BB vfs-model_pseudo-vfs.c_CC vfs-model_pseudo-vfs.c_BB^2 \
							engine-controllers_matlab.c_BB engine-controllers_matlab.c_CC engine-controllers_matlab.c_BB^2 \
							memory-manager_memman.c_BB memory-manager_memman.c_CC memory-manager_memman.c_BB^2

ASE_SCALE = $(ASE_SCALE_s) \
						picosat-913_PicoSAT_BB picosat-913_PicoSAT_CC picosat-913_PicoSAT_BB^2

ASE_QUERIES = ase-examples_list2.c_Q1 ase-examples_list2.c_Q2 ase-examples_list2.c_Q3 \
							ase-examples_sort2.c_Q4 ase-examples_sort2.c_Q5 ase-examples_list2.c_Q6 \
							ase-examples_list2.c_Q7 ase-examples_list2.c_Q8 ase-examples_list2.c_Q9 \
							ase-examples_sort1.c_Q10 ase-examples_sort1.c_Q11 ase-examples_sort2.c_Q12 \
							ase-examples_sort1.c_Q13 ase-examples_sort2.c_Q14 ase-examples_list2.c_Q15 \
							ase-examples_sort1.c_Q16 ase-examples_sort1.c_Q17 ase-examples_list2.c_Q18 \
							ase-examples_list2.c_Q19 ase-examples_list2.c_Q20 ase-examples_sort1.c_Q21 \
							ase-examples_sort1.c_Q22 ase-examples_sort2.c_Q23 ase-examples_sort2.c_Q24

JOPLIFT = joplift_BB joplift_BB^2_ctrl_loop joplift_PATHS_ctrl_loop

BASIC = basic_bitops_1 basic_bool_1 basic_boolop1_CC basic_boolop2_CC basic_cov-union_1 \
				basic_cov-union_2 basic_globals_1 basic_labels_1 basic_minimal_BB basic_repeat_1 \
				basic_undefined-func_1 basic_variables_pred basic_inline_1

SORTING = sorting_bs_BB sorting_bubble-fortas_BB sorting_bubble_BB sorting_sorter_BB \
					sorting_bubble-sort_CC sorting_insertion-sort_CC sorting_quick-sort_CC sorting_selection-sort_CC

NTDRIVERS_no_parclass = nt-drivers_cdaudio.c_BB nt-drivers_cdaudio.c_BB^2 nt-drivers_cdaudio.c_CC \
												nt-drivers_floppy.c_BB nt-drivers_floppy.c_BB^2 nt-drivers_floppy.c_CC \
												nt-drivers_kbfiltr.c_BB nt-drivers_kbfiltr.c_BB^2 nt-drivers_kbfiltr.c_CC \
												nt-drivers_parport.c_BB nt-drivers_parport.c_BB^2 nt-drivers_parport.c_CC
						
NTDRIVERS = $(NTDRIVERS_no_parclass) \
						nt-drivers_parclass.c_BB nt-drivers_parclass.c_BB^2 nt-drivers_parclass.c_CC

MAELARDALEN = maelardalen_adpcm.c_BB maelardalen_adpcm.c_CC maelardalen_adpcm.c_BB^2 \
							maelardalen_nsichneu.c_BB maelardalen_nsichneu.c_CC maelardalen_nsichneu.c_BB^2 \
							maelardalen_statemate.c_BB maelardalen_statemate.c_CC maelardalen_statemate.c_BB^2

PAPABENCH = PapaBench-0.4_autopilot_BB PapaBench-0.4_fly-by-wire_BB \
						PapaBench-0.4_autopilot_C1 PapaBench-0.4_autopilot_C2 \
						PapaBench-0.4_autopilot_C3 PapaBench-0.4_autopilot_C4 \
						PapaBench-0.4_autopilot_C5 PapaBench-0.4_autopilot_C6

all install install-strip uninstall check installcheck dist:

clean distclean maintainer-clean:
	$(MAKE) -C code clean
	rm -f .fshell2_history

prepare: $(fshell) $(testenv)
	[ -n $(LOG) ]
	mkdir -p logs

%.only: params += --verbosity 2 --statistics
%.only: prepare conf/%
	logf=logs/$(LOG).$@ fshell="$(strip $(fshell))" params="$(params)" testenv="$(strip $(testenv))" \
			 scripts/run_bench.sh $(filter-out $<, $^)

%.debug: params += --verbosity 9
%.debug: prepare conf/%
	debug=1 logf=logs/$(LOG).$@ fshell="$(strip $(fshell))" params="$(params)" testenv="$(strip $(testenv))" \
			 scripts/run_bench.sh $(filter-out $<, $^)

%.gdb: params += --verbosity 9
%.gdb: prepare conf/%
	mkdir -p test
	debug=2 logf=logs/$(LOG).$@ fshell="$(strip $(fshell))" params="$(params)" testenv="$(strip $(testenv))" \
			 scripts/run_bench.sh $(filter-out $<, $^)

%.suite: params += --verbosity 2 --statistics
.SECONDEXPANSION:
%.suite: prepare $$(addprefix conf/, $$($$*))
	logf=logs/$(LOG).$@ fshell="$(strip $(fshell))" params="$(params)" testenv="$(strip $(testenv))" \
			 scripts/run_bench.sh $(filter-out $<, $^)

.PHONY: prepare all install install-strip uninstall check installcheck dist clean distclean maintainer-clean

