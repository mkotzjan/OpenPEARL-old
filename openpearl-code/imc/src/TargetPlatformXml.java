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

import org.w3c.dom.Document;
import org.w3c.dom.Element;
import org.w3c.dom.NamedNodeMap;
import org.w3c.dom.Node;
import org.w3c.dom.NodeList;

/**
 * read xml definition for target system resources and provide specific
 * operations, like search for names as well as required and valid parameters.
 * 
 * 
 * @author mueller
 * 
 */

public class TargetPlatformXml {
	String indent = "";
	Document targetPlatform;
	boolean verbose;

    
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
	TargetPlatformXml(String fileName, boolean verbose) {
		this.verbose = verbose;
	
		ReadXml tgt = new ReadXml(fileName, verbose,  InstallationLocation.path+"/lib/");

		targetPlatform = tgt.getDocument(); // readXMLDocumentFromFile(fileName);
		if (targetPlatform == null) {
			System.err.println("error reading target definition file ("
					+ fileName + ")");
			System.exit(1);
			return;
		}

		if (verbose) {
			tgt.dumpDomTree();
		}
	}

	/**
	 * check if a system signal name exists
	 * 
	 * @param systemName 	 *            the expected system name 
	 * @return Node, of the entry, if the name exists on this target
	 *         platform <br>null, if the system name does not exist
	 */
	public Node hasSystemName(String systemName) {

		// get entry point in tree for signals
		NodeList nl = targetPlatform.getElementsByTagName("platform");
		nl = nl.item(0).getChildNodes();
		if (nl.getLength() > 0) {
			for (int i = 0; i < nl.getLength(); i++) {
				Node n = nl.item(i); // .getChildNodes();
				String sName;
				sName = null;
				if (n.getNodeType() == Node.ELEMENT_NODE
						&& n.getAttributes() != null) {
					if (n.getAttributes() != null) {
						if (n.getAttributes().getNamedItem("name") != null) {
							sName = n.getAttributes().getNamedItem("name")
									.getTextContent();
						}
					}

					if (sName != null && sName.equals(systemName)) {
						Error.info(systemName + " is available and has type "
								+ getNodeType(n));
						return n;
					}
				}
			}
		}
		Error.info(systemName + " is NOT available - maybe it is a user defined name");

		return null;
	}

	/**
	 * return the type of the given node as string
	 * 
	 * @param n
	 *            the node to be queried
	 * @return the type of the queried node --> "dation", "interrupt", ...
	 */
	static public String getNodeType(Node n) {
		String type = n.getNodeName();
		return type;
	}

	/**
	 * check, whether the given node provides the given kind of association
	 * 
	 * @param n
	 *            the node to be queried
	 * @param associationName
	 *            the name of the association
	 * @return true, if supported, else false
	 */
	static public Node provides(Node n, String associationName) {
		Node association = ModuleXml.getChildByName(n, "associationProvider");
		if (association != null) {
			NodeList cl = association.getChildNodes();
			for (int i = 0; i < cl.getLength(); i++) {
				if (cl.item(i).getNodeType() == Node.ELEMENT_NODE
						&& cl.item(i).getNodeName().trim()
								.equals("associationType")) {
					// System.out.println(cl.item(i).getNodeName());
					String cName = cl.item(i).getAttributes()
							.getNamedItem("name").getTextContent();
					if (cName.equals(associationName)) {
						Error.info("association type " + associationName
								+ " found");
						return cl.item(i);
					}
				}
			}
		}
		Error.error("association " + associationName + " not provided");
		return null;
	}

	/**
	 * read the number of registered clients for an association provider
	 * 
	 * The number of registered clients is stored as attribute of the node in the
	 * DOM tree
	 * 
	 * @param associationProvider
	 *            the node of the association provider
	 * @return the number of registered clients
	 */
	static int getAssociationClients(Node associationProvider) {
		Node a = associationProvider.getAttributes().getNamedItem("clients");
		int n = -1;

		if (a != null) {
			String nbr = a.getTextContent().trim();

			try {
				n = Integer.parseInt(nbr);
			} catch (NumberFormatException e) {
				n = -1;
			}
		}
		return (n);
	}

	/**
	 * check the type and value for the given system device node parameter
	 * 
	 * The rule are specified in the system devive parameter description, like
	 * FIXEDGT, FIXEDRANGE, ... This list is expected to be extended when
	 * needed.
	 * 
	 * @param n
	 *            the formal parameter to be checked
	 * @param p
	 *            the actual parameter from the user module
	 * @return true, if type and value are ok; false else
	 */
	static boolean checkParameterTypeAndValue(Node n, Parameter p, SystemEntry systemEntry) {
		String type;
		int length;

		type = n.getNodeName();
		Node lengthNode = n.getAttributes().getNamedItem("length");
		if (lengthNode == null) {
			Error.error("no attribute length for parameter for '" + systemEntry.getSystemName() +
					"' in system definition file");
			return false;
		}
		String lengthIsNumericalOrNickname = lengthNode.getTextContent();
		if (lengthIsNumericalOrNickname.contains("$")) {
		   System.out.println("lookup in nicknames missing");	
		   length = -1; // should produce an error 
		} else {
		   length = Integer.parseInt(lengthIsNumericalOrNickname);
		}
		
		if (type != p.getType()) {
			Error.error("parameter type mismatch (" + type + " -- "
					+ p.getType());
			return false;
		}
		if (length < p.length()) {
			Error.error("parameter of type " + type + "(" + length
					+ ") does not fit into expected type" + p.getType() + "("
					+ p.length() + ")");
			return false;
		}

		// look for a nickname of this parameter
		Node nickName = n.getAttributes().getNamedItem("nick");
		if (nickName!= null) {
			p.setNickName(nickName.getTextContent());
		}
		
		// test value
		NodeList childs = n.getChildNodes();
		for (int i = 0; i < childs.getLength(); i++) {
			if (childs.item(i).getNodeType() != Node.ELEMENT_NODE) {
				continue;
			}
			// rule found
			String rule = childs.item(i).getNodeName();
			String ruleContent = childs.item(i).getTextContent();
			Error.info("Rule " + rule + "  content " + ruleContent);
			if (rule.equals("VALUES")) {
				String[] items = ruleContent.split(",");
				for (int j = 0; j < items.length; j++) {
					if (items[j].trim().equals(p.getValue())) {
						return true;
					}
				}
				Error.error("illegal value: " + p.getValue()
						+ " not in supported list");
				return false;
			} else if (rule.equals("ConsistsOf")) {
				// only the elements of the comma separated list are allowed in the parameter
				boolean found=false;
				boolean ok=true;
				String[] ruleItems = ruleContent.split(",");
				String[] paramItems = p.getValue().split(" ");
				for (int pi =0; pi < paramItems.length; pi++) {
					found = false;
					for (int ri=0; ri < ruleItems.length && found == false; ri++) {			
						if (paramItems[pi].trim().equals(ruleItems[ri].trim())) {
							found = true;
						}
					}
					if (!found) {
						Error.error("parameter value "+paramItems[i]+" not supported");
						ok = false;
					}
				}
				return ok;
			} else if (rule.equals("FIXEDRANGE")) {
				String[] items = ruleContent.split(",");
				String evaluated = systemEntry.evaluateExpression(items[0]);
				int low = Integer.parseInt(evaluated);
				evaluated = systemEntry.evaluateExpression(items[1]);
//System.out.println("evaluated high as string: "+evaluated+"  item[1]="+items[1]);
				int high = Integer.parseInt(evaluated);
				int val = Integer.parseInt(p.getValue());
				if (val >= low && val <= high) {
					return true;
				} else {
					Error.error("value \"" + val + "\" out of range [" + low
							+ "," + high + "]");
					return false;
				}
			} else if (rule.equals("FIXEDGT")) {
				int low = Integer.parseInt(systemEntry.evaluateExpression(ruleContent));

				int val = Integer.parseInt(p.getValue());
				if (val > low) {
					return true;
				} else {
					Error.error("value out of range (val=" + val
							+ ") not > as " + low + ")");
					return false;
				}
			} else if (rule.equals("NotEmpty")) {
				if (p.length() > 0) {
					return true;
				} else {
					Error.error("value must not be empty");
					return false;
				}

			} else if (rule.equals("ALL")) {
				return true;
			} else {
				Error.info("no rule found -- accept everything");
				return true;
			}
		}

		return false;
	}

	/**
	 * test, whether the current element need an association provider
	 * 
	 * @param node
	 *            the node of the system element
	 * @return null, if no association provider is needed, or the name of the
	 *         association provider, if there is one specified
	 */
	static String associationRequiredProvider(Node node) {
		NodeList nl = node.getChildNodes();

		if (nl.getLength() > 0) {
			for (int i = 0; i < nl.getLength(); i++) {
				Node n = nl.item(i);
				if (n.getNodeType() == Node.ELEMENT_NODE
						&& n.getNodeName().equals("needAssociation")) {
					// get name and remove all surrounding white space characters
					NamedNodeMap nm = n.getAttributes();
					Node n1 = nm.getNamedItem("name");
					if (n1 == null) {
						Error.error("Error in platform xml file: needAssociation needs attribute 'name'");
						return null;
					} 
					String provider = n1.getTextContent();
					return provider;
				}
			}

		}
		return null;
	}

	/**
	 * increment the instanceCount counter, when a new client for an association was
	 * detected. The counter is located in the DOM tree.
	 * 
	 * The counting of instances is only done, if the the attribute 'instances'
	 * is specified, which defines the maximum of allowed instances to this
	 * kink of system element in one PEARL application.
	 * 
	 * @param targetNode
	 *            the node of the system element
	 */
	public void incrementInstances(Node targetNode) {
		Node instances = targetNode.getAttributes().getNamedItem("instances");
		// Node instances = ModuleXml.getChildByName(targetNode, "instances");
		if (instances != null) {
			int maxInstances = Integer.parseInt(instances.getNodeValue());
			// System.out.println(targetNode.getAttributes().getNamedItem("name")
			// .getTextContent()
			// + " provides instances: " + maxInstances);

			String currentValue = ((Element) targetNode)
					.getAttribute("instanceCount");
			int newCount = 1;
			if (currentValue != null && currentValue.length() > 0) {
				// System.out.println("current: "+ currentValue);
				newCount = Integer.parseInt(currentValue) + 1;
				if (newCount > maxInstances) {
					Error.error("too many instances for element "
							+ ((Element) targetNode).getAttribute("name"));
					return;
				}
			}
			
			// set new instance count into dom tree
			// or write the count as 1, if the counter did not exist
			((Element) targetNode).setAttribute("instanceCount",
						Integer.toString(newCount));
		}
	}
}
