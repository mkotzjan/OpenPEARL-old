/**
split the file content into two files
data.check --> data.prl 
           --> data.expect

data.prl contains all line of data.check which are out of the marked sections
data.err contains the marked sections contend

The markers are
'!!!!! BEGIN' and '!!!!! END' at the beginning of a line.

The marked areas are placed into data.expect with the speciality that
the fist lise is prepended with the current filename(.prl):liennumber:colnumber :

The linenumber reflects the last line number written to the .prl-output
eg
some normal text
!!!!! BEGIN
^
ERROR: illegal keyword 'some'
!!!!! END
normal text
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define START_TOKEN "!!!!! BEGIN"
#define END_TOKEN   "!!!!! END"

int main(int narg, char ** argv) {
   char inputname[200];
   char prlname[200];
   char expectname[200];
   int linenumber=0;
   int colnumber;
   char carrotLine[200];
   char inputLine[200];
   char lastPrlLine[200]="";

   int errorMode = 0;   // normal text is presetted

   FILE * input, *prl, *expect;
   char * fgetsResult;

   if (narg != 2) {
      fprintf(stderr,"call %s with filename without extension\n", argv[0]);
      exit(-11);
   }
   
   if (strlen(argv[1]) > 80) {
      fprintf(stderr,"given filename exeeds internal limits (%s)\n", argv[1]);
      exit(-11);
   }

   strcpy(prlname,argv[1]);
   strcat(prlname,".prl");

   strcpy(inputname,argv[1]);
   strcat(inputname,".check");

   strcpy(expectname,argv[1]);
   strcat(expectname,".expect");

   input = fopen(inputname,"r");
   if (input == NULL) {
       fprintf(stderr,"could not read %s\n", inputname);
       exit(-1);
   }
   prl = fopen(prlname,"w");
   if (prl == NULL) {
       fprintf(stderr,"could not create %s\n", prlname);
       exit(-1);
   }
   expect = fopen(expectname,"w");
   if (expect == NULL) {
       fprintf(stderr,"could not create %s\n", expectname);
       exit(-1);
   }

   while (! feof(input) ) {
      fgetsResult = fgets(inputLine, sizeof(inputLine), input);
      if (fgetsResult) {
         // we got some data -- check markers first
         if (strncmp(inputLine,START_TOKEN, strlen(START_TOKEN)) == 0) {
            errorMode=1;
            continue;
         } else if (strncmp(inputLine,END_TOKEN, strlen(END_TOKEN)) == 0) {
            errorMode=0;
            continue;
         }

         // text line
         if (errorMode==1) {
            // start of error mode
            // get position of carrot-marker in line
            if (! strchr(inputLine,'^') ) {
                fprintf(stderr,"expected carrot in line %s", inputLine);
                exit(-1);
            }
            colnumber = strchr(inputLine,'^')-inputLine+1;
            strcpy(carrotLine, inputLine);
            errorMode=2;
         } else if (errorMode == 2) {
            fprintf(expect,"%s:%d:%d: %s" , prlname, linenumber, colnumber, inputLine);
            fwrite(lastPrlLine, 1, strlen(lastPrlLine), expect);
            fwrite(carrotLine, 1, strlen(carrotLine), expect);
            errorMode == 3;
         } else if (errorMode == 3) {
            fwrite(inputLine, 1, strlen(inputLine), expect);
         }
         if (errorMode == 0) {
            linenumber ++;
            fwrite(inputLine, 1, strlen(inputLine), prl);
            strcpy(lastPrlLine,inputLine);
         } 
      }
   }
   if (errorMode != 0) {
      fprintf(stderr,"last error-clause not completed\n");
      exit(-1);
   }
   fclose(input);
   fclose(prl);
   fclose(expect);
}



