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

/**
\file

\brief Test routines for array operations

\page Testprograms

\section Array Array Tests
Test routines for the array types.

There are several unit tests using the google test framework.

\cond TREAT_EXAMPLES
*/
#include "gtest.h"

#include "Fixed.h"
#include "Array.h"
#include "compare.h"
#include "Signals.h"
#include "Log.h"

/**
test creation
*/
TEST(ArrayTest, creation1) {
//   pearlrt::Fixed<31> data_array1[100]; // arrayData Fixed(5,20);
   DCLARRAY(array1, 2, LIMITS({{0,4,20},{0,19,1}}));

  
   ASSERT_THROW(array1->lwb(pearlrt::Fixed<31>(0)), 
                pearlrt::ArrayIndexOutOfBoundsSignal);
   ASSERT_EQ(array1->lwb(pearlrt::Fixed<31>(1)).x, 0);
   ASSERT_EQ(array1->lwb(pearlrt::Fixed<31>(2)).x, 0);
   ASSERT_THROW(array1->lwb(pearlrt::Fixed<31>(3)), 
                pearlrt::ArrayIndexOutOfBoundsSignal);

   ASSERT_THROW(array1->upb(pearlrt::Fixed<31>(0)), 
                pearlrt::ArrayIndexOutOfBoundsSignal);
   ASSERT_EQ(array1->upb(pearlrt::Fixed<31>(1)).x, 4);
   ASSERT_EQ(array1->upb(pearlrt::Fixed<31>(2)).x, 19);
   ASSERT_THROW(array1->upb(pearlrt::Fixed<31>(3)), 
                pearlrt::ArrayIndexOutOfBoundsSignal);

}

TEST(ArrayTest, creation2) {
   DCLARRAY(array1, 2, LIMITS({{-4,4,20},{-10,9,1}}));

   ASSERT_THROW(array1->lwb(pearlrt::Fixed<31>(0)), 
                pearlrt::ArrayIndexOutOfBoundsSignal);
   ASSERT_EQ(array1->lwb(pearlrt::Fixed<31>(1)).x, -4);
   ASSERT_EQ(array1->lwb(pearlrt::Fixed<31>(2)).x, -10);
   ASSERT_THROW(array1->lwb(pearlrt::Fixed<31>(3)), 
                pearlrt::ArrayIndexOutOfBoundsSignal);

   ASSERT_THROW(array1->upb(pearlrt::Fixed<31>(0)), 
                pearlrt::ArrayIndexOutOfBoundsSignal);
   ASSERT_EQ(array1->upb(pearlrt::Fixed<31>(1)).x, 4);
   ASSERT_EQ(array1->upb(pearlrt::Fixed<31>(2)).x, 9);
   ASSERT_THROW(array1->upb(pearlrt::Fixed<31>(3)), 
                pearlrt::ArrayIndexOutOfBoundsSignal);
}

TEST(ArrayTest, readwrite) {
   pearlrt::Fixed<31> testvalue;

   pearlrt::Fixed<31> data_array1[100]; // arrayData Fixed(5,20);
   DCLARRAY(array1, 2, LIMITS({{0,4,20},{0,19,1}}));

   // read, and index exception tests
   ASSERT_NO_THROW(
      testvalue=*(data_array1+
                  array1->offset(pearlrt::Fixed<31>(0),pearlrt::Fixed<31>(0)))
    );
   ASSERT_NO_THROW(
      testvalue=*(data_array1+
                  array1->offset(pearlrt::Fixed<31>(4),pearlrt::Fixed<31>(19)))
    );
   ASSERT_THROW(
      testvalue=*(data_array1+
                  array1->offset(pearlrt::Fixed<31>(5),pearlrt::Fixed<31>(19))),
                pearlrt::ArrayIndexOutOfBoundsSignal);
   ASSERT_THROW(
      testvalue=*(data_array1+
                  array1->offset(pearlrt::Fixed<31>(4),pearlrt::Fixed<31>(20))),
                pearlrt::ArrayIndexOutOfBoundsSignal);

   ASSERT_THROW(
      testvalue=*(data_array1+
                  array1->offset(pearlrt::Fixed<31>(0),pearlrt::Fixed<31>(-1))),
                pearlrt::ArrayIndexOutOfBoundsSignal);
   ASSERT_THROW(
      testvalue=*(data_array1+
                  array1->offset(pearlrt::Fixed<31>(-1),pearlrt::Fixed<31>(0))),
                pearlrt::ArrayIndexOutOfBoundsSignal);

   // write
   ASSERT_NO_THROW(
      testvalue=*(data_array1+
                  array1->offset(pearlrt::Fixed<31>(1),pearlrt::Fixed<31>(0)));
      *(data_array1+
        array1->offset(pearlrt::Fixed<31>(0),pearlrt::Fixed<31>(1)))
         = testvalue;
    );
}

TEST(ArrayTest, order) {
   pearlrt::Fixed<31> testvalue;

   pearlrt::Fixed<31> data_array1[100]; // arrayData Fixed(5,20);
   DCLARRAY(array1, 2, LIMITS({{0,4,20},{0,19,1}}));

   // test pointer differences
   // last index runs first
   ASSERT_EQ(
      (data_array1+array1->offset(pearlrt::Fixed<31>(0),pearlrt::Fixed<31>(1)))-
      (data_array1+array1->offset(pearlrt::Fixed<31>(0),pearlrt::Fixed<31>(0))), 1);
   ASSERT_EQ(
      (data_array1+array1->offset(pearlrt::Fixed<31>(4),pearlrt::Fixed<31>(0)))-
      (data_array1+array1->offset(pearlrt::Fixed<31>(0),pearlrt::Fixed<31>(0))), 80);
   ASSERT_EQ(
      (data_array1+array1->offset(pearlrt::Fixed<31>(4),pearlrt::Fixed<31>(19)))-
      (data_array1+array1->offset(pearlrt::Fixed<31>(0),pearlrt::Fixed<31>(0))), 99);

}

void preset(pearlrt::Array * a, pearlrt::Fixed<31> * data) {
   for (int i=a->lwb(pearlrt::Fixed<31>(1)).x; 
             i<=a->upb(pearlrt::Fixed<31>(1)).x;i++) {
   for (int j=a->lwb(pearlrt::Fixed<31>(2)).x; 
             j<=a->upb(pearlrt::Fixed<31>(2)).x;j++) {
      *(data+
        a->offset(pearlrt::Fixed<31>(i),pearlrt::Fixed<31>(j)))=
       pearlrt::Fixed<31>(100*i+j);
   }}
}

void dump(pearlrt::Array * a, pearlrt::Fixed<31> * data) {
   // run fast running index in inner loop 
   for (int i=a->lwb(pearlrt::Fixed<31>(1)).x; 
             i<=a->upb(pearlrt::Fixed<31>(1)).x;i++) {
   for (int j=a->lwb(pearlrt::Fixed<31>(2)).x; 
             j<=a->upb(pearlrt::Fixed<31>(2)).x;j++) {
      printf("%4d", (int)(data+
        a->offset(pearlrt::Fixed<31>(i),pearlrt::Fixed<31>(j)))->get());
   }
   printf("\n");
   }
}


TEST(ArrayTest, functionpass) {
   pearlrt::Fixed<31> data_array1[100]; // arrayData Fixed(10,10);
   DCLARRAY(array1, 2, LIMITS({{0,9,10},{0,9,1}}));

   preset(array1,data_array1);
   dump(array1,data_array1);
}

TEST(ArrayTest,withStructs){
   struct S {
      int dummyData1;
      pearlrt::Fixed<31> data_array1[100]; // arrayData Fixed(0:9,0:9);
      int dummyData2;
   } data_structs[20];
   DCLARRAY(structs, 1, LIMITS({{0,20,1}}));

   DCLARRAY(structs_array1, 2, LIMITS({{0,9,10},{0,9,1}}));
   pearlrt::Fixed<31> testValue;
   testValue = *(
                 (  (*(data_structs+structs->offset(pearlrt::Fixed<31>(2)) )).data_array1+
                             structs_array1->offset(pearlrt::Fixed<31>(0),pearlrt::Fixed<31>(1))
                 )
                );
}
/**
\endcond
*/
