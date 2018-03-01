deps_config := \
	fm/install.fm \
	fm/doc.fm \
	fm/lpc1768.fm \
	fm/linux.fm \
	fm/commonDrivers.fm \
	fm/runtime.fm \
	fm/compiler.fm \
	fm/main.fm

include/auto.conf: \
	$(deps_config)


$(deps_config): ;
