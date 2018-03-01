/* classImpl : auto generated (Tue Jan  9 16:27:55 2018) */
class InternalSignal : public Signal {
 public:
 InternalSignal() {
   type = (char*)"internal signal";
   rstNum = 0;
 };
};

class InternalTaskSignal : public InternalSignal {
 public:
 InternalTaskSignal() {
   type = (char*)"internal tasking signal";
   rstNum = 1;
 };
};

class InternalSignalsSignal : public InternalSignal {
 public:
 InternalSignalsSignal() {
   type = (char*)"error in signal administration";
   rstNum = 2;
 };
};

class InternalDationSignal : public InternalSignal {
 public:
 InternalDationSignal() {
   type = (char*)"problem in dation";
   rstNum = 3;
 };
};

class InternalDatatypeSignal : public InternalSignal {
 public:
 InternalDatatypeSignal() {
   type = (char*)"Illegal operation in expression";
   rstNum = 4;
 };
};

class TaskSignal : public Signal {
 public:
 TaskSignal() {
   type = (char*)"*** will never be printed ***";
   rstNum = 100;
 };
};

class PrioOutOfRangeSignal : public TaskSignal {
 public:
 PrioOutOfRangeSignal() {
   type = (char*)"priority out of range";
   rstNum = 101;
 };
};

class TaskRunningSignal : public TaskSignal {
 public:
 TaskRunningSignal() {
   type = (char*)"activate an active task";
   rstNum = 102;
 };
};

class TaskSuspendedSignal : public TaskSignal {
 public:
 TaskSuspendedSignal() {
   type = (char*)"Illegal operation on suspended task";
   rstNum = 103;
 };
};

class TaskTerminatedSignal : public TaskSignal {
 public:
 TaskTerminatedSignal() {
   type = (char*)"Illegal operation on terminated task";
   rstNum = 104;
 };
};

class IllegalSchedulingSignal : public TaskSignal {
 public:
 IllegalSchedulingSignal() {
   type = (char*)"illegal scheduling condition";
   rstNum = 106;
 };
};

class PriorityNotMapableSignal : public TaskSignal {
 public:
 PriorityNotMapableSignal() {
   type = (char*)"priority is not mapable to linux range";
   rstNum = 105;
 };
};

class ArithmeticSignal : public Signal {
 public:
 ArithmeticSignal() {
   type = (char*)"*** will never be printed ***";
   rstNum = 200;
 };
};

class FixedRangeSignal : public ArithmeticSignal {
 public:
 FixedRangeSignal() {
   type = (char*)"fixed overflow";
   rstNum = 201;
 };
};

class FixedDivideByZeroSignal : public ArithmeticSignal {
 public:
 FixedDivideByZeroSignal() {
   type = (char*)"Fixed divide by zero";
   rstNum = 202;
 };
};

class ArithmeticOverflowSignal : public ArithmeticSignal {
 public:
 ArithmeticOverflowSignal() {
   type = (char*)"arithmetic overflow";
   rstNum = 203;
 };
};

class ArithmeticUnderflowSignal : public ArithmeticSignal {
 public:
 ArithmeticUnderflowSignal() {
   type = (char*)"arithmetic underflow";
   rstNum = 204;
 };
};

class ArithmeticDivideByZeroSignal : public ArithmeticSignal {
 public:
 ArithmeticDivideByZeroSignal() {
   type = (char*)"arithmetic divide by zero";
   rstNum = 205;
 };
};

class SemaOverflowSignal : public ArithmeticSignal {
 public:
 SemaOverflowSignal() {
   type = (char*)"semaphore value overflow";
   rstNum = 206;
 };
};

class BoltStateSignal : public Signal {
 public:
 BoltStateSignal() {
   type = (char*)"wrong bolt state";
   rstNum = 207;
 };
};

class DurationDivideByZeroSignal : public ArithmeticSignal {
 public:
 DurationDivideByZeroSignal() {
   type = (char*)"duration div 0";
   rstNum = 208;
 };
};

class DurationRangeSignal : public ArithmeticSignal {
 public:
 DurationRangeSignal() {
   type = (char*)"duration value overflow";
   rstNum = 209;
 };
};

class FloatIsNaNSignal : public ArithmeticSignal {
 public:
 FloatIsNaNSignal() {
   type = (char*)"Float result is NaN";
   rstNum = 210;
 };
};

class FloatIsINFSignal : public ArithmeticSignal {
 public:
 FloatIsINFSignal() {
   type = (char*)"Float result is INF";
   rstNum = 210;
 };
};

class BitIndexOutOfRangeSignal : public Signal {
 public:
 BitIndexOutOfRangeSignal() {
   type = (char*)"bit index out of range";
   rstNum = 301;
 };
};

class CharacterTooLongSignal : public Signal {
 public:
 CharacterTooLongSignal() {
   type = (char*)"character string too long";
   rstNum = 302;
 };
};

class CharacterIndexOutOfRangeSignal : public Signal {
 public:
 CharacterIndexOutOfRangeSignal() {
   type = (char*)"character index out of range";
   rstNum = 303;
 };
};

class DationSignal : public Signal {
 public:
 DationSignal() {
   type = (char*)"dation opertion failed";
   rstNum = 304;
 };
};

class DationParamSignal : public DationSignal {
 public:
 DationParamSignal() {
   type = (char*)"illegal parameter, not permitted operation";
   rstNum = 306;
 };
};

class DationNotOpenSignal : public DationSignal {
 public:
 DationNotOpenSignal() {
   type = (char*)"Dation not opened";
   rstNum = 306;
 };
};

class OpenFailedSignal : public DationSignal {
 public:
 OpenFailedSignal() {
   type = (char*)"device doesn't exist or no permission";
   rstNum = 307;
 };
};

class CloseFailedSignal : public DationSignal {
 public:
 CloseFailedSignal() {
   type = (char*)"invalid file descriptor or i/o error";
   rstNum = 308;
 };
};

class PositioningFailedSignal : public DationSignal {
 public:
 PositioningFailedSignal() {
   type = (char*)"Positioning the Dations failed";
   rstNum = 309;
 };
};

class InvalidPositioningSignal : public DationSignal {
 public:
 InvalidPositioningSignal() {
   type = (char*)"Positioning forbidden, because wrong or no dimension";
   rstNum = 310;
 };
};

class ReadingFailedSignal : public DationSignal {
 public:
 ReadingFailedSignal() {
   type = (char*)"reading data from dation failed";
   rstNum = 311;
 };
};

class WritingFailedSignal : public DationSignal {
 public:
 WritingFailedSignal() {
   type = (char*)"writing data to dation failed";
   rstNum = 312;
 };
};

class DationIndexBoundSignal : public DationSignal {
 public:
 DationIndexBoundSignal() {
   type = (char*)"Dation position out of bounds";
   rstNum = 313;
 };
};

class DationNotSupportedSignal : public DationSignal {
 public:
 DationNotSupportedSignal() {
   type = (char*)"Dation operation not supported";
   rstNum = 314;
 };
};

class DationEOFSignal : public DationSignal {
 public:
 DationEOFSignal() {
   type = (char*)"EOF encountered";
   rstNum = 315;
 };
};

class NoEOFDationSignal : public DationSignal {
 public:
 NoEOFDationSignal() {
   type = (char*)"EOF not supported";
   rstNum = 316;
 };
};

class DationTFURecordSignal : public DationSignal {
 public:
 DationTFURecordSignal() {
   type = (char*)"record is longer than TFU size";
   rstNum = 317;
 };
};

class DationDatatypeSignal : public DationSignal {
 public:
 DationDatatypeSignal() {
   type = (char*)"data type does not match with format element";
   rstNum = 318;
 };
};

class DationFormatRepetitionValue : public DationSignal {
 public:
 DationFormatRepetitionValue() {
   type = (char*)"number of repetitions must be >=0";
   rstNum = 319;
 };
};

class DationFormatRepetitionOverflow : public DationSignal {
 public:
 DationFormatRepetitionOverflow() {
   type = (char*)"too many loops in format";
   rstNum = 320;
 };
};

class ArrayIndexOutOfBoundsSignal : public Signal {
 public:
 ArrayIndexOutOfBoundsSignal() {
   type = (char*)"Array Index out of Bounds";
   rstNum = 321;
 };
};

class RefNotInitializedSignal : public Signal {
 public:
 RefNotInitializedSignal() {
   type = (char*)"Reference not initialized";
   rstNum = 322;
 };
};

class DeviceNotFoundSignal : public DationSignal {
 public:
 DeviceNotFoundSignal() {
   type = (char*)"Device not found";
   rstNum = 305;
 };
};

class FormatSignal : public Signal {
 public:
 FormatSignal() {
   type = (char*)"format";
   rstNum = 400;
 };
};

class NoMoreCharactersSignal : public FormatSignal {
 public:
 NoMoreCharactersSignal() {
   type = (char*)"no more characters found";
   rstNum = 401;
 };
};

class DurationFormatSignal : public FormatSignal {
 public:
 DurationFormatSignal() {
   type = (char*)"illegal parameter for D-format";
   rstNum = 402;
 };
};

class ClockFormatSignal : public FormatSignal {
 public:
 ClockFormatSignal() {
   type = (char*)"illegal parameter for T-format";
   rstNum = 403;
 };
};

class FixedFormatSignal : public FormatSignal {
 public:
 FixedFormatSignal() {
   type = (char*)"illegal parameter for F-format";
   rstNum = 404;
 };
};

class CharacterFormatSignal : public FormatSignal {
 public:
 CharacterFormatSignal() {
   type = (char*)"illegal parameter for A-format";
   rstNum = 405;
 };
};

class BitFormatSignal : public FormatSignal {
 public:
 BitFormatSignal() {
   type = (char*)"illegal parameter for B-format";
   rstNum = 406;
 };
};

class ClockValueSignal : public FormatSignal {
 public:
 ClockValueSignal() {
   type = (char*)"illegal value for T-format";
   rstNum = 407;
 };
};

class FixedValueSignal : public FormatSignal {
 public:
 FixedValueSignal() {
   type = (char*)"Illegal value for F-format ";
   rstNum = 408;
 };
};

class BitValueSignal : public FormatSignal {
 public:
 BitValueSignal() {
   type = (char*)"Illegal value for B-format ";
   rstNum = 409;
 };
};

class DurationValueSignal : public FormatSignal {
 public:
 DurationValueSignal() {
   type = (char*)"Illegal value for D-format ";
   rstNum = 410;
 };
};

class ExpFormatSignal : public FormatSignal {
 public:
 ExpFormatSignal() {
   type = (char*)"illegal parameter for E-format";
   rstNum = 411;
 };
};

class ExpValueSignal : public FormatSignal {
 public:
 ExpValueSignal() {
   type = (char*)"Illegal value for E-format ";
   rstNum = 412;
 };
};

/* extList : auto generated (Tue Jan  9 16:27:55 2018) */
extern InternalSignal theInternalSignal;
extern InternalTaskSignal theInternalTaskSignal;
extern InternalSignalsSignal theInternalSignalsSignal;
extern InternalDationSignal theInternalDationSignal;
extern InternalDatatypeSignal theInternalDatatypeSignal;
extern TaskSignal theTaskSignal;
extern PrioOutOfRangeSignal thePrioOutOfRangeSignal;
extern TaskRunningSignal theTaskRunningSignal;
extern TaskSuspendedSignal theTaskSuspendedSignal;
extern TaskTerminatedSignal theTaskTerminatedSignal;
extern IllegalSchedulingSignal theIllegalSchedulingSignal;
extern PriorityNotMapableSignal thePriorityNotMapableSignal;
extern ArithmeticSignal theArithmeticSignal;
extern FixedRangeSignal theFixedRangeSignal;
extern FixedDivideByZeroSignal theFixedDivideByZeroSignal;
extern ArithmeticOverflowSignal theArithmeticOverflowSignal;
extern ArithmeticUnderflowSignal theArithmeticUnderflowSignal;
extern ArithmeticDivideByZeroSignal theArithmeticDivideByZeroSignal;
extern SemaOverflowSignal theSemaOverflowSignal;
extern BoltStateSignal theBoltStateSignal;
extern DurationDivideByZeroSignal theDurationDivideByZeroSignal;
extern DurationRangeSignal theDurationRangeSignal;
extern FloatIsNaNSignal theFloatIsNaNSignal;
extern FloatIsINFSignal theFloatIsINFSignal;
extern BitIndexOutOfRangeSignal theBitIndexOutOfRangeSignal;
extern CharacterTooLongSignal theCharacterTooLongSignal;
extern CharacterIndexOutOfRangeSignal theCharacterIndexOutOfRangeSignal;
extern DationSignal theDationSignal;
extern DationParamSignal theDationParamSignal;
extern DationNotOpenSignal theDationNotOpenSignal;
extern OpenFailedSignal theOpenFailedSignal;
extern CloseFailedSignal theCloseFailedSignal;
extern PositioningFailedSignal thePositioningFailedSignal;
extern InvalidPositioningSignal theInvalidPositioningSignal;
extern ReadingFailedSignal theReadingFailedSignal;
extern WritingFailedSignal theWritingFailedSignal;
extern DationIndexBoundSignal theDationIndexBoundSignal;
extern DationNotSupportedSignal theDationNotSupportedSignal;
extern DationEOFSignal theDationEOFSignal;
extern NoEOFDationSignal theNoEOFDationSignal;
extern DationTFURecordSignal theDationTFURecordSignal;
extern DationDatatypeSignal theDationDatatypeSignal;
extern DationFormatRepetitionValue theDationFormatRepetitionValue;
extern DationFormatRepetitionOverflow theDationFormatRepetitionOverflow;
extern ArrayIndexOutOfBoundsSignal theArrayIndexOutOfBoundsSignal;
extern RefNotInitializedSignal theRefNotInitializedSignal;
extern DeviceNotFoundSignal theDeviceNotFoundSignal;
extern FormatSignal theFormatSignal;
extern NoMoreCharactersSignal theNoMoreCharactersSignal;
extern DurationFormatSignal theDurationFormatSignal;
extern ClockFormatSignal theClockFormatSignal;
extern FixedFormatSignal theFixedFormatSignal;
extern CharacterFormatSignal theCharacterFormatSignal;
extern BitFormatSignal theBitFormatSignal;
extern ClockValueSignal theClockValueSignal;
extern FixedValueSignal theFixedValueSignal;
extern BitValueSignal theBitValueSignal;
extern DurationValueSignal theDurationValueSignal;
extern ExpFormatSignal theExpFormatSignal;
extern ExpValueSignal theExpValueSignal;
