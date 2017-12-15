/*
 [A "BSD license"]
 Copyright (c) 2017 Rainer Mueller
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

#include "ConsoleCommon.h"

namespace pearlrt {

// input character codes
#define BS 0x08
#define ESC 27
#define BRACKETOPEN '['
#define RIGHT 67
#define LEFT  68
#define INSERT 50
#define DELETE 51
#define HOME   'H'    // ESC O H
#define END    'F'    // ESC O F
#define NL '\n'
#define BEL '\a'  // alarm code
#define SPACE ' '

   ConsoleCommon::ConsoleCommon() {
      insertMode = false;
      lastAdressedTask = NULL;
      waitingForInput = NULL;
   }

   void ConsoleCommon::setSystemDations(SystemDationNB* in,
                                        SystemDationNB* out) {
      systemIn = in;
      systemOut = out;
   }

   int ConsoleCommon::getchar() {
      char ch;
      systemIn->dationRead(&ch, 1);
      return (ch);
   }

   void ConsoleCommon::putchar(char ch) {
      systemOut->dationWrite(&ch, 1);
   }

   void ConsoleCommon::putString(const char* string) {
      systemOut->dationWrite((void*)string, strlen(string));
   }

   void ConsoleCommon::goRight(int n) {
      while (n--) {
         putchar(inputLine[cursorPosition]);
         cursorPosition++;
      }
   }

   void ConsoleCommon::goLeft(int n) {
      while (n--) {
         putchar(BS);
      }
   }

   TaskCommon* ConsoleCommon::treatLine(char** inputBuffer, size_t * length) {
      TaskCommon * t;
      static TaskCommon * lastTaskEntered = NULL;
      TaskCommon * previousTaskInList = NULL;
      int ch;
      size_t i;
      int cursor = 0;
      bool endOfLineReceived = false;

      nbrEnteredCharacters = 0;
      cursorPosition = 0;
      inputStarted = false;

      while (!endOfLineReceived) {
         cursor = 0;       // no cursor position code detected
         ch = getchar();   // wait passive until character is received
         inputStarted = true;

         // test for combined character
         if (ch == ESC) {
            ch = getchar();

            if (ch == BRACKETOPEN) {
               ch = getchar();

               if (ch == RIGHT) {
                  cursor = RIGHT;
               } else if (ch == LEFT) {
                  cursor = LEFT;
               } else if (ch == INSERT) {
                  insertMode = ! insertMode;
//printf("INSERTnmode = %d\n", insertMode);
                  ch = getchar(); // discard trailing ~ character
                  cursor = -1;  // discard ch
               } else if (ch == DELETE) {
                  ch = getchar(); // discard trailing ~ character
                  cursor = DELETE;
               } else {
                  // ignore all other
                  printf("ignore ESC [ %x\n", ch);

                  cursor = -1;  // discard ch
               }

//               printf("cursor control: %d ch=%x\n", cursor, ch);
            } else if (ch == 'O') { // ESC O sequences for HOME and END
               ch = getchar();

               if (ch == HOME) {
                  cursor = HOME;
//printf("HOME\n");
               } else if (ch == END) {
                  cursor = END;
//printf("END\n");
               } else {
                  // ignore all other
                  printf("ignore ESC O %x\n", ch);
                  cursor = -1;  // discard ch
               }
            }
         }

         switch (cursor) {
         case -1: // discard character
            continue;

         case 0: // normal input
            if (ch == NL) {
               putchar(ch);
               // always add NL at the end and the NIL also
               inputLine[nbrEnteredCharacters++] = ch;
               inputLine[nbrEnteredCharacters] = 0;
//     printf("end-of-record found cp=%zu nbr=%zu\n>%s<\n",
//             cursorPosition, nbrEnteredCharacters, inputLine);
               endOfLineReceived = true;
            } else if (ch == 0x07f) {
               // del to left
               if (cursorPosition > 0) {
                  for (i = cursorPosition; i < nbrEnteredCharacters; i++) {
                     inputLine[i - 1] = inputLine[i];
                  }

                  nbrEnteredCharacters--;
                  goLeft(1);
                  cursorPosition --;

                  for (i = cursorPosition; i < nbrEnteredCharacters; i++) {
                     putchar(inputLine[i]);
                     cursorPosition++;
                  }

                  putchar(SPACE);
                  goLeft(nbrEnteredCharacters - cursorPosition + 1);
               }
            } else if (ch >= ' ' && ch < 0x7f) {
               // reserve space for NL and NIL
               if (nbrEnteredCharacters < sizeof(inputLine) - 2) {
                  if (insertMode) {
                     for (i = nbrEnteredCharacters; i >= cursorPosition; i--) {
                        inputLine[i + 1] = inputLine[i];
                     }

                     inputLine[cursorPosition] = ch;
                     nbrEnteredCharacters++;
                     putchar(ch);
                     cursorPosition ++;

                     for (i = cursorPosition; i < nbrEnteredCharacters; i++) {
                        putchar(inputLine[i]);
                     }

                     goLeft(nbrEnteredCharacters - cursorPosition);
                  } else {
                     inputLine[cursorPosition] = ch;

                     if (cursorPosition == nbrEnteredCharacters) {
                        nbrEnteredCharacters++;
                     }

                     putchar(ch);
                     cursorPosition ++;
                  }
               } else {
                  putchar(BEL);
               }
            } else {
               printf("character %x found - not treated  yet\n", ch);
            }

            break;

         case DELETE: // del key --> delete from right
            if (cursorPosition < nbrEnteredCharacters) {
               nbrEnteredCharacters--;

               for (i = cursorPosition; i < nbrEnteredCharacters; i++) {
                  inputLine[i] = inputLine[i + 1];
               }

               for (i = cursorPosition; i < nbrEnteredCharacters; i++) {
                  putchar(inputLine[i]);
               }

               putchar(SPACE);
               goLeft(nbrEnteredCharacters - cursorPosition + 1);
            }

            break;

         case RIGHT: // position to right if possible
            if (cursorPosition < nbrEnteredCharacters) {
               goRight(1);
            }

            break;

         case LEFT: // position to left if possible

            // printf("cursor left  - not treated  yet\n");
            if (cursorPosition > 0) {
               goLeft(1);
               cursorPosition--;
            } else {
               putchar(BEL);
            }

            break;

         case HOME:  // position to first position
            goLeft(cursorPosition);
            cursorPosition = 0;
            break;

         case END:  // position after last entered position
            if (cursorPosition < nbrEnteredCharacters) {
               goRight(nbrEnteredCharacters - cursorPosition);
            }

            break;
         }

      }

      *length = nbrEnteredCharacters;
      inputStarted = false;

      // check if line starts with new task name
      if (inputLine[0] == ':') {
         // let's search the next colon
         for (i = 1; i < nbrEnteredCharacters; i++) {
            if (inputLine[i] == ':') {
               if (i > 2) {
                  // task name found
                  // update length information
                  * length -= i + 1;
                  // let's check if the task waits for input
                  // and pass the effecitive input to the input processing
                  // and quit this function
                  inputLine[i] = '\0';

// printf("Taskname=>%s<  waitingForInput=%p\n", inputLine+1, waitingForInput);
                  // pointer to predecessor in list for easy
                  // removal operation
                  for (t = waitingForInput;
                        t != NULL; t = t->getNext()) {
                     if (strcmp(t->getName(), inputLine + 1) == 0) {
                        // found adressed task

                        // remove this task from wait queue
                        if (previousTaskInList) {
                           previousTaskInList->setNext(t->getNext());
                           t->setNext(NULL);
                        } else {
                           waitingForInput = t->getNext();
                        }

                        // and set the return parameters
                        *inputBuffer = inputLine + i + 1;
                        lastTaskEntered = t;
                        return lastTaskEntered;
                     }

                     previousTaskInList = t;
                  }

                  if (t == NULL) {
                     lastTaskEntered = NULL; // forget last valid task
                     putString("\n:???: not waiting\n");
                     // return the complete input line
                     *length = nbrEnteredCharacters;
                     inputLine[i] = ':'; //restore 2nd colon
                     *inputBuffer = inputLine;
                     return NULL;
                  }
               }
            }
         }

         // no 2nd task name delimiter found
         // will be treated as if no colon is at the first position
      }

      *inputBuffer = inputLine;
      *length = nbrEnteredCharacters;

      // remove this task from wait queue
      if (lastTaskEntered) {
         if (previousTaskInList) {
            previousTaskInList->setNext(lastTaskEntered->getNext());
            lastTaskEntered->setNext(NULL);
         } else {
            waitingForInput = lastTaskEntered->getNext();
         }
      } else {
         putString("\n:???: no default task\n");
      }

      return lastTaskEntered;
   }


   void ConsoleCommon::registerWaitingTask(void * task, int direction) {
      TaskCommon * t = (TaskCommon*) task;

      if (direction == Dation::IN) {
         t->setNext(waitingForInput);
         waitingForInput = t;
      }

      if (direction == Dation::OUT) {

         if (inputStarted || waitingForOutput.getHead()) {
            // queue not empty --> add task as waiter
            waitingForOutput.insert(t);
         } else {
            // let the task do its output
            t->unblock();
         }
      }
   }

}
