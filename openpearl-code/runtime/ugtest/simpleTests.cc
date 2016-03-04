#include <iostream>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <vector>
#include <algorithm>
#include "Post.h"

#include "simpleTests.h"

namespace simpleTests {
TestBoss::TestBoss(const char* x, const char*y) : n1(x), n2(y) {
		fail=false;
		checks = 0;
	}

bool TestList::compare (TestBoss*lhs, TestBoss* rhs) {
             int help;
	     help = strcmp(lhs->n1,rhs->n1);
             if (help < 0) {
               return true;
             } else if (help > 0) {
               return false;
             }
	     help = strcmp(lhs->n2,rhs->n2);
             if (help < 0) {
               return true;
             }
             return false;
         }

TestList* TestList::getInstance() {
            if (instance == 0)  {
		instance = new TestList;
	    }
	    return instance;
        }

void TestList::add(TestBoss*n) {
	    tests.push_back(n);
        }
void TestList::sort() {
		std::sort(tests.begin(), tests.end(), compare);
	}
int TestList::testsMatching(const char * n1) {
           int count = 0;
	
	   for (unsigned int i=0; i< tests.size(); ++i) {
               if (strcmp(tests[i]->n1,n1) == 0) {
                  count ++;
               }
           }
           return count;
	}

TestList* TestList::instance = 0;
}

int main() {
	unsigned int  i;
        const char * group = 0;
	int nbrOfFails; 
        int testCases = 0;
        unsigned int nbrOfTests;

	pearlrt::Post::print();

	std::cout << std::endl << std::endl << 
                     "micro gtest started " << std::endl;
	simpleTests::TestList::getInstance()->sort();
        nbrOfTests = simpleTests::TestList::getInstance()->tests.size();
	for (i=0; i < nbrOfTests; ++i) {
	   simpleTests::TestBoss * t = simpleTests::TestList::getInstance()->tests[i];
        
           if (group == 0 || strcmp(group,t->n1) != 0) {
               std::cout << "[----------] " << 
                       simpleTests::TestList::getInstance()->testsMatching(t->n1) << 
                       " tests from " << t-> n1 << std::endl;  
               group = t->n1;
               testCases ++;
           } 
	   std::cout << "[ RUN      ] " << t->n1 << "." << t->n2 << std::endl;
	   t->run();
           if (t->fail) {
              std::cout << "[ FAILED   ] " << t->n1 << "." << t->n2 << std::endl;
           } else {
              std::cout << "[       OK ] " << t->checks << " checks done" << std::endl;
           }
	}
// ---------- summary ---------
        std::cout << "[==========] " << nbrOfTests << " tests from " << testCases <<
                " test cases run" << std::endl;

        nbrOfFails = 0;
	for (i=0; i< simpleTests::TestList::getInstance()->tests.size(); ++i) {
	   simpleTests::TestBoss * t = simpleTests::TestList::getInstance()->tests[i];
	   if (t->fail) {
              nbrOfFails ++;
           }
        }
        std::cout << "[ PASSED   ] " << nbrOfTests - nbrOfFails << " tests" << std::endl;

        if (nbrOfFails > 0) {
           std::cout << "[ FAILED   ] " << nbrOfFails << 
                      " tests, listed below" << std::endl;
	   for (i=0; i< simpleTests::TestList::getInstance()->tests.size(); ++i) {
	      simpleTests::TestBoss * t = simpleTests::TestList::getInstance()->tests[i];
	      if (t->fail) {
                 std::cout << "[ FAILED   ] " << t->n1 << "." << t->n2 << std::endl;
              }
	   }
	} 
}

