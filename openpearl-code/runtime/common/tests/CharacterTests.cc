#include "gtest.h"

#include <stdio.h>
#include <stdlib.h>
#include "Character.h"
#include "Fixed.h"
#include "compare.h"

/**
Test of class Character
*/
TEST(Char, operations) {
   {
      pearlrt::Character<10> c;
      EXPECT_EQ(c.upb().x, 10);
      EXPECT_EQ(sizeof(c), 10);
   }
   {
      pearlrt::Character<6> h(6, (char*)"World.");
      pearlrt::Character<6> w(6, (char*)"World.");
      EXPECT_FALSE((h < w).getBoolean());
      EXPECT_TRUE((h <= w).getBoolean());
      EXPECT_TRUE((h == w).getBoolean());
      EXPECT_FALSE((h != w).getBoolean());
      EXPECT_TRUE((h >= w).getBoolean());
      EXPECT_FALSE((h > w).getBoolean());
   }
   {
      pearlrt::Character<6> h(6, (char*)"Wo\rld.");
      pearlrt::Character<6> w(6, (char*)"World.");
      EXPECT_TRUE((h < w).getBoolean());
      EXPECT_TRUE((h <= w).getBoolean());
      EXPECT_FALSE((h == w).getBoolean());
      EXPECT_TRUE((h != w).getBoolean());
      EXPECT_FALSE((h >= w).getBoolean());
      EXPECT_FALSE((h > w).getBoolean());
      EXPECT_FALSE((h >= w).getBoolean());
   }
   {
      pearlrt::Character<6> h(6, (char*)"World.");
      pearlrt::Character<6> w(6, (char*)"Wo\rld.");
      EXPECT_FALSE((h < w).getBoolean());
      EXPECT_FALSE((h <= w).getBoolean());
      EXPECT_FALSE((h == w).getBoolean());
      EXPECT_TRUE((h != w).getBoolean());
      EXPECT_TRUE((h >= w).getBoolean());
      EXPECT_TRUE((h > w).getBoolean());
   }
   {
      pearlrt::Character<6> h(5, (char*)"Hello");
      pearlrt::Character<6> w(6, (char*)"World.");
      pearlrt::Character<12> sum;
      pearlrt::Character<12> res(12, (char*)"Hello World. ");
      pearlrt::catChar(h, w, sum);
      EXPECT_TRUE((sum == res).getBoolean());
   }
   {
      pearlrt::Character<12> res(12, (char*)"HelloWorld. ");
      pearlrt::Character<5> h;
      pearlrt::Character<5> k(5, (char*)"elloW");
      pearlrt::getSliceChar(res, 2, 6, h);
      EXPECT_TRUE((h == k).getBoolean());
      ASSERT_THROW(
         pearlrt::getSliceChar(res, 0, 4, h),
         pearlrt::CharacterIndexOutOfRangeSignal);
      ASSERT_THROW(
         pearlrt::getSliceChar(res, 10, 13, h),
         pearlrt::CharacterIndexOutOfRangeSignal);
      ASSERT_THROW(
         pearlrt::getSliceChar(res, 1, 6, h),
         pearlrt::CharacterTooLongSignal);
   }
   {
      pearlrt::Character<12> wrk(12, (char*)"HelloWorld. ");
      pearlrt::Character<12> r(12, (char*)"Hallo Welt. ");
      pearlrt::Character<9> k(9, (char*)"allo Welt");
      pearlrt::setSliceChar(wrk, 2, 10, k);
      EXPECT_TRUE((wrk == r).getBoolean());
      ASSERT_THROW(
         pearlrt::setSliceChar(wrk, 0, 8, k),
         pearlrt::CharacterIndexOutOfRangeSignal);
      ASSERT_THROW(
         pearlrt::setSliceChar(wrk, 4, 13, k),
         pearlrt::CharacterIndexOutOfRangeSignal);
   }
   {
      pearlrt::Character<12> res(12, (char*)"HelloWorld. ");
      pearlrt::Character<1> h;
      pearlrt::Character<1> k(1, (char*)"o");
      h = res.getCharAt(5);
      EXPECT_TRUE((h == k).getBoolean());
   }
   {
      pearlrt::Character<12> src(12, (char*)"HelloWorld. ");
      pearlrt::Character<12> res(12, (char*)"HelloWorld! ");
      pearlrt::Character<1> h;
      pearlrt::Character<1> k(1, (char*)"!");
      EXPECT_TRUE((src != res).getBoolean());
      src.setCharAt(11, k);
      EXPECT_TRUE((src == res).getBoolean());
   }
   {
      pearlrt::Character<1> h(1, (char*)" ");
      EXPECT_TRUE((h.toFixed() == (pearlrt::Fixed<8>)32).getBoolean());
   }
   {
      ASSERT_THROW(
         pearlrt::Character<32768> c, pearlrt::CharacterTooLongSignal);
   }
   {
      ASSERT_THROW(
         pearlrt::Character<0> c, pearlrt::CharacterTooLongSignal);
   }
   {
      ASSERT_THROW(
         pearlrt::Character<3> c(4, (char*)"1234"),
         pearlrt::CharacterTooLongSignal);
   }
   {
      pearlrt::Character<3> c(3, (char*)"1234");
      pearlrt::Character<1> d;
      ASSERT_THROW(
         d = c.getCharAt(0), pearlrt::CharacterIndexOutOfRangeSignal);
   }
   {
      pearlrt::Character<3> c(3, (char*)"1234");
      pearlrt::Character<1> d;
      ASSERT_THROW(
         d = c.getCharAt(4), pearlrt::CharacterIndexOutOfRangeSignal);
   }
   {
      pearlrt::Character<3> c(3, (char*)"1234");
      pearlrt::Character<1> d;
      ASSERT_THROW(
         c.setCharAt(0, d), pearlrt::CharacterIndexOutOfRangeSignal);
   }
   {
      pearlrt::Character<3> c(3, (char*)"1234");
      pearlrt::Character<1> d(1, (char*)"x");
      ASSERT_THROW(
         c.setCharAt(4, d), pearlrt::CharacterIndexOutOfRangeSignal);
   }
   {
      pearlrt::Character<3> c(3, (char*)"1234");
      ASSERT_THROW(
         c.toFixed(), pearlrt::CharacterTooLongSignal);
   }
}


