# ===========================================================================
# Kernel configuration targets
# These targets are used from top-level makefile

# ---- setting for standalone operation
obj = .
src=.
srctree = .
CFLAGS += -g -Wall
CONFIG_SHELL=bash
HOSTCC=gcc

TARGETS := conf nconf mconf
all: $(TARGETS)

# Use recursively expanded variables so we do not call gcc unless
# we really need to do so. (Do not call gcc as part of make mrproper)
HOST_EXTRACFLAGS += $(shell $(CONFIG_SHELL) $(check-lxdialog) -ccflags) \
                    -DLOCALE

# -- extend build variables; LDFLAGS are target specific
CFLAGS += $(HOST_EXTRACFLAGS)
#mconf: LDFLAGS += $(HOSTLOADLIBES_mconf) -ltinfo
#nconf: LDFLAGS += -lpanel -lmenu  $(HOSTLOADLIBES_mconf)
mconf: LDLIBS += $(HOSTLOADLIBES_mconf) -ltinfo
nconf: LDLIBS += -lpanel -lmenu  $(HOSTLOADLIBES_mconf)

# ===========================================================================
# Shared Makefile for the various kconfig executables:
# conf:	  Used for defconfig, oldconfig and related targets
# nconf:  Used for the nconfig target.
#         Utilizes ncurses
# mconf:  Used for the menuconfig target
#         Utilizes the lxdialog package
# object files used by all kconfig flavours

lxdialog := lxdialog/checklist.o lxdialog/util.o lxdialog/inputbox.o
lxdialog += lxdialog/textbox.o lxdialog/yesno.o lxdialog/menubox.o

# lxdialog stuff
check-lxdialog  := $(srctree)/$(src)/lxdialog/check-lxdialog.sh

conf-objs	:= conf.o  zconf.tab.o
mconf-objs     	:= mconf.o zconf.tab.o $(lxdialog)
nconf-objs     	:= nconf.o zconf.tab.o nconf.gui.o

cleanfiles += $(conf-objs) 
cleanfiles += $(mconf-objs) 
cleanfiles += $(nconf-objs) 

conf: $(conf-objs)
mconf: $(mconf-objs)
nconf: $(nconf-objs)

cleanfiles += conf mconf nconf

# shortcut for lex, yacc, gperf output files
zconf.hash.c: zconf.hash.c_shipped
	cp zconf.hash.c_shipped zconf.hash.c

zconf.lex.c: zconf.lex.c_shipped 
	cp zconf.lex.c_shipped zconf.lex.c

zconf.tab.c: zconf.tab.c_shipped
	cp zconf.tab.c_shipped zconf.tab.c

cleanfiles += zconf.hash.c zconf.lex.c zconf.tab.c

# Check that we have the required ncurses stuff installed for lxdialog (menuconfig)
PHONY += $(obj)/dochecklxdialog
$(addprefix $(obj)/,$(lxdialog)): $(obj)/dochecklxdialog
$(obj)/dochecklxdialog:
	$(Q)$(CONFIG_SHELL) $(check-lxdialog) -check $(HOSTCC) $(HOST_EXTRACFLAGS) $(HOSTLOADLIBES_mconf)

always := dochecklxdialog

# Add environment specific flags
HOST_EXTRACFLAGS += $(shell $(CONFIG_SHELL) $(srctree)/$(src)/check.sh $(HOSTCC) $(HOSTCFLAGS))

## generated files seem to need this to find local include files
#HOSTCFLAGS_zconf.lex.o	:= -I$(src)
#HOSTCFLAGS_zconf.tab.o	:= -I$(src)
#
#LEX_PREFIX_zconf	:= zconf
#YACC_PREFIX_zconf	:= zconf

HOSTLOADLIBES_mconf   = $(shell $(CONFIG_SHELL) $(check-lxdialog) -ldflags $(HOSTCC))

HOSTLOADLIBES_nconf	= $(shell \
				pkg-config --libs menuw panelw ncursesw 2>/dev/null \
				|| pkg-config --libs menu panel ncurses 2>/dev/null \
				|| echo "-lmenu -lpanel -lncurses"  )



$(obj)/zconf.tab.o: $(obj)/zconf.lex.c $(obj)/zconf.hash.c

.PHONY: clean
clean:
	rm -f $(cleanfiles)
