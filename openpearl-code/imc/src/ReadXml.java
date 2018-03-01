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

import java.io.File;

import javax.xml.parsers.DocumentBuilder;
import javax.xml.parsers.DocumentBuilderFactory;

import org.w3c.dom.Attr;
import org.w3c.dom.Document;
import org.w3c.dom.NamedNodeMap;
import org.w3c.dom.Node;
import org.w3c.dom.NodeList;

/**
 * read a xml file into a DOM tree and provide some utility methods like dump
 * 
 * @author mueller
 *
 */

public class ReadXml {
	String indent = "";
	Document document;
	boolean verbose;
	String searchPath;
	
	
	/**
	 * read the passed file as xml 
	 * 
	 * @param fileName plattform definition file name
	 * @param verbose  flag for verbose output; if true lot of messages are sent to System.out
	 */
	 ReadXml(String fileName, boolean verbose, String searchPrefix) {
		this.verbose=verbose;
        searchPath = searchPrefix;
        
		document = readXMLDocumentFromFile(fileName);

		
		if (document != null & verbose) {
			traverseDOMTree(document);
		}
	}
	
    Document getDocument() {
    	return (document);
    }
    
    public void dumpDomTree() {
    	traverseDOMTree(document);
    }
    
	private  Document readXMLDocumentFromFile(String file) {
		
		DocumentBuilderFactory factory = DocumentBuilderFactory.newInstance();
		Document xmlDoc=null;
		String prefix = "";
		
       while (xmlDoc == null) {
    	   
       
		try {
			File inputFile = new File(prefix+file);
			DocumentBuilder builder = factory.newDocumentBuilder();
			xmlDoc = builder.parse(inputFile);
			xmlDoc.getDocumentElement().normalize();
		} catch (java.io.FileNotFoundException e) {
			if (prefix.isEmpty() && searchPath!= null) {
			   prefix= searchPath;
			} else {
				return null;
			}
		} catch (Exception e) {
			e.printStackTrace();
			System.err.println("error reading XML file: " + file);
			return null;
		}
       }
		return xmlDoc;
	}

	private  void showNode(Node node) {
		short t;
		t = node.getNodeType();
		if (t == Node.ELEMENT_NODE) {
			System.out.println(indent + "Node: " + node.getNodeName());
			// System.out.println(indent+" TextContent: "+node.getTextContent());

			NamedNodeMap attributes = node.getAttributes();
			for (int i = 0; i < attributes.getLength(); i++) {
				Attr attr = (Attr) attributes.item(i);
				System.out.println(indent + "Attribute: " + attr.getName()
						+ "\t" + attr.getValue());
			}

		} else if (t == Node.TEXT_NODE) {
			String content = node.getTextContent();
			if (!content.trim().isEmpty()) {
				System.out.println(indent + "TextContent: " + content);
			}
		} else if (t == Node.COMMENT_NODE) {
			// comments are ok
		} else if (t == Node.DOCUMENT_NODE) {
			// document node is ok
		} else {
			System.err.println("unexpected node type " + t);
		}

	}

	public  void traverseDOMTree(Node node) {
		showNode(node);
		if (node.hasChildNodes()) {
			NodeList children = node.getChildNodes();
			for (int i = 0; i < children.getLength(); i++) {
				indent += "   ";
				traverseDOMTree(children.item(i));
				indent = indent.substring(3);
			}
		}
	}
}
