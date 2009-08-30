all: tester.mk tester.c
	$(MAKE) -f tester.mk

clean:
	-$(MAKE) -f tester.mk clean
	rm -f tester.mk tester.c

tester.mk tester.c: fshell-output.txt
	./TestEnvGenerator.pl < $<

.PHONY: all clean
