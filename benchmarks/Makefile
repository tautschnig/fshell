top_builddir = ../
fshell = $(top_builddir)/fshell2/main/fshell 
params = --verbosity 0 --statistics --outfile /dev/null


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

JOPLIFT = joplift_BB joplift_PATHS_ctrl_loop

all install install-strip uninstall check installcheck dist:

clean distclean maintainer-clean:
	$(MAKE) -C code clean
	rm -f .fshell2_history

prepare: $(fshell)
	[ -n $(LOG) ]
	mkdir -p logs

%.only: prepare conf/%
	logf=logs/$(LOG).$@ fshell="$(strip $(fshell))" params="$(params)" \
			 scripts/run_bench.sh $(filter-out $<, $^)

%.debug: prepare conf/%
	debug=1 logf=logs/$(LOG).$@ fshell="$(strip $(fshell))" params="--outfile /dev/null" \
			 scripts/run_bench.sh $(filter-out $<, $^)

%.gdb: prepare conf/%
	mkdir -p test
	debug=2 logf=logs/$(LOG).$@ fshell="$(strip $(fshell))" params="" \
			 scripts/run_bench.sh $(filter-out $<, $^)

.SECONDEXPANSION:
%.suite: prepare $$(addprefix conf/, $$($$*))
	logf=logs/$(LOG).$@ fshell="$(strip $(fshell))" params="$(params)" \
			 scripts/run_bench.sh $(filter-out $<, $^)

.PHONY: prepare all install install-strip uninstall check installcheck dist clean distclean maintainer-clean

