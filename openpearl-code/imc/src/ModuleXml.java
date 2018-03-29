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

import java.util.ArrayDeque;
import java.util.Deque;

import org.w3c.dom.Document;
import org.w3c.dom.Node;
import org.w3c.dom.NodeList;

/**
 * read xml definition for module import/export and provide specific operations,
 * like search for names as well as required and valid parameters
 * 
 * @author mueller
 * 
 */

public class ModuleXml {
	String indent = "";
	Document moduleXML;
	boolean verbose;
	String sourceFileName;
	TargetPlatformXml targetXml;
	ReadXml wrappedDomTree;
	int line;
	SystemEntry un;
	Deque<SystemEntry> associationStack = new ArrayDeque<SystemEntry>();

	/**
	 * create an object for the query operations for the target platform
	 * elements, like SIGNAL, DATION and INTERRUPT
	 * 
	 * @param fileName
	 *            platform definition file name
	 * @param verbose
	 *            flag for verbose output; if true lot of messages are sent to
	 *            System.out
	 */
	ModuleXml(String fileName, TargetPlatformXml targetXml, boolean verbose) {
		this.verbose = verbose;
		this.targetXml = targetXml;
		wrappedDomTree = new ReadXml(fileName, verbose, null);

		moduleXML = wrappedDomTree.getDocument(); // readXMLDocumentFromFile(fileName);
		if (moduleXML == null) {
			System.err.println("error reading module definition file ("
					+ fileName + ")");
			System.exit(1);
			return;
		}

		if (verbose) {
			wrappedDomTree.dumpDomTree();
		}

		Node m = getChildByName(moduleXML, "module");
		sourceFileName = m.getAttributes().getNamedItem("file")
				.getTextContent();
	}

	/**
	 * check existence and type of used system names, associations and
	 * configuration elements on the target platform *
	 * <p>
	 * <b>Note:</b> the methode emits error messages in case of problems. The
	 * check is aborted by the main program, if errors are detected in one stage
	 * of translation
	 */
	public void checkSystemEntries() {
		NodeList systemElements = moduleXML.getElementsByTagName("system");
		if (systemElements == null) {
			// no system part present
			return;
		}
		Node targetNode = null;

		// get all defined system elements
		try {
			systemElements = systemElements.item(0).getChildNodes();
		} catch (NullPointerException e) {
			// no system part
			return;
		}

		if (systemElements.getLength() > 0) {
			// search desired signal in the text section of concrete
			// signal-entry
			String userName = "config-element";
			for (int i = 0; i < systemElements.getLength(); i++) {
				Node n = systemElements.item(i); // .getChildNodes();
				if (n.getNodeType() == Node.ELEMENT_NODE
						&& n.getNodeName().equals("username")) {
					userName = n.getAttributes().getNamedItem("name")
							.getTextContent();
					line = Integer.parseInt(n.getAttributes()
							.getNamedItem("line").getTextContent());
					Error.setLocation(sourceFileName, line);
					Error.info("username definition '" + userName + "' found");

					un = SystemEntries.contains(userName);
					if (un != null) {
						// same user name found!
						if (un.getNode() != null) {
							// was complete definition --> create new entry and
							// produce the error message later
							un = new SystemEntry(userName, sourceFileName, line);
							SystemEntries.add(un);
						} else {
							un.setLocation(sourceFileName, line);
						}
					} else {
						un = new SystemEntry(userName, sourceFileName, line);
						SystemEntries.add(un);

					}
					

				} else if (n.getNodeType() == Node.ELEMENT_NODE
						&& n.getNodeName().equals("configuration")) {
					line = Integer.parseInt(n.getAttributes()
							.getNamedItem("line").getTextContent());
					Error.setLocation(sourceFileName, line);
					Error.info("configuration entry found");
					un = new SystemEntry(sourceFileName, line);
					SystemEntries.add(un);

				} else {
					continue; // lets check the next element
				}
				
				// treat association for username and configuration element
				// un is the current SystemEntry
				
				targetNode = treatSysName(n);
				if (targetNode == null) {
					System.err.println("configuration problem");
				}
				un.setTargetNode(targetNode);

				String mustProvide[] = un.getMustProvide();
				for (int p = 0; p < mustProvide.length; p++) {
					// check if the new detected system element provides the
					// expected interface
					// System.out.println("test "+mustProvide[p]);
					Node assocProvider = TargetPlatformXml.provides(
							targetNode, mustProvide[p]);
					if (assocProvider == null) {
						Error.error(userName
								+ " does not provide previously required interface '"
								+ mustProvide[p] + "'");
					} else {
						if (!un.setMaxClients(
								mustProvide[p],
								TargetPlatformXml
										.getAssociationClients(assocProvider))) {
							Error.error("too many clients for '"
									+ un.getName() + "'");
						}
					}
				}
			}

		}

		if (verbose) {
			wrappedDomTree.dumpDomTree();
		}

	}

	private Node treatSysName(Node n) {
		NodeList childs = n.getChildNodes();
		Node targetNode;

		for (int child = 0; child < childs.getLength(); child++) {
			Node currentChild = childs.item(child);

			if (currentChild.getNodeType() == Node.ELEMENT_NODE
					&& currentChild.getNodeName().equals("sysname")) {

				String systemName = currentChild.getAttributes()
						.getNamedItem("name").getTextContent();
//System.out.println("systemname found " + systemName);
				un.setSystemName(systemName);
				targetNode = targetXml.hasSystemName(systemName);
				if (targetNode != null) {
					targetXml.incrementInstances(targetNode);
					Error.info("System Name '" + systemName + "' has type '"
							+ TargetPlatformXml.getNodeType(targetNode));
					if (compareParameterTypes(currentChild, targetNode)) {
						Error.info("parameter types are ok");
					} else {
						Error.error("parameter mismatch for system name '"
								+ systemName + "'");
					}

					String typeOfElement = TargetPlatformXml
							.getNodeType(targetNode);
					/*
					 * Node typeNode = moduleXML.createElement(typeOfElement);
					 * Element e = (Element) n; e.appendChild(typeNode);
					 */
					un.setType(typeOfElement);

					checkAssociation(n, targetNode);
					/*
					 * ((Element) n).setAttribute("instanceCount", "yes"); }
					 * else { ((Element) n).setAttribute("instanceCount", "no");
					 * }
					 */
				}
				return targetNode;
			}
		}
		Error.error("no sysname attribute in node");
		return null;

	}

	private void checkAssociation(Node moduleNode, Node systemNode) {
		String provider = TargetPlatformXml
				.associationRequiredProvider(systemNode);
		Node association = null;
		// the first association resides in sysname
		// nested associations are located in moduleNode
		// try to find 'sysname'
		// - if present we are in a username or configuration item
		// - if NOT present we are in an association, lets try to find the next
		// level
		Node sysname = getChildByName(moduleNode, "sysname");
		if (sysname != null) {
			association = getChildByName(sysname, "association");
		} else {
			association = getChildByName(moduleNode, "association");
		}
		if (association != null) {
			String associationName = association.getAttributes()
					.getNamedItem("name").getTextContent();

			// System.out.println("association=" + associationName);
			// System.out.println("required provider=" + provider);
			Node associationSystemNode = targetXml
					.hasSystemName(associationName);
			if (associationSystemNode == null) {
				// System.out.println("maybe is user name");
				SystemEntry u = SystemEntries.contains(associationName);
				if (u == null) {
					// System.out.println("username may not been defined yet");
					// define this name - as long as there is no system node
					// connected with it, no code can be produced
					u = new SystemEntry(associationName, sourceFileName, line);
					if (!u.incrementAssociationClients(provider)) {
						Error.error("too many associations for " + provider);
					}
					SystemEntries.add(u);
					un.setProvider(u);
					return;
				} else {
					// System.out.println("user name " + associationName
					// + " is defined");
					// System.out.println("test for connection provider is missing");
					// test if username provides interface is missing
					Node assoc;

					if (u.getNode() != null) {
						assoc = TargetPlatformXml.provides(u.getNode(),
								provider);
						if (assoc == null) {
							Error.error(u.getName()
									+ " does not provide association type "
									+ provider);
							return;
						}
						u.setMaxClients(provider,
								TargetPlatformXml.getAssociationClients(assoc));
					} else {
						// mark that this system element must provide an
						// association of type provider
						u.mustProvide(provider);
					}

					if (!u.incrementAssociationClients(provider)) {
						Error.error("too many associations for '" + provider
								+ "'");
					}

					un.setProvider(u);
					return;
				}
			} else {
				Node assoc = TargetPlatformXml.provides(associationSystemNode,
						provider);
				if (assoc == null) {
					// the error message is already emitted in provides(...)
					return;
				} else {
					// System.out.println("connection type is ok -- lets check the parameters...");
					associationStack.push(un);
					SystemEntry u = new SystemEntry(sourceFileName, line);
					u.setSystemName(associationName);
					SystemEntries.add(u);
					un.setProvider(u);

					int maxClients = TargetPlatformXml
							.getAssociationClients(assoc);
					if (!u.setMaxClients(provider, maxClients)) {
						Error.error("too many clients for '" + provider + "'");
					}

					un = u;
					un.setTargetNode(associationSystemNode);
					if (compareParameterTypes(association,
							associationSystemNode) == false) {
						Error.error("parameter mismatch in connection Provider");
						return;
					}

					if (!u.incrementAssociationClients(provider)) {
						Error.error("too many associations for '" + provider
								+ "'");
					}
					if (TargetPlatformXml
							.associationRequiredProvider(associationSystemNode) != null) {
						// System.out.println("need to check for required associationType: "+
						// targetXml.associationRequiredProvider(associationSystemNode));
						// System.out.println("in association: "+associationName);

						checkAssociation(association, associationSystemNode);
					}
					un = associationStack.pop();

					return;
				}
			}
		} else {
			// no association in system part, let's check if system device needs
			// an association
			if (provider != null) {
				Error.error("system unit requires association with type '"
						+ provider +"'");
				return;
			}
			return;
		}
	}

	private int nbrParameters(Node n) {
		if (n != null) {
			NodeList childs = n.getChildNodes();
			int nbr = 0;

			for (int i = 0; i < childs.getLength(); i++) {
				if (childs.item(i).getNodeType() == Node.ELEMENT_NODE) {
					// System.out.println("ElementNode "
					// + childs.item(i).getNodeName());
					nbr++;
				}
			}
			return nbr;
		} else {
			return 0;

		}
	}

	/**
	 * find the child node of the given node with the given name
	 * 
	 * @param n
	 *            the node, which is expected to contain a child with the given
	 *            name
	 * @param name
	 *            the name of the child node
	 * @return null, if there is non fitting child; else the detected child node
	 */
	public static Node getChildByName(Node n, String name) {
		NodeList nList = n.getChildNodes();
		for (int i = 0; i < nList.getLength(); i++) {
			// if (nList.item(i).getNodeType() == Node.ELEMENT_NODE) {
			// System.out.println(nList.item(i).getNodeName());
			// }

			if (nList.item(i).getNodeType() == Node.ELEMENT_NODE
					&& nList.item(i).getNodeName().equals(name)) {
				return nList.item(i);
			}
		}
		return null;
	}

	private boolean compareParameterTypes(Node inModule, Node inTarget) {
		Node moduleParameters = getChildByName(inModule, "parameters");
		Node targetParameters = getChildByName(inTarget, "parameters");

		int nbrModuleParameters = nbrParameters(moduleParameters);
		int nbrTargetParameters = nbrParameters(targetParameters);

		// System.out.println("#P1= " + nbrModuleParameters + "  #P2="
		// + nbrTargetParameters);

		/*
		 * if (nbrModuleParameters != nbrTargetParameters) {
		 * System.err.println("number of parameters does not match (" +
		 * nbrModuleParameters + " != " + nbrTargetParameters + ")"); }
		 */
		if (nbrModuleParameters != nbrTargetParameters) {
			Error.error("number of parameter mismatch");
			return false;
		}
		if (nbrModuleParameters == 0) {
			return true; // both are zero
		}

		// check each parameter to have the same type
		NodeList moduleParameterNodes = moduleParameters.getChildNodes();
		NodeList targetParameterNodes = targetParameters.getChildNodes();
		int targetParameterIndex = 0;

		for (int i = 0; i < moduleParameterNodes.getLength(); i++) {
			if (moduleParameterNodes.item(i).getNodeType() == Node.ELEMENT_NODE) {
				// System.out.println("ElementNode "
				// + moduleParameterNodes.item(i).getNodeName());

				Parameter p1 = new Parameter(moduleParameterNodes.item(i));
				// System.out.println("P1: " + p1.getValue() + " " +
				// p1.getType()
				// + " " + p1.length());

				// find next target parameter
				while (targetParameterIndex < targetParameterNodes.getLength()
						&& targetParameterNodes.item(targetParameterIndex)
								.getNodeType() != Node.ELEMENT_NODE) {
					targetParameterIndex++;
				}

				if (targetParameterIndex >= targetParameterNodes.getLength()) {
					System.err.println(sourceFileName + 0
							+ ": too many parameters");
					return false;
				}

				TargetPlatformXml
						.checkParameterTypeAndValue(
								targetParameterNodes.item(targetParameterIndex),
								p1, un);
				targetParameterIndex++;
				un.addParameter(p1);
			}
		}

		return true;
	}

	/**
	 * check the problem part entries to fit to the system part definitions
	 */
	public void checkProblemPart() {
		NodeList problemElements = moduleXML.getElementsByTagName("problem");

		if (problemElements == null) {
			// no problem part
			return;
		}
		// get all defined system elements
		try {
			problemElements = problemElements.item(0).getChildNodes();
		} catch (NullPointerException e) {
			// no problem part
			return;
		}
		if (problemElements.getLength() > 0) {
			// search desired signal in the text section of concrete
			// signal-entry
			for (int i = 0; i < problemElements.getLength(); i++) {
				Node n = problemElements.item(i); // .getChildNodes();
				if (n.getNodeType() == Node.ELEMENT_NODE
						&& n.getNodeName().equals("spc")) {
					String userName = n.getAttributes().getNamedItem("name")
							.getTextContent();
					line = Integer.parseInt(n.getAttributes()
							.getNamedItem("line").getTextContent());
					String type = n.getAttributes().getNamedItem("type")
							.getTextContent();

					Error.setLocation(sourceFileName, line);
					Error.info("SPC found type= '" + type + "' '" + userName
							+ "' found");
					un = SystemEntries.contains(userName);
					if (un == null) {
						Error.error("no system definition for '" + userName
								+ "' defined.");
						continue; // next element in loop
					}
					un.markAsUsed();
					// ignore case since the compiler sets the type for signals
					// and interrupts in capital letters
					if (!un.getType().equalsIgnoreCase(type)) {
						Error.error(userName + " is expected as type '" + type
								+ "' but is defined as '" + un.getType()
								+ "'\nprevious declaraion is here: "
								+ un.getFileName() + ":" + un.getLine());
						continue;
					}
					// Signal and interrupt are completly tested
					// dations must support required elements
					if (type.equals("dation")) {
						checkDationAttributes(n, un);
					}
				} else if (n.getNodeType() == Node.ELEMENT_NODE
						&& n.getNodeName().equals("dcl")) {
					Error.error("DCL  entry found -/- not supported yet");

				}
			}

		}

	}

	/**
	 * check if attributes and data-type fit to the system device given by the
	 * userName The username object has a reference to the system entry in
	 * targetXml
	 * 
	 * @param n
	 *            the node of the specification entry
	 * @param un
	 *            the username from the system part
	 */
	private void checkDationAttributes(Node n, SystemEntry un) {
		// check data type
		String attributesInSpc = getChildByName(n, "attributes")
				.getTextContent();
		// System.out.println("SPC attr:" + attributesInSpc);

		String attributesInDevice = getChildByName(un.getNode(), "attributes")
				.getTextContent();
		// System.out.println("DCL attr:" + attributesInDevice);

		String[] attrListInSpc = attributesInSpc.split(",");
		String[] attrListInDevice = attributesInDevice.split(",");

		for (int i = 0; i < attrListInSpc.length; i++) {

			boolean found = false;

			for (int j = 0; j < attrListInDevice.length; j++) {
				if (attrListInDevice[j].trim().equals(attrListInSpc[i].trim())) {
					found = true;
				}
			}
			if (!found) {
				Error.error("dation attribute '" + attrListInSpc[i]
						+ "' not supported by system device");
			}
		}

		// check data type
		String dataInSpc = getChildByName(n, "data").getTextContent();
		// System.out.println("SPC data:" + dataInSpc);

		Node nDev = getChildByName(un.getNode(), "data");
		if (nDev == null) {
			Error.error("System Entry '" + un.getSystemName()
					+ "' contains no <data>-tag");
			return;
		}
		String dataInDevice = nDev.getTextContent();
		// System.out.println("DCL data:" + dataInDevice);

		// a system dation with type ALL accepts all data types
		// ALPHIC accepts ALPHIC and CHAR
		boolean dataTypeMatch = false;

		if (dataInDevice.equals("ALL")) {
			dataTypeMatch = true;
		}
		if (dataInDevice.equals("ALPHIC")) {
			if (dataInSpc.trim().equals("CHAR")) {
				dataTypeMatch = true;
			}
			if (dataInSpc.trim().equals("ALPHIC")) {
				dataTypeMatch = true;
			}
		}
		// expand nicknames
		dataInDevice = un.evaluateExpression(dataInDevice);

		if (dataInDevice.trim().equals(dataInSpc.trim())) {
			dataTypeMatch = true;
		}
		if (!dataTypeMatch) {
			Error.error("system device does not support data type '"
					+ dataInSpc.trim() + "'");
		}
	}

	/*
	 * private String expandNicknames(String dataInDevice, SystemEntry un) {
	 * 
	 * int nickNameStart = dataInDevice.indexOf('$'); int nickNameEnd; if
	 * (nickNameStart >= 0) { nickNameEnd = nickNameStart; boolean isLetter; do
	 * { nickNameEnd ++; char currentChar = dataInDevice.charAt(nickNameEnd);
	 * isLetter = (currentChar >= 'A' && currentChar <= 'Z') | (currentChar >=
	 * 'a' && currentChar <= 'z'); } while (isLetter);
	 * 
	 * String nickName = dataInDevice.substring(nickNameStart+1, nickNameEnd);
	 * String nickValue = un.getNickNameValue(nickName); dataInDevice =
	 * dataInDevice.substring(0,nickNameStart) + nickValue +
	 * dataInDevice.substring(nickNameEnd); } return dataInDevice; }
	 */
}
