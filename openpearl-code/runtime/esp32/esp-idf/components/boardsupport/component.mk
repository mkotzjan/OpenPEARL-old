COMPONENT_PRIV_INCLUDEDIRS := ../freertos/include/freertos ../freertos/PEARL
COMPONENT_SRCDIRS := . ../freertos/addOns
COMPONENT_OBJS := Esp32Clock.o Log.o main.o \
	Esp32BME280.o bme280.o \
	bme280_wrapper.o StdOut.o \
	../freertos/addOns/testStackOverflow.o
#COMPONENT_OBJS := Esp32Uart.o Esp32UartInternal.o
#CXXFLAGS += -frtti
