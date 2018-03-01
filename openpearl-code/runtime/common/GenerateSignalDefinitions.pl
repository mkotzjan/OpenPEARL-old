#!/usr/bin/perl
#/*
# [The "BSD license"]
# Copyright (c) 2013-2014 Florian Mahlecke
# Copyright (c) 2014 Rainer Mueller
# All rights reserved
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
#
# 1. Redistributions of source code must retain the above copyright
#    notice, this list of conditions and the following disclaimer.
# 2. Redistributions in binary form must reproduce the above copyright
#    notice, this list of conditions and the following disclaimer in the
#    documentation and/or other materials provided with the distribution.
# 3. The name of the author may not be used to endorse or promote products
#    derived from this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
# IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
# OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
# IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
# INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
# NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
# DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
# THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
# THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#*/

###########################################################
#changes:
#   July 2014: rm : enhancement for header/source file creation
#                   selection of plattforms doen by command line parameters


###########################################################
use strict;
#use warnings;
no warnings;

#### README ######
#
# needed module http://search.cpan.org/~hmbrand/Spreadsheet-Read/Read.pm
# install via CPAN, 
#             ActivePerl package manager(Win) or 
#             distribution package management system
#
# for LibreOffice spredsheets CPAN package Spreadsheet::ReadSXC is needed!
#
# PLEASE IGNORE WARNING:
#	Use of uninitialized value in join or string at \
#		/usr/share/perl5/site_perl/Spreadsheet/ReadSXC.pm line 198. 
# 
# FIX IN PROGRESS ...
#####################

use Spreadsheet::Read;

# path to data sheet
my $file = "Signals.ods";

#####################################################
# Usage
# Choose system type by command line parameter
#  specifiy all plattforms as denoted in the spreadsheet as
#  sepatate parameters (without quotes)
#  eg: perl GenerateSignals.pl LINUX
#
my @type;
my $target;
my $firstOutput;
my $fileHandle;
my $nbrOfSignals;

@type=@ARGV;

# read data sheet
my $xls  = ReadData($file);

# nr of spreadsheets 
my $nr_of_sheets = $xls->[0]->{'sheets'};
print "debug infos: \n";
print "Number of sheets: ". $nr_of_sheets."\n";

#############################################
# gather tab names from list in sheet#1
# required sheets are listed in tab#0 in first column starting in A3
# create dummy array with 26 entries, this should be sufficient
my @groupName = ("A" .. "Z");
my @groupIndex = (1..26);
my $groupCounter = 0;
my @groupDescr = ("A" .. "Z");

while ($xls->[1]{cell}[1][$groupCounter+3] ne "") {
   $groupName[$groupCounter]       = $xls->[1]{cell}[1][$groupCounter+3];
   if ($xls->[1]{cell}[2][$groupCounter+3] eq "") {
	die "missing starting index for group ".$groupName[$groupCounter]."\n";
   }
   $groupIndex[$groupCounter] = $xls->[1]{cell}[2][$groupCounter+3];
   $groupDescr[$groupCounter]       = $xls->[1]{cell}[3][$groupCounter+3];
   $groupCounter += 1;
}

#############################################
# verify document structure
if ($nr_of_sheets != $groupCounter+1) {
   die "not (all) sheets listed in group section on sheet#1\n";
}
for (my $i = 0; $i < $groupCounter; $i++) {

   if ($xls->[$i+2]->{'label'} ne $groupName[$i]) {
	die "sheet #".
		$i+2 . ": name mismatch: current(".$xls->[$i+2]->{'label'}.")".
                                       " expected: (".$groupName[$i].")\n";
   }
}
print " end of debug infos */\n";

if ( "LaTeX" ~~ @type) {
   print "create LaTeX groupCounter=".$groupCounter."\n";

   open(my $tex, ">signallist.tex");
   for (my $i = 0; $i < $groupCounter; $i++) {
      print $tex  "\\section{Signal Group: ".$groupName[$i]."}\n";
      print $tex  $groupDescr[$i]."\n\n";

      my $maxrow = $xls->[$i+2]->{'maxrow'};

      # skip header line 
      for (my $r=1; $r < $maxrow+1; $r++) {
           if ( $xls->[$i+2]->{cell}[1][$r+1] ne "") {
              my $sigNumber;
 	      $sigNumber = $xls->[$i+2]{cell}[1][$r+1]+
                              $groupIndex[$i] * 100;
              print $tex "\\begin{tabular}{|p{3.5cm}|p{10cm}|}\n";
	      print $tex "\\hline\n";
              print $tex "Number: \\textbf{".$sigNumber."} &".
                          "Plattform: ". $xls->[$i+2]{cell}[4][$r+1]."\\\\\n";
	      print $tex "\\hline\n";
              print $tex "\\multicolumn{2}{|p{13.5cm}|}{Name: \\textbf{".
                                    $xls->[$i+2]{cell}[2][$r+1]."}}\\\\\n";
	      print $tex "\\hline\n";
              print $tex "\\multicolumn{2}{|p{13.5cm}|}{Message: \\textbf{".
                          $xls->[$i+2]{cell}[5][$r+1]."}}\\\\\n";
	      print $tex "\\hline\n";
              print $tex "\\multicolumn{2}{|p{13.5cm}|}{".
                          $xls->[$i+2]{cell}[6][$r+1]."}\\\\\n";
	      print $tex "\\hline\n";
              print $tex "\\end{tabular}\n\n";
           }
      }
   }
   close($tex);
} else  {

   #############################################
   # create output files
   open(my $header, ">Signals.hh");
   open(my $src, ">Signals.hcc");
   open(my $xml, ">Signals.xml");

   $target = "classImpl";
   $firstOutput = 1;
   $fileHandle = $header;
   &printEntries;

   $firstOutput = 1;
   $target = "extList";
   $fileHandle = $header;
   &printEntries;

   $firstOutput = 1;
   $target = "objDCL";
   $fileHandle = $src;
   &printEntries;
   print $fileHandle "\n\n";

   $firstOutput = 1;
   $nbrOfSignals = 0;
   $target = "objArray";
   $fileHandle = $src;
   &printEntries;

   $firstOutput = 1;
   $nbrOfSignals = 0;
   $target = "Signals.xml";
   $fileHandle = $xml;
   &printEntries;

   close($header);
   close($src);
   close($xml)
}

#####################################################################
sub printEntries
{

   # print header
   if ($target eq "objDCL" ) {
      print $fileHandle "/* ".$target." : auto generated (".localtime().") */\n";
   } elsif ($target eq "objArray" ) {
      print $fileHandle "/* ".$target." : auto generated (".localtime().") */\n";
      printf $fileHandle "Signal *sv[] = {\n"; 
   } elsif ($target eq "classImpl") {
      print $fileHandle "/* ".$target." : auto generated (".localtime().") */\n";
   } elsif ($target eq "extList") {
      print $fileHandle "/* ".$target." : auto generated (".localtime().") */\n";
   } elsif ($target eq "Signals.xml") {
      print $fileHandle "<!-- ".$target." : auto generated (".localtime().") -->\n";
   } else {
      die "*** header: illegal parameter (".$target.")\n";
   }

   # q=1 first spreadsheet = info sheet
   for (my $cur_sheet_nr = 2; $cur_sheet_nr < $nr_of_sheets+1; $cur_sheet_nr++) {
      # max no. of rows
      my $maxrow = $xls->[$cur_sheet_nr]->{'maxrow'};

      foreach (@type) {
	for (my $i=0; $i < $maxrow+1; $i++) {
           if ( $xls->[$cur_sheet_nr]->{cell}[4][$i] eq $_) {
	      my $string;
              my $sigNumber;
		
 	       $sigNumber = $xls->[$cur_sheet_nr]{cell}[1][$i]+
                              $groupIndex[$cur_sheet_nr-2] * 100;

              if ($target eq "objDCL" ) {
                 $string = $xls->[$cur_sheet_nr]{cell}[2][$i]." ".
                        "the".$xls->[$cur_sheet_nr]{cell}[2][$i].";";  
	         print $fileHandle "$string\n";
	      } elsif ($target eq "objArray" ) {
                 $string = "      ";
                 $nbrOfSignals ++;
                 if ($firstOutput == 1) {
                     $firstOutput = 0;
                 } else {
                    $string .= ", ";
                 }
                 $string .= "(Signal*)&the".$xls->[$cur_sheet_nr]{cell}[2][$i];  
	         print $fileHandle "$string\n";
              } elsif ($target eq "classImpl") {
	         if ($xls->[$cur_sheet_nr]{cell}[3][$i] eq undef) {
		     # no parent class given
		     die "*** no parent class specified for ".
                         $xls->[$cur_sheet_nr]{cell}[2][$i]."\n";
		   } else {
                      # parent class specified
		      $string = "class ".$xls->[$cur_sheet_nr]{cell}[2][$i].
 				" : public ".
                                $xls->[$cur_sheet_nr]{cell}[3][$i]." {\n".
				" public:\n ".
			        $xls->[$cur_sheet_nr]{cell}[2][$i].
				"() {\n".
				"   type = (char*)\"".
                                 $xls->[$cur_sheet_nr]{cell}[5][$i]."\";\n".
				"   rstNum = ".$sigNumber.";\n".
				" };\n".
				"};\n";

		   }
		   print $fileHandle "$string\n";
              } elsif ($target eq "extList") {
                $string = "extern ".$xls->[$cur_sheet_nr]{cell}[2][$i].
                          " the".$xls->[$cur_sheet_nr]{cell}[2][$i];  
	         print $fileHandle "$string;\n";
              } elsif ($target eq "Signals.xml") {
                $string = "   <signal name=\"".$xls->[$cur_sheet_nr]{cell}[2][$i]."\"/>";  
	         print $fileHandle "$string\n";
              } else {
		die "*** body: illegal parameter (".$target.")\n";
	      }
	   }
         }
       }	
    }
	
   # print footer
   if ($target eq "objDCL" ) {
   } elsif ($target eq "objArray" ) {
      print $fileHandle "   };\n"; 
      print $fileHandle "size_t Signal::nbrOfSignals = ".$nbrOfSignals.";\n";
      print $fileHandle "Signal** Signal::signalVector = sv;\n";
   } elsif ($target eq "classImpl") {
   } elsif ($target eq "extList") {
   } elsif ($target eq "Signals.xml") {
   } else {
      die "*** footer: illegal parameter (".$_[0].")\n";
   }
}
