#include "sysincs.h"
#include "pearl.h"

SPCTASK(t1);

DCLTASK(t1,pearlrt::Prio(12), pearlrt::BitString<1>(1)) {
   pearlrt::Fixed<15> x;
}
