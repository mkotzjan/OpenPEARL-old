/////////////////////////////////////////////////////////////////////////////
// PROLOGUE
/////////////////////////////////////////////////////////////////////////////
#include <PearlIncludes.h>
#include <cmath>

const char* filename = (char*) "RPiGpio.prl";


/////////////////////////////////////////////////////////////////////////////
// CONSTANT POOL
/////////////////////////////////////////////////////////////////////////////
static /*const*/ pearlrt::Fixed<1>         CONSTANT_FIXED_POS_0_1(0);
static /*const*/ pearlrt::Fixed<1>         CONSTANT_FIXED_POS_1_1(1);
static /*const*/ pearlrt::Fixed<1>         CONSTANT_FIXED_NEG_1_1(-1);
static /*const*/ pearlrt::Fixed<5>         CONSTANT_FIXED_POS_25_5(25);
static /*const*/ pearlrt::Fixed<3>         CONSTANT_FIXED_POS_4_3(4);
static /*const*/ pearlrt::Fixed<5>         CONSTANT_FIXED_POS_27_5(27);

/////////////////////////////////////////////////////////////////////////////
// TASK SPECIFIERS
/////////////////////////////////////////////////////////////////////////////
SPCTASK(_T1);


/////////////////////////////////////////////////////////////////////////////
// SYSTEM PART
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// PROBLEM PART
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// DATION SPECIFICATIONS
/////////////////////////////////////////////////////////////////////////////
extern pearlrt::Device *d_biti;
static pearlrt::SystemDationB* _biti = static_cast<pearlrt::SystemDationB*>(d_biti); 

extern pearlrt::Device *d_bito;
static pearlrt::SystemDationB* _bito = static_cast<pearlrt::SystemDationB*>(d_bito); 



/////////////////////////////////////////////////////////////////////////////
// DATION DECLARATIONS
/////////////////////////////////////////////////////////////////////////////
pearlrt::DationTS _taste(_biti, pearlrt::Dation::IN );

pearlrt::DationTS _led(_bito, pearlrt::Dation::OUT );


/////////////////////////////////////////////////////////////////////////////
// TEMPORARY SEMAPHORE ARRAYS
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// TASK DECLARATIONS
/////////////////////////////////////////////////////////////////////////////
DCLTASK(_T1, (pearlrt::Prio( (pearlrt::Fixed<15>)255)), ((pearlrt::BitString<1>)1)) {
        #warning __cpp__ inline inserted
        pearlrt::BitString<1> _on(1);


        #warning __cpp__ inline inserted
        pearlrt::BitString<1> _off(0);


        #warning __cpp__ inline inserted
        pearlrt::BitString<5> _work(1);


        me->setLocation(28, filename);
        {
            _taste.dationOpen(
                0
            , (pearlrt::Character<1>*) 0
            , (pearlrt::Fixed<31>*) 0
            );
        }

        me->setLocation(29, filename);
        {
            _led.dationOpen(
                0
            , (pearlrt::Character<1>*) 0
            , (pearlrt::Fixed<31>*) 0
            );
        }

        me->setLocation(30, filename);
        {
                while ( 1 )
                {

                    me->setLocation(32, filename);
                    // SEND STATEMENT BEGIN
                    try {
                        _led.beginSequence(me);
                        _led.dationWrite(&_work, sizeof(_work));
                        _led.endSequence();
                    }
                    catch(pearlrt::Signal &s) {
                        if ( ! _led.updateRst(&s) ) {
                            _led.endSequence();
                            throw;
                        }
                        _led.endSequence();
                    }
                    // SEND STATEMENT END

                    me->setLocation(33, filename);
                        me->resume( pearlrt::Task::AFTER,
                                    /* at     */  pearlrt::Clock(),
                                    /* after  */  pearlrt::Duration(1.0)
                                  );

                    #warning __cpp__ inline inserted
                    _work = _work.bitCshift(1);


                    me->setLocation(34, filename);


                }

        }
}



