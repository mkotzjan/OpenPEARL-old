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

import java.util.Vector;


public class SystemEntries {
   static Vector<SystemEntry> userNames = new Vector<SystemEntry>();
   
   static void add(SystemEntry u) {
	   userNames.add(u);
   }
   
   static int size() {
	   return userNames.size();
   }
   
   static SystemEntry get(int i) {
	   return userNames.get(i);
   }
   
   static void dump() {
	   System.out.println("Defined User Names");
	   for (int i=0; i<userNames.size(); i++) {
		   SystemEntry un = userNames.get(i);
		   un.dump();
	   }
   }
   static SystemEntry contains(String uName) {
	   for (int i=0; i<userNames.size(); i++) {
		   SystemEntry un = userNames.get(i);
		   if (un.getName() != null && 
				   un.getName().equals(uName)) {
			   return un;
		   }
	   } 
	   return null;
   }
   
   static void check() {
	   // check for undefined
	   for (int i=0; i<userNames.size(); i++) {
		   SystemEntry u = userNames.get(i);
		   if (u.getNode() == null) {
			   Error.setLocation(u.getFileName(), u.getLine());
			   Error.error("systemname '"+u.getSystemName()+ "' not available.");
		   }
		   SystemEntry p = u.getProvider();
		   if (p != null && p.getNode() == null) {
			   Error.setLocation(u.getFileName(), u.getLine());
			   Error.error("association provider '"+p.getName()+"' is not defined");
		   }
		   
	   }
	   
	   // check for duplicates
	   if (userNames.size()>1) {
	   for (int i=0; i<userNames.size()-1; i++) {
		   SystemEntry u = userNames.get(i);
		   if (u.getName() != null) {
			   for (int j=i+1; j<userNames.size(); j++) {
				   SystemEntry v = userNames.get(j);
				   if (i != j && u.getName().equals(v.getName())) {
					   Error.setLocation(u.getFileName(), u.getLine());
					   Error.error("identifier '"+u.getName()+"' is duplicated defined\n\tother declaration was "+
					   v.getFileName()+":"+v.getLine()  );
				   }
			   }
		   }
	   }
	   }

   }

public static void searchUnUsed() {
	for (int i=0;  i<userNames.size(); i++) {
		SystemEntry u = userNames.get(i);
		if (u.getName() != null && !u.isUsed()) {
			Error.setLocation(u.getFileName(),u.getLine());
			Error.warn("identifier " + u.getName()+ " defined but not used");
		}
	}
	
}

  
}
