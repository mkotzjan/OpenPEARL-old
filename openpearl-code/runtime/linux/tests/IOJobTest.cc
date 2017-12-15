/////////////////////////////////////////////////////////////////////////////
// PROLOGUE
/////////////////////////////////////////////////////////////////////////////
#include <PearlIncludes.h>
#include <cmath>

const char* filename = (char*) "IOJobTest.prl";


/////////////////////////////////////////////////////////////////////////////
// CONSTANT POOL
/////////////////////////////////////////////////////////////////////////////
static /*const*/ pearlrt::Fixed<31>         CONSTANT_FIXED_POS_0_31(0);
static /*const*/ pearlrt::Fixed<31>         CONSTANT_FIXED_POS_1_31(1);
static /*const*/ pearlrt::Fixed<31>         CONSTANT_FIXED_NEG_1_31(-1);
static /*const*/ pearlrt::Fixed<31>         CONSTANT_FIXED_POS_13_31(13);
static /*const*/ pearlrt::Fixed<31>         CONSTANT_FIXED_POS_5_31(5);
static /*const*/ pearlrt::Fixed<31>         CONSTANT_FIXED_POS_6_31(6);
static /*const*/ pearlrt::Character<5>         CONSTANT_CHARACTER_5a7707ad_4efa_4d82_a801_951eba9d4126("Hallo");

/////////////////////////////////////////////////////////////////////////////
// TASK SPECIFIERS
/////////////////////////////////////////////////////////////////////////////
SPCTASK(_ttt);


/////////////////////////////////////////////////////////////////////////////
// SYSTEM PART
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// PROBLEM PART
/////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////
// DATION SPECIFICATIONS
/////////////////////////////////////////////////////////////////////////////
extern pearlrt::Device *d_stdout;
static pearlrt::SystemDationNB* _stdout = static_cast<pearlrt::SystemDationNB*>(d_stdout); 

extern pearlrt::Device *d_stdin;
static pearlrt::SystemDationNB* _stdin = static_cast<pearlrt::SystemDationNB*>(d_stdin); 



/////////////////////////////////////////////////////////////////////////////
// DATION DECLARATIONS
/////////////////////////////////////////////////////////////////////////////
static pearlrt::DationDim2 h_dim_so(80); 

pearlrt::DationPG _so(_stdout, pearlrt::Dation::OUT  | pearlrt::Dation::FORWARD, &h_dim_so);
pearlrt::DationPG _si(_stdin, pearlrt::Dation::IN  | pearlrt::Dation::FORWARD, &h_dim_so);


/////////////////////////////////////////////////////////////////////////////
// TEMPORARY SEMAPHORE ARRAYS
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// TEMPORARY BOLT ARRAYS
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// ARRAY DESCRIPTORS
/////////////////////////////////////////////////////////////////////////////
pearlrt::Fixed<31> data_array1[100]; // arrayData Fixed(0:4,0:19);
DCLARRAY(array1, 2, LIMITS({{0,4,20},{0,19,1}}));

/*
testvalue=*(data_array1+
                  array1->offset(pearlrt::Fixed<31>(0),pearlrt::Fixed<31>(0)));
*/


/////////////////////////////////////////////////////////////////////////////
// TASK DECLARATIONS
/////////////////////////////////////////////////////////////////////////////
DCLTASK(_ttt, (pearlrt::Prio( (pearlrt::Fixed<15>)255)), ((pearlrt::BitString<1>)1)) {
        pearlrt::Float<53>  _x(3.1425); 
        pearlrt::Fixed<15>  _y(4711); 
        pearlrt::Fixed<15>  _z;

        for (int i=0; i<100; i++) {
             data_array1[i] = (pearlrt::Fixed<31>)i;
        }
        me->setLocation(13, filename);
        {
            _so.dationOpen(
                0
            , (pearlrt::Character<1>*) 0
            , (pearlrt::Fixed<31>*) 0
            );
        }

        // PUT array1(3:1,18) TO so BY (4)((5)(F(6),X(2)),SKIP);
        {
             size_t resultOfExpr1=0;   // we need size_t for the format 

            pearlrt::IODataEntry dataEntries[]= {
               {.dataType={pearlrt::IODataEntry::FIXED,31},
                .dataPtr = {.outData= (data_array1+
                  array1->offset(pearlrt::Fixed<31>(3),pearlrt::Fixed<31>(1)))},
                .numberOfElements = &resultOfExpr1,
               }
            };
            pearlrt::IOFormatEntry formatEntries[]= {
               { // 0
                .format=pearlrt::IOFormatEntry::LoopStart,
                .fp1 ={.intValue=4},
                .fp2 ={.intValue=4},
               },
               { // 1
                .format=pearlrt::IOFormatEntry::LoopStart,
                .fp1 ={.intValue=2},
                .fp2 ={.intValue=5},
               },
               { // 2
                .format=pearlrt::IOFormatEntry::Fw,
                .fp1 ={.constF31Ptr = &CONSTANT_FIXED_POS_6_31},
               },
               { // 3
                  .format=pearlrt::IOFormatEntry::X,
                  .fp1={ .constF31Ptr =&CONSTANT_FIXED_POS_1_31}
               },
               { // 4
                  .format=pearlrt::IOFormatEntry::SKIP,
                  .fp1={ .constF31Ptr =&CONSTANT_FIXED_POS_1_31}
               },
           };

            pearlrt::IODataList dataList = {
                 .nbrOfEntries=sizeof(dataEntries)/sizeof(dataEntries[0]),
                 .entry=dataEntries};
            pearlrt::IOFormatList formatList = {
                 .nbrOfEntries=sizeof(formatEntries)/sizeof(formatEntries[0]),
                 .entry=formatEntries,
            };
            // expression 0 for array size
            try {
                resultOfExpr1 = ((pearlrt::Fixed<31>)(18)-
                                (pearlrt::Fixed<31>)(1) +
                                CONSTANT_FIXED_POS_1_31).x;
            } catch (Signal & s) {
               dataList.entry[0].dataType.baseType= pearlrt::IODataEntry::InduceData;
               dataList.entry[0].dataType.dataWidth= s.whichRST();
            }
            _so.put(me, &dataList, &formatList);

        }

        me->setLocation(14, filename);
        // PUT STATEMENT BEGIN
        //  PUT 'Hallo',x,y+1 TO so BY A, F(13,6),RST(z),X(-1),F(y+1),SKIP;
        {
             Fixed<15> resultOfExpr1;
             Fixed<31> resultOfExpr2;
             size_t one=1;

            pearlrt::IODataEntry dataEntries[]= {
               {.dataType={pearlrt::IODataEntry::CHAR,5},
                .dataPtr = {.outData=&CONSTANT_CHARACTER_5a7707ad_4efa_4d82_a801_951eba9d4126},
                .numberOfElements = &one
               },
               {.dataType={pearlrt::IODataEntry::FLOAT,53},
                .dataPtr = {.outData = &_x},
                .numberOfElements = &one
               },
               {.dataType={pearlrt::IODataEntry::FIXED,15},
                .dataPtr = {.outData = &resultOfExpr1},
                .numberOfElements = &one
               },
            };

          pearlrt::IOFormatEntry formatEntries[]= {
               {
                .format=pearlrt::IOFormatEntry::A,
                .fp1 ={.constF31Ptr=&CONSTANT_FIXED_POS_5_31}
               },
               {
                .format=pearlrt::IOFormatEntry::Fwd,
                .fp1 ={.constF31Ptr = &CONSTANT_FIXED_POS_13_31},
                .fp2 ={.constF31Ptr = &CONSTANT_FIXED_POS_6_31}
               },
               {
                  .format=pearlrt::IOFormatEntry::X,
                  .fp1={ .constF31Ptr =&CONSTANT_FIXED_POS_1_31}
               },
               {
                  .format=pearlrt::IOFormatEntry::RST,
                  .fp1 = {.voidPtr=&_y},
                  .fp2 = {.intValue=15}                
               },
               {
                .format=pearlrt::IOFormatEntry::Fw,
                .fp1 = {.f31Ptr=&resultOfExpr2},
               },
               {
                .format=pearlrt::IOFormatEntry::X,
                .fp1 ={.constF31Ptr=&CONSTANT_FIXED_POS_1_31}
               },

               {
                .format=pearlrt::IOFormatEntry::SKIP,
                .fp1 ={.constF31Ptr=&CONSTANT_FIXED_POS_1_31}
               },
            };
            pearlrt::IODataList dataList = {
                 .nbrOfEntries=sizeof(dataEntries)/sizeof(dataEntries[0]),
                 .entry=dataEntries};
            pearlrt::IOFormatList formatList = {
                 .nbrOfEntries=sizeof(formatEntries)/sizeof(formatEntries[0]),
                 .entry=formatEntries,
            };
            // expression 1 for data index 2: y+1
            try {
                resultOfExpr1 = _y + CONSTANT_FIXED_POS_1_31;
                //resultOfExpr1 = _y / CONSTANT_FIXED_POS_0_31;
            } catch (Signal & s) {
               dataList.entry[2].dataType.baseType= pearlrt::IODataEntry::InduceData;
               dataList.entry[2].dataType.dataWidth= s.whichRST();
               
            }
            // expression 2 for format index 4: y+1
            try {
                //resultOfExpr2 = /* _y  + */ CONSTANT_FIXED_POS_13_31;
                resultOfExpr1 = _y / CONSTANT_FIXED_POS_0_31;
            } catch (Signal & s) {
               // INDUCE in F-format --> DationPG treats this
               formatList.entry[4].format= IOFormatEntry::InduceFormat;
               formatList.entry[4].fp1.intValue= s.whichRST();
            }
            
            _so.put(me, &dataList, &formatList);
#if 0
        try {
            _so.beginSequence(me);
            _so.toA(CONSTANT_CHARACTER_5a7707ad_4efa_4d82_a801_951eba9d4126) ;
            _so.toF(_x,(pearlrt::Fixed<31>)(CONSTANT_FIXED_POS_13_31),CONSTANT_FIXED_POS_6_31);
            _so.toSkip((pearlrt::Fixed<31>)(CONSTANT_FIXED_POS_1_31));
            _so.endSequence();
        }
        catch(pearlrt::Signal &s) {
            if ( ! _so.updateRst(&s) ) {
                _so.endSequence();
                throw;
            }
            _so.endSequence();
        }
#endif
        }
        // PUT STATEMENT END

        //  PUT 'Hallo',z TO so BY A, F(6),SKIP;
        {
            size_t one = 1;

            IODataEntry dataEntries[]=
            {
               {.dataType={pearlrt::IODataEntry::CHAR,5},
                .dataPtr = {.outData=&CONSTANT_CHARACTER_5a7707ad_4efa_4d82_a801_951eba9d4126},
                .numberOfElements = &one
               },
               {.dataType={pearlrt::IODataEntry::FIXED,15},
                .dataPtr = {.outData = &_y},
                .numberOfElements = &one
               }
            };
            IOFormatEntry fmtEntries[]=
            {
               { // 0
                .format=pearlrt::IOFormatEntry::SKIP,
                .fp1 ={.constF31Ptr = &CONSTANT_FIXED_POS_1_31}
               },
               { // 1
                .format=pearlrt::IOFormatEntry::A,
                .fp1 ={.constF31Ptr=&CONSTANT_FIXED_POS_5_31}
               },
               { // 2
                .format=pearlrt::IOFormatEntry::Fwd,
                .fp1 = {.constF31Ptr=&CONSTANT_FIXED_POS_6_31},
                .fp2= {.constF31Ptr=&CONSTANT_FIXED_POS_0_31}
               },
               { // 3
                .format=pearlrt::IOFormatEntry::SKIP,
                .fp1 ={.constF31Ptr = &CONSTANT_FIXED_POS_1_31}
               },
            };
            pearlrt::IODataList dataList = {
                 .nbrOfEntries=sizeof(dataEntries)/sizeof(dataEntries[0]),
                 .entry=dataEntries};
            pearlrt::IOFormatList formatList = {
                 .nbrOfEntries=sizeof(fmtEntries)/sizeof(fmtEntries[0]),
                 .entry=fmtEntries};
            _so.put(me, &dataList, &formatList);
        }   // end PUT

        me->setLocation(100, filename);
        {
            _si.dationOpen(
                0
            , (pearlrt::Character<1>*) 0
            , (pearlrt::Fixed<31>*) 0
            );
        }
        //  GET z FROM si BY F(6),SKIP;
        {
            size_t one = 1;

            IODataEntry dataEntries[]=
            {
               {.dataType={pearlrt::IODataEntry::FIXED,15},
                .dataPtr = {.inData = &_z},
                .numberOfElements = &one
               }
            };
            IOFormatEntry fmtEntries[]=
            {
               { // 0
                .format=pearlrt::IOFormatEntry::Fwd,
                .fp1 = {.constF31Ptr=&CONSTANT_FIXED_POS_6_31},
                .fp2= {.constF31Ptr=&CONSTANT_FIXED_POS_0_31}
               },
               { // 1
                .format=pearlrt::IOFormatEntry::SKIP,
                .fp1 ={.constF31Ptr = &CONSTANT_FIXED_POS_1_31}
               },
            };
            pearlrt::IODataList dataList = {
                 .nbrOfEntries=sizeof(dataEntries)/sizeof(dataEntries[0]),
                 .entry=dataEntries};
            pearlrt::IOFormatList formatList = {
                 .nbrOfEntries=sizeof(fmtEntries)/sizeof(fmtEntries[0]),
                 .entry=fmtEntries};
            _si.get(me, &dataList, &formatList);
        }   // end GET
printf("z=%d\n", _z.x);

        me->setLocation(15, filename);
        _so.dationClose(0, (pearlrt::Fixed<15>*) 0);

}



