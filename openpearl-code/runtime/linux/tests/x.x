/**
Unit tests for sign and abs
*/
TEST(Fixed, SizeSignAbs) {
   pearlrt::Fixed<15> x;
   x = 0;
   EXPECT_EQ(x.sign(), (pearlrt::Fixed<15>)0);
   x = -10;
   EXPECT_EQ(x.sign(), (pearlrt::Fixed<15>)(-1));
   x = 10;
   EXPECT_EQ(x.sign(), (pearlrt::Fixed<15>)(1));
   x = 0;
   EXPECT_EQ(x.abs(), (pearlrt::Fixed<15>)0);
   x = -10;
   EXPECT_EQ(x.abs(), (pearlrt::Fixed<15>)(10));
   x = 10;
   EXPECT_EQ(x.abs(), (pearlrt::Fixed<15>)(10));
   x = -32768;
   ASSERT_THROW(x.abs(), pearlrt::FixedRangeSignal);
}

/**
Unit tests for fit
*/
TEST(Fixed, Fit) {
   pearlrt::Fixed<15> x15;
   pearlrt::Fixed<31> x31;
   x15 = 32;
   x31 = x15.fit(x31);
   EXPECT_EQ(x31, x15);
   x31 = 32;
   x15 = x31.fit(x15);
   EXPECT_EQ(x15, x31);
   x31 = 32768;
   EXPECT_THROW(x31.fit(x15), pearlrt::FixedRangeSignal);
}

