BENCHMARKS = ase-examples busybox-1.14.0 engine-controllers joplift \
						 memory-manager nt-drivers picosat-913 vfs-model basic \
						 sorting PapaBench-0.4 maelardalen 

all: $(BENCHMARKS:=.prep)

clean:
	rm -rf $(BENCHMARKS) $(BENCHMARKS:=.prep)

%.prep: %.tar.gz %.patch
	tar xzf $<
	cd $(subst .prep,,$@) && cat ../$(filter %.patch, $^) | patch -p1
	touch $@

%.prep: %.tar.bz2 %.patch
	tar xjf $<
	cd $(subst .prep,,$@) && cat ../$(filter %.patch, $^) | patch -p1
	touch $@

%.prep: %.tar.gz
	tar xzf $<
	touch $@

%.prep: %.tar.bz2
	tar xjf $<
	touch $@

.PHONY: all clean

