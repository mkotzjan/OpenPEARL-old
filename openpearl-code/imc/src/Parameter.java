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

import org.w3c.dom.Node;

/**
 * A parameter object to store the value and some utility methods
 * 
 * @author mueller
 *
 */
public class Parameter {
   private String value=null;
   private String type=null;
   private String nickName=null;
   
   Parameter(Node node) {
	 //  NodeList childs = node.getChildNodes();
	   
		//for (int i = 0; i < childs.getLength(); i++) {
		//	if (childs.item(i).getNodeType() == Node.ELEMENT_NODE) {
		//		System.out.println("ElementNode "
		//				+ childs.item(i).getNodeName());
				type = node.getNodeName();
				value = node.getTextContent().trim();
	//		}
//		}	   
   }
   
   String getType() {
	   return type;
   }
   String getValue() {
	   return value;
   }
   
   boolean hasType(String type) {
	   return this.type.equals(type);
   }
   
   void setNickName(String n) {
	   nickName=n;
   }
   String getNickName() {
	   return nickName;
   }
   
   int length() {
	   int numberOfBits = 0;
	   if (hasType("FIXED")) {
		   int v = Integer.parseInt(value);
		   numberOfBits = 32-Integer.numberOfLeadingZeros(v);
		   return(numberOfBits);
	   } else if (hasType("BIT")) {
	      if (value.endsWith("'B")) {
	    	  numberOfBits = (value.length()-3);
	      } else if (value.endsWith("'B1")) {
	    	  numberOfBits = (value.length()-4);
	      } else if (value.endsWith("'B2")) {
	    	  numberOfBits = (value.length()-4)*2;
	      } else if (value.endsWith("'B3")) {
	    	  numberOfBits = (value.length()-4)*3;
	      } else if (value.endsWith("'B4")) {
	    	  numberOfBits = (value.length()-4)*4;
	      } else {
	    	  System.err.println("illegal type ("+value+")");
	      }
	      return (numberOfBits);
	   } else if (hasType("CHAR")) {
		   numberOfBits = value.length()-2;
	   } else if (hasType("FLOAT")) {
		   numberOfBits = 53;
	   } else {
		   System.err.println("unknown type");
	   }
	   
	   return numberOfBits;
   }
   
   String getCppCode() {
	   if (hasType("FIXED")) {
		   return value;
	   } else if (hasType("CHAR")) {
		   String result="(char*)\"";
		   for (int i=1; i<value.length()-1; i++) {
			   if (value.charAt(i) == '\'' && value.charAt(i+1) == '\\') {
				   Error.error("control caharacters are not supported ");	   
		       } else if (value.charAt(i) == '"' ) {
				   result += "\"";
			   } else {
				   result += value.charAt(i);
			   }
		   }
		   result += "\"";
		   return result;
	   } else if (hasType("BIT")) {
		   String result="";
		   int val=0;
		   int shift = length() - ((length()-1) / 8 + 1) * 8;
		   if (value.endsWith("'B")) {
		    	  val = Integer.parseInt(value.substring(1,value.length()-2), 2);
		      } else if (value.endsWith("'B1")) {
		    	  val = Integer.parseInt(value.substring(1,value.length()-3), 2);
		      } else if (value.endsWith("'B2")) {
		    	  val = Integer.parseInt(value.substring(1,value.length()-3), 4);
		      } else if (value.endsWith("'B3")) {
		    	  val = Integer.parseInt(value.substring(1,value.length()-3), 8);
		      } else if (value.endsWith("'B4")) {
		    	  val = Integer.parseInt(value.substring(1,value.length()-3), 16);
		      } else {
		    	  System.err.println("illegal type ("+value+")");
		      }
		   val <<= shift;
		   result = "0x"+Integer.toHexString(val) ;
		   return result;
		   
	   } else {
           return "unknown parameter type";
	   }
   }
}
