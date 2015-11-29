%{
/*
include collector
  - reads from stdin 
  - copies all normal code to output
  - collects system includes in one table and dumps them to "sysincl.h"
  - insert user includes at the appropriate location
  - treats #define/#undef/#ifdef/#ifndef/#else (and copies #if)
  - passthru mode to pass conditional code to the output to enable 
    tests when compiling productive code (like #ifNDEF NAN --> #error)
    Tags must be positioned at the beginning of a line
        //IncludeCollector::PassThru begin 
        //IncludeCollector::PassThru end

the result is 
   - one file with all content of all referenced user include
   - one file with all system includes in the same sequence
     (without any duplicates)

remarks:
   - empty line remain during include processing
   - they may be reduced in a second pass

author: r. mueller (mueller@hs-furtwangen.de)
license: [BSD]
*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/* -------------------------return constants ---------------------*/
#define IDENTIFIER 1
#define CSTRING 2
#define INCLUDE 3
#define SYSINCFILE 4
#define ENDREACHED 5

#define DEFINE 10
#define UNDEF 11
#define IFDEF 12
#define IFNDEF 13
#define ELSE 14
#define ENDIF 15
#define IF 16

//#undef ECHO
//#define ECHO sendToOutput(yytext,yyleng)

 /* -------------------------common variables ---------------------*/

char buf[100];
char *s;
int isOutputOn();
int endInclude();
int sendToOutput(char * s, int length);
int keepComment = 1;

#define INCLUDEDEPTH 	20
static struct FileStack{
   int sp;
   struct IncStackElem {
       YY_BUFFER_STATE entry;
       char fn[128];
       int line;
   } stack[INCLUDEDEPTH];
} fileStack = {0};
  
%}

 /* -------------------------------- definitions ------------------*/

digit     [0-9]
letter    [A-Za-z]
filenamechar	digit|letter|.


 /* --- exclusice start conditions -------------------------------*/
%x STRING SYSINC COMMENT  LINECOMMENT PASSTHRU

%%
"#include"		{return INCLUDE;}
<<EOF>>			{if (endInclude()) {
                             return ENDREACHED;
 			 }
                        }
"#define"[ \t]*		{return DEFINE;}
"#undef"[ \t]*		{return UNDEF;}
"#ifdef"[ \t]*		{return IFDEF;}
"#if"[ \t]*		{return IF;}
"#ifndef"[ \t]*		{return IFNDEF;}
"#else"[ \t]*		{return ELSE;}
"#endif"[ \t]*		{return ENDIF;}
[_a-zA-Z][_a-zA-Z0-9]* 	{
 			strcpy(buf,yytext);
			return(IDENTIFIER); 
		 }
"<"		{ BEGIN SYSINC; s = buf; *s = 0;}
<SYSINC>>	{ *s = 0;
		  BEGIN 0;
                  return SYSINCFILE;
                }
<SYSINC>[a-zA-Z_.0-9/]  {
		 *s++ = *yytext;
                 *s = 0;
		}
<SYSINC>.	{if (isOutputOn()) {
                   fputs("<",yyout);
                   fputs(buf,yyout);
		   ECHO;
                 }
                  BEGIN 0;}

<SYSINC>\n	{if (isOutputOn()) {
                   fputs("<",yyout);
                   fputs(buf,yyout);
		   ECHO;
                 }
                  BEGIN 0;}

\"              { BEGIN STRING; s = buf; }
<STRING>\"      { 
                  *s = 0;
                  BEGIN 0;
		  return CSTRING;
                }
<STRING>\n      { fprintf(stderr,"%s:%d: ***error  : invalid string\n",
                          fileStack.stack[fileStack.sp].fn, 
                          fileStack.stack[fileStack.sp].line);
		  exit(1);
                }
<STRING>.       { *s++ = *yytext; }

"/*"		{BEGIN COMMENT; 
                  if (keepComment && isOutputOn()) {
                     ECHO;
		  }
                }
<COMMENT>\n    {if (keepComment && isOutputOn()){
                   ECHO;
               }
               fileStack.stack[fileStack.sp].line ++;
               }
<COMMENT>.    {if (keepComment && isOutputOn()){
                   ECHO;
               } }
<COMMENT>"*/"    {BEGIN 0;
                 if (keepComment && isOutputOn()) {ECHO;
	     	 }
                }
^"//IncludeCollector::PassThru begin"	{ 
                   BEGIN PASSTHRU;
		}
<PASSTHRU>.	{ if (isOutputOn()) {
               	   ECHO;
		   }
                }
<PASSTHRU>^"//IncludeCollector::PassThru end"	{
		   BEGIN 0;
                }
                    
"//"		{ BEGIN LINECOMMENT;
                  if (keepComment && isOutputOn()) {
		     ECHO; 
                  }
		}
<LINECOMMENT>.	{if (keepComment && isOutputOn()) {
                    ECHO;
                  }
                }
<LINECOMMENT>\n {
		   ECHO;
                   BEGIN 0;
                   fileStack.stack[fileStack.sp].line ++;
                }
\n		{
                  fileStack.stack[fileStack.sp].line ++;
                  if (isOutputOn()){
                       ECHO;
                   }
		}
.		{
                  if (isOutputOn()){
                       ECHO;
                   }
                }
%%
int yywarp() {
   return 1;
}

/* send characters to yyout, checking if max 2 \n are in sequence */

#define SYSINCS	500
static struct SysIncList{
     int nbr;
     char fileName[SYSINCS][30];
} sysIncFiles = {0 };

void addToSysIncList(char * fileName) {
   int i;
   for (i=0; i<sysIncFiles.nbr; i++) {
       if (strcmp(sysIncFiles.fileName[i], fileName) == 0) {
           return;
       }
   }
  
   if (sysIncFiles.nbr < SYSINCS) {
       if (strlen(fileName) < sizeof(sysIncFiles.fileName[0])) {
          strcpy(sysIncFiles.fileName[sysIncFiles.nbr], fileName);
          sysIncFiles.nbr ++;
       } else {
           fprintf(stderr,"%s:%d: ***error  : system include <%s> too long\n",
                          fileStack.stack[fileStack.sp].fn, 
                          fileStack.stack[fileStack.sp].line,
                          fileName);
          _exit(1);
       }
    } else {
           fprintf(stderr,"%s:%d: ***error  : system include list overflow\n",
                          fileStack.stack[fileStack.sp].fn, 
                          fileStack.stack[fileStack.sp].line);
       _exit(1);
    }
}

#define DEFINEENTRIES	500
static struct DefineList{
     int nbr;
     char defName[DEFINEENTRIES][60];
} defineList = {0 };

void removeFromDefines(char * name) {
   int i;
   for (i=0; i<defineList.nbr; i++) {
       if (strcmp(defineList.defName[i], name) == 0) {
           defineList.defName[i][0] = 0;
       }
   }
}

int isInDefines(char * name) {
   int i;
   for (i=0; i<defineList.nbr; i++) {
       if (strcmp(defineList.defName[i], name) == 0) {
           return 1;
       }
   }
   return 0;
}



void addToDefines(char * name) {
   int i;
   int alreadyDefined = 0;
   for (i=0; i<defineList.nbr; i++) {
       if (strcmp(defineList.defName[i], name) == 0) {
           fprintf(stderr,"%s:%d: ***warning: multiple definition of %s\n",
                  fileStack.stack[fileStack.sp].fn, 
                  fileStack.stack[fileStack.sp].line,
                   name);
           alreadyDefined = 1;
       }
   }
   if (alreadyDefined == 0) { 
      if (defineList.nbr < DEFINEENTRIES) {
          if (strlen(name) < sizeof(defineList.defName[0])) {
             strcpy(defineList.defName[defineList.nbr], name);
             defineList.nbr ++;
          } else {
             fprintf(stderr,"%s:%d: ***error  : define entry <%s> too long\n",
                  fileStack.stack[fileStack.sp].fn, 
                  fileStack.stack[fileStack.sp].line,
		  name);
             _exit(1);
          }
       } else {
          fprintf(stderr,"%s:%d: ***error  : define list overflow\n",
                  fileStack.stack[fileStack.sp].fn, 
                  fileStack.stack[fileStack.sp].line);
          _exit(1);
       }
    }
}

#define IFDEPTH 20
static struct OutStack {
    int sp;
    int entryThen[IFDEPTH];
    int entryElse[IFDEPTH];
    int copyElseEndif[IFDEPTH];
} outputControlStack= {0,{1,1,1}};

int isOutputOn() {
   int ret = (outputControlStack.entryThen[outputControlStack.sp]);
   return(ret);
}

int copyElseEndif() {
   int ret = (outputControlStack.copyElseEndif[outputControlStack.sp]);
   //printf("else/endif output=%d\n", ret);
   return(ret);
}


void pushOutputLevel(int stateThen, int stateElse, int copyElseEndif) {
   //printf("push %d %d\n", stateThen, stateElse,copyElseEndif);
   if (outputControlStack.sp < IFDEPTH) {
      outputControlStack.sp++;
      outputControlStack.entryThen[outputControlStack.sp] = stateThen;
      outputControlStack.entryElse[outputControlStack.sp] = stateElse;
      outputControlStack.copyElseEndif[outputControlStack.sp] = copyElseEndif;
   } else {
      fprintf(stderr,"%s:%d: ***error  : #if depth exceeded \n",
                     fileStack.stack[fileStack.sp].fn, 
                     fileStack.stack[fileStack.sp].line);
      _exit(1);
   }
}
void elseOutputLevel() {
   //printf("else \n");
   outputControlStack.entryThen[outputControlStack.sp] = 
            outputControlStack.entryElse[outputControlStack.sp];
}
 
void popOutputLevel() {
   //printf("pop \n");
   if (outputControlStack.sp > 0) {
      outputControlStack.sp--;
   } else {
      fprintf(stderr,"%s:%d: ***error  : #if nesting wrong\n",
                     fileStack.stack[fileStack.sp].fn, 
                     fileStack.stack[fileStack.sp].line);
      _exit(1);
   }
}

#define INC_PATH_DEPTH  100
#define INC_PATH_NAME_LENGTH   100
static struct IncPaths{
   int nbr;
   char entry[INC_PATH_DEPTH][INC_PATH_NAME_LENGTH];
} incPaths = {0};
  
void add2IncList(char * p) {
    if (incPaths.nbr < INC_PATH_DEPTH) {
        if (strlen(p) < sizeof(incPaths.entry[0])) {
           strcpy(incPaths.entry[incPaths.nbr++], p);
        } else {
           fprintf(stderr,"%s:%d: ***error  : "
			"include path element too long (%s) (%d required)\n",
                     fileStack.stack[fileStack.sp].fn, 
                     fileStack.stack[fileStack.sp].line,
                     p, strlen(p));
           _exit(1);
        }
     } else {
           fprintf(stderr,"%s:%d: ***error  : "
                          "include path list overflow\n",
                     fileStack.stack[fileStack.sp].fn, 
                     fileStack.stack[fileStack.sp].line,
 		      p);
        _exit(1);
     }
}

void doInclude(char * fileName) {
   char fn[128];
   int i;

//   fprintf(yyout,"/**** included from %s ******/\n", fileName);

   if ( fileStack.sp >= INCLUDEDEPTH ) {
           fprintf(stderr,"%s:%d: ***error  : "
                          "includes nested too deep\n",
                     fileStack.stack[fileStack.sp].fn, 
                     fileStack.stack[fileStack.sp].line);
      _exit( 1 );
   }

  fileStack.stack[fileStack.sp].entry = YY_CURRENT_BUFFER;
  strcpy(fn,fileName);   
  yyin = fopen( fn, "r" );
  if (yyin == 0 && fn[0] != '/') {
      for (i=0; i<incPaths.nbr && yyin == 0; i++) {
         strcpy(fn,incPaths.entry[i]);
         strcat(fn,"/");
         strcat(fn,fileName);   
         yyin = fopen( fn, "r" );
      }
  }
  if ( ! yyin ) {
           fprintf(stderr,"%s:%d: ***error  : "
                    "could not open %s\n",
                     fileStack.stack[fileStack.sp].fn, 
                     fileStack.stack[fileStack.sp].line,
                     fileName);
     _exit(1);
  } 
  fileStack.sp++;
  strncpy(fileStack.stack[fileStack.sp].fn, fn, sizeof(fileStack.stack[0].fn));
  fileStack.stack[fileStack.sp].line = 1;

  yy_switch_to_buffer( yy_create_buffer( yyin, YY_BUF_SIZE ) );
  //printf("switch to buffer %s\n",fileName);
} 

int endInclude() {
   if (--fileStack.sp < 0) {
        return -1; // no more files
   } 
   yy_switch_to_buffer(fileStack.stack[fileStack.sp].entry );
   //printf("returned to prev buffer (sp=%d)\n",fileStack.sp);
   return 0; // ok
}

int main(int narg, char **argv) {
    int ret;
    FILE * fp;
    int i;	 

    strcpy(fileStack.stack[0].fn,"<stdin>");
    fileStack.stack[0].line = 1;
    /* read options */
    while ((i=getopt(narg,argv,"cI:?")) != -1) {
       switch (i) {
           case 'c': keepComment = 0;
                     break;
           case 'I': add2IncList(optarg);
                     break;
           case '?':
           default:
             fprintf(stderr," usage: %s [option]\n", argv[0]);
             fprintf(stderr,
                    "     -c          : remove comments (default keep) \n");
             fprintf(stderr,
                    "     -I <folder> : additional include folder \n");
             return 1;
           }
    }
    do {
       ret = yylex();
       switch (ret) {
	 default:	 	//printf("**** ret=%d\n", ret);
				break;
         case SYSINCFILE:
 			if (isOutputOn()) {
                           fputs("<",yyout);
                           fputs(buf,yyout);
                           fputs(">",yyout);
                        }
			break;
         case CSTRING:
 			if (isOutputOn()) {
                           fputs("\"",yyout);
                           fputs(buf,yyout);
                           fputs("\"",yyout);
                        }
			break;
         case IDENTIFIER:
 			if (isOutputOn()) {
                           fputs(buf,yyout);
                        }
			break;
         case INCLUDE:  if (isOutputOn()) {
                           ret = yylex();
			   switch (ret) {
		   	   case CSTRING: // user include
				doInclude(buf);
			   	break;
			   case SYSINCFILE: // sysinc
				addToSysIncList(buf);
				break;
			   default:
           			fprintf(stderr,"%s:%d: ***error  : "
				        " *** strange #include statement\n",
                                      fileStack.stack[fileStack.sp].fn, 
                                      fileStack.stack[fileStack.sp].line);
				      fprintf(stderr,"    ret = %d  buf=>%s<\n",ret,buf);
				_exit(1);
			   }
                        }
			break;
         case DEFINE:   if (isOutputOn()) {
                           ret = yylex();
                           if (ret !=  IDENTIFIER) {
           			fprintf(stderr,"%s:%d: ***error  : "
			                     "identifer expected\n",
                                      fileStack.stack[fileStack.sp].fn, 
                                      fileStack.stack[fileStack.sp].line);
                              _exit(1);
                           }
			   printf("\n#define %s", buf);
                           addToDefines(buf);
                        }
			break;
         case UNDEF:    if (isOutputOn()) {
			   ret = yylex();
                           if (ret !=  IDENTIFIER) {
			      fprintf(stderr,"identifer expected\n");
                              _exit(1);
                           }
			   printf("\n#undef %s", buf);
                           removeFromDefines(buf);
                        }
			break;
         case IFDEF:    ret = yylex();
                        if (ret !=  IDENTIFIER) {
           			fprintf(stderr,"%s:%d: ***error  : "
			                  "IFDEF identifer expected\n",
                                      fileStack.stack[fileStack.sp].fn, 
                                      fileStack.stack[fileStack.sp].line);
                           _exit(1);
                        }
                        //printf("IFDEF %d\n", isInDefines(buf));
		        pushOutputLevel(isInDefines(buf) && isOutputOn(),
                                        !isInDefines(buf) && isOutputOn(),
                                        0);
			break;
         case IFNDEF:    ret = yylex();
                        if (ret !=  IDENTIFIER) {
			   fprintf(stderr,"*** IFNDEF identifer expected\n");
                           _exit(1);
                        }
                        //printf("IFNDEF %d\n", isInDefines(buf));
		        pushOutputLevel(!isInDefines(buf) && isOutputOn(),
                                        isInDefines(buf) && isOutputOn(),
                                        0);
			break;
         case IF:	if (isOutputOn() ) {
			   ECHO;
                        } 
		        pushOutputLevel(isOutputOn(),
                                        isOutputOn(),
                                        1);
			break;
         case ELSE:     if (isOutputOn() && copyElseEndif() ) {
			   ECHO;
                        } 
		        elseOutputLevel(); 
			break;
         case ENDIF:    if (isOutputOn() && copyElseEndif() ) {
			   ECHO;
                       } 
		        popOutputLevel();
			break;
       }
   } while (ret != 0 && ret != 5);
   /* dump sysinc list */
   fp = fopen("sysincs.h","w");
   if (fp != 0) {
      for (i=0; i<sysIncFiles.nbr; i++) {
         fprintf(fp,"#include <%s>\n",sysIncFiles.fileName[i]);
      }
      fclose(fp); 
   } else {
      fprintf(stderr,"file write error on sysinc.h");
      _exit(1);
   }
   return 0;
}
