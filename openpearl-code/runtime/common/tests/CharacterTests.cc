#include "gtest.h"

#include <stdio.h>
#include <stdlib.h>
#include "Character.h"
#include "CharSlice.h"
#include "Fixed.h"
#include "compare.h"

/**
Test of class Character
*/
TEST(Char, asize) {
      pearlrt::Character<10> c;
      EXPECT_EQ(c.upb().x, 10);
      EXPECT_EQ(sizeof(c), 10);
}
TEST(Char, compare) {
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
}

// the comparision function is identical to all character/charSlice 
// comparisons - the comparison itself is testet with Character<->Character
// comparisions- lets see if all combinations work
TEST(Char, compareCharSlice_Char) {
   {
      pearlrt::Character<6> w(6, (char*)"World.");
      pearlrt::Character<6> k(6, (char*)"World.");
      pearlrt::CharSlice h(k);
      EXPECT_FALSE((h < w).getBoolean());
      EXPECT_TRUE((h <= w).getBoolean());
      EXPECT_TRUE((h == w).getBoolean());
      EXPECT_FALSE((h != w).getBoolean());
      EXPECT_TRUE((h >= w).getBoolean());
      EXPECT_FALSE((h > w).getBoolean());
   }
   {
      pearlrt::Character<6> h(6, (char*)"World.");
      pearlrt::Character<6> k(6, (char*)"World.");
      pearlrt::CharSlice w(k);
      EXPECT_FALSE((h < w).getBoolean());
      EXPECT_TRUE((h <= w).getBoolean());
      EXPECT_TRUE((h == w).getBoolean());
      EXPECT_FALSE((h != w).getBoolean());
      EXPECT_TRUE((h >= w).getBoolean());
      EXPECT_FALSE((h > w).getBoolean());
   }
   {
      pearlrt::Character<6> k(6, (char*)"World.");
      pearlrt::CharSlice h(k);
      pearlrt::CharSlice w(k);
      EXPECT_FALSE((h < w).getBoolean());
      EXPECT_TRUE((h <= w).getBoolean());
      EXPECT_TRUE((h == w).getBoolean());
      EXPECT_FALSE((h != w).getBoolean());
      EXPECT_TRUE((h >= w).getBoolean());
      EXPECT_FALSE((h > w).getBoolean());
   }
}
TEST(Char, catChar) {
   {
      pearlrt::Character<6> h(5, (char*)"Hello");
      pearlrt::Character<6> w(6, (char*)"World.");
      pearlrt::Character<12> sum;
      pearlrt::Character<12> res(12, (char*)"Hello World. ");
      sum = h.catChar(w);
      //pearlrt::catChar(h, w, sum);
      EXPECT_TRUE((sum == res).getBoolean());
   }
}

TEST(Char, getSlice) {
      pearlrt::Character<12> res(12, (char*)"HelloWorld. ");
      pearlrt::Character<5> h;
      pearlrt::Character<5> k(5, (char*)"elloW");
      h = *(pearlrt::CharSlice(res).getSlice(2,6).mkCharPtr((pearlrt::Character<5>*)0));
      EXPECT_TRUE((h == k).getBoolean());
      ASSERT_THROW(
         pearlrt::CharSlice(h).getSlice(0,5),
         pearlrt::CharacterIndexOutOfRangeSignal);
      ASSERT_THROW(
         pearlrt::CharSlice(h).getSlice(2,6),
         pearlrt::CharacterIndexOutOfRangeSignal);
}

TEST(Char, setSlice) {
      pearlrt::Character<12> wrk(12, (char*)"HelloWorld. ");
      pearlrt::Character<12> r (12, (char*)"Hallo Welt. ");
      pearlrt::Character<12> r1(12, (char*)"Haallo Welt ");
      pearlrt::Character<9> k(9, (char*)"allo Welt");

      pearlrt::CharSlice(wrk)
	  .getSlice(2,10)
	  .setSlice(pearlrt::CharSlice(k));
      EXPECT_TRUE((wrk == r).getBoolean());

      ASSERT_THROW(
     	 pearlrt::CharSlice(wrk)
	    .getSlice(0,9)
	    .setSlice(pearlrt::CharSlice(k)),
         pearlrt::CharacterIndexOutOfRangeSignal);
 
     ASSERT_THROW(
         pearlrt::CharSlice(wrk)
	   .getSlice(4,13)
	   .setSlice(pearlrt::CharSlice(k)),
         pearlrt::CharacterIndexOutOfRangeSignal);

      ASSERT_THROW(
         pearlrt::CharSlice(wrk)
  	   .getSlice(5,12)
	   .setSlice(pearlrt::CharSlice(k)),
         pearlrt::CharacterTooLongSignal);

      pearlrt::CharSlice(wrk)
          .getSlice(3,12).setSlice(pearlrt::CharSlice(k));
      EXPECT_TRUE((wrk == r1).getBoolean());
}

TEST(Char, setSlice1) {             //123456789012
      pearlrt::Character<12> src(12, "Hello World.");
      pearlrt::Character<12> wrk;
      pearlrt::Character<12> r (12,  "H1234     d.");
      pearlrt::Character<12> r1(12,  "H12lo World.");
      pearlrt::Character<12> r2(12,  "HHell World.");
      pearlrt::Character<12> r3(12,  "elloo World.");
      pearlrt::Character<4> k(4, "1234");

      wrk = src;
      pearlrt::CharSlice(wrk)
	  .getSlice(2,10)
	  .setSlice(pearlrt::CharSlice(k));
      EXPECT_TRUE((wrk == r).getBoolean());

      wrk = src;
      ASSERT_THROW(
      pearlrt::CharSlice(wrk)
	  .getSlice(2,4)
	  .setSlice(pearlrt::CharSlice(k)),
         pearlrt::CharacterTooLongSignal);

      wrk = src;
      pearlrt::CharSlice(wrk)
	  .getSlice(2,3)
	  .setSlice(pearlrt::CharSlice(k).getSlice(1,2));
      EXPECT_TRUE((wrk == r1).getBoolean());

      // copy overlapping parts from the same string
      wrk = src;
      pearlrt::CharSlice(wrk)
	  .getSlice(2,5)
	  .setSlice(pearlrt::CharSlice(wrk).getSlice(1,4));
      EXPECT_TRUE((wrk == r2).getBoolean());
//printf("wrk=>%-12.12s<\n", wrk.get());
      wrk = src;
      pearlrt::CharSlice(wrk)
	  .getSlice(1,4)
	  .setSlice(pearlrt::CharSlice(wrk).getSlice(2,5));
      EXPECT_TRUE((wrk == r3).getBoolean());
//printf("wrk=>%-12.12s<\n", wrk.get());
      
}

TEST(CHAR,set_get_charAt) {
   {
      pearlrt::Character<12> res(12, (char*)"HelloWorld. ");
      pearlrt::Character<1> h;
      pearlrt::Character<1> k(1, (char*)"o");
      h = res.getCharAt(pearlrt::Fixed<15>(5));
//      h = res.getCharAt(5);
      EXPECT_TRUE((h == k).getBoolean());
   }
      pearlrt::Character<12> src(12, (char*)"HelloWorld. ");
      pearlrt::Character<12> res(12, (char*)"HelloWorld! ");
      pearlrt::Character<1> h;
      pearlrt::Character<1> k(1, (char*)"!");
      EXPECT_TRUE((src != res).getBoolean());
      src.setCharAt(11, k);
      EXPECT_TRUE((src == res).getBoolean());
}
TEST(CHAR, toFixed) {
      pearlrt::Character<1> h(1, (char*)" ");
      EXPECT_TRUE((h.toFixed() == (pearlrt::Fixed<8>)32).getBoolean());
}
TEST(Char, throw) {
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


