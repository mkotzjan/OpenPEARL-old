/////////////////////////////////////////////////////////////////////////////
// PROLOGUE
/////////////////////////////////////////////////////////////////////////////
#include <PearlIncludes.h>
#include <cmath>

const char* filename = (char*) "lm75.prl";


/////////////////////////////////////////////////////////////////////////////
// CONSTANT POOL
/////////////////////////////////////////////////////////////////////////////
static /*const*/ pearlrt::Fixed<1>         CONSTANT_FIXED_POS_0_1(0);
static /*const*/ pearlrt::Fixed<1>         CONSTANT_FIXED_POS_1_1(1);
static /*const*/ pearlrt::Fixed<1>         CONSTANT_FIXED_NEG_1_1(-1);
static /*const*/ pearlrt::Fixed<17>         CONSTANT_FIXED_POS_100000_17(100000);
static /*const*/ pearlrt::Float<24>         CONSTANT_FLOAT_POS_10_0_24(10.0);

/////////////////////////////////////////////////////////////////////////////
// TASK SPECIFIERS
/////////////////////////////////////////////////////////////////////////////
SPCTASK(_t1);


/////////////////////////////////////////////////////////////////////////////
// SYSTEM PART
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// PROBLEM PART
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// DATION SPECIFICATIONS
/////////////////////////////////////////////////////////////////////////////
extern pearlrt::Device *d_lm75;
static pearlrt::SystemDationB* _lm75 = static_cast<pearlrt::SystemDationB*>(d_lm75); 

extern pearlrt::Device *d_stdout;
static pearlrt::SystemDationNB* _stdout = static_cast<pearlrt::SystemDationNB*>(d_stdout); 



/////////////////////////////////////////////////////////////////////////////
// DATION DECLARATIONS
/////////////////////////////////////////////////////////////////////////////
pearlrt::DationTS _thermometer(_lm75, pearlrt::Dation::IN );
static pearlrt::DationDim2 h_dim_terminal(80); 

pearlrt::DationPG _terminal(_stdout, pearlrt::Dation::OUT  | pearlrt::Dation::FORWARD, &h_dim_terminal);


/////////////////////////////////////////////////////////////////////////////
// TEMPORARY SEMAPHORE ARRAYS
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// TASK DECLARATIONS
/////////////////////////////////////////////////////////////////////////////
DCLTASK(_t1, (pearlrt::Prio( (pearlrt::Fixed<15>)255)), ((pearlrt::BitString<1>)1)) {
        #warning __cpp__ inline inserted
        pearlrt::Character<15> _text1 ("Temperatur");

        pearlrt::Fixed<15>  _t; 

        pearlrt::Float<53>  _temp; 


        me->setLocation(28, filename);
        {
            _terminal.dationOpen(
                0
            , (pearlrt::Character<1>*) 0
            , (pearlrt::Fixed<31>*) 0
            );
        }

        me->setLocation(29, filename);
        {
            _thermometer.dationOpen(
                0
            , (pearlrt::Character<1>*) 0
            , (pearlrt::Fixed<31>*) 0
            );
        }

        me->setLocation(30, filename);
        // PUT STATEMENT BEGIN
        try {
            _terminal.beginSequence(me);
            _terminal.toA(_text1) ;
            _terminal.toSkip((pearlrt::Fixed<31>)(1));
            _terminal.endSequence();
        }
        catch(pearlrt::Signal &s) {
            if ( ! _terminal.updateRst(&s) ) {
                _terminal.endSequence();
                throw;
            }
            _terminal.endSequence();
        }
        // PUT STATEMENT END

        me->setLocation(31, filename);
        {
                while ( 1 )
                {

                    me->setLocation(32, filename);
                    // TAKE STATEMENT BEGIN
                    try {
                        _thermometer.beginSequence(me);
                        _thermometer.dationRead(&_t, sizeof(_t));
                        _thermometer.endSequence();
                    }
                    catch(pearlrt::Signal &s) {
                        if ( ! _thermometer.updateRst(&s) ) {
                            _thermometer.endSequence();
                            throw;
                        }
                        _thermometer.endSequence();
                    }
                    // TAKE STATEMENT END

                    me->setLocation(33, filename);
                    _temp = _t/CONSTANT_FLOAT_POS_10_0_24;

                }

        }
}



