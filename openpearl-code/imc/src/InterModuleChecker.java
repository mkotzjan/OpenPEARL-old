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

/**
 * utility to check the compatibility of system part and problem part resources.
 * 
 * @author mueller
 *
 */
public class InterModuleChecker {
	static String target;
	static String outputFile;
	static TargetPlatformXml targetXml;

	static List<String> inputFiles = new ArrayList<String>();
	static List<ModuleXml> moduleXml = new ArrayList<ModuleXml>();

	static boolean verbose = false;

	/**
	 * the main entry
	 * 
	 * command line arguments are documented by the option --help
	 * 
	 * @param args
	 * @throws Exception
	 */
	public static void main(String[] args) throws Exception {

		if (args.length < 1) {
			printHelp();
			System.exit(1);
			return;
		}
 
		if (!checkAndProcessArguments(args)) {
			System.exit(1);
			return;
		}
		
		// set default output file name
		if (outputFile == null) {
			outputFile="system.cc";
		}
		
		Error.setShowInfo(verbose);
		if (target == null) {
			System.err.println("no target specified -- use -b option");
			System.exit(1);
		}
		if (inputFiles.isEmpty()) {
			System.err.println("no input file specified -- error exit");
			System.exit(1);
		}
		if (verbose) {
			System.out.println("Target=" + target);
			for (int i = 0; i < inputFiles.size(); i++) {
				System.out.println("Input file: " + inputFiles.get(i));
			}
		}

		// read target-xml
		targetXml = new TargetPlatformXml(target + ".xml", false);


		// read all input files
		for (int i = 0; i < inputFiles.size(); i++) {
			ModuleXml mXml = new ModuleXml(inputFiles.get(i) + ".xml", targetXml, false);
			moduleXml.add(mXml);
		}

		Error.exitIfErrors();
		
		// check system-problem-part
		for (int i = 0; i < inputFiles.size(); i++) {
			moduleXml.get(i).checkSystemEntries();
		}
		
		//SystemEntries.dump();
		SystemEntries.check();
		Error.exitIfErrors();
		
		for (int i=0; i<inputFiles.size(); i++) {
			moduleXml.get(i).checkProblemPart();
		}
		Error.exitIfErrors();
		

		
		// create system.cpp
		CodeGenerator.create(outputFile);
		SystemEntries.searchUnUsed();
	}

	private static boolean checkAndProcessArguments(String[] args) {
		int i = 0;
		boolean returnValue = true;

		while (i < args.length) {
			String arg = args[i++];
			if (arg.equals("--help")) {
				printHelp();
			} else if (arg.equals("--verbose")) {
				verbose = true;
			} else if (arg.equals("-b")) {
				target = args[i++];
			} else if (arg.equals("-o")) {
				outputFile = args[i++];
			} else if (arg.charAt(0) != '-') {
				// normal input file
				inputFiles.add(arg);
				continue;
			} else {
				System.err.println("unknown option (" + arg + ")");
				returnValue = false;
			}
		}

		return returnValue;
	}

	private static void printHelp() {

		System.err
				.println("java InterModuleChecker                           \n"
						+ " Options:                                                     \n"
						+ "  --help                      Print this help message         \n"
						+ "  --verbose                   Print lot of information        \n"
						+ "  -b <target>                 Disable semantic checker        \n"
						+ "  -o <output>                 output file (default: system.cc)\n"
						+ "  infile ...                  input files					   \n");
	}

}
