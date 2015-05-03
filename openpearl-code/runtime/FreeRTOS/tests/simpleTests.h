#include <iostream>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <vector>
#include <algorithm>


namespace simpleTests {

/**
parent class for all unit test cases

This simple test framework is inspired from google tests.
The syntax of the checks is kept identical. This provides the 
possiblility to use test cases which are used with google test
in this framework, which is capable to run on the microcontroller.
*/
class TestBoss {
   public:
        bool fail;    ///< test result for the individual test case
        int checks;   ///< number of ASSERT and EXPECT statements in this test case
	const char * n1; ///< group name for the test case
	const char * n2;  ///< individual name of the test case
        /** the body of the concrete test. This method
        is created by the macro TEST and the subsequent 
        code statements
        */
	virtual void run()=0;
        /** the ctor of a test. It just stores test group name
        and individual test name 
        \param x group name of the unit test
        \param y individual test within a group
        */
	TestBoss(const char* x, const char*y);
};

/**
container for all defined test cases
*/
class TestList {
   public:
        /** 
        vector with all defined test cases
        */
	std::vector<TestBoss*> tests;
   private:
	static TestList* instance;
	static bool compare (TestBoss*lhs, TestBoss* rhs) ;

   public:
        /**
        get access to the test list.

        Due to the static initialization desaster problem, this
        is solved as singleton
      
        \returns pointer to the list of test cases
        */
	static TestList* getInstance();

        /**
        insert a test case to the list
        \param n pointer to the new test case 
        */
	void add(TestBoss*n);

        /**
        sort the test cases to ascending group name and individual name 
        */
	void sort();

        /**
        obtain the number of test cases within a group
        \param n1 group name to be scanned
        \returns the number of test cases within this group
        */
	int testsMatching(const char * n1);
};

}

#define TEST(x,y) \
	class x ## y : public simpleTests::TestBoss {			\
	public: \
	   void run();	\
	   x ## y(const char* n1, const char* n2): TestBoss(n1,n2) { \
          simpleTests::TestList::getInstance()->add(this); \
        }	\
       } _##x ## y(#x,#y); \
	void x ## y::run() 

#define EXPECT_EQ(x,y) \
	checks ++;						\
	if ((x) != (y) ) {					\
	   std::cout << __FILE__ << ":" << __LINE__<<  	\
	           " EXPECT_EQ failed " << std::endl <<		\
	           "   LHS: " << #x << " = " << (x) << std::endl <<	\
		   "   RHS: " << #y << " = " << (y) << std::endl; \
	   fail = true;						\
        }

#define EXPECT_STREQ(x,y) \
	checks ++;						\
	if (strcmp((x),(y))!= 0) {					\
	   std::cout << __FILE__ << ":" << __LINE__<<  	\
	           " EXPECT_STREQ failed " << std::endl <<		\
	           "   LHS: " << #x << " = " << (x) << std::endl <<	\
		   "   RHS: " << #y << " = " << (y) << std::endl; \
	   fail = true;						\
        }

#define ASSERT_STREQ(x,y) \
	checks ++;						\
	if (strcmp((x),(y))!= 0) {					\
	   std::cout << __FILE__ << ":" << __LINE__<<  	\
	           " EXPECT_STREQ failed " << std::endl <<		\
	           "   LHS: " << #x << " = " << (x) << std::endl <<	\
		   "   RHS: " << #y << " = " << (y) << std::endl; \
	   fail = true;						\
	   return;						\
        }

#define ASSERT_EQ(x,y) \
	checks ++;						\
	if ((x) != (y) ) {					\
	   std::cout << __FILE__ << ":" << __LINE__<<  	\
	           " ASSERT_EQ failed" << std::endl <<		\
	           "   LHS: " << #x << " = " << (x) << std::endl <<	\
		   "   RHS: " << #y << " = " << (y) << std::endl; \
	   fail = true;						\
           return;   \
        }

#define EXPECT_TRUE(x) \
	checks ++;						\
	if (!(x) ) {					\
	   std::cout << __FILE__ << ":" << __LINE__<<  	\
	           " EXPECT_TRUE failed " << std::endl <<		\
	           "   LHS: " << #x << " = " << (x) << std::endl; 	\
	   fail = true;						\
        }

#define ASSERT_TRUE(x) \
	checks ++;						\
	if (!(x) ) {					\
	   std::cout << __FILE__ << ":" << __LINE__<<  	\
	           " EXPECT_TRUE failed " << std::endl <<		\
	           "   LHS: " << #x << " = " << (x) << std::endl;	\
	   fail = true;						\
	  return;						\
        }

#define EXPECT_FALSE(x) \
	checks ++;						\
	if ((x) ) {					\
	   std::cout << __FILE__ << ":" << __LINE__<<  	\
	           " EXPECT_FALSE failed " << std::endl <<		\
	           "   LHS: " << #x << " = " << (x) << std::endl; 	\
	   fail = true;						\
        }

#define ASSERT_FALSE(x) \
	checks ++;						\
	if ((x) ) {					\
	   std::cout << __FILE__ << ":" << __LINE__<<  	\
	           " EXPECT_FALSE failed " << std::endl <<		\
	           "   LHS: " << #x << " = " << (x) << std::endl; 	\
	   fail = true;						\
	   retrun;						\
        }


#define EXPECT_THROW(x,y) \
	{ bool gotException = false;	\
	checks ++;				\
	try { x; } catch (y _y) {	\
	    gotException = true;			\
	} catch(...) {			\
	std::cout << __FILE__<<":"<<__LINE__<< \
		" EXPECT_THROW failed" << std::endl << \
		" got unexpected exception" << std::endl; \
	   fail = true;						\
	    gotException = true;			\
	}		\
	if (!gotException) {			\
		std::cout << __FILE__<<":"<<__LINE__<< \
		" EXPECT_THROW failed" << std::endl << \
		" got no exception" << std::endl; \
	   fail = true;						\
	} 	\
        }

#define EXPECT_NO_THROW(x) \
	{ 	\
	checks ++;				\
	try { x;  	\
	} catch(...) {			\
	std::cout << __FILE__<<":"<<__LINE__<< \
		" EXPECT_NO_THROW failed" << std::endl << \
		" got unexpected exception" << std::endl; \
	   fail = true;						\
	}		\
        }

#define ASSERT_THROW(x,y) \
	{ bool gotException = false; 	\
	checks ++;				\
	try { x; } catch (y _y) {	\
	    gotException = true;			\
	} catch(...) {			\
	std::cout << __FILE__<<":"<<__LINE__<< \
		" EXPECT_THROW failed" << std::endl << \
		" got unexpected exception" << std::endl; \
	   fail = true;						\
	    return; 			\
	}		\
	if (!gotException) {			\
		std::cout << __FILE__<<":"<<__LINE__<< \
		" EXPECT_THROW failed" << std::endl << \
		" got no exception" << std::endl; \
	   fail = true;						\
	    return; 			\
	} 	\
        }


#define ASSERT_NO_THROW(x) \
	{ 	\
	checks ++;				\
	try { x;  	\
	} catch(...) {			\
	std::cout << __FILE__<<":"<<__LINE__<< \
		" ASSERT_NO_THROW failed" << std::endl << \
		" got unexpected exception" << std::endl; \
	   fail = true;						\
   	    return;					\
	}		\
        }






