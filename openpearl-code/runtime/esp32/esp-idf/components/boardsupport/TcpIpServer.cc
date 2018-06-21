/*
 [The "BSD license"]
 Copyright (c) 2013-2014 Holger Koelle
 Copyright (c) 2014 Rainer Mueller
 Copyright (c) 2018 Michael Kotzjan
 All rights reserved.

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions
 are met:

 1. Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.
 2. Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in the
    documentation and/or other materials provided with the distribution.
 3. The name of the author may not be used to endorse or promote products
    derived from this software without specific prior written permission.

 THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

/**
\file
\brief Implementation of a TCP/IP Webserver for ESP32

*/

#include "TcpIpServer.h"
#include "Character.h"
#include "RefChar.h"
#include "Dation.h"
#include "Log.h"
#include "Signals.h"
#include "lwip/sockets.h"
#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>

#define PORT 30000

// These structs outside of the class are preventing the usage
// of multiple TCP/IP Server at the same time. This has been
// done due to issues with the Include Composer. A better
// solution would be the usage of faketypes in the header file.
struct sockaddr_in server, client;
bool initsocket;

namespace pearlrt {
   //char* TcpIpServer::devicePath = NULL;


   TcpIpServer::TcpIpServer(const int port) :
      SystemDationNB() {
      /* ctor is called before multitasking starts --> no mutex required */
      mutex.name("TcpIpServer");
      this->port = port;
      translateNewLineFlag = false;
	initsocket = true;	

   }

   int TcpIpServer::capabilities() {
      return IN | OUT | INOUT | FORWARD |  ANY | CAN | PRM;
   }

  

   TcpIpServer* TcpIpServer::dationOpen(const char * idfValue, int openParams) {
      unsigned int len;
      mutex.lock();

	if(initsocket){
	initsocket = false;

	//sockt erstellen, bind, listen, accep
	
	sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	
	if(sock < 0)
		Log::error("TcpIpServer: Create Server Error");
	
	memset(&server, 0 , sizeof(server));
	
	server.sin_family = AF_INET;

	server.sin_addr.s_addr = htonl(INADDR_ANY);

	server.sin_port = htons(port);

	if(bind(sock,(struct sockaddr*)&server, sizeof(server))<0){
		Log::error("TcpIpServer: Cannot bind Socket");
	}

	if(listen(sock, 5) == -1)
		Log::error("TcpIpServer: Listen error");

	} // intisocket

      // check parameters
      if (openParams & IDF) {
         Log::error("TcpIpServer: no IDF allowed");
         mutex.unlock();
         throw theDationParamSignal;
      }

	len = sizeof(client);
	fd = accept(sock, (struct sockaddr*) &client, &len);
	if(fd <0)
        	Log::error("TcpIpServer: Accept error");
		
	

	
		
	

      // setup objects data
      
      mutex.unlock();
	return this;
   }

   void TcpIpServer::dationClose(int closeParams) {
      int ret;
      //ret = close(fd);
      ret = shutdown(fd,SHUT_WR);
      if (ret != 0) {
         Log::error("TcpIpServer: error at close (%d)", errno);
         throw theCloseFailedSignal;
      }
   }


   void TcpIpServer::TcpIpServer::translateNewLine(bool doNewLineTranslation) {
      translateNewLineFlag = doNewLineTranslation;	
   }

   void TcpIpServer::dationRead(void * destination, size_t size) {
      char * dest = (char*) destination;
      int ret;

      errno = 0;
     
      if((ret = recv(fd, dest, size,0))<0){
          Log::error("TcpIpServer: error at read (%s)", strerror(errno));
          throw theReadingFailedSignal;
      }
      
      if (translateNewLineFlag) {
          // see if we got a CR NL sequence and remove the CR in this case
          // this is only on in alphic mode, which reads
          // always only 1 character
          if (*dest == '\r') {
             if((ret = recv(fd, dest, size,0))<0){
                Log::error("TcpIpServer: error at read (%s)", strerror(errno));
                throw theReadingFailedSignal;
             }
          }
      }
      
   }


   void TcpIpServer::dationWrite(void * source, size_t size) {
      int ret;
      errno = 0;
      static char carriageReturn = '\r';

      if (translateNewLineFlag) {
          // see if we got a CR NL sequence and remove the CR in this case
          // this is only on in alphic mode, which reads
          // always only 1 character
          if (*(char*)source == '\n') {
             if((ret = send(fd, &carriageReturn, 1,0))<0){
                Log::error("TcpIpServer: error at send (%s)", strerror(errno));
                throw theReadingFailedSignal;
             }
          }
      }
      if((ret = send(fd, source, size, 0))<0){
         Log::error("TcpIpServer: error at write (%s)", strerror(errno));
         throw theWritingFailedSignal;
      }
      
   }

   void TcpIpServer::dationUnGetChar(const char x) {
printf("TcpIpServer::unget %x\n", x);
      //ungetc(x, fp);
   }
}
