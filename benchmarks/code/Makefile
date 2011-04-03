BENCHMARKS = ase-examples busybox-1.14.0 engine-controllers joplift \
						 memory-manager nt-drivers picosat-913 vfs-model basic \
						 sorting PapaBench-0.4 maelardalen 

all: $(BENCHMARKS:=.prep)

clean:
	set -e ; for b in $(BENCHMARKS) ; do \
		if [ -d $$b ] ; then \
			cd $$b ; cpbm update ../$$b.tar.* ; cd .. ; \
			rm -r $$b ; \
		fi ; \
		rm -f $$b.prep ; \
	done

%.prep: %.tar.gz %.cprover-bm.tar.gz
	cpbm unpack $^
	touch $@

%.prep: %.tar.bz2 %.cprover-bm.tar.gz
	cpbm unpack $^
	touch $@

.PHONY: all clean

