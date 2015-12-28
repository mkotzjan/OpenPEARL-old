#include "PearlIncludes.h"
#include <stdio.h>

using namespace std;
using namespace pearlrt;

// select clock source
//pearlrt::Lpc17xxClock r(0);

SPCTASK(clockTest);


DCLTASK(clockTest, pearlrt::Fixed<15>(10), pearlrt::BitString<1>(1)) {
   pearlrt::Duration d(2.0);
   pearlrt::Duration d1(0.000001);
   printf("\nclockTest started\n");
   me->resume(pearlrt::Task::AFTER, pearlrt::Clock(),d, 0);
   me->resume(pearlrt::Task::AFTER, pearlrt::Clock(),d1, 0);
   me->resume(pearlrt::Task::AFTER, pearlrt::Clock(),d, 0);
   printf("clockTest finished\n");
}

