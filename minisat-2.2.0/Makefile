all check:
	CFLAGS="$(filter -I%, $(CXXFLAGS))" LFLAGS="$(filter -L%, $(CXXFLAGS))" $(MAKE) MROOT=$$PWD -C simp s

install install-strip uninstall check installcheck dist:

clean distclean maintainer-clean:
	$(MAKE) MROOT=$$PWD -C simp clean

.PHONY: all install install-strip uninstall check installcheck dist clean distclean maintainer-clean

