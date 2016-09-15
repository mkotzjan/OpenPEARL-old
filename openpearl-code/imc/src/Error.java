/*
 [A "BSD license"]
 Copyright (c) 2016 Rainer Mueller
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

public class Error {
	static int numberOfErrors = 0;
	static String file;
	static int lineNbr;
	static boolean showInfo = true;
	
	static void setLocation(String fileName, int line) {
		file = fileName;
		lineNbr = line;
	}
	static void error(String message) {
		numberOfErrors ++;
		System.err.println(file+":" + lineNbr + ": error: "+message);
	}
	static void info(String message) {
		if (showInfo) {
		System.err.println(file+":" + lineNbr + ": info: "+message);
		}
	}
	
	static void exitIfErrors() {
		if (numberOfErrors > 0) {
			System.err.println("*** check aborted due to errors");
			System.exit(1);
		}
	}
	public static void setShowInfo(boolean v) {
		showInfo = v;
		
	}
	public static void warn(String message) {
		System.err.println(file+":" + lineNbr + ": warning: "+message);
		
	}
	
}
