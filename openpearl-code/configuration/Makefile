# modify location of autoconf.h
export KCONFIG_AUTOHEADER=include/autoconf.h

.PHONY: nconfig menuconfig clean

# normal update
include/autoconf.h: .config $(deps_config)
	(cd kconfig; make conf)
	mkdir -p include/config
	kconfig/conf --silentoldconfig fm/main.fm

# invoke menuconfig if .config does not exist
ifeq ("$(wildcard .config)","")
.config: menuconfig
endif

# alternative menu system
nconfig: 
	(cd kconfig; make nconf)
	 kconfig/nconf fm/main.fm

# usual menu system
menuconfig:
	(cd kconfig; make mconf)
	 kconfig/mconf fm/main.fm

clean:
	rm -rf include
	(cd kconfig; make clean)


-include include/config/auto.conf.cmd
# show dependency list from include
# $(info deps_config = $(deps_config))
# add configuration dependencies to target 
include/autoconf.h: $(deps_config)

