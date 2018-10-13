

my_objs := mconf.o zconf.tab.o


all: $(my_objs) mconf
	$(MAKE) -C lxdialog


mconf: mconf.o zconf.tab.o

zconf.tab.o: lex.zconf.c zconf.hash.c

lkc_defs.h: lkc_proto.h
	sed < $< > $@ 's/P(\([^,]*\),.*/#define \1 (\*\1_p)/'

LKC_GENPARSER := 1

ifdef LKC_GENPARSER

zconf.tab.c: zconf.y
lex.zconf.c: zconf.l
zconf.hash.c: zconf.gperf

%.tab.c: %.y
	bison -l -b $* -p $(notdir $*) $<
	cp $@ $@_shipped

lex.%.c: %.l
	flex -L -P$(notdir $*) -o$@ $<
	cp $@ $@_shipped

%.hash.c: %.gperf
	gperf < $< > $@
	cp $@ $@_shipped

endif

clean:
	rm -f $(my_objs) mconf lxdialog/lxdialog
	find . -name "*.o" -exec rm {} \;

