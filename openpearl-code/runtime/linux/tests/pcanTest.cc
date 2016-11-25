/////////////////////////////////////////////////////////////////////////////
// PROLOGUE
/////////////////////////////////////////////////////////////////////////////
#include <PearlIncludes.h>
#include <cmath>

const char* filename = (char*) "pcanTest.prl";


/////////////////////////////////////////////////////////////////////////////
// CONSTANT POOL
/////////////////////////////////////////////////////////////////////////////
static /*const*/ pearlrt::Fixed<1>         CONSTANT_FIXED_POS_0_1(0);
static /*const*/ pearlrt::Fixed<1>         CONSTANT_FIXED_POS_1_1(1);
static /*const*/ pearlrt::Fixed<1>         CONSTANT_FIXED_NEG_1_1(-1);
static /*const*/ pearlrt::Fixed<20>         CONSTANT_FIXED_POS_1000000_20(1000000);
static /*const*/ pearlrt::Fixed<3>         CONSTANT_FIXED_POS_4_3(4);
static /*const*/ pearlrt::Fixed<2>         CONSTANT_FIXED_POS_2_2(2);
static /*const*/ pearlrt::Fixed<3>         CONSTANT_FIXED_POS_6_3(6);
static /*const*/ pearlrt::Fixed<2>         CONSTANT_FIXED_POS_3_2(3);
static /*const*/ pearlrt::Character<11>         CONSTANT_CHARACTER_11__dev_pcan32("/dev/pcan32");
static /*const*/ pearlrt::Character<25>         CONSTANT_CHARACTER_25_enter_identifier_to_send_("enter identifier to send:");
static /*const*/ pearlrt::Character<14>         CONSTANT_CHARACTER_14_is_RTR___y_n__("is RTR? (y/n) ");
static /*const*/ pearlrt::Character<11>         CONSTANT_CHARACTER_11_data_size__("data size? ");
static /*const*/ pearlrt::Character<11>         CONSTANT_CHARACTER_11_data_byte__("data byte #");

/////////////////////////////////////////////////////////////////////////////
// TASK SPECIFIERS
/////////////////////////////////////////////////////////////////////////////
SPCTASK(_main);
SPCTASK(_canWrite);
SPCTASK(_canRead);


/////////////////////////////////////////////////////////////////////////////
// SYSTEM PART
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// PROBLEM PART
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// DATION SPECIFICATIONS
/////////////////////////////////////////////////////////////////////////////
extern pearlrt::Device *d_termout;
static pearlrt::SystemDationNB* _termout = static_cast<pearlrt::SystemDationNB*>(d_termout); 

extern pearlrt::Device *d_termin;
static pearlrt::SystemDationNB* _termin = static_cast<pearlrt::SystemDationNB*>(d_termin); 

extern pearlrt::Device *d_pcan;
static pearlrt::SystemDationB* _pcan = static_cast<pearlrt::SystemDationB*>(d_pcan); 



/////////////////////////////////////////////////////////////////////////////
// DATION DECLARATIONS
/////////////////////////////////////////////////////////////////////////////
static pearlrt::DationDim2 h_dim_stdout(80); 

pearlrt::DationPG _stdout(_termout, pearlrt::Dation::OUT  | pearlrt::Dation::FORWARD | pearlrt::Dation::STREAM, &h_dim_stdout);
static pearlrt::DationDim2 h_dim_stdin(80); 

pearlrt::DationPG _stdin(_termin, pearlrt::Dation::IN  | pearlrt::Dation::FORWARD | pearlrt::Dation::STREAM, &h_dim_stdin);


/////////////////////////////////////////////////////////////////////////////
// TEMPORARY SEMAPHORE ARRAYS
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// TASK DECLARATIONS
/////////////////////////////////////////////////////////////////////////////
DCLTASK(_main, (pearlrt::Prio( (pearlrt::Fixed<15>)255)), ((pearlrt::BitString<1>)1)) {
        pearlrt::Character<18>  _hello("PCan Test started"); 


        me->setLocation(18, filename);
        {
            _stdout.dationOpen(
                0
            , (pearlrt::Character<1>*) 0
            , (pearlrt::Fixed<31>*) 0
            );
        }

        me->setLocation(19, filename);
        {
            _stdin.dationOpen(
                0
            , (pearlrt::Character<1>*) 0
            , (pearlrt::Fixed<31>*) 0
            );
        }

        me->setLocation(21, filename);
        // PUT STATEMENT BEGIN
        try {
            _stdout.beginSequence(me);
            _stdout.toA(_hello) ;
            _stdout.toSkip((pearlrt::Fixed<31>)(CONSTANT_FIXED_POS_1_1));
            _stdout.endSequence();
        }
        catch(pearlrt::Signal &s) {
            if ( ! _stdout.updateRst(&s) ) {
                _stdout.endSequence();
                throw;
            }
            _stdout.endSequence();
        }
        // PUT STATEMENT END

        me->setLocation(23, filename);
            _canRead.activate( me,
                              0,
                              /* prio   */  pearlrt::Prio(),
                              /* at     */  pearlrt::Clock(),
                              /* after  */  pearlrt::Duration(),
                              /* all    */  pearlrt::Duration(),
                              /* until  */  pearlrt::Clock(),
                              /* during */  pearlrt::Duration()
                            );

        me->setLocation(24, filename);
            _canWrite.activate( me,
                              0,
                              /* prio   */  pearlrt::Prio(),
                              /* at     */  pearlrt::Clock(),
                              /* after  */  pearlrt::Duration(),
                              /* all    */  pearlrt::Duration(),
                              /* until  */  pearlrt::Clock(),
                              /* during */  pearlrt::Duration()
                            );

}
DCLTASK(_canWrite, (pearlrt::Prio( (pearlrt::Fixed<15>)255)), ((pearlrt::BitString<1>)0)) {
        pearlrt::Character<11>  _promptId("identifier:"); 

        pearlrt::DationTS _candev(_pcan, pearlrt::Dation::OUT );
        pearlrt::Fixed<11>  _id; 

        pearlrt::BitString<1>  _rtr; 

        pearlrt::Fixed<4>  _len; 

        pearlrt::BitString<8>  _data; 

        pearlrt::Character<1>  _rtrText; 

        #warning __cpp__ inline inserted
         struct _CanMessage {
           pearlrt::Fixed<11>    identifier;
           pearlrt::BitString<1> rtrRequest; 
           pearlrt::Fixed<4>     dataLength; 
           pearlrt::BitString<8> data[8];    
         } _canMessage; 


        me->setLocation(47, filename);
        {
            _candev.dationOpen(
                0
            , (pearlrt::Character<1>*) 0
            , (pearlrt::Fixed<31>*) 0
            );
        }

        me->setLocation(49, filename);
        {
                while ( 1 )
                {

                    me->setLocation(50, filename);
                    // PUT STATEMENT BEGIN
                    try {
                        _stdout.beginSequence(me);
                        _stdout.toA(CONSTANT_CHARACTER_25_enter_identifier_to_send_) ;
                        _stdout.toSkip((pearlrt::Fixed<31>)(CONSTANT_FIXED_POS_1_1));
                        _stdout.endSequence();
                    }
                    catch(pearlrt::Signal &s) {
                        if ( ! _stdout.updateRst(&s) ) {
                            _stdout.endSequence();
                            throw;
                        }
                        _stdout.endSequence();
                    }
                    // PUT STATEMENT END

                    me->setLocation(51, filename);
                    // GET STATEMENT BEGIN
                    try {
                        _stdin.beginSequence(me);
                        _stdin.fromF(_id,(pearlrt::Fixed<31>)(CONSTANT_FIXED_POS_4_3)) ;
                        _stdin.fromSkip((pearlrt::Fixed<31>)(1));
                        _stdin.endSequence();
                    }
                    catch(pearlrt::Signal &s) {
                        if ( ! _stdin.updateRst(&s) ) {
                            _stdin.endSequence();
                            throw;
                        }
                        _stdin.endSequence();
                    }
                    // GET STATEMENT END

                    me->setLocation(52, filename);
                    // PUT STATEMENT BEGIN
                    try {
                        _stdout.beginSequence(me);
                        _stdout.toA(CONSTANT_CHARACTER_14_is_RTR___y_n__) ;
                        _stdout.endSequence();
                    }
                    catch(pearlrt::Signal &s) {
                        if ( ! _stdout.updateRst(&s) ) {
                            _stdout.endSequence();
                            throw;
                        }
                        _stdout.endSequence();
                    }
                    // PUT STATEMENT END

                    me->setLocation(53, filename);
                    // GET STATEMENT BEGIN
                    try {
                        _stdin.beginSequence(me);
                        _stdin.fromA(_rtrText) ;
                        _stdin.fromSkip((pearlrt::Fixed<31>)(1));
                        _stdin.endSequence();
                    }
                    catch(pearlrt::Signal &s) {
                        if ( ! _stdin.updateRst(&s) ) {
                            _stdin.endSequence();
                            throw;
                        }
                        _stdin.endSequence();
                    }
                    // GET STATEMENT END

                    me->setLocation(54, filename);
                    // PUT STATEMENT BEGIN
                    try {
                        _stdout.beginSequence(me);
                        _stdout.toA(CONSTANT_CHARACTER_11_data_size__) ;
                        _stdout.toSkip((pearlrt::Fixed<31>)(CONSTANT_FIXED_POS_1_1));
                        _stdout.endSequence();
                    }
                    catch(pearlrt::Signal &s) {
                        if ( ! _stdout.updateRst(&s) ) {
                            _stdout.endSequence();
                            throw;
                        }
                        _stdout.endSequence();
                    }
                    // PUT STATEMENT END

                    me->setLocation(55, filename);
                    // GET STATEMENT BEGIN
                    try {
                        _stdin.beginSequence(me);
                        _stdin.fromF(_len,(pearlrt::Fixed<31>)(CONSTANT_FIXED_POS_1_1)) ;
                        _stdin.fromSkip((pearlrt::Fixed<31>)(1));
                        _stdin.endSequence();
                    }
                    catch(pearlrt::Signal &s) {
                        if ( ! _stdin.updateRst(&s) ) {
                            _stdin.endSequence();
                            throw;
                        }
                        _stdin.endSequence();
                    }
                    // GET STATEMENT END

                    me->setLocation(56, filename);
                    {
                        pearlrt::Fixed<15> a_value(1);

                        pearlrt::Fixed<15> e_value;
                        e_value = _len;

                        pearlrt::Fixed<15> s_value(1);

                        pearlrt::Fixed<15> _i;
                        _i = a_value;

                            while ((((s_value > CONSTANT_FIXED_POS_0_1).getBoolean()) &&
                                    ((a_value <= e_value).getBoolean())) ||
                                    (((s_value < CONSTANT_FIXED_POS_0_1).getBoolean()) &&
                                    ((a_value >= e_value).getBoolean())))
                            {

                                _i = a_value;

                                me->setLocation(57, filename);
                                // PUT STATEMENT BEGIN
                                try {
                                    _stdout.beginSequence(me);
                                    _stdout.toA(CONSTANT_CHARACTER_11_data_byte__) ;
                                    _stdout.toF(_i,(pearlrt::Fixed<31>)(CONSTANT_FIXED_POS_1_1));
                                    _stdout.endSequence();
                                }
                                catch(pearlrt::Signal &s) {
                                    if ( ! _stdout.updateRst(&s) ) {
                                        _stdout.endSequence();
                                        throw;
                                    }
                                    _stdout.endSequence();
                                }
                                // PUT STATEMENT END

                                me->setLocation(58, filename);
                                // GET STATEMENT BEGIN
                                try {
                                    _stdin.beginSequence(me);
                                    _stdin.fromB4(_data,CONSTANT_FIXED_POS_2_2);
                                    _stdin.fromSkip((pearlrt::Fixed<31>)(1));
                                    _stdin.endSequence();
                                }
                                catch(pearlrt::Signal &s) {
                                    if ( ! _stdin.updateRst(&s) ) {
                                        _stdin.endSequence();
                                        throw;
                                    }
                                    _stdin.endSequence();
                                }
                                // GET STATEMENT END

                                #warning __cpp__ inline inserted
                                _canMessage.data[_i.x] = _data;


                                me->setLocation(59, filename);


                                me->setLocation(56, filename);

                                if (((s_value > CONSTANT_FIXED_POS_0_1).getBoolean() &&
                                     (a_value <= (e_value - s_value)).getBoolean()) ||
                                    ((s_value < CONSTANT_FIXED_POS_0_1).getBoolean() &&
                                    (a_value >= (e_value - s_value)).getBoolean())) {
                                    a_value = a_value + s_value;
                                } else {
                                    break;
                                }
                            }
                    }

                }

        }
}
DCLTASK(_canRead, (pearlrt::Prio( (pearlrt::Fixed<15>)255)), ((pearlrt::BitString<1>)0)) {
        pearlrt::DationTS _candev(_pcan, pearlrt::Dation::IN );
        pearlrt::Fixed<11>  _id; 

        pearlrt::BitString<1>  _rtr; 

        pearlrt::Fixed<4>  _len; 

        pearlrt::BitString<8>  _d1; 
        pearlrt::BitString<8>  _d2; 
        pearlrt::BitString<8>  _d3; 
        pearlrt::BitString<8>  _d4; 
        pearlrt::BitString<8>  _d5; 
        pearlrt::BitString<8>  _d6; 
        pearlrt::BitString<8>  _d7; 
        pearlrt::BitString<8>  _d8; 

        #warning __cpp__ inline inserted
         struct _CanMessage {
           pearlrt::Fixed<11>    identifier;
           pearlrt::BitString<1> rtrRequest; 
           pearlrt::Fixed<4>     dataLength; 
           pearlrt::BitString<8> data[8];    
         } _canMessage; 


        me->setLocation(79, filename);
        {
            _candev.dationOpen(
                0
            , (pearlrt::Character<1>*) 0
            , (pearlrt::Fixed<31>*) 0
            );
        }

        me->setLocation(80, filename);
        {
                while ( 1 )
                {

                    me->setLocation(81, filename);
                    // TAKE STATEMENT BEGIN
                    try {
                        _candev.beginSequence(me);
                        _candev.dationRead(&_canMessage, sizeof(_canMessage));
                        _candev.endSequence();
                    }
                    catch(pearlrt::Signal &s) {
                        if ( ! _candev.updateRst(&s) ) {
                            _candev.endSequence();
                            throw;
                        }
                        _candev.endSequence();
                    }
                    // TAKE STATEMENT END

                    #warning __cpp__ inline inserted
                       _id = _canMessage.identifier;
                       _rtr = _canMessage.rtrRequest;
                       _len = _canMessage.dataLength;
                       _d1 = _canMessage.data[0];
                       _d2 = _canMessage.data[1];
                       _d3 = _canMessage.data[2];
                       _d4 = _canMessage.data[3];
                       _d5 = _canMessage.data[4];
                       _d6 = _canMessage.data[5];
                       _d7 = _canMessage.data[6];
                       _d8 = _canMessage.data[7];
                     printf("id=%d rtr=%d len=%d data=%02x %02x %02x %02x %02x %02x %02x %02x\n",
                        _id.x, !!_rtr.x, _len.x, _d1.x, _d2.x, _d3.x, _d4.x,
                                                 _d5.x, _d6.x, _d7.x, _d8.x);


                    me->setLocation(82, filename);
                    me->setLocation(99, filename);
                    // PUT STATEMENT BEGIN
                    try {
                        _stdout.beginSequence(me);
                        _stdout.toF(_id,(pearlrt::Fixed<31>)(CONSTANT_FIXED_POS_6_3));
                        _stdout.toX((pearlrt::Fixed<31>)(CONSTANT_FIXED_POS_3_2));
                        _stdout.toB4(_rtr,CONSTANT_FIXED_POS_1_1);
                        _stdout.toX((pearlrt::Fixed<31>)(CONSTANT_FIXED_POS_3_2));
                        _stdout.toF(_len,(pearlrt::Fixed<31>)(CONSTANT_FIXED_POS_4_3));
                        _stdout.toB4(_d1,CONSTANT_FIXED_POS_2_2);
                        _stdout.toX((pearlrt::Fixed<31>)(CONSTANT_FIXED_POS_2_2));
                        _stdout.toB4(_d2,CONSTANT_FIXED_POS_2_2);
                        _stdout.toX((pearlrt::Fixed<31>)(CONSTANT_FIXED_POS_2_2));
                        _stdout.toB4(_d3,CONSTANT_FIXED_POS_2_2);
                        _stdout.toX((pearlrt::Fixed<31>)(CONSTANT_FIXED_POS_2_2));
                        _stdout.toB4(_d4,CONSTANT_FIXED_POS_2_2);
                        _stdout.toX((pearlrt::Fixed<31>)(CONSTANT_FIXED_POS_2_2));
                        _stdout.toB4(_d5,CONSTANT_FIXED_POS_2_2);
                        _stdout.toX((pearlrt::Fixed<31>)(CONSTANT_FIXED_POS_2_2));
                        _stdout.toB4(_d6,CONSTANT_FIXED_POS_2_2);
                        _stdout.toX((pearlrt::Fixed<31>)(CONSTANT_FIXED_POS_2_2));
                        _stdout.toB4(_d7,CONSTANT_FIXED_POS_2_2);
                        _stdout.toX((pearlrt::Fixed<31>)(CONSTANT_FIXED_POS_2_2));
                        _stdout.toB4(_d8,CONSTANT_FIXED_POS_2_2);
                        _stdout.toX((pearlrt::Fixed<31>)(CONSTANT_FIXED_POS_2_2));
                        _stdout.toSkip((pearlrt::Fixed<31>)(CONSTANT_FIXED_POS_1_1));
                        _stdout.endSequence();
                    }
                    catch(pearlrt::Signal &s) {
                        if ( ! _stdout.updateRst(&s) ) {
                            _stdout.endSequence();
                            throw;
                        }
                        _stdout.endSequence();
                    }
                    // PUT STATEMENT END


                }

        }
}



