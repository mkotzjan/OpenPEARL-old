COMPONENT_PRIV_INCLUDEDIRS := ../freertos/include/freertos ../freertos/PEARL ../lwip/inlcude/lwip
COMPONENT_SRCDIRS := . ../freertos/addOns
COMPONENT_OBJS := Log.o main.o StdOut.o SystemConsole.o I2CBus.o Esp32Uart.o Esp32UartInternal.o Esp32Clock.o Esp32Wifi.o TcpIpServer.o ../freertos/addOns/testStackOverflow.o
#CXXFLAGS += -frtti
