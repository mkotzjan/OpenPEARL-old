g++ -DTARGET=1 -Wall -g  -fstack-protector -pthread -O2 -fno-non-call-exceptions -DTARGET=1   -I "/usr/local"/include system.cc $fn.cc -o $fn "/usr/local"/lib/libOpenPEARLlinux.a -lpthread -lrt 
