#include <PearlIncludes.h>


pearlrt::Fixed<15> _f(15);
pearlrt::Ref< pearlrt::Fixed<15> > _rf;
pearlrt::Ref< pearlrt::Fixed<15> > _rf1(&_f);

DCLTASK(t1, pearlrt::Prio(10), pearlrt::BitString<1>(1)) {


    pearlrt::Fixed<15> g(42);
    me->setLocation(__LINE__,__FILE__);
    g = *_rf1;
    if (g.x != 15) {
       printf("error 1\n");
    }
    g.x = 42;
    me->setLocation(__LINE__,__FILE__);
    *_rf1 = g;
    if (_f.x != 42) {
      printf("error 2\n");
    }
    _f.x = 15;

    try {
       g = *_rf;
    } catch (pearlrt::RefNotInitializedSignal s) {
       printf("uninitialized access detected -- fine!\n");
    }

    me->setLocation(__LINE__,__FILE__);
    _rf = &g;  // assign the pointer
    me->setLocation(__LINE__,__FILE__);
    _f = *_rf;
    if (_f.x != 42) {
      printf("error 3\n");
    }

}
