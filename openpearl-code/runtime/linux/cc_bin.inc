g++ -DTARGET=1 -Wall -g  -fstack-protector -pthread -fno-non-call-exceptions -I . -I ../common  -I "/usr/local"/include "/usr/local"/lib/libOpenPEARLlinux.a $fn.cc -lrt -pthread -o $fn
