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

import java.util.ArrayList;
import java.util.List;

import javax.script.ScriptEngine;
import javax.script.ScriptEngineManager;
import javax.script.ScriptException;

import org.w3c.dom.Node;

/**
 * Store a user defined name in system part together with some attributes
 * 
 * temp: LM75(0x48) --- I2CBUS('/dev/I2C-0", 100000); --> temp is stored with
 * reference to document and node in target-document
 * 
 * @author mueller
 * 
 */
public class SystemEntry {

	private String userName;
	private String systemName;
	private String type;
	private SystemEntry provider;

	// private TargetXml targetXml;
	private Node node;
	private List<Parameter> parameters = new ArrayList<Parameter>();
	private String file;
	private int line;
	private boolean isUsed = false;
	private boolean codeIsCompleted = false;
	static private int configItemNbr=0;

	private List<AssociationEntry> mustProvide = new ArrayList<AssociationEntry>();

	/**
	 * user name of system entry with reference to the system entry
	 * 
	 * @param uName
	 *            the username
	 * @param f
	 *            filename of definition
	 * @param l
	 *            line number of definition
	 */
	SystemEntry(String uName, String f, int l) {
		provider = null;
		userName = uName;
		file = f;
		line = l;
		// targetXml = t;
	}

	/**
	 * create anonymoud entry (e.g. association entry)
	 * 
	 * @param f
	 *            filename of definition
	 * @param l
	 *            line number of definition
	 * */
	public SystemEntry(String f, int l) {
		provider = null;
		userName = null;
		file = f;
		line = l;
	}

	/**
	 * read user name
	 * 
	 * @return the username
	 */
	String getName() {
		return userName;
	}

	Node getNode() {
		return node;
	}

	String getNickNameValue(String nickName) {
		for (int i = 0; i < parameters.size(); i++) {
			Parameter p = parameters.get(i);
			String n = p.getNickName();
			if (n != null && n.equals(nickName)) {
				return parameters.get(i).getValue();
			}
		}
		return null;
	}

	/**
	 * read type of the user name as derived form the system name
	 * 
	 * @return type ("dation", "interrupt" or "signal"
	 */
	String getType() {
		return type;
	}

	/**
	 * set provider name if there is one
	 * 
	 * @param p
	 *            the providers name
	 */
	void setProvider(SystemEntry p) {
		provider = p;
	}

	/**
	 * store the ctor parameters for later code generation
	 * 
	 * @param p
	 *            a parameter
	 */
	public void addParameter(Parameter p) {
		parameters.add(p);
	}

	/**
	 * create ctor entry for this username
	 * 
	 * @return the code lines
	 */
	public void getCompleteCode(StringBuilder simple, StringBuilder dations) {
		String params;
		if (type == null) {
			return;
		}
		if (type.equals("signal")) {
			simple.append("static pearlrt::" + systemName + " _" + userName + ";");
			simple.append("\t// "+file+":"+line+"\n");
			simple.append("       pearlrt::Signal * generalized_" + userName
					+ "= &_" + userName + ";\n");
			codeIsCompleted = true;
		} else if (type.equals("dation")) {
			params = translateParameters();
			dations.append("\n\t// "+file+":"+line+"\n");
			dations.append("\tstatic pearlrt::" + systemName + " s_" + userName);
			if (!params.isEmpty()) {
				dations.append("(" + params + ")");
			}
			dations.append(";\n");
			dations.append("\td_" + userName + " = &s_"
					+ userName + ";\n");
			
			simple.append("pearlrt::Device * d_" + userName+";");
			simple.append("\t// "+file+":"+line+"\n");

			codeIsCompleted = true;
		} else if (type.equals("interrupt")) {
			params = translateParameters();
			dations.append("\n\t// "+file+":"+line+"\n");
			dations.append("\tstatic pearlrt::" + systemName + " sys_" + userName);
			if (!params.isEmpty()) {
				dations.append("(" + params + ")");
			}
			dations.append(";\n");
			dations.append("\t_" + userName	+ " = (pearlrt::Interrupt*)& sys_" + userName + ";\n");
			simple.append("       pearlrt::Interrupt * _" + userName + ";");
			simple.append("\t// "+file+":"+line+"\n");
			codeIsCompleted = true;
		} else if (type.equals("connection")) {
			params = translateParameters();
			dations.append("\n\t// "+file+":"+line+"\n");			
			dations.append("\tstatic pearlrt::" + systemName + " s_" + userName);
			if (!params.isEmpty()) {
				dations.append("(" + params + ")");
			}
			dations.append(";\n");
			codeIsCompleted = true;
		} else if (type.equals("configuration")) {
			dations.append("\n\t// "+file+":"+line+"\n");			
			dations.append("\tstatic pearlrt::" + systemName + " config" + configItemNbr++);
			params = translateParameters();
	 	    if (!params.isEmpty()) {
	 	  	   dations.append("("+translateParameters() + ")");
			}
			dations.append(";\n");
			codeIsCompleted = true;
		} else {
			System.out.println("could not create code for type " + type);
		}

		return;
	}


	private String translateParameters() {
		String p = "";
		if (provider != null) {
			if (provider.getName() == null) {
				p += "new " + provider.getSystemName() + "("
						+ provider.translateParameters() + ")";

			} else {
				p += "& s_" + provider.getName();
				provider.markAsUsed();
			}
		}
		for (int i = 0; i < parameters.size(); i++) {
			if (p.length() == 0) {
				p = parameters.get(i).getCppCode();
			} else {
				p += ", " + parameters.get(i).getCppCode();
			}
		}
		return p;
	}

	public String getSystemName() {
		return systemName;
	}

	public void setSystemName(String sName) {
		systemName = sName;
	}

	public void setType(String t) {
		type = t;
	}

	public void setTargetNode(Node targetNode) {
		node = targetNode;
	}

	/**
	 * print the tree -- used for testing purposes
	 */
	public void dump() {
		System.out.print("Username: ");
		if (userName != null) {
			System.out.print(userName);
		} else {
			System.out.print("(anonymous)");
		}

		System.out.print("  Type ");
		if (systemName != null) {
			System.out.print(type);
		} else {
			System.out.print("(anonymous)");
		}

		System.out.print(" -- System Name ");
		if (systemName != null) {
			System.out.print(systemName);
		} else {
			System.out.print("(anonymous)");
		}

		System.out.print(" -- has system node: ");
		if (node == null) {
			System.out.print("no ");
		} else {
			System.out.print("yes");
		}

		System.out.print(" -- has provider:");
		if (provider == null) {
			System.out.print("no ");
		} else {
			System.out.print("yes");
		}

		System.out.println("");
	}

	public int getLine() {
		return line;

	}

	public String getFileName() {
		return file;
	}

	public SystemEntry getProvider() {
		return provider;
	}

	

	public void markAsUsed() {
		isUsed = true;
	}

	public boolean isUsed() {
		return isUsed;
	}

	private AssociationEntry findAssociationEntry(String name) {
		for (int i = 0; i < mustProvide.size(); i++) {
			if (mustProvide.get(i).getAssociationName().equals(name)) {
				return mustProvide.get(i);
			}
		}
		AssociationEntry ae = new AssociationEntry(name);
		mustProvide.add(ae);
		return ae;
	}

	public boolean setMaxClients(String associationName, int associationClients) {
		boolean ok = findAssociationEntry(associationName).setMaxClients(
				associationClients);
		return ok;
	}

	public boolean incrementAssociationClients(String associationName) {
		boolean ok = findAssociationEntry(associationName).incrementUsage();
		return ok;
	}

	public void mustProvide(String p) {
		AssociationEntry ae = null;
		for (int i = 0; i < mustProvide.size(); i++) {
			if (mustProvide.get(i).getAssociationName().equals(p)) {
				ae = mustProvide.get(i);
			}
		}
		if (ae == null) {
			ae = new AssociationEntry(p);
			mustProvide.add(ae);
		}
	}

	public String[] getMustProvide() {
		int nbr = mustProvide.size();
		String[] returnValue = new String[nbr];
		for (int i = 0; i < nbr; i++) {
			returnValue[i] = mustProvide.get(i).getAssociationName();
		}
		return returnValue;
	}

	public SystemEntry requiresOtherSystemEntry() {

		if (provider != null && provider.getName() != null) {
			if (!provider.codeIsCompleted) {
				return provider;
			}
		}
		return null;
	}

	
	public boolean codeIsCompleted() {
		return codeIsCompleted;
	}

	/**
	 * safe the location in the source files, where this item is defined
	 * 
	 * this information is used for error reporting. The elements refer the
	 * original PEARL source file.
	 *  
	 * @param sourceFileName the source file name 
	 * @param l  the line in the source file
	 */
	public void setLocation(String sourceFileName, int l) {
		file = sourceFileName;
		line = l;

	}

	/**
	 * substitute nicknames by their current values and evaluate the resulting
	 * expression as integer. The nicknames are prefixed with '$' and may
	 * consist only of upper and lower case letters. Expressions must be braced
	 * with square brackets. e.g. 'anyText[1+4*$nick + $name]anyotherText'
	 * 
	 * multiple expressions are allowed
	 * 
	 * @param expr
	 *            the expression with nicknames and operators
	 * @return
	 */
	public String evaluateExpression(String expr) {

		int nickNameEnd;
		String expanded = expr;
		int nickNameStart = expanded.indexOf('$');
		while (nickNameStart >= 0) {
			nickNameEnd = nickNameStart;
			boolean isLetter;
			do {
				nickNameEnd++;
				char currentChar = expanded.charAt(nickNameEnd);
				isLetter = (currentChar >= 'A' && currentChar <= 'Z')
						| (currentChar >= 'a' && currentChar <= 'z');
			} while (isLetter);

			String nickName = expr.substring(nickNameStart + 1, nickNameEnd);
			String nickValue = getNickNameValue(nickName);
			expanded = expr.substring(0, nickNameStart) + nickValue
					+ expr.substring(nickNameEnd);
//System.out.println("evaluateExpression: expanded="+expanded);
			nickNameStart = expanded.indexOf('$');
		}

		int exprStart = expanded.indexOf('[');
		while (exprStart >= 0) {
			int exprEnd = exprStart;
			while (exprEnd < expr.length() && expanded.charAt(exprEnd) != ']') {
				exprEnd++;
			}
			if (expanded.charAt(exprEnd) != ']') {
				Error.info("missing ']' in expression in target plattform definition file");
				return null;
			}

			// evaluate the expression
			String nakedExpression = expanded.substring(exprStart + 1, exprEnd);
//System.out.println("nakedExpression="+nakedExpression);

			ScriptEngineManager mgr = new ScriptEngineManager();
			ScriptEngine engine = mgr.getEngineByName("JavaScript");
			Object result;

			try {
				result = engine.eval(nakedExpression);

			} catch (ScriptException e) {
				Error.info("wrong expression in target plattform definition file ("
						+ nakedExpression + ")");
				return null;
			}
//System.out.println("result="+result);

			// insert expression result in the expanded string
                        String value;
                        if (result.getClass() == Integer.class) {
                           value=result.toString();
                        } else if (result.getClass() == Double.class) {
                           Integer ihelp=((Double)result).intValue();  // the type must be int
                           value=ihelp.toString();
                        } else {
			   Error.error("can not handle result type "+
			              result.getClass().getName()+"\n"+
                          "SystemEntry::evaluateExpression");
                           value = "";
                        } 
                            
			//Double d = (Double) result;
			//String value = new Integer(d.intValue()).toString();
			// System.out.println("nakedExpression --> "+value);
			expanded = expanded.substring(0, exprStart) + value
					+ expanded.substring(exprEnd + 1);
//System.out.println("new expended="+expanded);

			// test for further expressions in the expanded string
			exprStart = expanded.indexOf('[');
		}
		return expanded;

	}

}
