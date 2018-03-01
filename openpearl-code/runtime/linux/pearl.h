#include <stdlib.h>
#include <cstdarg>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>
#include <string>
#include <cmath>
#include <math.h>
#include <stddef.h>
#include <unistd.h>
#include <termios.h>
#include <signal.h>



#define SIGNALS_INCLUDES

namespace pearlrt {

   class Signal {
   private:
      static Signal ** signalVector;
      static size_t nbrOfSignals;
   public:

      static void throwSignalByRst(int rst);
   protected:
      char* type;
      int rstNum;
      int currentRst;
   public:

      Signal();

      const char* which(void);

      int whichRST(void);

      void induce(void);

      void induce(int r);

      int getCurrentRst(void);
   };


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
}

#define LOG_INCLUDED


#define SYSTEMDATIONNB_INCLUDED

#define SYSTEMDATION_INCLUDED

#define DATION_INCLUDED

#define DEVICE_INCLUDED
namespace pearlrt {

   class Device {
   };
}

#define FIXED_H_INCLUDED




#define FIXED63_INCLUDED

namespace pearlrt {


   class Fixed63 {
   private:

      typedef uint64_t UFixed63_t;
   public:

      typedef int64_t Fixed63_t;
   private:

      typedef int32_t __int32;

      typedef uint32_t __uint32;
      void regMultiply(const UFixed63_t& a,
                       const UFixed63_t& b, UFixed63_t *ret) ;
      Fixed63_t x;
      static const Fixed63_t MinInt = 0x8000000000000000LL;
      static const Fixed63_t MaxInt = 0x7fffffffffffffffLL;
   public:
      Fixed63();
      Fixed63(Fixed63_t y);
      Fixed63(double y);
      Fixed63_t get() const;
      Fixed63 operator+=(const Fixed63& rhs);
      Fixed63 operator+(const Fixed63& rhs) const;
      Fixed63 operator-=(const Fixed63& rhs);
      Fixed63 operator-(const Fixed63& rhs) const;
      Fixed63 operator-() const;
      Fixed63 operator/=(const Fixed63& rhs);
      Fixed63 operator/(const Fixed63& rhs) const;
      Fixed63 operator%=(const Fixed63& rhs);
      Fixed63 operator%(const Fixed63& rhs) const;
      Fixed63 operator*=(const Fixed63& rhs);
      Fixed63 operator*(const Fixed63& rhs) const;
      int compare(const Fixed63& rhs) const;
   };
}

#define AUTOCONF_H_INCLUDED
#define CONFIG_INSTALL_Target "/usr/local"
#define CONFIG_LINUX 1
#define CONFIG_COMPILER_ANTLR "/usr/local/lib/antlr-4.5-complete.jar"

/**
\file stackcheck.h
*/
#ifndef NOSTACKCHECK
#if (TARGET==2 && CONFIG_LPC1768_CHECK_STACK_OVERFLOW==1)
/**
Stack checking is useful but it costs lot of execution time for
templated functions.
There is an option to enable/disbale stack checking on the
microcontroller part of the OpenPEARL system.
On normal linux systems this option is not supported by gcc.
*/
#define NOSTACKCHECK __attribute__((no_instrument_function))
#else
/**
\def NOSTACKCHECK
Stack checking is useful but it costs lot of execution time for
templated functions.
There is an option to enable/disbale stack checking on the
microcontroller part of the OpenPEARL system.
On normal linux systems this option is not supoorted by gcc.
*/
#define NOSTACKCHECK /* nothing */
#endif
#endif

#define IFTHENELSETEMPLATE_INCLUDED
namespace pearlrt {

   template< bool Condition, class THEN, class ELSE > struct IF { };

   template<class THEN, class ELSE>
   struct IF<true, THEN, ELSE > {

      typedef THEN SELECT_CLASS;
   };

   template<class THEN, class ELSE >
   struct IF<false, THEN, ELSE > {

      typedef ELSE SELECT_CLASS;
   };
}

#define NUMBEROFBYTES_INCLUDE
namespace pearlrt {

   template< unsigned int len > class NumberOfBytes {
   public:

      enum NumberOfBytesLen {
         NumberOfBytesN0 = (0 < len && len <= 8) ? 1 : 0,
         NumberOfBytesN1 = (8 < len && len <= 16) ? 2 : 0,
         NumberOfBytesN2 = (16 < len && len <= 32) ? 4 : 0,
         NumberOfBytesN3 = (32 < len && len <= 64) ? 8 : 0,
      };

      enum NumberOfBytesNbr {resultFixed = NumberOfBytesN0 + NumberOfBytesN1 +
                              NumberOfBytesN2 + NumberOfBytesN3};

      enum NumberOfBytesB {
         NumberOfBytesB0 = (0 < len && len <= 8) ? 1 : 0,
         NumberOfBytesB1 = (8 < len && len <= 16) ? 2 : 0,
         NumberOfBytesB2 = (16 < len && len <= 32) ? 4 : 0,
         NumberOfBytesB3 = (32 < len && len <= 64) ? 8 : 0,
      };

      enum R {resultBitString = NumberOfBytesB0 + NumberOfBytesB1 +
                                NumberOfBytesB2 + NumberOfBytesB3};
   };
}
namespace pearlrt {
   template<int S> class FixedTypes;

   template<> class FixedTypes<1> {
   public:

      typedef int8_t NativeFixedType;

      typedef uint8_t UNativeFixedType;

      typedef int16_t LongerNativeFixedType;
   };

   template<> class FixedTypes<2> {
   public:

      typedef int16_t NativeFixedType;

      typedef uint16_t UNativeFixedType;

      typedef int32_t LongerNativeFixedType;
   };

   template<> class FixedTypes<4> {
   public:

      typedef int32_t NativeFixedType;

      typedef uint32_t UNativeFixedType;

      typedef int64_t LongerNativeFixedType;
   };

   template<> class FixedTypes<8> {
   public:

      typedef int64_t NativeFixedType;

      typedef uint64_t UNativeFixedType;

      typedef int64_t LongerNativeFixedType;
   };


   template<int S> class Fixed {
   public:
   private:

      static const int len = NumberOfBytes < S + 1 >::resultFixed;
   public:

      typedef typename FixedTypes<len>::NativeFixedType NativeType;

      typedef typename FixedTypes<len>::UNativeFixedType UNativeType;

      typedef typename FixedTypes<len>::LongerNativeFixedType LongerNativeType;

      NativeType x;

      NativeType get() const NOSTACKCHECK {
         return x;
      }
   private:
      struct LessThan32Bits {
         static Fixed add(const Fixed & lhs,
                          const Fixed & rhs) NOSTACKCHECK  {
            Fixed h;
            LongerNativeType r;
            h.x = rhs.x + lhs.x;
            r = rhs.x;
            r += lhs.x;
            if (h.x != r || r < minValue || r > maxValue) {
               throw theFixedRangeSignal;
            }
            return h;
         }
         static Fixed complement(const Fixed & lhs)  {
            Fixed h;
            LongerNativeType r;
            h.x = -lhs.x;
            r = 0;
            r -= lhs.x;
            if (h.x != r || r < minValue || r > maxValue) {
               throw theFixedRangeSignal;
            }
            return h;
         }
         static Fixed substract(const Fixed & lhs,
                                const Fixed & rhs) {
            Fixed h;
            LongerNativeType r;
            h.x = lhs.x - rhs.x;
            r = lhs.x;
            r -= rhs.x;
            if (r != h.x || r < minValue || r > maxValue) {
               throw theFixedRangeSignal;
            }
            return h;
         }
         static Fixed multiply(const Fixed & lhs,
                               const Fixed & rhs) {
            Fixed h;
            LongerNativeType r;
            r = rhs.x;
            r *= lhs.x;
            if (r < minValue || r > maxValue) {
               throw theFixedRangeSignal;
            }
            h = r;
            return h;
         }
         static Fixed divide(const Fixed & lhs,
                             const Fixed & rhs) {
            Fixed h;
            LongerNativeType r;
            if (rhs.x == 0) {
               throw theFixedDivideByZeroSignal;
            }
            r = lhs.x;
            r /= rhs.x;
            if (r < minValue || r > maxValue) {
               throw theFixedRangeSignal;
            }
            h.x = r;
            return h;
         }
         static Fixed modulo(const Fixed & lhs, const Fixed & rhs) {
            Fixed h;
            if (rhs.x == 0) {
               throw theFixedDivideByZeroSignal;
            }
            if (lhs.x == minValue && rhs.x == -1) {
               return 0;
            }
            h.x = lhs.x % rhs.x;
            return h;
         }
      };
      struct MoreThan31Bits {
         static Fixed add(const Fixed & lhs,
                          const Fixed & rhs)  {
            Fixed r;
            Fixed63 h(lhs.x);
            h += Fixed63(rhs.x);
            r.x = h.get();
            if (r.x < minValue || r.x > maxValue) {
               throw theFixedRangeSignal;
            }
            return r;
         }
         static Fixed complement(const Fixed& lhs) {
            Fixed r;
            Fixed63 h;
            h =  -Fixed63(lhs.x);
            r.x = h.get();
            if (r.x < minValue || r.x > maxValue) {
               throw theFixedRangeSignal;
            }
            return r;
         }
         static Fixed substract(const Fixed& lhs, const Fixed & rhs) {
            Fixed r;
            Fixed63 h(lhs.x);
            h -= Fixed63(rhs.x);
            r.x = h.get();
            if (r.x < minValue || r.x > maxValue) {
               throw theFixedRangeSignal;
            }
            return r;
         }
         static Fixed multiply(const Fixed& lhs, const Fixed& rhs) {
            Fixed r;
            Fixed63 h(lhs.x);
            h *= Fixed63(rhs.x);
            r.x = h.get();
            if (r.x < minValue || r.x > maxValue) {
               throw theFixedRangeSignal;
            }
            return r;
         }
         static Fixed divide(const Fixed& lhs, const Fixed& rhs) {
            Fixed r;
            Fixed63 h(lhs.x);
            if (rhs.x == 0) {
               throw theFixedDivideByZeroSignal;
            }
            h /= Fixed63(rhs.x);
            r.x = h.get();
            if (r.x < minValue || r.x > maxValue) {
               throw theFixedRangeSignal;
            }
            return r;
         }
         static Fixed modulo(const Fixed& lhs, const Fixed& rhs) {
            Fixed r;
            Fixed63 h(lhs.x);
            h %= Fixed63(rhs.x);
            r.x = h.get();
            if (r.x < minValue || r.x > maxValue) {
               throw theFixedRangeSignal;
            }
            return r;
         }
      };
   private:
      static const NativeType maxValue =
         (((UNativeType)(-1))) >> (sizeof(x) * 8 - S);
      static const NativeType minValue =
         (((UNativeType)(-1)) >> S) << S;
      static const LongerNativeType mask = ((LongerNativeType)minValue) << 1;
   public:

      Fixed() NOSTACKCHECK {
         x = 0;
      }

      Fixed(LongerNativeType y) NOSTACKCHECK {
         if (y > maxValue || y < minValue) {
            throw theFixedRangeSignal;
         }
         x = y;
      }

      template<int P> Fixed operator=(const Fixed<P> & y) {
         if (y.x > maxValue || y.x < minValue) {
            throw theFixedRangeSignal;
         }
         x = y.x;
         return *this;
      }

      template<int P> operator Fixed<P> () const {
         Fixed<P> result(x);
         return result;
      }

      Fixed abs() {
         Fixed y(x);
         if (x < 0) {
            y = Fixed(-x);
         }
         return y;
      }

      Fixed<1> sign() {
         if (x < 0) {
            return Fixed<1>(-1);
         } else if (x > 0) {
            return Fixed<1>(1);
         }
         return Fixed<1>(0);
      }

      Fixed add(const Fixed & rhs) const {
         return (IF < (S < 32),
                 LessThan32Bits,
                 MoreThan31Bits >::SELECT_CLASS::add(*this, rhs)
                );
      }

      Fixed operator- () const {
         return (IF < (S < 32),
                 LessThan32Bits,
                 MoreThan31Bits >::SELECT_CLASS::complement(*this)
                );
      }

      Fixed substract(const Fixed & rhs) const {
         return (IF < (S < 32),
                 LessThan32Bits,
                 MoreThan31Bits >::
                 SELECT_CLASS::substract(*this, rhs)
                );
      }

      Fixed multiply(const Fixed & rhs) const {
         return (IF < (S < 32),
                 LessThan32Bits,
                 MoreThan31Bits >::
                 SELECT_CLASS::multiply(*this, rhs)
                );
      }

      Fixed divide(const Fixed & rhs) const {
         return (IF < (S < 32),
                 LessThan32Bits,
                 MoreThan31Bits >::
                 SELECT_CLASS::divide(*this, rhs)
                );
      }

      Fixed modulo(const Fixed & rhs) const {
         return (IF < (S < 32),
                 LessThan32Bits,
                 MoreThan31Bits >::
                 SELECT_CLASS::modulo(*this, rhs)
                );
      }

      template<int P>Fixed pow(const Fixed<P>& rhs) const {
         Fixed result(1);
         Fixed base(x);
         int64_t exp = rhs.x;
         if (exp == 0) {
            if (x != 0) {
               result.x = 1;
            } else {
               throw  theFixedRangeSignal;
            }
         } else  if (exp > 0) {
            try {
               while (exp) {
                  if (exp & 1) {
                     result = result * base;
                  }
                  exp >>= 1;
                  if (exp) {
                     base = base * base;
                  }
               }
            } catch (ArithmeticOverflowSignal e) {
               throw  theFixedRangeSignal;
            }
            if (result.x > maxValue || result.x < minValue) {
               throw  theFixedRangeSignal;
            }
         } else if (exp == -1) {
            result.x = 1 / x;
         } else if (exp < -1) {
            if (x ==  1 || x == -1) {
               if (exp & 1) {


                  result.x = x;
               } else {

                  result.x = 1;
               }
            } else {
               result.x = 0;;
            }
         }
         return result;
      }

      template<int P>
      Fixed<P> fit(const Fixed<P>& rhs) {
         Fixed<P> result;
         result = *this;
         return result;
      }
   };


   template<int S, int P>
   Fixed < (S > P) ? S : P > operator+ (Fixed<S> lhs, const Fixed<P> & rhs) {
      Fixed < (S > P) ? S : P > l(lhs.x);
      Fixed < (S > P) ? S : P > r(rhs.x);
      return l.add(r);
   }

   template<int S, int P>
   Fixed < (S > P) ? S : P > operator- (Fixed<S> lhs, const Fixed<P> & rhs) {
      Fixed < (S > P) ? S : P > l(lhs.x), r(rhs.x);
      return l.substract(r);
   }

   template<int S, int P>
   Fixed < (S > P) ? S : P > operator* (Fixed<S> lhs, const Fixed<P> & rhs) {
      Fixed < (S > P) ? S : P > l(lhs.x), r(rhs.x);
      return l.multiply(r);
   }

   template<int S, int P>
   Fixed < (S > P) ? S : P > operator/ (Fixed<S> lhs, const Fixed<P> & rhs) {
      Fixed < (S > P) ? S : P > l(lhs.x), r(rhs.x);
      return l.divide(r);
   }

   template<int S, int P>
   Fixed < (S > P) ? S : P > operator% (Fixed<S> lhs, const Fixed<P> & rhs) {
      Fixed < (S > P) ? S : P > l(lhs.x), r(rhs.x);
      return l.modulo(r);
   }

   void assignIntToFixedViaVoidPointer(
      void* voidPointerToFixed, size_t lengthOfFixed, int valueToAssign);
}
# undef NOSTACKCHECK
namespace pearlrt {


   class Dation: public Device {
   public:

      enum DationParams {

         IN = 0x0001,

         OUT = 0x0002,

         INOUT = 0x0004,

         IDF = 0x0008,

         ANY = 0x0010,

         NEW = 0x0020,

         OLD = 0x0040,

         PRM = 0x0080,

         CAN = 0x0100,

         DIRECT = 0x0200,

         FORWARD = 0x0400,

         FORBACK = 0x0800,

         RST = 0x1000,

         CYCLIC = 0x2000,

         NOCYCL = 0x4000,

         STREAM = 0x08000,

         NOSTREAM = 0x10000
      };

      static const int DIRECTIONMASK = IN | OUT | INOUT;

      static const int OPENMASK = IDF | ANY | OLD | NEW | CAN | PRM;

      static const int CLOSEMASK = PRM | CAN;

      static const int POSITIONINGMASK = DIRECT | FORWARD | FORBACK;

      static const int DIMMASK = STREAM | CYCLIC | NOSTREAM | NOCYCL;

      enum DationStatus {

         OPENED = 1,

         CLOSED = 2
      };

      int dationParams;

      DationStatus dationStatus;

      virtual void dationRead(void * destination, size_t size) = 0;

      virtual void dationWrite(void * destination, size_t size) = 0;
   };

}

namespace pearlrt {


   class SystemDation: public Dation {
   public:

      virtual int capabilities() = 0;

      virtual void dationClose(int closeParam) = 0;

      virtual bool allowMultipleIORequests();
   };
}

namespace pearlrt {


   class SystemDationNB: public SystemDation {
   public:

      virtual SystemDationNB* dationOpen(const char * idfValue,
                                         int openParam) = 0;

      virtual void dationUnGetChar(const char c) = 0;

      virtual void translateNewLine(bool doNewLineTranslation) = 0;

      virtual void dationSeek(const Fixed<31> & p, const int dationParam) ;

      virtual Fixed<31> dationEof();

      virtual void registerWaitingTask(void * task, int direction);
   };
}

#define CHARACTER_INCLUDED



namespace pearlrt {

   int characterCompare(const char * s1, int l1, const char* s2, int l2);

   void characterFillSpaces(char * dest, int len);

   void characterSafeCopy(char*dest, const char* source, int len) ;

   template<size_t length>
   class Character {
   public:

      char data[length];
   public:

      Character<length>() {
         size_t i;
         if (length > 32767 || length < 1) {
            printf("Character: illegal length (%d)\n", (int)length);
            throw theCharacterTooLongSignal;
         }
         for (i = 0; i < length; i++) {
            data[i] = ' ';
         }
      }

      Character(const char * string) {
         unsigned int i;
         unsigned int l = strlen(string);
         if (length > 32767 || l > length) {
            printf("Character: illegal length (%d)\n", (int)length);
            throw theCharacterTooLongSignal;
         }
         for (i = 0; i < l; i++) {
            data[i] = string[i];
         }
         for (i = l; i < length; i++) {
            data[i] = ' ';
         }
      }

      Character(int len, const char * string) {
         unsigned int i;
         unsigned int l = len;
         if (length > 32767 || l > length) {

            throw theCharacterTooLongSignal;
         }
         for (i = 0; i < l; i++) {
            data[i] = string[i];
         }
         for (i = l; i < length; i++) {
            data[i] = ' ';
         }
      }

      template<size_t T> Character(Character<T> &string) {

         if (length < T) {

            throw theCharacterTooLongSignal;
         }
         characterSafeCopy(data, string.data, T);
         characterFillSpaces(data + T, length - T);
      }

      Fixed<15> upb() const {
         return Fixed<15>(length);
      }

      Character<1>  getCharAt(Fixed<15> p) {
         if ((size_t)p.x > length || p.x < 1) {

            throw theCharacterIndexOutOfRangeSignal;
         }
         Character<1> c;
         c.data[0] = data[p.x - 1];
         return c;
      }

      void setCharAt(Fixed<15> p, Character<1> c) {
         if ((size_t)p.x > length || p.x < 1) {

            throw theCharacterIndexOutOfRangeSignal;
         }
         data[p.x - 1] = c.data[0];
      }

      Fixed<8> toFixed() {
         if (length > 1) {

            throw theCharacterTooLongSignal;
         }
         return (Fixed<8>)data[0];
      }

      char *  get() {
         return data;
      }

      template<size_t LRHS>
      Character<length>&  operator=(const Character<LRHS> & rhs) {
         size_t i;
         if (length < LRHS) {
            printf("internal error: assign to smaller string\n");
            throw theInternalSignal;
         }


         if ((void*)this != (void*)&rhs) {
            characterSafeCopy(data, rhs.data, LRHS);
            characterFillSpaces(data + LRHS, length - LRHS);
         }
         return *this;
      }

      template<size_t LRHS>
      Character < length + LRHS > catChar(Character<LRHS> & rhs) {
         Character < length + LRHS > result;
         characterSafeCopy(result.data, data, length);
         characterSafeCopy(result.data + length, rhs.data, LRHS);
         return result;
      }
   };




   Character<1>  toChar(int x);
#if 0

   template<size_t LLHS, size_t LRHS>
   int compareChar(Character<LLHS>& lhs, Character<RLHS> & rhs) {
      return BitString<1> (characterCompare(lhs.data, LLHS, rhs.data, LRHS));
      size_t i;
      size_t len = (size_t)(lhs.upb().x);
      if (len > (size_t)(rhs.upb().x)) {
         len = (size_t)(rhs.upb().x);
      }
      for (i = 0; i < len && (lhs.data[i] == rhs.data[i]); i++) {
         ;
      }
      if (i < len) {
         return lhs.data[i] - rhs.data[i];
      }
      if (len < (size_t)(lhs.upb().x)) {
         for (i = len; i < (size_t)(lhs.upb().x) && lhs.data[i] == ' '; i++) {
            ;
         }
         if (i == (size_t)(lhs.upb().x)) {
            return (0);
         } else {
            return lhs.data[i] - ' ';
         }
      }
      if (len < (size_t)(rhs.upb().x)) {
         for (i = len; i < (size_t)(rhs.upb().x) && rhs.data[i] == ' '; i++) {
            ;
         }
         if (i == (size_t)(rhs.upb().x)) {
            return (0);
         } else {
            return ' ' - rhs.data[i];
         }
      }
      return 0;
   }
#endif
}

#define REFCHAR_INCLUDED



namespace pearlrt {



   class RefCharacter {
   public:
      size_t max;
      size_t current;
      char * data;


      RefCharacter();

      template<size_t S>
      RefCharacter(Character<S> & rhs) {
         max = rhs.upb().x;
         current = 0;
         data = &rhs.data[0];
      }

      template<size_t S>
      void setWork(Character<S> & rhs) {
         max = rhs.upb().x;
         current = 0;
         data = &rhs.data[0];
      }
      void setWork(void * s, size_t len);

      char getCharAt(size_t index) ;

      char getNextChar();

      char * getCstring() ;

      void clear();

      void fill();

      void add(const RefCharacter & rhs);

      void add(const char * rhs);

      size_t getMax();

      size_t getCurrent();

      void setCurrent(size_t newpos);

      void add(const char rhs);

      template<size_t S>
      void add(const Character<S> & rhs) {
         if ((size_t)(rhs.upb().x) > (max - current)) {
            throw theCharacterTooLongSignal;
         }
         for (size_t i = 0; i < (size_t)(rhs.upb().x); i++) {
            data[current + i] = rhs.data[i];
         }
         current += rhs.upb().x;
      }

      template<size_t S>
      void store(Character<S> & dest) {
         size_t len;
         len = current;
         if (len > (size_t)(dest.upb().x)) {
            throw theCharacterTooLongSignal;
         }
         for (size_t i = 0; i < len; i++) {
            dest.data[i] = data[i];
         }
         if (len < (size_t)(dest.upb().x)) {
            for (size_t i = len; i < (size_t)(dest.upb().x); i++) {
               dest.data[i] = ' ';
            }
         }
      }
   };

}
namespace pearlrt {
#define ERRORMESSAGE "\n                     **** above line truncated ****\n"

   class Log {
   private:
      Log();
      static int logFileHandle;
      static bool initialized;
      static Log* instance;
      static bool ctorIsActive;

      static int logLevel;
      static SystemDationNB * provider;

      void doit(const Character<7>& type, const char * format,
                       va_list args);
   public:

      static void doFormat(const Character<7>& type,
                           RefCharacter &rc,
                           const char * format,
                           va_list args);

      enum LogLevel {DEBUG = 1, INFO = 2, WARN = 4, ERROR = 8};

      static void setLevel(int level);

      Log(SystemDationNB * _provider, char* level);

      static Log* getInstance();

      static void info(const char * format, ...)
      __attribute__((format(printf, 1, 2)));

      static void error(const char * format, ...)
      __attribute__((format(printf, 1, 2)));

      static void warn(const char * format, ...)
      __attribute__((format(printf, 1, 2)));

      static void debug(const char * format, ...)
      __attribute__((format(printf, 1, 2)));

      static void exit();
   };
}
namespace pearlrt {


   template<class C> class Ref {
   private:
      C * x;
   public:

      Ref() {
         x = NULL;
      }

      Ref(C* pValue) : x(pValue) {}

      C& operator*() {
         if (x) {
            return *x;
         }
         Log::error("Ref::use of uninitialized reference");
         throw theRefNotInitializedSignal;
      }
   };

}

#define TASK_INCLUDED



#define BITSTRING_H_INCLUDED
template<int S> class BitString;







/**
\file stackcheck.h
*/
#ifndef NOSTACKCHECK
#if (TARGET==2 && CONFIG_LPC1768_CHECK_STACK_OVERFLOW==1)
/**
Stack checking is useful but it costs lot of execution time for
templated functions.
There is an option to enable/disbale stack checking on the
microcontroller part of the OpenPEARL system.
On normal linux systems this option is not supported by gcc.
*/
#define NOSTACKCHECK __attribute__((no_instrument_function))
#else
/**
\def NOSTACKCHECK
Stack checking is useful but it costs lot of execution time for
templated functions.
There is an option to enable/disbale stack checking on the
microcontroller part of the OpenPEARL system.
On normal linux systems this option is not supoorted by gcc.
*/
#define NOSTACKCHECK /* nothing */
#endif
#endif
namespace pearlrt {


   template<int S> class Bits;

   template<> class Bits<1> {
   public:

      typedef uint8_t BitType;

      typedef int8_t SignedBitType;
   };

   template<> class Bits<2> {
   public:

      typedef uint16_t BitType;

      typedef int16_t  SignedBitType;
   };

   template<> class Bits<4> {
   public:

      typedef uint32_t BitType;

      typedef int32_t  SignedBitType;
   };

   template<> class Bits<8> {
   public:

      typedef uint64_t BitType;

      typedef int64_t  SignedBitType;
   };



   template<int S> class BitString {
   private:

      static const int len = NumberOfBytes<S>::resultBitString;
   public:

      typedef typename Bits<len>::BitType DataType;

      typedef typename Bits<len>::SignedBitType SignedDataType;

      DataType x;
   private:
      static const int shiftSize = sizeof(x) * 8 - S;
      static const DataType mask = ((DataType)(-1) >> shiftSize)
                                   << shiftSize;
   public:

      BitString() NOSTACKCHECK {
         x = 0;
      }

      BitString(DataType y) NOSTACKCHECK {
         x = y;
         x <<= shiftSize;
         x &= mask;
      }

      BitString(Fixed<S-1> y) NOSTACKCHECK {
         x = y.x;
         x <<= shiftSize;
         x &= mask;
      }

      template <int P> BitString(BitString<P> y) {
         *this = y;
      }

      BitString<S> bitShift(const Fixed<15> l) {
         BitString<S> retval;
         retval.x = x;
         if (l.x > 0) {
            retval.x <<= l.x;
         } else {
            retval.x >>= -l.x;
         }
         retval.x &= mask;
         return (retval);
      }

      BitString<S> bitCshift(const Fixed<15> l) {
         BitString<S> retval;
         retval.x = x;
         Fixed<15> effectiveShift;



         effectiveShift = l.modulo(S);
         if (effectiveShift.x > 0) {
            retval.x <<= effectiveShift.x;
            retval.x |= (x >> (S - effectiveShift.x));
         } else {
            retval.x >>= -effectiveShift.x;
            retval.x |= x << (S + effectiveShift.x);
         }
         retval.x &= mask;
         return (retval);
      }

      BitString<S> bitNot() {
         BitString z;
         z.x = ~ x;
         z.x &= mask;
         return (z);
      }

      template<int P> BitString < S + P >
      bitCat(BitString<P> y) {
         BitString < S + P > z,z1;

         z.x = x << ((sizeof(z) - sizeof(x)) * 8);

         if (sizeof(z) > sizeof(y)) {
            z1.x = y.x << ((sizeof(z) - sizeof(y)) * 8 - S);
         } else {
            z1.x = y.x >> S;
         }

         z.x |= z1.x;
         return z;
      }

      template <int P> BitString<P>
      getSlice(const Fixed<15>start, BitString<P>* dummy) {
         if (start.x - 1 + P > S || start.x < 1) {
            throw theBitIndexOutOfRangeSignal;
         }
         DataType s;
         s = x >> (sizeof(s) * 8 - (start.x - 1 + P));
         BitString<P> returnValue(s);
         return returnValue;
      }

      template <int P> void
      setSlice(const Fixed<15>start, const BitString<P> slice) {
         if (start.x - 1 + P >= S || start.x < 1) {
            throw theBitIndexOutOfRangeSignal;
         }
         DataType s, m;
         static const int lengthAdjust = (sizeof(s) - sizeof(slice)) * 8;
         s = slice.x;
         s <<= lengthAdjust;
         s >>= start.x - 1;







         m = mask;
         m <<= start.x - 1;
         m >>= start.x - 1;
         m >>= (sizeof(m) * 8 - P - start.x + 1 + lengthAdjust);
         m <<= (sizeof(m) * 8 - P - start.x + 1 + lengthAdjust);
         x &= ~m;
         x |= s;
         return;
      }

      BitString<1>
      getBit(const Fixed<15>start) const {
         if (start.x > S || start.x < 1) {
            throw theBitIndexOutOfRangeSignal;
         }
         DataType s;
         s = x >> (sizeof(s) * 8 - start.x);
         BitString<1> returnValue(s);
         return returnValue;
      }

      void setBit(const Fixed<15>start, const BitString<1>newValue) {
         if (start.x > S || start.x < 1) {
            throw theBitIndexOutOfRangeSignal;
         }
         DataType s;
         s = 1 << (sizeof(s) * 8 - start.x);
         if (newValue.x) {
            x |= s;
         } else {
            x &= ~s;
         }
         return;
      }

      Fixed<S-1> toFixed() const {
         Fixed<S-1> returnValue;
         returnValue.x = ((SignedDataType)x) >> shiftSize;
         return returnValue;
      }

      bool inline getBoolean() {
         return (!!x);
      }
   private:







      template<int P> struct THENAND {
         static BitString < (S < P) ? P : S >
         bitAnd(BitString<S>x, BitString<P> y) {
            BitString < (S < P) ? P : S > z;
            z.x = x.x;
            z.x <<= (sizeof(y) - sizeof(x)) * 8;
            z.x &= y.x;
            return z;
         }
         static BitString < (S < P) ? P : S >
         bitOr(BitString<S> x, BitString<P> y) {
            BitString < (S < P) ? P : S > z;
            z.x = x.x;
            z.x <<= (sizeof(y) - sizeof(x)) * 8;
            z.x |= y.x;
            return z;
         }
         static BitString < (S < P) ? P : S >
         bitXor(BitString<S> x, BitString<P> y) {
            BitString < (S < P) ? P : S > z;
            z.x = x.x;
            z.x <<= (sizeof(y) - sizeof(x)) * 8;
            z.x ^= y.x;
            return z;
         }
         static void
         bitAssign(BitString<S>* x, BitString<P> y) {
            Log::error("assignment only allowed to larger or equal length "
                       " Bit(%d) := Bit(%d)", S, P);
            throw theInternalDatatypeSignal;
         }
         static BitString<1>
         bitCompare(const BitString<S> x, const BitString<P> y) {
            BitString < (S < P) ? P : S > z;
            z.x = x.x;
            z.x <<= (sizeof(y) - sizeof(x)) * 8;
            if (z.x == y.x) {
               BitString<1> result(1);
               return result;
            } else {
               BitString<1> result(0);
               return result;
            }
         }
      };

      template<int P> struct ELSEAND {
         static BitString < (S < P) ? P : S >
         bitAnd(BitString<S> x, BitString<P> y) {
            BitString < (S < P) ? P : S > z;
            z.x = y.x;
            z.x <<= (sizeof(x) - sizeof(y)) * 8;
            z.x &= x.x;
            return z;
         }
         static BitString < (S < P) ? P : S >
         bitOr(BitString<S> x, BitString<P> y) {
            BitString < (S < P) ? P : S > z;
            z.x = y.x;
            z.x <<= (sizeof(x) - sizeof(y)) * 8;
            z.x |= x.x;
            return z;
         }
         static BitString < (S < P) ? P : S >
         bitXor(BitString<S> x, BitString<P> y) {
            BitString < (S < P) ? P : S > z;
            z.x = y.x;
            z.x <<= (sizeof(x) - sizeof(y)) * 8;
            z.x ^= x.x;
            return z;
         }
         static void
         bitAssign(BitString<S>* x, BitString<P> y) {
            x->x = y.x;
            x->x <<= (sizeof(*x) - sizeof(y)) * 8;
            return;
         }
         static BitString<1>
         bitCompare(const BitString<S> x, const BitString<P> y) {
            BitString < (S < P) ? P : S > z;
            z.x = y.x;
            z.x <<= (sizeof(x) - sizeof(y)) * 8;
            if (z.x == x.x) {
               BitString<1> result(1);
               return result;
            } else {
               BitString<1> result(0);
               return result;
            }
         }
      };
   public:

      template< int P>
      BitString <S> operator= (const BitString<P> y) {
         IF < (S < P), THENAND<P>, ELSEAND<P> >::
         SELECT_CLASS::bitAssign(this, y);
         return *this;
      }

      template<int P>
      BitString < (S < P) ? P : S > bitAnd(BitString<P> y) {
         return IF < (S < P), THENAND<P>, ELSEAND<P> >::
                SELECT_CLASS::bitAnd(*this, y);
      }

      template<int P>
      BitString < (S < P) ? P : S > bitOr(BitString<P> y) {
         return IF < (S < P), THENAND<P>, ELSEAND<P> >::
                SELECT_CLASS::bitOr(*this, y);
      }

      template< int P>
      BitString < (S < P) ? P : S > bitXor(BitString<P> y) {
         return IF < (S < P), THENAND<P>, ELSEAND<P> >::
                SELECT_CLASS::bitXor(*this, y);
      }

      template<int P>
      BitString<1> operator== (const BitString<P> y) const {
         return IF < (S < P), THENAND<P>, ELSEAND<P> >::
                SELECT_CLASS::bitCompare(*this, y);
      }

      template<int P>
      BitString<1> operator!= (const BitString<P> y) const {
         return (IF < (S < P), THENAND<P>, ELSEAND<P> >::
                    SELECT_CLASS::bitCompare(*this, y)).bitNot();
      }
   };

}
# undef NOSTACKCHECK

#define PRIOMAPPER_INCLUDED
namespace pearlrt {

   class PrioMapper {
   private:
      PrioMapper();
      int min;
      int max;
      static PrioMapper* instance;
   public:

      static PrioMapper* getInstance();

      int fromPearl(const Fixed<15> p);

      int getSystemPrio();

      void logPriorities();
   };
}

#define PRIO_INCLUDED
namespace pearlrt {


   class Prio {
   private:
      Fixed<15> prio;
   public:

      Prio();

      Prio(const Fixed<15>& p);

      Fixed<15> get() const;
   };
}

#define CSEMA_INCLUDED


#define CSEMACOMMON_INCLUDED
namespace pearlrt {

   class CSemaCommon {
   protected:

      const char * id;
   public:

      CSemaCommon();

      void name(const char * s);

      virtual void request() = 0;

      virtual void release() = 0;
   };
}
namespace pearlrt {

   class CSema : public CSemaCommon {
   private:
      sem_t sem;
      const char * id;
   public:

      CSema(int preset = 0);

      void name(const char * s);

      ~CSema();

      void request();

      void release();
   };
}

#define CLOCK_H_INCLUDED



namespace pearlrt {
   class Clock;
}

#define DURATION_H_INCLUDED


#define FLOAT_H_INCLUDED


#define FLOATHELPER_H_INCLUDED


namespace pearlrt {

   class FloatHelper {
   public:

      static void testFloatResult(double x);
   };
}


#ifndef NAN
#error "Float.h needs IEEE754 support"
#endif
namespace pearlrt {

   template < int T > struct InternalFloatType;

   template < > struct InternalFloatType<24> {

      typedef float InternalType;
   };

   template < > struct InternalFloatType<53> {

      typedef double InternalType;
   };

   template<int S> class Float {
   public:

      template<int fixedSize, int floatSize> struct FloatResult;

      template<int fixedSize > struct FloatResult<fixedSize, 24> {
         typedef Float < fixedSize <= 24 ? 24 : 53 > ResultType;
      };

      template<int fixedSize> struct FloatResult<fixedSize, 53> {

         typedef  Float<53>  ResultType;
      };

      typedef typename InternalFloatType<S>::InternalType InternalType1;

      InternalType1 x;

      Float<S>() {
         x = NAN;
      }

      Float<S>(double xx) {
         x = xx;
         FloatHelper::testFloatResult(x);
      }

      Float<S>(Fixed<31> xx) {
         x = xx.x;
      }

      template<int T> Float < S < T ? 0 : S > operator=(const Float<T> &rhs) {
         x = rhs.x;
         FloatHelper::testFloatResult(x);
         return *this;
      }

      template<int T>Float < S < T ? T : S >
      operator+(const Float<T> & rhs) const {
         Float < S < T ? T : S > result;
         result.x = x + rhs.x;
         FloatHelper::testFloatResult(result.x);
         return result;
      }

      template<int T, template<int> class R>
      typename FloatResult<T, S>::ResultType
      operator+(const R<T> & rhs) const {
         typename FloatResult<T, S>::ResultType result;
         result.x = x + rhs.x;
         FloatHelper::testFloatResult(result.x);
         return result;
      }

      Float<S> operator-() const {
         Float<S> result;
         result.x = -x;
         FloatHelper::testFloatResult(result.x);
         return result;
      }

      template<int T, template<int> class R>
      typename FloatResult<T, S>::ResultType
      operator-(const R<T> & rhs) const {
         typename FloatResult<T, S>::ResultType result;
         result.x = x - rhs.x;
         FloatHelper::testFloatResult(result.x);
         return result;
      }

      template<int T, template<int> class R>
      typename FloatResult<T, S>::ResultType
      operator*(const R<T> & rhs) const {
         typename FloatResult<T, S>::ResultType result;
         result.x = x * rhs.x;
         FloatHelper::testFloatResult(result.x);
         return result;
      }

      template<int T, template<int> class R>
      typename FloatResult<T, S>::ResultType
      operator/(const R<T> & rhs) const {
         typename FloatResult<T, S>::ResultType result;
         result.x = x / rhs.x;
         FloatHelper::testFloatResult(result.x);
         return result;
      }

      template<int T> Float<T> fit(const Float<T> &rhs) const {
         Float<T> result;
         result.x = x;
         FloatHelper::testFloatResult(result.x);
         return result;
      }

      Float<S> abs() const {
         Float<S> result;
         result.x = std::fabs(x);
         FloatHelper::testFloatResult(result.x);
         return result;
      }

      Fixed<1> sign() const {
         Fixed<1> result(0);
         if (x < 0) {
            result.x = -1;
         } else if (x > 0) {
            result.x = 1;
         }
         return result;
      }

      Fixed<S> entier() const {
         Fixed<S> result;
         if (x >= 0) {
            result.x = x;
         } else {
            result.x = -x;
            if (result.x != -x) {
               result = result + Fixed<S>(1);
            }
            result = -result;
         }
         return result;
      }

      Fixed<S> round() const {
         Fixed<S> result;
         if (x < 0) {
            result.x = -x + 0.5;
            result.x = -result.x;
         } else {
            result.x = x + 0.5;
         }
         return result;
      }

      Float<S> sin() const {
         Float<S> result;
         result.x = std::sin(x);
         FloatHelper::testFloatResult(result.x);
         return result;
      }

      Float<S> cos() const {
         Float<S> result;
         result.x = std::cos(x);
         FloatHelper::testFloatResult(result.x);
         return result;
      }

      Float<S> tan() const {
         Float<S> result;
         result.x = std::tan(x);
         FloatHelper::testFloatResult(result.x);
         return result;
      }

      Float<S> atan() const {
         Float<S> result;
         result.x = std::atan(x);
         FloatHelper::testFloatResult(result.x);
         return result;
      }

      Float<S> tanh() const {
         Float<S> result;
         result.x = std::tanh(x);
         FloatHelper::testFloatResult(result.x);
         return result;
      }

      Float<S> exp() const {
         Float<S> result;
         result.x = std::exp(x);
         FloatHelper::testFloatResult(result.x);
         return result;
      }

      Float<S> ln() const {
         Float<S> result;
         result.x = std::log(x);
         FloatHelper::testFloatResult(result.x);
         return result;
      }

      Float<S> sqrt() const {
         Float<S> result;
         result.x = std::sqrt(x);
         FloatHelper::testFloatResult(result.x);
         return result;
      }

      template <int P>Float<S> pow(const Fixed<P>& rhs)const {
         Float<S> result(1);
         Float<S> base(x);
         Float<S> one(1);
         int64_t exp = rhs.x;
         if (exp == 0) {
            result.x = 1;
         } else {
            if (exp < 0) {
               exp = -exp;
               base = one / base;
            }

            while (exp) {
               if (exp & 1) {
                  result = result * base;
               }
               exp >>= 1;
               if (exp) {
                  base = base * base;
               }
            }
         }
         return result;
      }
   };

   template <int S, int T> Float<T>
   operator+(const Fixed<S> & lhs, const Float<T> & rhs) {
      Float<T> result;
      result.x = lhs.x + rhs.x;
      FloatHelper::testFloatResult(result.x);
      return result;
   }

   template <int S, int T> Float<T>
   operator-(const Fixed<S> & lhs, const Float<T> & rhs) {
      Float<T> result;
      result.x = lhs.x - rhs.x;
      FloatHelper::testFloatResult(result.x);
      return result;
   }

   template <int S, int T> Float<T>
   operator*(const Fixed<S> & lhs, const Float<T> & rhs) {
      Float<T> result;
      result.x = lhs.x * rhs.x;
      FloatHelper::testFloatResult(result.x);
      return result;
   }

   template <int S, int T> Float<T>
   operator/(const Fixed<S> & lhs, const Float<T> & rhs) {
      Float<T> result;
      result.x = lhs.x / rhs.x;
      FloatHelper::testFloatResult(result.x);
      return result;
   }
}

namespace pearlrt {
   class Duration;
}

namespace pearlrt {


   class Duration {
   private:
      Fixed63 intval;
   public:

      Duration();

      Duration(const double d);

      Fixed63 get() const;

      int getSec() const;

      int getUsec(void) const;

      Duration& operator+=(const Duration& rhs);

      const Duration operator+(const Duration& rhs) const;

      Duration& operator-=(const Duration& rhs);

      const Duration operator-(const Duration& rhs) const;

      const Duration operator-() const;

      template <int S>Duration& operator*=(const Fixed<S>& rhs) {
         intval = intval.get() * rhs.x;
         return *this;
      }

      template<int S>const Duration  operator*(const Fixed<S>& rhs) const {
         return Duration(*this) *= rhs;
      }

      template <int S>Duration& operator/=(const Fixed<S>& rhs) {
         if ((rhs == Fixed<S>(0.0)).getBoolean()) {
            throw theDurationDivideByZeroSignal;
         }
         Fixed63 help;
         try {
            help = intval.get() / rhs.x;
         } catch (FloatIsINFSignal &s) {
            throw theDurationRangeSignal;
         }
         intval = help;
         return (*this);
      }

      template <int S>Duration operator/(const Fixed<S>& rhs) {
         return Duration(*this) /= rhs;
      }

      template <int S>Duration& operator*=(const Float<S>& rhs) {
         intval = (double)(intval.get()) * rhs.x;
         return *this;
      }

      template<int S>const Duration  operator*(const Float<S>& rhs) const {
         return Duration(*this) *= rhs;
      }

      template <int S>Duration& operator/=(const Float<S>& rhs) {
         if ((rhs == Float<S>(0.0)).getBoolean()) {
            throw theDurationDivideByZeroSignal;
         }
         Float<S> help;
         try {
            help = Float<S>(intval.get()) / rhs;
         } catch (FloatIsINFSignal &s) {
            throw theDurationRangeSignal;
         }
         intval = Fixed63(help.x);
         return (*this);
      }

      template <int S>Duration operator/(const Float<S>& rhs) {
         return Duration(*this) /= rhs;
      }

      Float<24> operator/=(const Duration& rhs);

      const Float<24> operator/(const Duration& rhs) const;

      int compare(const Duration& rhs) const;

      BitString<1> operator==(const Duration& rhs) const;

      BitString<1> operator!=(const Duration& rhs) const;

      BitString<1> operator>(const Duration& rhs) const;

      BitString<1> operator<(const Duration& rhs) const;

      BitString<1> operator>=(const Duration& rhs) const;

      BitString<1> operator<=(const Duration& rhs) const;

      Duration abs() const;

      Fixed<1> sign() const;
   };

   template<int S>const Duration  operator*(const Fixed<S>& lhs,
         const Duration &rhs) {
      Duration result(rhs);
      result *= lhs;
      return result;
   }

   template<int S>const Duration  operator*(const Float<S>& lhs,
         const Duration &rhs) {
      Duration result(rhs);
      result *= lhs;
      return result;
   }
}
namespace pearlrt {


   class Clock {
   private:
      Fixed63 intval;

      void adjust();
   public:

      static Clock now(void);

      Clock();

      Clock(double x);

      int getSec(void) const;

      int getUsec(void) const;

      const Fixed63& get(void) const;

      Clock& operator+(const Duration& d);

      Clock& operator-(const Duration& d);

      Duration operator-(const Clock& c);

      int compare(const Clock& rhs) const;

      BitString<1> operator<(const Clock& rhs) const;

      BitString<1> operator<=(const Clock& rhs) const;

      BitString<1> operator==(const Clock& rhs) const;

      BitString<1> operator!=(const Clock& rhs) const;

      BitString<1> operator>=(const Clock& rhs) const;

      BitString<1> operator>(const Clock& rhs) const;
   };
}


#define INTERRUPT_INCLUDED

#define TASKWHENLINKS_INCLUDED
namespace pearlrt {

   class TaskWhenLinks {
   protected:

      TaskWhenLinks* nextActivate;

      TaskWhenLinks* nextContinue;
   public:

      TaskWhenLinks* getNextContinue();

      void setNextContinue(TaskWhenLinks * next);

      TaskWhenLinks* getNextActivate();

      void setNextActivate(TaskWhenLinks * next);

      virtual void triggeredContinue() = 0;

      virtual void triggeredActivate() = 0;
   };
}
namespace pearlrt {



   class Interrupt {
   private:
      TaskWhenLinks * headContinueTaskQueue;
      TaskWhenLinks * headActivateTaskQueue;
      bool         isEnabled;
   public:

      Interrupt();

      void enable();

      void disable();

      void trigger();

      void registerActivate(TaskWhenLinks* t, TaskWhenLinks **next);

      void registerContinue(TaskWhenLinks* t, TaskWhenLinks **next);

      void unregisterActivate(TaskWhenLinks* t);

      void unregisterContinue(TaskWhenLinks* t);
      protected:

      virtual void devEnable()=0;

      virtual void devDisable()=0;
   };

}



#define TASKCOMMON_INCLUDED
namespace pearlrt {
   class TaskCommon;
   class Semaphore;
   class Bolt;
   class SystemDation;

   enum BlockReason {
      NOTBLOCKED,
      REQUEST,
      ENTER,
      RESERVE,
      IO
   };

   struct BlockData {
      BlockReason reason;

      union BlockReasons {

         struct BlockSema {
            int nsemas;
            Semaphore **semas;
         } sema;

         struct BlockBolt {
            int nbolts;
            Bolt **bolts;
         } bolt;

         struct BlockIO {
            SystemDation* dation;
         } io;
      } u;
   };
}






#define MUTEX_INCLUDED


#define IMUTEXCOMMON_INCLUDED
namespace pearlrt {

   class MutexCommon {
   protected:

      const char * id;
   public:

      MutexCommon();

      void name(const char * s);

      const char * getName();


      virtual void lock() = 0;

      virtual void unlock() = 0;
   };
}
namespace pearlrt {

   class Mutex : public MutexCommon {
   private:
      pthread_mutex_t mutex;
   public:

      Mutex();

      ~Mutex();

      void lock();

      void unlock();
   };
}


namespace pearlrt {
   class UserDation;
}
#define USERDATION_INCLUDED


#define RST_INCLUDED


namespace pearlrt {


   class Rst {
   protected:

      Fixed<15> * rstValue;

      void * rstVoidPointer;

      size_t rstLength;
   public:

      void rst(Fixed<15> & rstVar);

      void rst(void * rstVoidPointer, size_t len);

      bool updateRst(Signal * s);
   };

}






namespace pearlrt {

   class UserDation : public Dation, public Rst {
   protected:

      Mutex mutex;
   protected:

      TaskCommon* currentTask;

      int currentDirection;

      SystemDation * systemDation;
   public:

      UserDation();
   private:

      virtual void internalDationOpen(int p,
                                      RefCharacter* rc) = 0;
   public:

      template <size_t S, int R>
      void dationOpen(int p,
                      Character<S> * idf,
                      Fixed<R> * rst) {
         try {
            if (p & RST) {
               if (! rst) {
                  Log::error("UserDation: RST is set but no"
                             " variable given");
                  throw theInternalDationSignal;
               }
               *rst = 0;
            }
            if ((!!(p & Dation::IDF)) != (idf != 0)) {
               Log::error("UserDation: ether both or non of IDF and filename");
               throw theInternalDationSignal;
            }
            if (S > 64) {
               Log::error("filename exceeds 64 characters");
               throw theDationParamSignal;
            }
            RefCharacter rc;
            Character<64>  fileName;
            rc.setWork(fileName);
            rc.clear();
            if (p & Dation::IDF) {
               rc.add(*idf);
            }
            internalDationOpen(p, &rc);
         } catch (Signal & s) {
            if (rst) {
               *rst = s.whichRST();
            } else {
               throw;
            }
         }
      }
   private:
      void internalDationClose(const int  p = 0);
   public:

      template<int S> void dationClose(const int  p, Fixed<S> * rst) {
         Fixed<S>* intRst = NULL;
         try {
            if (p & RST) {
               if (! rst) {
                  Log::error("UserDation: RST is set but no variable given");
                  throw theInternalDationSignal;
               }
               *rst = 0;
               intRst  = rst;
            }
            internalDationClose(p);
         } catch (Signal &  s) {
            if (intRst != NULL) {
               *intRst = (Fixed<31>)s.whichRST();
            } else {
               throw;
            }
         }
      }

      virtual void closeSystemDation(int dationParams) = 0;
   protected:

      void assertOpen();
   public:

      void beginSequence(TaskCommon * me,
                         Dation::DationParams dir = Dation::OUT);

      void endSequence();
   public:

      void suspend();

      void cont();
   protected:

      virtual void beginSequenceHook(TaskCommon* me) = 0;

      virtual void endSequenceHook(void) = 0;
   };
}


#define TASKTIMERCOMMONINCLUDED




namespace pearlrt {

   class TaskTimerCommon {
   public:

      typedef void (*TimerCallback)(TaskCommon*);
   public:

      virtual void set(
         int condition,
         Clock at,
         Duration after,
         Duration all,
         Clock until,
         Duration during) = 0;

      virtual int cancel() = 0;

      virtual int start() = 0;

      virtual bool isActive() = 0;

      virtual bool isSet() = 0;
   };
}

namespace pearlrt {

   class TaskCommon : public TaskWhenLinks {
   protected:

      UserDation * dation;

      void testScheduleCondition(int condition, Duration during, Duration all);
   private:
      static CSema mutexTasks;
   protected:

      struct Schedule {
         bool whenRegistered;
         Interrupt * when;
         Fixed<15> prio;
         TaskTimerCommon * taskTimer;
      };

      Schedule schedActivateData;

      Schedule  schedContinueData;

      bool schedActivateOverrun;
   public:

      enum TaskState {
         TERMINATED,
         RUNNING,
         SUSPENDED,
         SEMA_SUSPENDED_BLOCKED,
         SEMA_BLOCKED,
         IO_SUSPENDED_BLOCKED,
         IO_BLOCKED
      };

      enum TaskScheduling {
         AT = 1, AFTER = 2, WHEN = 4, ALL = 8,
         DURING = 16, UNTIL = 32, PRIO = 64
      };
   protected:
      Fixed<15> defaultPrio;

      Fixed<15> currentPrio;

      char * name;
      int isMain;
      enum TaskState taskState;

      int sourceLine;
      const char * fileName;

      struct BlockParams {
         CSema semaBlock;

         TaskCommon * next;

         BlockData why;
      } blockParams;

      TaskCommon(char * n, Prio prio, BitString<1> isMain);
   public:

      void suspend(TaskCommon* me);

      void cont(TaskCommon* me,
                int condition = 0,
                Prio prio = Prio(),
                Clock at = 0.0,
                Duration after = 0.0,
                Duration all = 0.0,
                Clock until = 0.0,
                Duration during = 0.0,
                Interrupt* when = 0);

      void activate(TaskCommon * me,
                    int condition = 0,
                    Prio prio = Prio(),
                    Clock at = 0.0, Duration after = 0.0,
                    Duration all = 0.0,
                    Clock until = 0.0,
                    Duration during = 0.0,
                    Interrupt * when = 0);

      void scheduledActivate(int condition,
                             Fixed<15>& prio,
                             Clock& at, Duration& after,
                             Duration& all, Clock& until,
                             Duration& during,
                             Interrupt* when);

      void terminate(TaskCommon * me);

      void resume(int condition,
                  Clock at = 0.0,
                  Duration after = 0.0,
                  Interrupt* when = 0);

      virtual void resume2() = 0;

      void prevent(TaskCommon * me);

      char * getName();

      int getIsMain();

      TaskState getTaskState();

      Fixed<15> getPrio();
   public:

      virtual void scheduleCallback(bool isLocked = false);

      void setLocation(int lineNumber, const char * fileName);

      const char* getLocationFile();

      int getLocationLine();

      void block(BlockData * why);

      void unblock();

      void getBlockingRequest(BlockData *why);

      TaskCommon* getNext();

      void setNext(TaskCommon*t);

      static void mutexLock();

      static void mutexUnlock();

      void enterIO(UserDation * d);

      void leaveIO();

      bool isMySelf(TaskCommon * me);

      static void activateHandler(TaskCommon * tsk);

      void timedActivate();

      static void continueHandler(TaskCommon * tsk);

      void timedContinue();

      void triggeredContinue();

      void triggeredActivate();


      virtual void directActivate(const Fixed<15>& prio) = 0;

      virtual void terminateMySelf() = 0;

      virtual void terminateFromOtherTask() = 0;

      virtual void suspendMySelf() = 0;

      virtual void suspendFromOtherTask() = 0;

      virtual void continueFromOtherTask(int condition,
                                         Prio prio) = 0;
   };
}

#define TASKTIMERINCLUDED






namespace pearlrt {

   class TaskTimer : public TaskTimerCommon {
   public:

      typedef void (*TimerCallback)(TaskCommon*);
   private:
      int counts;
      int countsBackup;
      TaskCommon* task;
      TimerCallback callback;
      int signalNumber;
      timer_t timer;
      struct itimerspec its;
   public:

      TaskTimer();

      void set(
         int condition,
         Clock at,
         Duration after,
         Duration all,
         Clock until,
         Duration during);

      int cancel();
   private:

      int stop();
   public:

      int start();

      bool isActive();

      bool isSet();

      static void init(int p);

      void create(TaskCommon * task, int signalNumber, TimerCallback cb);

      void update();

      void detailedStatus(char * id, char * line);
   };
}
namespace pearlrt {

   class Task : public TaskCommon {
   private:
      static int useNormalSchedulerFlag;
      static int schedPrioMax;
      Task();
   public:

      typedef void (*TaskEntry)(Task *);
      void scheduleCallback(bool isLocked = false);
   private:
      void (*entryPoint)(Task * me);

      CSema suspendDone;
      int suspendWaiters;

      CSema continueDone;
      int continueWaiters;

      CSema terminateDone;
      int terminateWaiters;

      CSema activateDone;

      int pipeResume[2];

      volatile bool asyncTerminateRequested;
      volatile bool asyncSuspendRequested;
      pthread_t threadPid;
      pthread_attr_t attr;
      struct sched_param param;
      TaskTimer activateTaskTimer;
      TaskTimer continueTaskTimer;

      void setNormalPriority();

      void setOSPriority();
   public:

      void resume2();

      void setSuspendRequested();
   private:

      void suspendMySelf();

      void internalSuspendMySelf(bool releaseJobDone);

      void terminateMySelf();

      void internalTerminateMySelf(bool releaseJobDone);

      void terminateFromOtherTask();
   public:

      Task(void (*entry)(Task*), char * n, Prio prio,
           BitString<1> isMain);
   private:

      void suspendFromOtherTask();

      void changeThreadPrio(const Fixed<15>& prio);

      void continueFromOtherTask(int condition,
                                 Prio prio);

      void continueSuspended();
   public:

      void entry();
   private:

      void directActivate(const Fixed<15>& prio);
   public:

      pthread_t getPid();

      TaskEntry getEntry();

      static void useNormalScheduler();

      static void setSchedPrioMax(int p);

      void switchToSchedPrioMax();

      void switchToSchedPrioCurrent();
   private:

      void switchThreadSchedPrio(int p);
   public:

      int detailedTaskState(char * lines[3]);
   };
}
#define SPCTASK(t) \
extern pearlrt::Task t;
#if 0
#define SPCTASKGLOBAL(t,ns) \
extern pearlrt::Task #ns::t;
#endif
#define DCLTASK(x, prio, ismain)  	\
namespace pearlrt {  	\
   static void x ## _entry (pearlrt::Task * me) ; \
   static void x ## _body (pearlrt::Task * me) ; \
}   	\
pearlrt::Task x ( pearlrt::x ## _entry, (char*)#x, \
                       prio, ismain);	\
namespace pearlrt {  	\
static void x ## _entry (pearlrt::Task * me) {  \
      me->entry();    	\
      try {   \
         x ## _body (me);	  	\
      } catch (pearlrt::Signal & p) { 	\
         char line[256];                                \
         printf("++++++++++++++++++++++++++++++\n");	\
         sprintf(line,"%s:%d Task: %s   terminated due to: %s",\
             me->getLocationFile(), me->getLocationLine(), \
             me->getName(), p.which()); 	\
         printf("%s\n",line);  \
         pearlrt::Log::error(line); 	\
         printf("++++++++++++++++++++++++++++++\n");	\
      }   	\
      me->terminate(me);    \
   }    	\
}   	\
   	\
static void pearlrt::x ## _body (pearlrt::Task * me)
#if 0
#define DCLTASKGLOBAL(x, prio, ismain,ns)  \
}   	\
namespace pearlrt {  	\
   static void x ## _entry (pearlrt::Task * me) ;	\
   static void x ## _body (pearlrt::Task * me) ;	\
}   	\
namespace #x {   \
pearlrt::Task x ( pearlrt::x ## _entry, (char*)#x, 	\
                       prio, ismain); 	\
}   	\
namespace pearlrt {  	\
static void x ## _entry (pearlrt::Task * me) {  \
      me->entry();    	\
      try {   \
         x ## _body (me);	  	\
      } catch (pearlrt::Signal & p) { 	\
         char line[256];                                \
         printf("++++++++++++++++++++++++++++++\n");	\
         sprintf(line,"%s:%d Task: %s   terminated due to: %s",\
             me->getLocationFile(), me->getLocationLine(), \
             me->getName(), p.which()); 	\
         printf("%s\n",line);  \
         pearlrt::Log::error(line); 	\
         printf("++++++++++++++++++++++++++++++\n");	\
      }   	\
      me->terminate(me);    \
   }    	\
}   	\
   	\
namespace #x {   \
static void pearlrt::x ## _body (pearlrt::Task * me)
#endif




#define TASKMONITOR_INCLUDED

namespace pearlrt {

   class TaskMonitor {
   private:
      volatile int nbrPendingTasks;
      Mutex mutex;
      TaskMonitor();
      TaskMonitor(TaskMonitor const&);
      TaskMonitor& operator= (TaskMonitor const&);
   public:

      static TaskMonitor& Instance();

      void incPendingTasks();

      void decPendingTasks();

      int getPendingTasks();
   };
}

#define ARRAY_H_INCLUDED





#define LIMITS(...) __VA_ARGS__
#define DCLARRAY(name,dimensions,limits) \
   pearlrt::ArrayDescriptor<dimensions> a_##name = { dimensions, limits }; \
   pearlrt::Array b_##name((pearlrt::ArrayDescriptor<0>*)&a_##name); \
   pearlrt::Array * name = &(b_##name);
namespace pearlrt {

   template <int DIM>
   struct ArrayDescriptor {

      int dim;

      struct Limits {

         int low;

         int high;

         int size;
      } lim[DIM];
   };

   class Array {
   private:
      ArrayDescriptor<0>* descriptor;
      Array() {};
   public:

      Array(ArrayDescriptor<0> * descr) : descriptor(descr) {}

      size_t offset(Fixed<31> i, ...);

      Fixed<31> upb(Fixed<31> x);

      Fixed<31> lwb(Fixed<31> x);
   };
}


#define PUTCLOCK_H_INCLUDED


#define SINK_H_INCLUDED
namespace pearlrt {


   class Sink {
   public:

      virtual void putChar(char c)  = 0;
   };
}

namespace pearlrt {


   class PutClock {
   public:

      static void toT(const Clock & x,
                      const Fixed<31>& w, const Fixed<31>& d,
                      Sink& sink) ;
   };

}

#define GETCLOCK_H_INCLUDED




#define SOURCE_H_INCLUDED
namespace pearlrt {


   class Source {
   private:
      char unGetCharacter;
      bool unGetCharacterAvailable;
   public:

      Source();

      char getChar(void);

      virtual char realGetChar(void) = 0;

      void unGetChar(char c);

      void forgetUnGetChar(void);
   };
}



namespace pearlrt {


   class GetClock {
   public:

      static int fromT(Clock&c,
                       const Fixed<31> w,
                       const Fixed<31> d,
                       Source & source);
   };

}


#define PUTDURATION_H_INCLUDED



namespace pearlrt {



   class PutDuration {
   public:

      static void toD(const Duration& dur,
                      const Fixed<31>& w,
                      const Fixed<31>& d,
                      Sink& s);
   };

}

#define GETDURATION_H_INCLUDED





namespace pearlrt {


   class GetDuration {
   public:

      static int fromD(Duration&c,
                       const Fixed<31> w,
                       const Fixed<31> d,
                       Source & source);
   };

}



#define PUTFIXED_H_INCLUDED





#define COMPARE_H_INCLUDED





#define CHARSLICE_INCLUDED




namespace pearlrt {

   class CharSlice {
   public:

      char *data;

      Fixed<15> length;

      template<size_t S>CharSlice(Character<S>& source) {
         length = Fixed<15>(S);
         data   = source.get();
      }

      template<size_t S>
      Character<S>* mkCharPtr(Character<S>* dummy) {
         if ((int)S > length.x) {
            printf("mkCharacter: illegal length %d <-> %d", (int)S, length.x);
            throw theCharacterTooLongSignal;
         }
         return (Character<S>*) data;
      }

      CharSlice& getSlice(Fixed<15> start, Fixed<15> end);

      void setSlice(const CharSlice& rhs);

      Fixed<15> upb() const;

      Character<1>  getCharAt(Fixed<15> p);

      void setCharAt(Fixed<15> p, Character<1> c);

      Fixed<8> toFixed();

      char *  get();
   };
};
namespace pearlrt {




   template<int S, int P>
   BitString<1> operator== (const Fixed<S> lhs, const Fixed<P> & rhs)  {
      Fixed < (S > P) ? S : P > l(lhs.x), r(rhs.x);
      return BitString<1>(lhs.x == rhs.x);
   }

   template<int S, int P>
   BitString<1> operator!= (const Fixed<S> lhs, const Fixed<P> & rhs)  {
      Fixed < (S > P) ? S : P > l(lhs.x), r(rhs.x);
      return BitString<1>(lhs.x != rhs.x);
   }

   template<int S, int P>
   BitString<1> operator< (const Fixed<S> lhs, const Fixed<P> & rhs)  {
      Fixed < (S > P) ? S : P > l(lhs.x), r(rhs.x);
      return BitString<1>(lhs.x < rhs.x);
   }

   template<int S, int P>
   BitString<1> operator<= (const Fixed<S> lhs, const Fixed<P> & rhs)  {
      Fixed < (S > P) ? S : P > l(lhs.x), r(rhs.x);
      return BitString<1>(lhs.x <= rhs.x);
   }

   template<int S, int P>
   BitString<1> operator> (const Fixed<S> lhs, const Fixed<P> & rhs)  {
      Fixed < (S > P) ? S : P > l(lhs.x), r(rhs.x);
      return BitString<1>(lhs.x > rhs.x);
   }

   template<int S, int P>
   BitString<1> operator>= (const Fixed<S> lhs, const Fixed<P> & rhs)  {
      Fixed < (S > P) ? S : P > l(lhs.x), r(rhs.x);
      return BitString<1>(lhs.x >= rhs.x);
   }




   template <int S, int T, template<int> class R>
   BitString<1> operator<(const Float<S> & lhs, const R<T> & rhs) {
      return lhs.x < rhs.x;
   }

   template <int S, int T, template<int> class R>
   BitString<1> operator<=(const Float<S> & lhs, const R<T> & rhs) {
      return lhs.x <= rhs.x;
   }

   template <int S, int T, template<int> class R>
   BitString<1> operator!=(const Float<S> & lhs, const R<T> & rhs) {
      return lhs.x != rhs.x;
   }

   template <int S, int T, template<int> class R>
   BitString<1> operator==(const Float<S> & lhs, const R<T> & rhs) {
      return lhs.x == rhs.x;
   }

   template <int S, int T, template<int> class R>
   BitString<1> operator>=(const Float<S> & lhs, const R<T> & rhs) {
      return lhs.x >= rhs.x;
   }

   template <int S, int T, template<int> class R>
   BitString<1> operator>(const Float<S> & lhs, const R<T> & rhs) {
      return lhs.x > rhs.x;
   }

   template <int S, int T>
   BitString<1> operator<(const Fixed<S> & lhs, const Float<T> & rhs) {
      return lhs.x < rhs.x;
   }

   template <int S, int T>
   BitString<1> operator<=(const Fixed<S> & lhs, const Float<T> & rhs) {
      return lhs.x <= rhs.x;
   }

   template <int S, int T>
   BitString<1> operator!=(const Fixed<S> & lhs, const Float<T> & rhs) {
      return lhs.x != rhs.x;
   }

   template <int S, int T>
   BitString<1> operator==(const Fixed<S> & lhs, const Float<T> & rhs) {
      return lhs.x == rhs.x;
   }

   template <int S, int T>
   BitString<1> operator>=(const Fixed<S> & lhs, const Float<T> & rhs) {
      return lhs.x >= rhs.x;
   }

   template <int S, int T>
   BitString<1> operator>(const Fixed<S> & lhs, const Float<T> & rhs) {
      return lhs.x > rhs.x;
   }




   template<size_t S, size_t P>
   BitString<1> operator== (const Character<S> & lhs,
                            const Character<P> & rhs) {
      int r = characterCompare(lhs.data, S, rhs.data, P);
      return BitString<1>(r == 0);
   }

   template<size_t S, size_t P>
   BitString<1> operator!= (const Character<S> & lhs,
                            const Character<P> & rhs) {
      int r = characterCompare(lhs.data, S, rhs.data, P);
      return BitString<1>(r != 0);
   }

   template<size_t S, size_t P>
   BitString<1> operator< (const Character<S> & lhs,
                           const Character<P> & rhs) {
      int r = characterCompare(lhs.data, S, rhs.data, P);
      return BitString<1>(r < 0);
   }

   template<size_t S, size_t P>
   BitString<1> operator<= (const Character<S> & lhs,
                            const Character<P> & rhs) {
      int r = characterCompare(lhs.data, S, rhs.data, P);
      return BitString<1>(r <= 0);
   }

   template<size_t S, size_t P>
   BitString<1> operator> (const Character<S> & lhs,
                           const Character<P> & rhs) {
      int r = characterCompare(lhs.data, S, rhs.data, P);
      return BitString<1>(r > 0);
   }

   template<size_t S, size_t P>
   BitString<1> operator>= (const Character<S> & lhs,
                            const Character<P> & rhs) {
      int r = characterCompare(lhs.data, S, rhs.data, P);
      return BitString<1>(r >= 0);
   }




   template<size_t P>
   BitString<1> operator== (const CharSlice & lhs,
                            const Character<P> & rhs) {
      int r = characterCompare(lhs.data, lhs.length.x, rhs.data, P);
      return BitString<1>(r == 0);
   }

   template<size_t P>
   BitString<1> operator!= (const CharSlice & lhs,
                            const Character<P> & rhs) {
      int r = characterCompare(lhs.data, lhs.length.x, rhs.data, P);
      return BitString<1>(r != 0);
   }

   template<size_t P>
   BitString<1> operator< (const CharSlice & lhs,
                           const Character<P> & rhs) {
      int r = characterCompare(lhs.data, lhs.length.x, rhs.data, P);
      return BitString<1>(r < 0);
   }

   template<size_t P>
   BitString<1> operator<= (const CharSlice & lhs,
                            const Character<P> & rhs) {
      int r = characterCompare(lhs.data, lhs.length.x, rhs.data, P);
      return BitString<1>(r <= 0);
   }

   template<size_t P>
   BitString<1> operator> (const CharSlice & lhs,
                           const Character<P> & rhs) {
      int r = characterCompare(lhs.data, lhs.length.x, rhs.data, P);
      return BitString<1>(r > 0);
   }

   template<size_t P>
   BitString<1> operator>= (const CharSlice & lhs,
                            const Character<P> & rhs) {
      int r = characterCompare(lhs.data, lhs.length.x, rhs.data, P);
      return BitString<1>(r >= 0);
   }




   template<size_t S>
   BitString<1> operator== (const Character<S> & lhs,
                            const CharSlice & rhs) {
      int r = characterCompare(lhs.data, S, rhs.data, rhs.length.x);
      return BitString<1>(r == 0);
   }

   template<size_t S>
   BitString<1> operator!= (const Character<S> & lhs,
                            const CharSlice & rhs) {
      int r = characterCompare(lhs.data, S, rhs.data, rhs.length.x);
      return BitString<1>(r != 0);
   }

   template<size_t S>
   BitString<1> operator< (const Character<S> & lhs,
                           const CharSlice & rhs) {
      int r = characterCompare(lhs.data, S, rhs.data, rhs.length.x);
      return BitString<1>(r < 0);
   }

   template<size_t S>
   BitString<1> operator<= (const Character<S> & lhs,
                            const CharSlice & rhs) {
      int r = characterCompare(lhs.data, S, rhs.data, rhs.length.x);
      return BitString<1>(r <= 0);
   }

   template<size_t S>
   BitString<1> operator> (const Character<S> & lhs,
                           const CharSlice & rhs) {
      int r = characterCompare(lhs.data, S, rhs.data, rhs.length.x);
      return BitString<1>(r > 0);
   }

   template<size_t S>
   BitString<1> operator>= (const Character<S> & lhs,
                            const CharSlice & rhs) {
      int r = characterCompare(lhs.data, S, rhs.data, rhs.length.x);
      return BitString<1>(r >= 0);
   }




   BitString<1> operator== (const CharSlice & lhs,
                            const CharSlice & rhs);

   BitString<1> operator!= (const CharSlice & lhs,
                            const CharSlice & rhs);

   BitString<1> operator< (const CharSlice & lhs,
                           const CharSlice & rhs);

   BitString<1> operator<= (const CharSlice & lhs,
                            const CharSlice & rhs);

   BitString<1> operator> (const CharSlice & lhs,
                           const CharSlice & rhs);

   BitString<1> operator>= (const CharSlice & lhs,
                            const CharSlice & rhs);
}
using namespace std;
namespace pearlrt {



   template<int S> class PutFixed {
   private:

      PutFixed() {}
   public:

      static void toF(
         Fixed<S> fixedValue,
         Fixed<31> w,
         Fixed<31> d,
         Sink & sink) {
         Fixed<S> x, y, absValue;
         int32_t prePointDigits, postPointDigits;
         int32_t leadingSpaces, digits, ch;
         Fixed<31> digitsFixed;
         bool signNeeded;
         bool pointNeeded;

         if (w.x <= 0) {
            Log::info("F: width < 0");
            throw theFixedFormatSignal;
         }
         if ((d < (Fixed<31>)0).getBoolean() || (d >= w).getBoolean()) {
            Log::info("F: width and decimals violation");
            throw theFixedFormatSignal;
         }

         signNeeded = false;
         x = fixedValue;
         if ((x < (Fixed<S>)0).getBoolean()) {
            x = -x;
            signNeeded = true;
         }
         absValue = x;
         pointNeeded = false;
         if (d.x > 0) {
            pointNeeded = true;
         }

         digits = 1;
         y = (Fixed<S>) 10;
         while ((x >= y).getBoolean()) {
            x = x / y;
            digits ++;
         }
         postPointDigits = d.x;
         prePointDigits = digits;
         leadingSpaces = w.x -
                         ((signNeeded ? 1 : 0) +
                          (pointNeeded ? 1 : 0) +
                          prePointDigits +
                          postPointDigits
                         );
         if (leadingSpaces < 0) {
            Log::info("F: width too small");
            throw theFixedValueSignal;
         }
         while (leadingSpaces > 0) {
            sink.putChar(' ');
            leadingSpaces --;
         }
         if (signNeeded) {
            sink.putChar('-');
         }
         digitsFixed = (Fixed<31>)(digits - 1);
         x = y.pow(digitsFixed);

         do {
            if (x.x == 0) {
               ch = 0;
            } else {
               ch = (absValue / x).x;
            }
            sink.putChar(ch + '0');
            absValue = absValue - x * (Fixed<S>)ch;
            x = x / y;
            prePointDigits --;
         } while (prePointDigits > 0);
         if (pointNeeded) {
            sink.putChar('.');
         }
         while (postPointDigits > 0) {
            if (x.x == 0) {
               ch = 0;
            } else {
               ch = (absValue / x).x;
            }
            sink.putChar(ch + '0');
            absValue = absValue - x * (Fixed<S>)ch;
            x = x / y;
            postPointDigits --;
         }
         return;
      }
   };

}

#define GETFIXED_H_INCLUDED





#define GETHELPER_H_INCLUDED







namespace pearlrt {



   class GetHelper {
   private:
      int width;
      Source * source;
      int delimiter;
   public:

      static double pow10(int exp);

      GetHelper(const Fixed<31> w,
                Source * s);

      int getRemainingWidth();

      int skipSpaces();

      void discardRemaining();

      int readInteger(int * x, const int digits);

      int readFixedInteger(int * x, const int digits);

      int readMantissa(double * x, const int digits, const int decimals);

      int readSeconds(double * x, const int w, const int d);

      int readString(const char* s);

      enum Delimiters {

         DoubleSpace = 1,

         EndOfLine = 2,

         Comma = 4,

         EndOfFile = 8
      };

      void setDelimiters(int del);

      int readChar();

      void readCharacterByA(RefCharacter * rc);

      void readB4(uint64_t * value, const int nbrOfBitsToSample);

      void readB123(uint64_t * value, const int nbrOfBitsToSample,
                    const int base);

      void readFixedByF(Fixed63 * value, int d);

      void readFloatByF(Float<53> * value, int d);

      void readFloatByE(Float<53> * value);
   };

}
namespace pearlrt {


   template <int S>
   class GetFixed {
   public:

      static void fromF(Fixed<S> &f,
                        const Fixed<31> w,
                        const Fixed<31> d,
                        Source & source) {
         Fixed63 value;
         if (w.x <= 0 || d.x < 0 || w.x < d.x) {
            throw theFixedFormatSignal;
         }
         GetHelper helper(w, &source);
         helper.setDelimiters(GetHelper::EndOfLine);

         helper.readFixedByF(&value, d.x);
         f = Fixed<S>(value.get());
         return;
      }
   };

}

#define PUTFLOAT_H_INCLUDED






#define PUTHELPER_H_INCLUDED




namespace pearlrt {


   class PutHelper {
   public:

      static void doPutChar(int length, RefCharacter* rc, Sink * s);

      const static Float<53> binExpValues[];

      const static int nbrBinExpValues;
   };

}
using namespace std;
namespace pearlrt {


   template<int S> class PutFloat {
   private:

      PutFloat() {}
   public:

      static void toF(
         Float<S> floatValue,
         Fixed<31> w,
         Fixed<31> d,
         Sink & sink) {
         Float<S> x, y, absValue;
         int32_t prePointDigits, postPointDigits;
         int32_t leadingSpaces, digits, ch;
         Fixed<31> digitsFixed;
         bool signNeeded;
         bool pointNeeded;

         if (w.x <= 0) {
            Log::info("F: width < 0");
            throw theFixedFormatSignal;
         }
         if ((d < (Fixed<31>)0).getBoolean() || (d >= w).getBoolean()) {
            Log::info("F: width and decimals violation");
            throw theFixedFormatSignal;
         }

         signNeeded = false;
         x = floatValue;
         if ((x < (Fixed<S>)0).getBoolean()) {
            x = -x;
            signNeeded = true;
         }
         absValue = x;
         pointNeeded = false;
         if (d.x > 0) {
            pointNeeded = true;
         }

         digits = 1;
         y = (Float<S>) 10;
         while ((x >= y).getBoolean()) {
            x = x / y;
            digits ++;
         }
         postPointDigits = d.x;
         prePointDigits = digits;
         leadingSpaces = w.x -
                         ((signNeeded ? 1 : 0) +
                          (pointNeeded ? 1 : 0) +
                          prePointDigits +
                          postPointDigits
                         );
         if (leadingSpaces < 0) {
            Log::info("F: width too small");
            throw theFixedValueSignal;
         }
         while (leadingSpaces > 0) {
            sink.putChar(' ');
            leadingSpaces --;
         }
         if (signNeeded) {
            sink.putChar('-');
         }
         digitsFixed = (Fixed<31>)(digits - 1);
         x = y.pow(digitsFixed);

         do {
            if (x.x == 0) {
               ch = 0;
            } else {
               ch = (absValue / x).x;
            }
            sink.putChar(ch + '0');
            absValue = absValue - x * (Fixed<S>)ch;
            x = x / y;
            prePointDigits --;
         } while (prePointDigits > 0);
         if (pointNeeded) {
            sink.putChar('.');
         }
         while (postPointDigits > 0) {
            if (x.x == 0) {
               ch = 0;
            } else {
               ch = (absValue / x).x;
            }
            sink.putChar(ch + '0');
            absValue = absValue - x * (Fixed<S>)ch;
            x = x / y;
            postPointDigits --;
         }
         return;
      }

      static void toE(
         Float<S> floatValue,
         Fixed<31> w,
         Fixed<31> d,
         Fixed<31> s,
         Fixed<31> eSize,
         Sink & sink) {
         Float<53> x, y, testExponent;
         static const Float<53> one(1.0);
         static const Float<53> two(2.0);

         int32_t leadingSpaces;
         int32_t expValue;
         int32_t i,e;
         int32_t sign=1;
         int32_t prePointDigits, postPointDigits;
         int ch;

         if (w.x <= 0) {
            Log::info("E: width < 0");
            throw theExpFormatSignal;
         }
         if ((s < d+(Fixed<31>)1).getBoolean() ) {
            Log::info("E: significance and decimals violation");
            throw theExpFormatSignal;
         }
         if ( eSize.x != 2 && eSize.x != 3) {
            Log::info("E: exponent field with illegal (%d)", (int)eSize.x);
            throw theExpFormatSignal;
         }




         leadingSpaces = w.x - s.x - 3 - eSize.x;
         x = floatValue;
         if ((x < (Float<53>)0).getBoolean()) {
            x = -x;
            sign = -1;
            leadingSpaces --;
         }
         if ((d < (Fixed<31>)0).getBoolean() || leadingSpaces < 0 ) {
            Log::info("E: width and decimals violation");
            throw theExpFormatSignal;
         }

         expValue = 0;
         if ((x >= one).getBoolean()) {
            testExponent = GetHelper::pow10(100);
            if ((x >= testExponent).getBoolean()) {
               Log::info("E: number too large");
               throw theExpValueSignal;
            }
            e = 256;
            y = x;
            for (i=0; i<PutHelper::nbrBinExpValues; i++) {
               if ( (y >= PutHelper::binExpValues[i]).getBoolean()) {
                  y = y / PutHelper::binExpValues[i];
                  expValue += e;
               }
               e >>= 1;
            }
         } else {
            e = 256;
            y = x;
            for (i=0; i<PutHelper::nbrBinExpValues; i++) {
               if ( (y * PutHelper::binExpValues[i] < two).getBoolean()) {
                  y = y * PutHelper::binExpValues[i];
                  expValue += e;
               }
               e >>= 1;
            }
            expValue = -expValue;
         }

         if (expValue < -99) {
            x = (Float<53>)0.0;
            expValue = 0;
            prePointDigits = 1;
            postPointDigits = d.x;

            sign=1;
            prePointDigits = 1;
            leadingSpaces = w.x - 6 - d.x;
         } else {
            prePointDigits = (s.x - d.x);
            postPointDigits = d.x;
            expValue -= (s.x - d.x -1);
         }
         while(leadingSpaces > 0) {
            sink.putChar(' ');
            leadingSpaces --;
         }
         if (sign == -1) {
            sink.putChar('-');
         }
         x = x / Float<53>(GetHelper::pow10(expValue));
         y = GetHelper::pow10(prePointDigits-1);
         while (prePointDigits > 0) {
            ch = (x / y).x;
            sink.putChar(ch+'0');
            x = x - (Float<53>)ch * y;
            y = y / (Float<53>)10.0;
            prePointDigits --;
         }
         sink.putChar('.');
         y = (Float<53>)10.0;
         while (postPointDigits > 0) {
            x  = (x * y);
            ch = x.x;
            sink.putChar(ch+'0');
            x = x - (Float<53>)ch;
            postPointDigits --;
         }
         sink.putChar('E');
         if (expValue>=0) {
            sink.putChar('+');
         } else {
            sink.putChar('-');
            expValue = - expValue;
         }
         if (eSize.x == 3) {

            ch = expValue / 100 + '0';
            sink.putChar(ch);
            expValue /= 10;
         }
         ch = expValue / 10 + '0';
         sink.putChar(ch);
         ch = expValue % 10 + '0';
         sink.putChar(ch);
      }
   };

}

#define GETFLOAT_H_INCLUDED






namespace pearlrt {


   template <int S>
   class GetFloat {
   public:

      static void fromF(Float<S> &f,
                        const Fixed<31> w,
                        const Fixed<31> d,
                        Source & source) {
         Float<53> value;
         if (w.x <= 0 || d.x < 0 || w.x < d.x) {
            throw theFixedFormatSignal;
         }
         GetHelper helper(w, &source);
         helper.setDelimiters(GetHelper::EndOfLine);

         helper.readFloatByF(&value, d.x);
         f = value.fit(f);
         return;
      }

      static void fromE(Float<S> &f,
                        const Fixed<31> w,
                        const Fixed<31> d,
 	const Fixed<31> s,
                        Source & source) {
         Float<53> value;
         if (w.x <= 0 || d.x < 0 || s.x < 1) {
            throw theExpFormatSignal;
         }
         GetHelper helper(w, &source);
         helper.setDelimiters(GetHelper::EndOfLine);

         helper.readFloatByE(&value);
         f = value.fit(f);
         return;
      }
   };

}




#define PUTCHARACTER_H_INCLUDED






using namespace std;
namespace pearlrt {


   template<size_t S>
   class PutCharacter {
   private:

      PutCharacter() {}
   public:

      static void toA(
         Character<S> &charValue,
         const Fixed<31> w,
         Sink & sink) {
         RefCharacter rc;
         rc.setWork(charValue);

         PutHelper::doPutChar(w.x, &rc, &sink);
      }

      static void toA(
         Character<S> &charValue,
         Sink & sink) {
         toA(charValue, S, sink);
      }
   };

}

#define GETCHARACTER_H_INCLUDED





namespace pearlrt {


   template <size_t S>
   class GetCharacter {
   public:

      static void fromA(Character<S> &c,
                        const Fixed<31> w,
                        Source & source) {
          GetHelper helper(w, &source);
          helper.setDelimiters(GetHelper::EndOfLine);
          RefCharacter rc;
          rc.setWork(c);

          helper.readCharacterByA(&rc);
      }
   };

}


#define SCHEDULEDSIGNALACTION_INCLUDES


namespace pearlrt {

   class ScheduleSignalAction {
   private:
      Signal* signal;
      int currentHandlerIndex;
   public:

      ScheduleSignalAction(Signal * s);

      static int getAction(Signal *s,
                           size_t nbrOfActions,
                           ScheduleSignalAction scheduleSignalAction[]);

      void setActionIndex(int index);
   };
}



#define PUTBITSTRING_H_INCLUDED


namespace pearlrt {


   template<int S> class PutBits;

   template<> class PutBits<1> {
      public:

         static void toBit(Bits<1>::BitType data, int len,
                           int w, int base, Sink &sink);
   };

   template<> class PutBits<2> {
      public:

         static void toBit(Bits<2>::BitType data, int len,
                           int w, int base, Sink &sink);
   };

   template<> class PutBits<4> {
      public:

         static void toBit(Bits<4>::BitType data, int len,
                           int w, int base, Sink &sink);
   };

   template<> class PutBits<8> {
      public:

         static void toBit(Bits<8>::BitType data, int len,
                           int w, int base, Sink &sink);
   };

   template<int S> class PutBitString {
   private:

      PutBitString() {}

      static const int len=NumberOfBytes<S>::resultBitString;
   public:

      static void toB4(
         BitString<S> &bitstring,
         Fixed<31> w,
         Sink & sink) {
         PutBits<len>::toBit(bitstring.x,S, w.x, 4, sink);
         return;
      }

      static void toB1(
         BitString<S> &bitstring,
         Fixed<31> w,
         Sink & sink) {
         PutBits<len>::toBit(bitstring.x, S, w.x, 1, sink);
         return;
      }

      static void toB2(
         BitString<S> &bitstring,
         Fixed<31> w,
         Sink & sink) {
         PutBits<len>::toBit(bitstring.x, S, w.x, 2, sink);
         return;
      }

      static void toB3(
         BitString<S> &bitstring,
         Fixed<31> w,
         Sink & sink) {
         PutBits<len>::toBit(bitstring.x, S, w.x, 3, sink);
         return;
      }
   };

}

#define GETBITSTRING_H_INCLUDED



using namespace std;
namespace pearlrt {


   template<int S> class GetBitString {
   private:

      GetBitString() {}
   public:

      static void fromB123(
         BitString<S> &bitstring,
         const Fixed<31> w,
         const int base,
         Source & source) {
         if (w.x < 0) {
            throw theBitFormatSignal;
         }
         if (w.x == 0) {
            bitstring.x = 0;
            return;
         }
         GetHelper helper(w, &source);
         helper.setDelimiters(GetHelper::EndOfLine);
         uint64_t fixedValue = 0;

         helper.readB123(&fixedValue, S, base);
         BitString<S> retVal(fixedValue);
         bitstring.x = retVal.x;
         return;
      }

      static void fromB4(
         BitString<S> &bitstring,
         const Fixed<31> w,
         Source & source) {
         if (w.x < 0) {
            throw theBitFormatSignal;
         }
         if (w.x == 0) {
            bitstring.x = 0;
            return;
         }
         GetHelper helper(w, &source);
         helper.setDelimiters(GetHelper::EndOfLine);
         uint64_t fixedValue=0;
         helper.readB4(&fixedValue, S);
         BitString<S> retVal(fixedValue);
         bitstring.x = retVal.x;
         return;
      }
   };

}

#define REFCHARSINK_H_INCLUDED


namespace pearlrt {



   class RefCharSink: public Sink {
   private:
      RefCharacter* sinkObj;
   public:

      RefCharSink(RefCharacter & s);

      void putChar(char c);

      void pos(size_t newColumn);


      size_t sop();
   };

}




#define SYSTEMDATIONB_INCLUDED



namespace pearlrt{
class SystemDationB: public SystemDation {
protected:
public:

    virtual SystemDationB* dationOpen(const char * idfValue,
                                       int openParam) = 0;

	virtual void dationClose(int parameters=0)=0;

	virtual void dationWrite(void * start, size_t size)=0;

	virtual void dationRead(void * start, size_t size)=0;
};
}

namespace pearlrt {
   class UserDation;
}

#define DATIONPG_INCLUDED

#define USERDATIONNB_INCLUDED


namespace pearlrt {
   class UserDation;
}





#define DATIONDIM_INCLUDED


namespace pearlrt {


   class DationDim {
   public:

      virtual Fixed<31> getIndex() const = 0;

      Fixed<31> getCapacity() const;

      int getDimensions() const;
   protected:

      DationDim(const Fixed<31> p, const Fixed<31>r, const Fixed<31> c,
                const int d, const bool b);

      const Fixed<31> pages;

      const Fixed<31> rows;

      const Fixed<31> cols;
   private:

      const int dimensions;

      const bool boundedDimension;
   protected:

      Fixed<31> page;

      Fixed<31> row;

      Fixed<31> col;

      static const Fixed<31> one;

      static const Fixed<31> zero;

      int dationParams;
   public:

      void setDationParams(const int p);

      Fixed<31> getColumn() const;

      Fixed<31> getColumns() const;

      Fixed<31> getRow() const;

      Fixed<31> getRows() const;

      Fixed<31> getPage() const;

      Fixed<31> getPages() const;

      bool checkRemainingWidth(Fixed<15> w);

      void reset();

      bool isBounded();

      void gotoNextRecord();
   };

}


#define TFUBUFFER_INCLUDED




namespace pearlrt {



   class TFUBuffer : public Sink, public Source {
   private:
      SystemDationNB* system;
      int sizeOfRecord;
      char * record;
      int readWritePointer;
      bool containsData;
      int oldRecordNumber;
      int paddingElement;
   public:

      void flushRecord();

      void readRecord(bool untilNL);

      TFUBuffer();

      void setSystemDation(SystemDationNB* sys);

      void setupRecord(int recSize, char * rec, int padding);

      void prepare();

      bool isUsed();

      bool isNotEmpty();

      void markEmpty();

      void setPosition(int newPos);

      void write(void * data, size_t n);

      void read(void * data, size_t n);

      void putChar(char c);

      char realGetChar();
   };

}

#define IOJOB_INCLUDED



#define SYSTEMDATIONNBSINK_H_INCLUDED


namespace pearlrt {


   class SystemDationNBSink: public Sink {
   private:

      SystemDationNB* sinkObj;
   public:

      SystemDationNBSink();

      void setSystemDationNB(SystemDationNB * s);

      void putChar(char c);
   };
}
namespace pearlrt {
   class IODataEntry;
   class IOFormatEntry;
   class IOFormatList;


   class LoopControl {
#define MAX_LOOP_LEVEL 11
   private:
      int loopLevel;
      int numberOfItems;
      bool workCyclic;
      int item;
      struct {
         int lastItem;
         int startItem;
         int loops;
         size_t sizeOfBody;
         size_t offsetIncrement;
      } loopControl[MAX_LOOP_LEVEL];
   public:

      LoopControl(int numberOfItems, bool cyclic);

      int next();

      int enter(int nbrOfItems, int nbrOfRepetitions, size_t sizeOfBody=0);

      size_t getOffset();
   };

   class IODataList {
   public:

      size_t nbrOfEntries;

      IODataEntry * entry;
   };

   class IOFormatList {
   protected:

   public:

      size_t nbrOfEntries;

      IOFormatEntry * entry;
   };

   class IODataEntry {
   public:

      enum IODataType  {
         CHAR,
         FLOAT,
         FIXED,
         BIT,

         BITSLICE,
         CLOCK,
         DURATION,

         LoopStart,

         InduceData,
      };


      struct {

         IODataType baseType: 8;

         unsigned int dataWidth: 24;
      } dataType;

      union {
         void * inData;
         const void * outData;

         size_t   offsetIncrement;
      } dataPtr;

      union { size_t *numberOfElements;
              size_t * start;} param1;
      union { size_t * end;} param2;
   };

   class IOFormatEntry {
   public:

      enum IOFormat  {

         A,

         Aw,

         Fw,

         Fwd,

         E2wd,

         E2wds,

         E3wd,

         E3wds,

         B1,

         B2,

         B3,

         B4,

         B1w,

         B2w,

         B3w,

         B4w,

         Tw,

         Twd,

         Dw,

         Dwd,

         LIST,

         LoopStart,

         IsPositioning,

         RST,

         X,

         SKIP,

         PAGE,

         EOFPOS,

         ADV1,

         ADV2,

         ADV3,


         COL,

         ROW,

         POS1,

         POS2,

         POS3,

         SOP1,

         SOP2,

         SOP3,

         InduceFormat
      };

      IOFormat format;

      union FormatParameter {

         Fixed<31> * f31Ptr;

         Fixed<31> const * constF31Ptr;

         void * voidPtr;

         int intValue;
      } fp1,
      fp2,
      fp3;
   };

}
namespace pearlrt {

   class UserDationNB: public UserDation {
   protected:

      enum DationType {ALPHIC, TYPE};

      TFUBuffer  tfuBuffer;
   private:
      static const Fixed<31> one;
      static const Fixed<31> zero;
      const DationType dationType;
      char paddingElement;
      void fill(const Fixed<31> n, char fillChar);
      void skipX(const Fixed<31> n);
      void skipUntil(const Fixed<31> n, char fillChar);
      void skipAny(const Fixed<31> n);
      void internalSop(Fixed<31>* page, Fixed<31>* row, Fixed<31>* element);
      Fixed<31> internalAdv(Fixed<31> p, Fixed<31> r, Fixed<31> c);
      void doTfuAndSeekStuff();
   public:

      void markTFUBufferUsed();

      void getTFUBuffer();

      void beginSequenceHook(TaskCommon * me);

      void endSequenceHook(void);

      virtual void dationUnGetChar(const char c) = 0;
   public:

      Fixed<31> adv(Fixed<31> c);

      Fixed<31> adv(Fixed<31> r, Fixed<31> c);

      Fixed<31> adv(Fixed<31> p, Fixed<31> r, Fixed<31> c);
   protected:

      DationDim * dim;
   public:

      SystemDationNB* system;

      SystemDationNB* work;

      Fixed<31> stepSize;

      UserDationNB(SystemDationNB* parent,
                   int & dationParams,
                   DationDim * dimensions,
                   DationType dt);

      void pos(Fixed<31> c);

      void pos(Fixed<31> row, Fixed<31> col);

      void pos(Fixed<31> page, Fixed<31> row, Fixed<31> col);

      void sop(Fixed<31>* element);

      void sop(Fixed<31>* row, Fixed<31>* element);

      void sop(Fixed<31>* page, Fixed<31>* row, Fixed<31>* element);

      void toAdv(Fixed<31> c);

      void toAdv(Fixed<31> row, Fixed<31> c);

      void toAdv(Fixed<31> page, Fixed<31> row, Fixed<31> col);

      void toX(const Fixed<31> n);

      void toSkip(const Fixed<31> n);

      void toPage(const Fixed<31> n);

      void fromAdv(Fixed<31> c);

      void fromAdv(Fixed<31> row, Fixed<31> c);

      void fromAdv(Fixed<31> page, Fixed<31> row, Fixed<31> col);

      void fromX(const Fixed<31> n);

      void fromSkip(const Fixed<31> n);

      void fromPage(const Fixed<31> n);

      void eof();

      virtual void internalOpen() = 0;

      virtual void internalClose() = 0;
   private:

      void internalDationOpen(int p,
                              RefCharacter* rc);
   public:

      void closeSystemDation(int dationParams);

      int toPositioningFormat(TaskCommon * me, IOFormatEntry * jobFormat);

      int fromPositioningFormat(TaskCommon * me, IOFormatEntry * jobFormat);
   protected:

      void assertOpenDirect();

      void assertOpenDirectOrForward();
   };
}




#define SYSTEMDATIONNBSOURCE_H_INCLUDED



namespace pearlrt {


   class SystemDationNBSource: public Source {
   private:

      SystemDationNB * src;
   public:

      SystemDationNBSource();

      void setSystemDationNB(SystemDationNB * s);

      char realGetChar(void);
   };
}

#define IOFORMATS_H_INCLUDED












namespace pearlrt {



   class IOFormats {
   private:
      Sink  *sink;
      Source  *source;
   public:

      void setupIOFormats(Sink * _sink, Source* _source);

      virtual void checkCapacity(Fixed<31> n) = 0;

      template<size_t S>
      void toA(Character<S> & s) {
         toA(s, S);
      };

      template<size_t S>
      void fromA(Character<S> & s) {
         fromA(s, S);
      };

      template<size_t S>
      void toA(Character<S> &s, Fixed<31> w) {
         checkCapacity(w);
         RefCharacter rc;
         rc.setWork(s);

         PutHelper::doPutChar(w.x, &rc, sink);
      }


      void toA(void *s, size_t len, Fixed<31> w);

      void fromA(void *s, size_t len, Fixed<31> w);

      template<size_t S>
      void fromA(Character<S> & s, Fixed<31> w) {
         checkCapacity(w);
         GetCharacter<S>::fromA(s, w, *source);
         return;
      }

      void toBit(void *s, size_t index,
                 size_t len, int base,
                 const Fixed<31> w);

      template<int S>
      void toB1(BitString<S>  s, Fixed<31> w) {
         checkCapacity(w);
         PutBitString<S>::toB1(s, w, *sink);
      }

      template<int S>
      void fromB1(BitString<S> & s, Fixed<31> w) {
         checkCapacity(w);
         GetBitString<S>::fromB123(s, w, 1, *source);
         return;
      }

      template<int S>
      void toB2(BitString<S>  s, Fixed<31> w) {
         checkCapacity(w);
         PutBitString<S>::toB2(s, w, *sink);
      }

      template<int S>
      void fromB2(BitString<S> & s, Fixed<31> w) {
         checkCapacity(w);
         GetBitString<S>::fromB123(s, w, 2, *source);
         return;
      }

      template<int S>
      void toB3(BitString<S>  s, Fixed<31> w) {
         checkCapacity(w);
         PutBitString<S>::toB3(s, w, *sink);
      }

      template<int S>
      void fromB3(BitString<S> & s, Fixed<31> w) {
         checkCapacity(w);
         GetBitString<S>::fromB123(s, w, 3, *source);
         return;
      }

      template<int S>
      void toB4(BitString<S>  s, Fixed<31> w) {
         checkCapacity(w);
         PutBitString<S>::toB4(s, w, *sink);
      }

      template<int S>
      void fromB4(BitString<S> & s, Fixed<31> w) {
         checkCapacity(w);
         GetBitString<S>::fromB4(s, w, *source);
         return;
      }

      template<int S>
      void toF(Fixed<S>  f,
               const Fixed<31> w,
               const Fixed<31> d = 0) {
         checkCapacity(w);
         PutFixed<S>::toF(f, w, d, *sink);
      };

      void toFloatF(void *f, size_t len, size_t index, const Fixed<31> w,
                    const Fixed<31> d = 0) ;

      void fromFloatF(void *f, size_t len, size_t index, const Fixed<31> w,
                    const Fixed<31> d = 0) ;

      void toFixedF(void *f, size_t len, size_t index, const Fixed<31> w,
                    const Fixed<31> d = 0) ;

      void fromFixedF(void *f, size_t len, size_t index, const Fixed<31> w,
                    const Fixed<31> d = 0) ;

      template<int S>
      void fromF(Fixed<S> & f,
                 const Fixed<31> w,
                 const Fixed<31> d = 0) {
         checkCapacity(w);
         GetFixed<S>::fromF(f, w, d, *source);
      };

      template<int S>
      void toF(Float<S>  f,
               const Fixed<31> w,
               const Fixed<31> d = 0) {
         checkCapacity(w);
         PutFloat<S>::toF(f, w, d, *sink);
      };

      template<int S>
      void fromF(Float<S> & f,
                 const Fixed<31> w,
                 const Fixed<31> d = 0) {
         checkCapacity(w);
         GetFloat<S>::fromF(f, w, d, *source);
      };

      template<int S>
      void toE(Float<S>  f,
               const Fixed<31> w,
               const Fixed<31> d,
               const Fixed<31> s,
               const Fixed<31> e) {
         checkCapacity(w);
         PutFloat<S>::toE(f, w, d, s, e, *sink);
      };

      template<int S>
      void fromE(Float<S> & f,
                 const Fixed<31> w,
                 const Fixed<31> d,
                 const Fixed<31> s) {
         checkCapacity(w);
         GetFloat<S>::fromE(f, w, d, s, *source);
      };

      void toT(const Clock f,
               const Fixed<31> w,
               const Fixed<31> d = 0);

      void fromT(Clock & f,
                 const Fixed<31> w,
                 const Fixed<31> d = 0) ;

      void toD(const Duration f,
               const Fixed<31> w,
               const Fixed<31> d = 0);

      void fromD(Duration& f,
                 const Fixed<31> w,
                 const Fixed<31> d = 0);

      int putDataFormat(TaskCommon * me,
             IODataEntry * dataEntry, size_t index,
             size_t loopOffset,
             IOFormatEntry * format);

      int getDataFormat(TaskCommon * me,
             IODataEntry * dataEntry, size_t index,
             size_t loopOffset,
             IOFormatEntry * format);
   };

}









namespace pearlrt {



   class DationPG: public UserDationNB, public IOFormats {
   private:

      void internalOpen();

      void internalClose();
      SystemDationNBSink  sink;
      SystemDationNBSource   source;
   public:

      DationPG(SystemDationNB* parent,
               int dationParams,
               DationDim * dimensions,
               void * tfubuffer = NULL);

      void dationRead(void * destination, size_t size);

      void dationWrite(void * destination, size_t size);

      void dationSeek(const Fixed<31> & p, const int dationParam);

      void dationUnGetChar(const char c);

      void checkCapacity(Fixed<31> n);

      void put(TaskCommon *me, IODataList *dataEntries,
               IOFormatList * formatEntries);

      void get(TaskCommon *me, IODataList *dataEntries,
               IOFormatList * formatEntries);
   };

}

#define STRINGDATIONCONVERT_INCLUDED


#define REFCHARSOURCE_H_INCLUDED



namespace pearlrt {



   class RefCharSource: public Source {
   private:

      RefCharacter * sourceObj;
   public:

      RefCharSource(RefCharacter & s);

      char realGetChar(void);

      void rewind();

      void pos(size_t pos);

      size_t sop();
   };

}


namespace pearlrt {


   class StringDationConvert: public IOFormats, public Rst {
   private:
      RefCharacter* string;
      RefCharSink   sink;
      RefCharSource source;
      Fixed<31> currentPosition;
      bool isOutput;
   public:

      StringDationConvert(RefCharacter* string, bool isOutput);
     private:

      void dationUnGetChar(const char c);

      void checkCapacity(Fixed<31> n);

      int toPositioningFormat(TaskCommon * me, IOFormatEntry * jobFormat);

      int fromPositioningFormat(TaskCommon * me, IOFormatEntry * jobFormat);

     public:

      void toX(Fixed<31> n);

      void fromX(Fixed<31> n);

      void adv(Fixed<31> n);

      void pos(Fixed<31> n);

      void sop(Fixed<31> &n);

      void put(TaskCommon *me, IODataList *dataEntries,
               IOFormatList * formatEntries);

      void get(TaskCommon *me, IODataList *dataEntries,
               IOFormatList * formatEntries);
   };

}

#define DATIONRW_INCLUDED




namespace pearlrt {


   class DationRW: public UserDationNB {
   private:

      void internalOpen();

      void internalClose();

      void zeroFill(const Fixed<31> n);
   public:

      DationRW(SystemDationNB* parent,
               int dationParams,
               DationDim * dimensions,
               const Fixed<15> stepsize,
               void * tfubuffer = NULL);

      void dationRead(void* data, size_t size);

      void dationWrite(void* data, size_t size);

      void dationSeek(const Fixed<31> & p, const int dationParam);

      void dationUnGetChar(const char c);

      void write(TaskCommon*me,
                 IODataList * dataList, IOFormatList * formatList);

      void read(TaskCommon*me,
                IODataList * dataList, IOFormatList * formatList);
   };

}

#define DATIONTS_INCLUDED

namespace pearlrt {
   class UserDation;
}




namespace pearlrt {


   class DationTS: public UserDation {
   private:

      SystemDationB* system;

      SystemDationB* work;

      void internalOpen();

      void internalClose();
   private:

      void internalDationOpen(int p,
                              RefCharacter* rc);
   public:

      DationTS(SystemDationB* parent,
               int dationParams);

      void closeSystemDation(int dationParams);

      void dationRead(void* data, size_t size);

      void dationWrite(void* data, size_t size);

     void beginSequenceHook(TaskCommon * me);


     void endSequenceHook(void);

      bool allowMultipleIORequests();
   };

}


#define TFURECORD_INCLUDED
namespace pearlrt {



   template <size_t Size>
   class TFURecord {
       private:
          char data[Size] __attribute__ ((aligned(8)));
    };

}



#define DATIONDIM1_INCLUDED



namespace pearlrt {


   class DationDim1 : public DationDim {
   public:

      Fixed<31> getIndex() const;

      DationDim1();

      DationDim1(Fixed<31> c);

      void pos(const Fixed<31> c);

      void adv(const Fixed<31> c);
   };

}

#define DATIONDIM2_INCLUDED



namespace pearlrt {


   class DationDim2 : public DationDim {
   private:

      Fixed<31> getIndex(const Fixed<31> r, const Fixed<31> c) const;
   public:

      Fixed<31> getIndex() const;

      DationDim2(Fixed<31> c);

      DationDim2(Fixed<31> r, Fixed<31> c);

      void pos(const Fixed<31> r, const Fixed<31> c);

      void adv(const Fixed<31> r, const Fixed<31> c);

      Fixed<31> getElements4Skip(const Fixed<31> n);
   };

}

#define DATIONDIM3_INCLUDED



namespace pearlrt {


   class DationDim3 : public DationDim {
   private:

      Fixed<31> getIndex(const Fixed<31> p,
                         const Fixed<31> r,
                         const Fixed<31> c) const;
   public:

      Fixed<31> getIndex() const;

      DationDim3(Fixed<31> r, Fixed<31> c);

      DationDim3(Fixed<31> p, Fixed<31> r, Fixed<31> c);

      void pos(const Fixed<31> p, const Fixed<31> r, const Fixed<31> c);

      void adv(const Fixed<31> p, const Fixed<31> r, const Fixed<31> c);

      Fixed<31> getElements4Skip(const Fixed<31> n);

      Fixed<31> getElements4Page(const Fixed<31> n);
   };

}

#define PRLSEMA_INCLUDED
namespace pearlrt {
   class Semaphore;
}






#define PRIORITYQUEUE_INCLUDED
namespace pearlrt {
   class PriorityQueue ;
}

namespace pearlrt {

   class PriorityQueue {
   private:
      TaskCommon* head;
   public:

      PriorityQueue();

      void insert(TaskCommon * x);

      void remove(TaskCommon * x);

      TaskCommon* getHead();

      TaskCommon* getNext(TaskCommon * x);
   };
}
namespace pearlrt {

   class Semaphore {
   private:
      Semaphore();
      uint32_t value;
      const char * name;
      static PriorityQueue waiters;
      static int check(BlockData::BlockReasons::BlockSema *bd);
   public:

      Semaphore(uint32_t  preset = 0, const char * n = NULL);

      const char* getName(void);

      void decrement(void);

      void increment(void);

      uint32_t getValue(void);
   public:

      static void request(TaskCommon* me,  int nbrOfSemas, Semaphore** semas);

      static int dotry(TaskCommon* me,  int nbrOfSemas, Semaphore** semas);

      static void release(TaskCommon* me,  int nbrOfSemas, Semaphore** semas);

      static void removeFromWaitQueue(TaskCommon * t);

      static void addToWaitQueue(TaskCommon * t);

      static void updateWaitQueue(TaskCommon * t);
   };
}
#define SPCSEMA(x,fromModule,inModule) 	\
}      \
namespace pearl_ ## fromNameSpace { \
   extern pearlrt::Semaphore x; 	\
}   \
namespace pearl_ ## inNameSpace {
#define DCLSEMA(x,preset) \
static  pearlrt::Semaphore x(preset,#x)
#define DCLSEMAGLOBAL(x,preset) \
   pearlrt::Semaphore x(preset,#x)

#define PRLBOLT_INCLUDED
namespace pearlrt {
   class Bolt;
}






namespace pearlrt {

   class Bolt {
   private:
      Bolt();
      uint state;
      uint32_t nbrOfEnterOperations;
      const char * name;
      static PriorityQueue waiters;
      static int check(BlockReason r, BlockData::BlockReasons::BlockBolt *bd);
      enum BoltStates {FREE = 0, ENTERED = 1, RESERVED = 2};

      void setState(int newState);

      int getState(void);

      static void enterOrReserve(TaskCommon* me,
                                 BlockReason operation,
                                 int newState,  int nbrOfBolts, Bolt** bolts);

      static void leaveOrFree(TaskCommon* me,  int oldState,
                              int nbrOfBolts, Bolt** bolts);
      void incrementEnter();
      void decrementEnter();
      uint32_t getNbrOfEnterOperations();
   public:

      Bolt(const char * n = NULL);

      const char* getName(void);

      static void enter(TaskCommon* me,  int nbrOfBolts, Bolt** bolts);

      static void reserve(TaskCommon* me,  int nbrOfBolts, Bolt** bolts);

      static void leave(TaskCommon* me,  int nbrOfBolts, Bolt** bolts);

      static void free(TaskCommon* me,  int nbrOfBolts, Bolt** bolts);

      static void removeFromWaitQueue(TaskCommon * t);

      static void addToWaitQueue(TaskCommon * t);

      static void updateWaitQueue(TaskCommon * t);

      char* getStateName(void);
   };
}
#define SPCBOLT(x, fromModule, inModule) 	\
}   \
namespace pearl_ ## fromModule { \
   extern pearlrt::Bolt x; 	\
}   \
namespace pearl_ ##  inModule {
#define DCLBOLT(x) \
static pearlrt::Bolt x(#x)
#define DCLBOLTGLOBAL(x) \
   pearlrt::Bolt x(#x)



#define CONTROL_INCLUDED

namespace pearlrt {

   class Control {
   private:
      Control();
      static int exitCode;
   public:

      static void setExitCode(const Fixed<15> x);

      static int getExitCode();
   };
}

#define STDIN_INCLUDED





namespace pearlrt {


   class StdIn: public SystemDationNB {
   private:

      Mutex mutex;

      int cap;

      bool inUse;

      int capacity;

      FILE* fp;
      static bool _isDefined;
   public:

      StdIn();

      int capabilities();

      StdIn* dationOpen(const char * fileName, int openParams);

      void dationClose(int closeParams);

      void dationRead(void * destination, size_t size);

      void dationWrite(void * destination, size_t size);

      void dationUnGetChar(const char c);

      static bool isDefined();

      void translateNewLine(bool doNewLineTranslation);
   };

}

#define STDOUT_INCLUDED





namespace pearlrt {


   class StdOut: public SystemDationNB {
   private:

      Mutex mutex;

      int cap;

      bool inUse;

      FILE* fp;
      static int declaredDations;
   public:

      StdOut();

      int capabilities();

      StdOut* dationOpen(const char * fileName, int openParams);

      void dationClose(int closeParams);

      void dationRead(void * destination, size_t size);

      void dationWrite(void * destination, size_t size);

      void dationUnGetChar(const char c);


      void translateNewLine(bool doNewLineTranslation);
   };

}

#define STDERROR_INCLUDED





namespace pearlrt {


   class StdError: public SystemDationNB {
   private:

      Mutex mutex;

      int cap;

      bool inUse;

      int capacity;

      FILE* fp;
   public:

      StdError();

      int capabilities();

      StdError* dationOpen(const char * fileName, int openParams);

      void dationClose(int closeParams);

      void dationRead(void * destination, size_t size);

      void dationWrite(void * destination, size_t size);

      void dationUnGetChar(const char c);

      void translateNewLine(bool doNewLineTranslation);
   };

}

#define CONSOLE_INCLUDED






#define CONSOLECOMMON_INCLUDED
namespace pearlrt {

   class ConsoleCommon {
   private:
      PriorityQueue waitingForOutput;
      TaskCommon* waitingForInput;
      SystemDationNB * systemIn;
      SystemDationNB * systemOut;
      TaskCommon*  lastAdressedTask;
      bool insertMode;
      size_t  nbrEnteredCharacters;
      size_t  cursorPosition;
      bool inputStarted;
      char inputLine[80];
      int getChar(void);
      void putChar(char ch);
      void putString(const char* string);
      void goLeft(int n);
      void goRight(int n);
   public:

      ConsoleCommon();

      void setSystemDations(SystemDationNB* in, SystemDationNB* out);

      TaskCommon* treatLine(char** inputBuffer, size_t * length);

      void registerWaitingTask(void * task, int direction);
   };
}



namespace pearlrt {


   class Console: public SystemDationNB {
   private:

      Mutex consoleMutex;

      int cap;

      bool inUse;

      int capacity;
      static Console * instance;
      ConsoleCommon consoleCommon;
      StdIn stdIn;
      StdOut stdOut;
      struct termios oldTerminalSetting;
      pid_t consoleThread;
      TaskCommon* currentTask;
      char * bufferFromConsoleCommon;
      size_t lengthOfConsoleCommonBuffer;
      size_t deliveredCharacters;
   public:

      static Console* getInstance();

      TaskCommon* treat();

      Console();

      ~Console();

      int capabilities();

      Console* dationOpen(const char * fileName, int openParams);

      void dationClose(int closeParams);

      void dationRead(void * destination, size_t size);

      void dationWrite(void * destination, size_t size);

      void dationUnGetChar(const char c);

      void translateNewLine(bool doNewLineTranslation);

      void registerWaitingTask(void * task, int direction);

      bool allowMultipleIORequests();

      void openSubDations();
   };

}

#define PIPE_INCLUDED





namespace pearlrt {


   class Pipe: public SystemDationNB {
   public:

      class PipeFile : public SystemDationNB {
      private:
         FILE * fp;
         Pipe * 	myPipe;
      public:

         bool inUse;

         PipeFile(Pipe* disc);

         int capabilities();

         PipeFile* dationOpen(const char * fileName, int openParams);

         void dationClose(int closeParams);

         void dationRead(void * destination, size_t size);

         void dationWrite(void * destination, size_t size);

         void dationUnGetChar(const char c);

         void translateNewLine(bool doNewLineTranslation);
      };
   private:

      Mutex mutex;

      int cap;

      int capacity;

      int usedCapacity;
      PipeFile**  object;

      static char * devicePath;
      FILE* defaultReader;
      bool removeFile;
   public:

      Pipe(const char* name, const int nbrOfFiles, const char* params = NULL);

      ~Pipe();

      int capabilities();

      PipeFile* dationOpen(const char * fileName, int openParams);

      void dationClose(int closeParams);

      void dationRead(void * destination, size_t size);

      void dationWrite(void * destination, size_t size);

      void dationUnGetChar(const char c);

      void translateNewLine(bool doNewLineTranslation);
   };

}

#define DISC_INCLUDED





namespace pearlrt {


   class Disc: public SystemDationNB {
   public:

      class DiscFile : public SystemDationNB {
      private:
         FILE * fp;
         RefCharacter   rcFn;
         Character<256> completeFileName;
         Disc * 	myDisc;
      public:

         bool inUse;

         DiscFile(Disc* disc);

         int capabilities();

         DiscFile* dationOpen(const char * fileName, int openParams);

         void dationClose(int closeParams);

         void dationRead(void * destination, size_t size);

         void dationWrite(void * destination, size_t size);

         void dationSeek(const Fixed<31>&p, const int dationParam);

         void dationUnGetChar(const char c);

         Fixed<31> dationEof();

         void translateNewLine(bool doNewLineTranslation);
      };
   private:

      Mutex mutex;

      int cap;

      int capacity;

      int usedCapacity;
      DiscFile**  object;
      char * devicePath;
   public:

      Disc(const char* dev, const int nbrOfFiles);

      ~Disc();

      int capabilities();

      DiscFile* dationOpen(const char * fileName, int openParams);

      void dationClose(int closeParams);

      void dationRead(void * destination, size_t size);

      void dationWrite(void * destination, size_t size);

      void dationUnGetChar(const char c);

      void translateNewLine(bool doNewLineTranslation);
   };

}

#define UNIXSIGNAL_INCLUDED


namespace pearlrt {


   class UnixSignal : public Interrupt {
   private:
      static UnixSignal *listOfUnixSignals;
      UnixSignal * next;
      int        sigNum;
      static int isSet;
      static UnixSignal * sig1;
      static UnixSignal * sig2;
      static UnixSignal * sig3;
      static UnixSignal * sig15;
      static UnixSignal * sig16;
      static UnixSignal * sig17;
      static void unixSignalHandler(int sig);
   public:

      UnixSignal(int sigNum);

      static void updateSigMask(sigset_t * sig);

      static bool treat(int sig);

      void devEnable();

      void devDisable();
   };
}

#define SOFTINT_INCLUDED


namespace pearlrt {



   class SoftInt : public Interrupt {
   private:
      static int isSet;
   public:

      SoftInt(int nbr);

      void devEnable();

      void devDisable();
   };

}



#define SAMPLEBASICDATION_H_INCLUDED




namespace pearlrt {

   class SampleBasicDation: public SystemDationB {
   private:
      int16_t echo;
      void internalDationOpen();
      void internalDationClose();
   public:

      SampleBasicDation();



      SystemDationB* dationOpen(const char* idf = 0, int openParam = 0);

      void dationClose(int closeParam = 0);

      void dationRead(void * data, size_t size);

      void dationWrite(void * data, size_t size);

      int capabilities();
   };
}

