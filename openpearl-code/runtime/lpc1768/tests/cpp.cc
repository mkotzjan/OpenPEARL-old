#include <stdio.h>
#include <iostream>
#include "hy32a.h"
#include "Post.h"
#include <vector>

extern "C" {
void retarget_init();
}

using namespace std;
class XXX ;
vector<XXX*> list;
class XXX {
public:
   XXX() {
    cout << "in ctor of XXX" << endl;
    list.push_back(this);
    cout << "list length now: " << list.size() << endl;
   }
} x1, x2, x3;

struct test_class {
  test_class () { 
    m = new char[10];
    printf("In ctor\n");
  }
  ~test_class () {
    delete m;
    printf("In dtor\n");
  }
  char * m;
} g;

int main()
{
	LCD_Initializtion();
	LCD_Clear(Blue);
	GUI_Text(10,10, (uint8_t *)"Hallo",White, Black);

//	cout << "CoreClock=" << Chip_Clock_GetSystemClockRate()
//              << endl;

	printf("In main\n");
	Post::print();
	Post::config();

	printf("test exceptions ...\n");
        try {
          printf("in try\n");
          throw g;
          printf("**** should not hit this line\n");
        } catch(int* e) {
          printf("received int* exception \n");
        } catch(test_class e) {
          printf("received test_class exception \n");
        } catch(test_class* e) {
          printf("received test_class* exception \n");
       } 
          printf("after try\n");
            
while(1);
	return 0;
}
