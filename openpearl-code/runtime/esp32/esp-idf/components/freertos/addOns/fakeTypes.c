/**
\file

\maketitle
This simple program creates a C/C++ header file with simulates the 
internal types from FreeRTOS  and other packages like Chan's FatFS
for the OpenPEARL API headers.

The simulation is justa bunch of data of the same size and alignment 
of 32 bit.
*/

#include <stdio.h>
#include <string.h>
#include <stdint.h>

#define FILENAME "sizes.txt"

int sizeTCB_T = -1;
int sizeSTACKTYPE = -1;
int sizePORTSHORT = -1;
int sizeUBASETYPE = -1;
int sizeFATFS = -1;
int sizeFIL = -1;
int sizeVOLUMES = -1;

int main() {

    FILE * fp;
    char line[128];
    int lineNbr=0;
    int n, addr, len;
    char name[20];  // no longer name required
    char* shorttype="";
    char* stacktype="";
    char* ubasetype="";
    fp = fopen(FILENAME,"r");

    // discard the first two lines
    lineNbr ++;
    fgets(line, sizeof(line), fp);
    if (strlen(line) >= sizeof(line)-2) {
       fprintf(stderr,"%s:%d input line too long\n", FILENAME, lineNbr);
    }
    lineNbr ++;
    fgets(line, sizeof(line), fp);
    if (strlen(line) >= sizeof(line)-2) {
       fprintf(stderr,"%s:%d input line too long\n", FILENAME, lineNbr);
    }

    while(! feof(fp)){
       lineNbr ++;
       fgets(line, sizeof(line), fp);

       if (strlen(line) >= sizeof(line)-2) {
          fprintf(stderr,"%s:%d input line too long\n", FILENAME, lineNbr);
       }

       // parse line
       n = sscanf(line,"%19s %d %d", name, &len, &addr);
       if (n==2 && strcmp(name,"Total")==0) {
         break;  // last good line found
       }
       if (n != 3) {
          fprintf(stderr,"%s:%d parse error (format)\n", FILENAME, lineNbr);
       }

       if (strcmp(name,"TCB_T") == 0) {
          sizeTCB_T = len;
       }
       if (strcmp(name,"STACKTYPE") == 0) {
          sizeSTACKTYPE = len;
       }
       if (strcmp(name,"PORTSHORT") == 0) {
          sizePORTSHORT = len;
       }     
       if (strcmp(name,"UBASETYPE") == 0) {
          sizeUBASETYPE = len;
       }     
       if (strcmp(name,"FATFS") == 0) {
          sizeFATFS = len;
       }     
       if (strcmp(name,"FIL") == 0) {
          sizeFIL = len;
       }     
       if (strcmp(name,"VOLUMES") == 0) {
          sizeVOLUMES = len;
       }     
   }
   fclose(fp); 

   // file parsed 
//   printf("TCB: %d  STACK: %d  SHORT: %d\n",
//             sizeTCB_T, sizeSTACKTYPE, sizePORTSHORT);

   switch (sizeSTACKTYPE) {
      case 1: stacktype = "uint8_t";
              break;
      case 4: stacktype = "uint32_t";
              break;
      default:
            fprintf(stderr,"unsupported type for portSTACK_TYPE (size=%d)\n",
                    sizeSTACKTYPE);
   }

   switch (sizePORTSHORT) {
      case 2: shorttype = "int16_t";
              break;
      default:
            fprintf(stderr,"unsupported type for portSHORT (size=%d)\n",
                    sizeSTACKTYPE);
   }
   switch (sizeUBASETYPE) {
      case 1: ubasetype ="unsigned char";
              break;
      case 4: ubasetype ="unsigned long";
              break;
      default:
            fprintf(stderr,"unsupported type for UBaseType (size=%d)\n",
                    sizeSTACKTYPE);
   }

   fp = fopen("FakeTypes.h","w");
   fprintf(fp,"// fake for FreeRTOS types for the usage in OpenPEARL API\n"
              "// this filke is auto generated -- do not edit\n\n");

   fprintf(fp,"#ifndef FAKETYPES_H_INCLUDED\n"
              "#define FAKETYPES_H_INCLUDED\n");
 
   fprintf(fp,"#ifdef _cplusplus\n"
              "extern \"C\" {\n"
              "#endif\n\n"
              "#include <stdint.h>\n\n");

   fprintf(fp,"\n// FreeRTOS stuff\n");
   fprintf(fp,"#define FakePortSHORT %s\n", shorttype);
   fprintf(fp,"typedef %s FakeUBaseType_t;\n", ubasetype);

   fprintf(fp,"typedef %s FakeStackType_t;\n", stacktype);
   fprintf(fp,"typedef void* FakeSemaphoreHandle_t;\n"
              "#define FakexSemaphoreHandle FakeSemaphoreHandle_t\n\n");

   fprintf(fp,"typedef void* FakeTaskHandle_t;\n");
   fprintf(fp,"typedef struct {uint32_t assertAlign; char data[%d];}"
              " FakeTCB_t;\n",
               sizeTCB_T-sizeof(uint32_t));

   if (sizeFATFS > 0 && sizeFIL >0 && sizeVOLUMES > 0) {
      fprintf(fp,"\n// Chan's FatFS stuff\n");
      fprintf(fp,"typedef struct {uint32_t asserAlign; char data[%d];}"
                 " FakeFATFS;\n", sizeFATFS-sizeof(int32_t));

      fprintf(fp,"typedef struct {uint32_t asserAlign; char data[%d];}"
                 " FakeFIL;\n", sizeFIL-sizeof(int32_t));

      fprintf(fp,"#define FAKE_VOLUMES %d\n", sizeVOLUMES);
   }

   fprintf(fp,"#ifdef _cplusplus\n"
              "}\n"
              "#endif\n\n");

   fprintf(fp,"#endif\n");

   return 0;
}
