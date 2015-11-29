#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <cstdarg>
#include <string.h>
#include <stddef.h>
#include <string>
#include <cmath>
#include <math.h>
#include <time.h>


#define TASK_H_

#define BITSTRING_H_INCLUDED



#define FIXED_H_INCLUDED



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
class DurationDivideByZeroSignal : public ArithmeticSignal {
 public:
 DurationDivideByZeroSignal() {
   type = (char*)"duration div 0";
   rstNum = 207;
 };
};
class DurationRangeSignal : public ArithmeticSignal {
 public:
 DurationRangeSignal() {
   type = (char*)"duration value overflow";
   rstNum = 208;
 };
};
class FloatIsNaNSignal : public ArithmeticSignal {
 public:
 FloatIsNaNSignal() {
   type = (char*)"Float result is NaN";
   rstNum = 209;
 };
};
class FloatIsINFSignal : public ArithmeticSignal {
 public:
 FloatIsINFSignal() {
   type = (char*)"Float result is INF";
   rstNum = 210;
 };
};
class BitSignal : public Signal {
 public:
 BitSignal() {
   type = (char*)"never created";
   rstNum = 301;
 };
};
class BitIndexOutOfRangeSignal : public BitSignal {
 public:
 BitIndexOutOfRangeSignal() {
   type = (char*)"bit index out of range";
   rstNum = 302;
 };
};
class CharacterTooLongSignal : public Signal {
 public:
 CharacterTooLongSignal() {
   type = (char*)"character string too long";
   rstNum = 303;
 };
};
class CharacterIndexOutOfRangeSignal : public Signal {
 public:
 CharacterIndexOutOfRangeSignal() {
   type = (char*)"character index out of range";
   rstNum = 304;
 };
};
class DationSignal : public Signal {
 public:
 DationSignal() {
   type = (char*)"dation";
   rstNum = 305;
 };
};
class IllegalParamSignal : public DationSignal {
 public:
 IllegalParamSignal() {
   type = (char*)"illegal parameter, not permitted operation";
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
class PositioningForbiddenSignal : public DationSignal {
 public:
 PositioningForbiddenSignal() {
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
class IllegalPathSignal : public DationSignal {
 public:
 IllegalPathSignal() {
   type = (char*)"Path points to normal file. Not allowed";
   rstNum = 313;
 };
};
class NoFileInfoSignal : public DationSignal {
 public:
 NoFileInfoSignal() {
   type = (char*)"Can't get Fileinfo for given path";
   rstNum = 314;
 };
};
class NotAllowedSignal : public DationSignal {
 public:
 NotAllowedSignal() {
   type = (char*)"This Action is forbidden by specification";
   rstNum = 315;
 };
};
class DationSeekNotRealizedSignal : public DationSignal {
 public:
 DationSeekNotRealizedSignal() {
   type = (char*)"no seek operation available";
   rstNum = 316;
 };
};
class DationIndexBoundSignal : public DationSignal {
 public:
 DationIndexBoundSignal() {
   type = (char*)"Dation position out of bounds";
   rstNum = 317;
 };
};
class DationNotSupportedSignal : public DationSignal {
 public:
 DationNotSupportedSignal() {
   type = (char*)"Dation operation not supoorted";
   rstNum = 318;
 };
};
class DationEOFSignal : public DationSignal {
 public:
 DationEOFSignal() {
   type = (char*)"Dation EOF encoundered";
   rstNum = 319;
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
class NoDataInFieldSignal : public FormatSignal {
 public:
 NoDataInFieldSignal() {
   type = (char*)"input field is empty";
   rstNum = 402;
 };
};
class DurationFormatSignal : public FormatSignal {
 public:
 DurationFormatSignal() {
   type = (char*)"illegal parameter for D-format";
   rstNum = 403;
 };
};
class ClockFormatSignal : public FormatSignal {
 public:
 ClockFormatSignal() {
   type = (char*)"illegal parameter for T-format";
   rstNum = 404;
 };
};
class FixedFormatSignal : public FormatSignal {
 public:
 FixedFormatSignal() {
   type = (char*)"illegal parameter for F-format";
   rstNum = 405;
 };
};
class CharacterFormatSignal : public FormatSignal {
 public:
 CharacterFormatSignal() {
   type = (char*)"illegal parameter for A-format";
   rstNum = 406;
 };
};
class BitFormatSignal : public FormatSignal {
 public:
 BitFormatSignal() {
   type = (char*)"illegal parameter for B-format";
   rstNum = 407;
 };
};
class ClockValueSignal : public FormatSignal {
 public:
 ClockValueSignal() {
   type = (char*)"illegal value for T-format";
   rstNum = 408;
 };
};
class FixedValueSignal : public FormatSignal {
 public:
 FixedValueSignal() {
   type = (char*)"Illegal value for F-format ";
   rstNum = 409;
 };
};
class BitValueSignal : public FormatSignal {
 public:
 BitValueSignal() {
   type = (char*)"Illegal value for B-format ";
   rstNum = 410;
 };
};
class DurationValueSignal : public FormatSignal {
 public:
 DurationValueSignal() {
   type = (char*)"Illegal value for D-format ";
   rstNum = 411;
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
extern DurationDivideByZeroSignal theDurationDivideByZeroSignal;
extern DurationRangeSignal theDurationRangeSignal;
extern FloatIsNaNSignal theFloatIsNaNSignal;
extern FloatIsINFSignal theFloatIsINFSignal;
extern BitSignal theBitSignal;
extern BitIndexOutOfRangeSignal theBitIndexOutOfRangeSignal;
extern CharacterTooLongSignal theCharacterTooLongSignal;
extern CharacterIndexOutOfRangeSignal theCharacterIndexOutOfRangeSignal;
extern DationSignal theDationSignal;
extern IllegalParamSignal theIllegalParamSignal;
extern OpenFailedSignal theOpenFailedSignal;
extern CloseFailedSignal theCloseFailedSignal;
extern PositioningFailedSignal thePositioningFailedSignal;
extern PositioningForbiddenSignal thePositioningForbiddenSignal;
extern ReadingFailedSignal theReadingFailedSignal;
extern WritingFailedSignal theWritingFailedSignal;
extern IllegalPathSignal theIllegalPathSignal;
extern NoFileInfoSignal theNoFileInfoSignal;
extern NotAllowedSignal theNotAllowedSignal;
extern DationSeekNotRealizedSignal theDationSeekNotRealizedSignal;
extern DationIndexBoundSignal theDationIndexBoundSignal;
extern DationNotSupportedSignal theDationNotSupportedSignal;
extern DationEOFSignal theDationEOFSignal;
extern FormatSignal theFormatSignal;
extern NoMoreCharactersSignal theNoMoreCharactersSignal;
extern NoDataInFieldSignal theNoDataInFieldSignal;
extern DurationFormatSignal theDurationFormatSignal;
extern ClockFormatSignal theClockFormatSignal;
extern FixedFormatSignal theFixedFormatSignal;
extern CharacterFormatSignal theCharacterFormatSignal;
extern BitFormatSignal theBitFormatSignal;
extern ClockValueSignal theClockValueSignal;
extern FixedValueSignal theFixedValueSignal;
extern BitValueSignal theBitValueSignal;
extern DurationValueSignal theDurationValueSignal;
}

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
namespace pearlrt {

#define IFTHENELSETEMPLATE_INCLUDED

   template< bool Condition, class THEN, class ELSE > struct IF { };

   template<class THEN, class ELSE>
   struct IF<true, THEN, ELSE > {

      typedef THEN SELECT_CLASS;
   };

   template<class THEN, class ELSE >
   struct IF<false, THEN, ELSE > {

      typedef ELSE SELECT_CLASS;
   };

#define NUMBEROFBYTES_INCLUDE

   template< unsigned int len > class NumberOfBytes {
   public:

      enum Len {
         N0 = (0 < len && len <= 8) ? 1 : 0,
         N1 = (8 < len && len <= 16) ? 2 : 0,
         N2 = (16 < len && len <= 32) ? 4 : 0,
         N3 = (32 < len && len <= 64) ? 8 : 0,
      };

      enum Nbr {resultFixed = N0 + N1 + N2 + N3};

      enum B {
         B0 = (0 < len && len <= 8) ? 1 : 0,
         B1 = (8 < len && len <= 16) ? 2 : 0,
         B2 = (16 < len && len <= 32) ? 4 : 0,
         B3 = (32 < len && len <= 64) ? 8 : 0,
      };

      enum R {resultBitString = B0 + B1 + B2 + B3};
   };
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
   private:

      static const int len = NumberOfBytes < S + 1 >::resultFixed;
   public:

      typedef typename FixedTypes<len>::NativeFixedType NativeType;

      typedef typename FixedTypes<len>::UNativeFixedType UNativeType;

      typedef typename FixedTypes<len>::LongerNativeFixedType LongerNativeType;

      NativeType x;
   private:
      struct LessThan32Bits {
         static Fixed add(const Fixed & lhs,
                          const Fixed & rhs)  {
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

      Fixed() {
         x = 0;
      }

      Fixed(LongerNativeType y) {
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

      template<int P>Fixed pow(const Fixed<P>& rhs)const {
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
   Fixed < (S > P) ? S : P > operator+ (Fixed<S> lhs, const Fixed<P> & rhs)  {
      Fixed < (S > P) ? S : P > l(lhs.x), r(rhs.x);
      return l.add(r);
   }

   template<int S, int P>
   Fixed < (S > P) ? S : P > operator- (Fixed<S> lhs, const Fixed<P> & rhs)  {
      Fixed < (S > P) ? S : P > l(lhs.x), r(rhs.x);
      return l.substract(r);
   }

   template<int S, int P>
   Fixed < (S > P) ? S : P > operator* (Fixed<S> lhs, const Fixed<P> & rhs)  {
      Fixed < (S > P) ? S : P > l(lhs.x), r(rhs.x);
      return l.multiply(r);
   }

   template<int S, int P>
   Fixed < (S > P) ? S : P > operator/ (Fixed<S> lhs, const Fixed<P> & rhs)  {
      Fixed < (S > P) ? S : P > l(lhs.x), r(rhs.x);
      return l.divide(r);
   }

   template<int S, int P>
   Fixed < (S > P) ? S : P > operator% (Fixed<S> lhs, const Fixed<P> & rhs)  {
      Fixed < (S > P) ? S : P > l(lhs.x), r(rhs.x);
      return l.modulo(r);
   }
}


#define LOG_INCLUDED


#define CHARACTER_INCLUDED



namespace pearlrt {

   template<size_t length>
   class Character {
   public:

      char data[length];
   public:

      Character<length>() {
         size_t i;
         if (length > 32767 || length < 1) {
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

      Fixed<15> upb() const {
         return Fixed<15>(length);
      }

      Character<1>  getCharAt(size_t p) {
         if (p > length || p < 1) {
            throw theCharacterIndexOutOfRangeSignal;
         }
         Character<1> c;
         c.data[0] = data[p - 1];
         return c;
      }

      void setCharAt(size_t p, Character<1> c) {
         if (p > length || p < 1) {
            throw theCharacterIndexOutOfRangeSignal;
         }
         data[p - 1] = c.data[0];
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
   };




   Character<1>  toChar(int x);

   template<class Tres, class Tleft, class Tright>
   void catChar(Tleft& left, Tright& right, Tres& res) {
      int j = 0;
      if ((left.upb() + right.upb() != res.upb()).getBoolean()) {
         throw theCharacterTooLongSignal;
      }
      for (unsigned int i = 0; i < (size_t)(left.upb().x); i++) {
         res.data[j++] = left.data[i];
      }
      for (unsigned int i = 0; i < (size_t)(right.upb().x); i++) {
         res.data[j++] = right.data[i];
      }
   }

   template<class Tres, class Tcharx>
   void getSliceChar(Tcharx& c, size_t start, size_t end, Tres & res) {
      size_t len = end - start + 1;
      if (len > (size_t)(res.upb().x)) {
         throw theCharacterTooLongSignal;
      }
      if (start < 1 || end > (size_t)(c.upb().x)) {
         throw theCharacterIndexOutOfRangeSignal;
      }
      for (size_t i = 0; i < len; i++) {
         res.data[i] = c.data[start + i - 1];
      }
   }

   template<class Tres, class Tcharx>
   void setSliceChar(Tres & res, size_t start, size_t end, Tcharx & c) {
      size_t len = end - start + 1;
      if (len < (size_t)(c.upb().x)) {
         throw theCharacterTooLongSignal;
      }
      if (start < 1 || end > (size_t)(res.upb().x)) {
         throw theCharacterIndexOutOfRangeSignal;
      }
      for (size_t i = 0; i < len; i++) {
         res.data[start - 1 + i] = c.data[i];
      }
      for (size_t i = len; i < (size_t)(c.upb().x); i++) {
         res.data[start - 1 + i] = ' ';
      }
   }

   template<class Tcharl, class Tcharr>
   int compareChar(Tcharl& lhs, Tcharr & rhs) {
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
}

#define MUTEX_INCLUDED
 



#define INC_FREERTOS_H


 


  





#define FREERTOS_CONFIG_H
#define configASSERT( x ) if( ( x ) == 0 ) vAssertCalled( __FILE__, __LINE__ );
#define configUSE_PREEMPTION 1
#define configUSE_IDLE_HOOK 	0
#define configMAX_PRIORITIES ( 260 )
#define configUSE_TICK_HOOK 	0
#define configCPU_CLOCK_HZ 	( ( unsigned long ) 96000000 )
#define configTICK_RATE_HZ 	( ( TickType_t ) 1000 )
#define configMINIMAL_STACK_SIZE	( ( unsigned short ) 80 )
#define configTOTAL_HEAP_SIZE ( ( size_t ) ( 15 * 1024 ) )
#define configUSE_MALLOC_FAILED_HOOK	1
#define configMAX_TASK_NAME_LEN ( 12 )
#define configUSE_TRACE_FACILITY	0
#define configUSE_16_BIT_TICKS 0
#define configIDLE_SHOULD_YIELD 0
#define configUSE_CO_ROUTINES  0
#define configUSE_MUTEXES 	1
#define configUSE_PORT_OPTIMISED_TASK_SELECTION   0
#define INCLUDE_xTaskGetSchedulerState  1
#define INCLUDE_xTaskResumeFromISR	1
#define configTIMER_TASK_PRIORITY       (configMAX_PRIORITIES - 1)
#define configTIMER_QUEUE_LENGTH  50
#define configTIMER_TASK_STACK_DEPTH	80
#define configMAX_CO_ROUTINE_PRIORITIES ( 2 )
#define configUSE_COUNTING_SEMAPHORES 	1
#define configUSE_ALTERNATIVE_API  0
#define configCHECK_FOR_STACK_OVERFLOW	1
#define configUSE_RECURSIVE_MUTEXES 1
#define configQUEUE_REGISTRY_SIZE 10
#define configGENERATE_RUN_TIME_STATS	0
#define INCLUDE_vTaskPrioritySet 	1
#define INCLUDE_uxTaskPriorityGet 	1
#define INCLUDE_vTaskDelete  	1
#define INCLUDE_vTaskCleanUpResources 0
#define INCLUDE_vTaskSuspend  1
#define INCLUDE_vTaskDelayUntil  1
#define INCLUDE_vTaskDelay  	1
#define INCLUDE_uxTaskGetStackHighWaterMark	1
#define INCLUDE_pcTaskGetTaskName	1
#define configUSE_STATS_FORMATTING_FUNCTIONS	0
#define configMAC_ADDR0	0x00
#define configMAC_ADDR1	0x12
#define configMAC_ADDR2	0x13
#define configMAC_ADDR3	0x10
#define configMAC_ADDR4	0x15
#define configMAC_ADDR5	0x11
#define configIP_ADDR0 192
#define configIP_ADDR1 168
#define configIP_ADDR2 0
#define configIP_ADDR3 201
#define configNET_MASK0 255
#define configNET_MASK1 255
#define configNET_MASK2 255
#define configNET_MASK3 0

#define configPRIO_BITS       5
#define configKERNEL_INTERRUPT_PRIORITY 	( 31 << (8 - configPRIO_BITS) )
#define configMAX_SYSCALL_INTERRUPT_PRIORITY 	( 5 << (8 - configPRIO_BITS) )
#define configEMAC_INTERRUPT_PRIORITY 5
#define configUSB_INTERRUPT_PRIORITY 6



 



#define PROJDEFS_H


typedef void (*TaskFunction_t)( void * );



#define pdMS_TO_TICKS( xTimeInMs ) ( ( TickType_t ) ( ( ( TickType_t ) ( xTimeInMs ) * ( TickType_t ) configTICK_RATE_HZ ) / ( TickType_t ) 1000 ) )


#define pdFALSE 	( ( BaseType_t ) 0 )

#define pdTRUE 	( ( BaseType_t ) 1 )


#define pdPASS 	( pdTRUE )

#define pdFAIL 	( pdFALSE )

#define errQUEUE_EMPTY	( ( BaseType_t ) 0 )

#define errQUEUE_FULL	( ( BaseType_t ) 0 )



#define errCOULD_NOT_ALLOCATE_REQUIRED_MEMORY	( -1 )

#define errQUEUE_BLOCKED   ( -4 )

#define errQUEUE_YIELD   	( -5 )




#define configUSE_LIST_DATA_INTEGRITY_CHECK_BYTES 0


#if( configUSE_16_BIT_TICKS == 1 )

#define pdINTEGRITY_CHECK_VALUE 0x5a5a
#else

#define pdINTEGRITY_CHECK_VALUE 0x5a5a5a5aUL
#endif



#define pdFREERTOS_LITTLE_ENDIAN	0

#define pdFREERTOS_BIG_ENDIAN 1








 





#define PORTABLE_H


 



#define DEPRECATED_DEFINITIONS_H

























































































































	 




#define PORTMACRO_H







#define portCHAR char

#define portFLOAT float

#define portDOUBLE double

#define portLONG long

#define portSHORT short

#define portSTACK_TYPE	uint32_t

#define portBASE_TYPE	long

typedef portSTACK_TYPE StackType_t;
typedef long BaseType_t;
typedef unsigned long UBaseType_t;

#if( configUSE_16_BIT_TICKS == 1 )
	typedef uint16_t TickType_t;

#define portMAX_DELAY ( TickType_t ) 0xffff
#else
	typedef uint32_t TickType_t;

#define portMAX_DELAY ( TickType_t ) 0xffffffffUL

	

#define portTICK_TYPE_IS_ATOMIC 1
#endif




#define portSTACK_GROWTH 	( -1 )

#define portTICK_PERIOD_MS 	( ( TickType_t ) 1000 / configTICK_RATE_HZ )

#define portBYTE_ALIGNMENT 	8




extern void vPortYield( void );

#define portNVIC_INT_CTRL_REG ( * ( ( volatile uint32_t * ) 0xe000ed04 ) )

#define portNVIC_PENDSVSET_BIT ( 1UL << 28UL )

#define portYIELD()  	vPortYield()

#define portEND_SWITCHING_ISR( xSwitchRequired ) if( xSwitchRequired ) portNVIC_INT_CTRL_REG = portNVIC_PENDSVSET_BIT

#define portYIELD_FROM_ISR( x ) portEND_SWITCHING_ISR( x )



extern void vPortEnterCritical( void );
extern void vPortExitCritical( void );
extern uint32_t ulPortSetInterruptMask( void );
extern void vPortClearInterruptMask( uint32_t ulNewMaskValue );

#define portSET_INTERRUPT_MASK_FROM_ISR() ulPortSetInterruptMask()

#define portCLEAR_INTERRUPT_MASK_FROM_ISR(x)	vPortClearInterruptMask(x)

#define portDISABLE_INTERRUPTS()  ulPortSetInterruptMask()

#define portENABLE_INTERRUPTS()  	vPortClearInterruptMask(0)

#define portENTER_CRITICAL()  	vPortEnterCritical()

#define portEXIT_CRITICAL()   vPortExitCritical()




#define portTASK_FUNCTION_PROTO( vFunction, pvParameters ) void vFunction( void *pvParameters )

#define portTASK_FUNCTION( vFunction, pvParameters ) void vFunction( void *pvParameters )




	extern void vPortSuppressTicksAndSleep( TickType_t xExpectedIdleTime );

#define portSUPPRESS_TICKS_AND_SLEEP( xExpectedIdleTime ) vPortSuppressTicksAndSleep( xExpectedIdleTime )






#if configUSE_PORT_OPTIMISED_TASK_SELECTION == 1

	
	__attribute__( ( always_inline ) ) static inline uint8_t ucPortCountLeadingZeros( uint32_t ulBitmap )
	{
	uint8_t ucReturn;

 __asm volatile ( "clz %0, %1" : "=r" ( ucReturn ) : "r" ( ulBitmap ) );
 return ucReturn;
	}

	
	#if( configMAX_PRIORITIES > 32 )
 #error configUSE_PORT_OPTIMISED_TASK_SELECTION can only be set to 1 when configMAX_PRIORITIES is less than or equal to 32.  It is very rare that a system requires more than 10 to 15 difference priorities as tasks that share a priority will time slice.
	#endif

	

#define portRECORD_READY_PRIORITY( uxPriority, uxReadyPriorities ) ( uxReadyPriorities ) |= ( 1UL << ( uxPriority ) )

#define portRESET_READY_PRIORITY( uxPriority, uxReadyPriorities ) ( uxReadyPriorities ) &= ~( 1UL << ( uxPriority ) )

	


#define portGET_HIGHEST_PRIORITY( uxTopPriority, uxReadyPriorities ) uxTopPriority = ( 31 - ucPortCountLeadingZeros( ( uxReadyPriorities ) ) )

#endif 




	void vPortValidateInterruptPriority( void );

#define portASSERT_IF_INTERRUPT_PRIORITY_INVALID() 	vPortValidateInterruptPriority()




#define portNOP()








#if portBYTE_ALIGNMENT == 8

#define portBYTE_ALIGNMENT_MASK ( 0x0007 )
#endif

#if portBYTE_ALIGNMENT == 4

#define portBYTE_ALIGNMENT_MASK	( 0x0003 )
#endif

#if portBYTE_ALIGNMENT == 2

#define portBYTE_ALIGNMENT_MASK	( 0x0001 )
#endif

#if portBYTE_ALIGNMENT == 1

#define portBYTE_ALIGNMENT_MASK	( 0x0000 )
#endif





#define portNUM_CONFIGURABLE_REGIONS 1




 



#define MPU_WRAPPERS_H





#define PRIVILEGED_FUNCTION

#define PRIVILEGED_DATA

#define portUSING_MPU_WRAPPERS 0









#if( portUSING_MPU_WRAPPERS == 1 )
	StackType_t *pxPortInitialiseStack( StackType_t *pxTopOfStack, TaskFunction_t pxCode, void *pvParameters, BaseType_t xRunPrivileged ) PRIVILEGED_FUNCTION;
#else
	StackType_t *pxPortInitialiseStack( StackType_t *pxTopOfStack, TaskFunction_t pxCode, void *pvParameters ) PRIVILEGED_FUNCTION;
#endif


typedef struct HeapRegion
{
	uint8_t *pucStartAddress;
	size_t xSizeInBytes;
} HeapRegion_t;


void vPortDefineHeapRegions( const HeapRegion_t * const pxHeapRegions );



void *pvPortMalloc( size_t xSize ) PRIVILEGED_FUNCTION;
void vPortFree( void *pv ) PRIVILEGED_FUNCTION;
void vPortInitialiseBlocks( void ) PRIVILEGED_FUNCTION;
size_t xPortGetFreeHeapSize( void ) PRIVILEGED_FUNCTION;
size_t xPortGetMinimumEverFreeHeapSize( void ) PRIVILEGED_FUNCTION;


BaseType_t xPortStartScheduler( void ) PRIVILEGED_FUNCTION;


void vPortEndScheduler( void ) PRIVILEGED_FUNCTION;


#if( portUSING_MPU_WRAPPERS == 1 )
	struct xMEMORY_REGION;
	void vPortStoreTaskMPUSettings( xMPU_SETTINGS *xMPUSettings, const struct xMEMORY_REGION * const xRegions, StackType_t *pxBottomOfStack, uint16_t usStackDepth ) PRIVILEGED_FUNCTION;
#endif





































#if configUSE_CO_ROUTINES != 0
	
#endif



#define INCLUDE_xTaskGetIdleTaskHandle 0




#define INCLUDE_xTimerGetTimerDaemonTaskHandle 0




#define INCLUDE_xQueueGetMutexHolder 0




#define INCLUDE_xSemaphoreGetMutexHolder INCLUDE_xQueueGetMutexHolder






#define configUSE_APPLICATION_TASK_TAG 0




#define configNUM_THREAD_LOCAL_STORAGE_POINTERS 0






#define INCLUDE_eTaskGetState 0








#define configUSE_TIMERS 0








#define portCRITICAL_NESTING_IN_TCB 0






#if configMAX_TASK_NAME_LEN < 1
	#error configMAX_TASK_NAME_LEN must be set to a minimum of 1 in FreeRTOSConfig.h
#endif





#define INCLUDE_xEventGroupSetBitFromISR 0




#define INCLUDE_xTimerPendFunctionCall 0




#define configASSERT_DEFINED 1



#if configUSE_TIMERS == 1

	

	

	

#endif 





#define INCLUDE_xTaskGetCurrentTaskHandle 0









#define portCLEAN_UP_TCB( pxTCB ) ( void ) pxTCB




#define portPRE_TASK_DELETE_HOOK( pvTaskToDelete, pxYieldPending )




#define portSETUP_TCB( pxTCB ) ( void ) pxTCB




#if ( configQUEUE_REGISTRY_SIZE < 1 )

#define vQueueAddToRegistry( xQueue, pcName )

#define vQueueUnregisterQueue( xQueue )
#endif



#define portPOINTER_SIZE_TYPE uint32_t




	

#define traceSTART()



	

#define traceEND()



	

#define traceTASK_SWITCHED_IN()



	

#define traceINCREASE_TICK_COUNT( x )



	

#define traceLOW_POWER_IDLE_BEGIN()



	

#define traceLOW_POWER_IDLE_END()



	

#define traceTASK_SWITCHED_OUT()



	

#define traceTASK_PRIORITY_INHERIT( pxTCBOfMutexHolder, uxInheritedPriority )



	

#define traceTASK_PRIORITY_DISINHERIT( pxTCBOfMutexHolder, uxOriginalPriority )



	

#define traceBLOCKING_ON_QUEUE_RECEIVE( pxQueue )



	

#define traceBLOCKING_ON_QUEUE_SEND( pxQueue )








#define traceMOVED_TASK_TO_READY_STATE( pxTCB )




#define traceQUEUE_CREATE( pxNewQueue )




#define traceQUEUE_CREATE_FAILED( ucQueueType )




#define traceCREATE_MUTEX( pxNewQueue )




#define traceCREATE_MUTEX_FAILED()




#define traceGIVE_MUTEX_RECURSIVE( pxMutex )




#define traceGIVE_MUTEX_RECURSIVE_FAILED( pxMutex )




#define traceTAKE_MUTEX_RECURSIVE( pxMutex )




#define traceTAKE_MUTEX_RECURSIVE_FAILED( pxMutex )




#define traceCREATE_COUNTING_SEMAPHORE()




#define traceCREATE_COUNTING_SEMAPHORE_FAILED()




#define traceQUEUE_SEND( pxQueue )




#define traceQUEUE_SEND_FAILED( pxQueue )




#define traceQUEUE_RECEIVE( pxQueue )




#define traceQUEUE_PEEK( pxQueue )




#define traceQUEUE_PEEK_FROM_ISR( pxQueue )




#define traceQUEUE_RECEIVE_FAILED( pxQueue )




#define traceQUEUE_SEND_FROM_ISR( pxQueue )




#define traceQUEUE_SEND_FROM_ISR_FAILED( pxQueue )




#define traceQUEUE_RECEIVE_FROM_ISR( pxQueue )




#define traceQUEUE_RECEIVE_FROM_ISR_FAILED( pxQueue )




#define traceQUEUE_PEEK_FROM_ISR_FAILED( pxQueue )




#define traceQUEUE_DELETE( pxQueue )




#define traceTASK_CREATE( pxNewTCB )




#define traceTASK_CREATE_FAILED()




#define traceTASK_DELETE( pxTaskToDelete )




#define traceTASK_DELAY_UNTIL()




#define traceTASK_DELAY()




#define traceTASK_PRIORITY_SET( pxTask, uxNewPriority )




#define traceTASK_SUSPEND( pxTaskToSuspend )




#define traceTASK_RESUME( pxTaskToResume )




#define traceTASK_RESUME_FROM_ISR( pxTaskToResume )




#define traceTASK_INCREMENT_TICK( xTickCount )




#define traceTIMER_CREATE( pxNewTimer )




#define traceTIMER_CREATE_FAILED()




#define traceTIMER_COMMAND_SEND( xTimer, xMessageID, xMessageValueValue, xReturn )




#define traceTIMER_EXPIRED( pxTimer )




#define traceTIMER_COMMAND_RECEIVED( pxTimer, xMessageID, xMessageValue )




#define traceMALLOC( pvAddress, uiSize )




#define traceFREE( pvAddress, uiSize )




#define traceEVENT_GROUP_CREATE( xEventGroup )




#define traceEVENT_GROUP_CREATE_FAILED()




#define traceEVENT_GROUP_SYNC_BLOCK( xEventGroup, uxBitsToSet, uxBitsToWaitFor )




#define traceEVENT_GROUP_SYNC_END( xEventGroup, uxBitsToSet, uxBitsToWaitFor, xTimeoutOccurred ) ( void ) xTimeoutOccurred




#define traceEVENT_GROUP_WAIT_BITS_BLOCK( xEventGroup, uxBitsToWaitFor )




#define traceEVENT_GROUP_WAIT_BITS_END( xEventGroup, uxBitsToWaitFor, xTimeoutOccurred ) ( void ) xTimeoutOccurred




#define traceEVENT_GROUP_CLEAR_BITS( xEventGroup, uxBitsToClear )




#define traceEVENT_GROUP_CLEAR_BITS_FROM_ISR( xEventGroup, uxBitsToClear )




#define traceEVENT_GROUP_SET_BITS( xEventGroup, uxBitsToSet )




#define traceEVENT_GROUP_SET_BITS_FROM_ISR( xEventGroup, uxBitsToSet )




#define traceEVENT_GROUP_DELETE( xEventGroup )




#define tracePEND_FUNC_CALL(xFunctionToPend, pvParameter1, ulParameter2, ret)




#define tracePEND_FUNC_CALL_FROM_ISR(xFunctionToPend, pvParameter1, ulParameter2, ret)




#define traceQUEUE_REGISTRY_ADD(xQueue, pcQueueName)




#if ( configGENERATE_RUN_TIME_STATS == 1 )

	
 #error If configGENERATE_RUN_TIME_STATS is defined then portCONFIGURE_TIMER_FOR_RUN_TIME_STATS must also be defined.  portCONFIGURE_TIMER_FOR_RUN_TIME_STATS should call a port layer function to setup a peripheral timer/counter that can then be used as the run time counter time base.
	

	
 
 	#error If configGENERATE_RUN_TIME_STATS is defined then either portGET_RUN_TIME_COUNTER_VALUE or portALT_GET_RUN_TIME_COUNTER_VALUE must also be defined.  See the examples provided and the FreeRTOS web site for more information.
 
	

#endif 



#define portCONFIGURE_TIMER_FOR_RUN_TIME_STATS()






#define portPRIVILEGE_BIT ( ( UBaseType_t ) 0x00 )




#define portYIELD_WITHIN_API portYIELD




#define pvPortMallocAligned( x, puxStackBuffer ) ( ( ( puxStackBuffer ) == NULL ) ? ( pvPortMalloc( ( x ) ) ) : ( puxStackBuffer ) )




#define vPortFreeAligned( pvBlockToFree ) vPortFree( pvBlockToFree )






#define configEXPECTED_IDLE_TIME_BEFORE_SLEEP 2


#if configEXPECTED_IDLE_TIME_BEFORE_SLEEP < 2
	#error configEXPECTED_IDLE_TIME_BEFORE_SLEEP must not be less than 2
#endif



#define configUSE_TICKLESS_IDLE 0




#define configPRE_SLEEP_PROCESSING( x )




#define configPOST_SLEEP_PROCESSING( x )




#define configUSE_QUEUE_SETS 0




#define portTASK_USES_FLOATING_POINT()




#define configUSE_TIME_SLICING 1




#define configINCLUDE_APPLICATION_DEFINED_PRIVILEGED_FUNCTIONS 0




#define configUSE_NEWLIB_REENTRANT 0










#define mtCOVERAGE_TEST_MARKER()




#define mtCOVERAGE_TEST_DELAY()




#define portASSERT_IF_IN_ISR()






#define configAPPLICATION_ALLOCATED_HEAP 0




#define configUSE_TASK_NOTIFICATIONS 1




#if( portTICK_TYPE_IS_ATOMIC == 0 )
	

#define portTICK_TYPE_ENTER_CRITICAL() portENTER_CRITICAL()

#define portTICK_TYPE_EXIT_CRITICAL() portEXIT_CRITICAL()

#define portTICK_TYPE_SET_INTERRUPT_MASK_FROM_ISR() portSET_INTERRUPT_MASK_FROM_ISR()

#define portTICK_TYPE_CLEAR_INTERRUPT_MASK_FROM_ISR( x ) portCLEAR_INTERRUPT_MASK_FROM_ISR( ( x ) )
#else
	

#define portTICK_TYPE_ENTER_CRITICAL()

#define portTICK_TYPE_EXIT_CRITICAL()

#define portTICK_TYPE_SET_INTERRUPT_MASK_FROM_ISR() 0

#define portTICK_TYPE_CLEAR_INTERRUPT_MASK_FROM_ISR( x ) ( void ) x
#endif




#define configENABLE_BACKWARD_COMPATIBILITY 1


#if configENABLE_BACKWARD_COMPATIBILITY == 1

#define eTaskStateGet eTaskGetState

#define portTickType TickType_t

#define xTaskHandle TaskHandle_t

#define xQueueHandle QueueHandle_t

#define xSemaphoreHandle SemaphoreHandle_t

#define xQueueSetHandle QueueSetHandle_t

#define xQueueSetMemberHandle QueueSetMemberHandle_t

#define xTimeOutType TimeOut_t

#define xMemoryRegion MemoryRegion_t

#define xTaskParameters TaskParameters_t

#define xTaskStatusType	TaskStatus_t

#define xTimerHandle TimerHandle_t

#define xCoRoutineHandle CoRoutineHandle_t

#define pdTASK_HOOK_CODE TaskHookFunction_t

#define portTICK_RATE_MS portTICK_PERIOD_MS

	

#define tmrTIMER_CALLBACK TimerCallbackFunction_t

#define pdTASK_CODE TaskFunction_t

#define xListItem ListItem_t

#define xList List_t
#endif 






 



#define SEMAPHORE_H



 




#define QUEUE_H







typedef void * QueueHandle_t;


typedef void * QueueSetHandle_t;


typedef void * QueueSetMemberHandle_t;



#define queueSEND_TO_BACK ( ( BaseType_t ) 0 )

#define queueSEND_TO_FRONT ( ( BaseType_t ) 1 )

#define queueOVERWRITE 	( ( BaseType_t ) 2 )



#define queueQUEUE_TYPE_BASE  ( ( uint8_t ) 0U )

#define queueQUEUE_TYPE_SET  	( ( uint8_t ) 0U )

#define queueQUEUE_TYPE_MUTEX   ( ( uint8_t ) 1U )

#define queueQUEUE_TYPE_COUNTING_SEMAPHORE	( ( uint8_t ) 2U )

#define queueQUEUE_TYPE_BINARY_SEMAPHORE	( ( uint8_t ) 3U )

#define queueQUEUE_TYPE_RECURSIVE_MUTEX ( ( uint8_t ) 4U )



#define xQueueCreate( uxQueueLength, uxItemSize ) xQueueGenericCreate( uxQueueLength, uxItemSize, queueQUEUE_TYPE_BASE )



#define xQueueSendToFront( xQueue, pvItemToQueue, xTicksToWait ) xQueueGenericSend( ( xQueue ), ( pvItemToQueue ), ( xTicksToWait ), queueSEND_TO_FRONT )



#define xQueueSendToBack( xQueue, pvItemToQueue, xTicksToWait ) xQueueGenericSend( ( xQueue ), ( pvItemToQueue ), ( xTicksToWait ), queueSEND_TO_BACK )



#define xQueueSend( xQueue, pvItemToQueue, xTicksToWait ) xQueueGenericSend( ( xQueue ), ( pvItemToQueue ), ( xTicksToWait ), queueSEND_TO_BACK )



#define xQueueOverwrite( xQueue, pvItemToQueue ) xQueueGenericSend( ( xQueue ), ( pvItemToQueue ), 0, queueOVERWRITE )



BaseType_t xQueueGenericSend( QueueHandle_t xQueue, const void * const pvItemToQueue, TickType_t xTicksToWait, const BaseType_t xCopyPosition ) PRIVILEGED_FUNCTION;



#define xQueuePeek( xQueue, pvBuffer, xTicksToWait ) xQueueGenericReceive( ( xQueue ), ( pvBuffer ), ( xTicksToWait ), pdTRUE )


BaseType_t xQueuePeekFromISR( QueueHandle_t xQueue, void * const pvBuffer ) PRIVILEGED_FUNCTION;



#define xQueueReceive( xQueue, pvBuffer, xTicksToWait ) xQueueGenericReceive( ( xQueue ), ( pvBuffer ), ( xTicksToWait ), pdFALSE )



BaseType_t xQueueGenericReceive( QueueHandle_t xQueue, void * const pvBuffer, TickType_t xTicksToWait, const BaseType_t xJustPeek ) PRIVILEGED_FUNCTION;


UBaseType_t uxQueueMessagesWaiting( const QueueHandle_t xQueue ) PRIVILEGED_FUNCTION;


UBaseType_t uxQueueSpacesAvailable( const QueueHandle_t xQueue ) PRIVILEGED_FUNCTION;


void vQueueDelete( QueueHandle_t xQueue ) PRIVILEGED_FUNCTION;



#define xQueueSendToFrontFromISR( xQueue, pvItemToQueue, pxHigherPriorityTaskWoken ) xQueueGenericSendFromISR( ( xQueue ), ( pvItemToQueue ), ( pxHigherPriorityTaskWoken ), queueSEND_TO_FRONT )




#define xQueueSendToBackFromISR( xQueue, pvItemToQueue, pxHigherPriorityTaskWoken ) xQueueGenericSendFromISR( ( xQueue ), ( pvItemToQueue ), ( pxHigherPriorityTaskWoken ), queueSEND_TO_BACK )



#define xQueueOverwriteFromISR( xQueue, pvItemToQueue, pxHigherPriorityTaskWoken ) xQueueGenericSendFromISR( ( xQueue ), ( pvItemToQueue ), ( pxHigherPriorityTaskWoken ), queueOVERWRITE )



#define xQueueSendFromISR( xQueue, pvItemToQueue, pxHigherPriorityTaskWoken ) xQueueGenericSendFromISR( ( xQueue ), ( pvItemToQueue ), ( pxHigherPriorityTaskWoken ), queueSEND_TO_BACK )


BaseType_t xQueueGenericSendFromISR( QueueHandle_t xQueue, const void * const pvItemToQueue, BaseType_t * const pxHigherPriorityTaskWoken, const BaseType_t xCopyPosition ) PRIVILEGED_FUNCTION;
BaseType_t xQueueGiveFromISR( QueueHandle_t xQueue, BaseType_t * const pxHigherPriorityTaskWoken ) PRIVILEGED_FUNCTION;


BaseType_t xQueueReceiveFromISR( QueueHandle_t xQueue, void * const pvBuffer, BaseType_t * const pxHigherPriorityTaskWoken ) PRIVILEGED_FUNCTION;


BaseType_t xQueueIsQueueEmptyFromISR( const QueueHandle_t xQueue ) PRIVILEGED_FUNCTION;
BaseType_t xQueueIsQueueFullFromISR( const QueueHandle_t xQueue ) PRIVILEGED_FUNCTION;
UBaseType_t uxQueueMessagesWaitingFromISR( const QueueHandle_t xQueue ) PRIVILEGED_FUNCTION;



BaseType_t xQueueAltGenericSend( QueueHandle_t xQueue, const void * const pvItemToQueue, TickType_t xTicksToWait, BaseType_t xCopyPosition );
BaseType_t xQueueAltGenericReceive( QueueHandle_t xQueue, void * const pvBuffer, TickType_t xTicksToWait, BaseType_t xJustPeeking );

#define xQueueAltSendToFront( xQueue, pvItemToQueue, xTicksToWait ) xQueueAltGenericSend( ( xQueue ), ( pvItemToQueue ), ( xTicksToWait ), queueSEND_TO_FRONT )

#define xQueueAltSendToBack( xQueue, pvItemToQueue, xTicksToWait ) xQueueAltGenericSend( ( xQueue ), ( pvItemToQueue ), ( xTicksToWait ), queueSEND_TO_BACK )

#define xQueueAltReceive( xQueue, pvBuffer, xTicksToWait ) xQueueAltGenericReceive( ( xQueue ), ( pvBuffer ), ( xTicksToWait ), pdFALSE )

#define xQueueAltPeek( xQueue, pvBuffer, xTicksToWait ) xQueueAltGenericReceive( ( xQueue ), ( pvBuffer ), ( xTicksToWait ), pdTRUE )


BaseType_t xQueueCRSendFromISR( QueueHandle_t xQueue, const void *pvItemToQueue, BaseType_t xCoRoutinePreviouslyWoken );
BaseType_t xQueueCRReceiveFromISR( QueueHandle_t xQueue, void *pvBuffer, BaseType_t *pxTaskWoken );
BaseType_t xQueueCRSend( QueueHandle_t xQueue, const void *pvItemToQueue, TickType_t xTicksToWait );
BaseType_t xQueueCRReceive( QueueHandle_t xQueue, void *pvBuffer, TickType_t xTicksToWait );


QueueHandle_t xQueueCreateMutex( const uint8_t ucQueueType ) PRIVILEGED_FUNCTION;
QueueHandle_t xQueueCreateCountingSemaphore( const UBaseType_t uxMaxCount, const UBaseType_t uxInitialCount ) PRIVILEGED_FUNCTION;
void* xQueueGetMutexHolder( QueueHandle_t xSemaphore ) PRIVILEGED_FUNCTION;


BaseType_t xQueueTakeMutexRecursive( QueueHandle_t xMutex, TickType_t xTicksToWait ) PRIVILEGED_FUNCTION;
BaseType_t xQueueGiveMutexRecursive( QueueHandle_t pxMutex ) PRIVILEGED_FUNCTION;



#define xQueueReset( xQueue ) xQueueGenericReset( xQueue, pdFALSE )


#if configQUEUE_REGISTRY_SIZE > 0
	void vQueueAddToRegistry( QueueHandle_t xQueue, const char *pcName ) PRIVILEGED_FUNCTION; 
#endif


#if configQUEUE_REGISTRY_SIZE > 0
	void vQueueUnregisterQueue( QueueHandle_t xQueue ) PRIVILEGED_FUNCTION;
#endif


QueueHandle_t xQueueGenericCreate( const UBaseType_t uxQueueLength, const UBaseType_t uxItemSize, const uint8_t ucQueueType ) PRIVILEGED_FUNCTION;


QueueSetHandle_t xQueueCreateSet( const UBaseType_t uxEventQueueLength ) PRIVILEGED_FUNCTION;


BaseType_t xQueueAddToSet( QueueSetMemberHandle_t xQueueOrSemaphore, QueueSetHandle_t xQueueSet ) PRIVILEGED_FUNCTION;


BaseType_t xQueueRemoveFromSet( QueueSetMemberHandle_t xQueueOrSemaphore, QueueSetHandle_t xQueueSet ) PRIVILEGED_FUNCTION;


QueueSetMemberHandle_t xQueueSelectFromSet( QueueSetHandle_t xQueueSet, const TickType_t xTicksToWait ) PRIVILEGED_FUNCTION;


QueueSetMemberHandle_t xQueueSelectFromSetFromISR( QueueSetHandle_t xQueueSet ) PRIVILEGED_FUNCTION;


void vQueueWaitForMessageRestricted( QueueHandle_t xQueue, TickType_t xTicksToWait ) PRIVILEGED_FUNCTION;
BaseType_t xQueueGenericReset( QueueHandle_t xQueue, BaseType_t xNewQueue ) PRIVILEGED_FUNCTION;
void vQueueSetQueueNumber( QueueHandle_t xQueue, UBaseType_t uxQueueNumber ) PRIVILEGED_FUNCTION;
UBaseType_t uxQueueGetQueueNumber( QueueHandle_t xQueue ) PRIVILEGED_FUNCTION;
uint8_t ucQueueGetQueueType( QueueHandle_t xQueue ) PRIVILEGED_FUNCTION;








typedef QueueHandle_t SemaphoreHandle_t;


#define semBINARY_SEMAPHORE_QUEUE_LENGTH	( ( uint8_t ) 1U )

#define semSEMAPHORE_QUEUE_ITEM_LENGTH ( ( uint8_t ) 0U )

#define semGIVE_BLOCK_TIME  	( ( TickType_t ) 0U )




#define vSemaphoreCreateBinary( xSemaphore )           	\
	{                	\
 ( xSemaphore ) = xQueueGenericCreate( ( UBaseType_t ) 1, semSEMAPHORE_QUEUE_ITEM_LENGTH, queueQUEUE_TYPE_BINARY_SEMAPHORE );	\
 if( ( xSemaphore ) != NULL )            	\
 {                \
 	( void ) xSemaphoreGive( ( xSemaphore ) );          	\
 }                \
	}



#define xSemaphoreCreateBinary() xQueueGenericCreate( ( UBaseType_t ) 1, semSEMAPHORE_QUEUE_ITEM_LENGTH, queueQUEUE_TYPE_BINARY_SEMAPHORE )



#define xSemaphoreTake( xSemaphore, xBlockTime ) xQueueGenericReceive( ( QueueHandle_t ) ( xSemaphore ), NULL, ( xBlockTime ), pdFALSE )



#define xSemaphoreTakeRecursive( xMutex, xBlockTime )	xQueueTakeMutexRecursive( ( xMutex ), ( xBlockTime ) )




#define xSemaphoreAltTake( xSemaphore, xBlockTime ) xQueueAltGenericReceive( ( QueueHandle_t ) ( xSemaphore ), NULL, ( xBlockTime ), pdFALSE )



#define xSemaphoreGive( xSemaphore ) xQueueGenericSend( ( QueueHandle_t ) ( xSemaphore ), NULL, semGIVE_BLOCK_TIME, queueSEND_TO_BACK )



#define xSemaphoreGiveRecursive( xMutex )	xQueueGiveMutexRecursive( ( xMutex ) )



#define xSemaphoreAltGive( xSemaphore ) xQueueAltGenericSend( ( QueueHandle_t ) ( xSemaphore ), NULL, semGIVE_BLOCK_TIME, queueSEND_TO_BACK )



#define xSemaphoreGiveFromISR( xSemaphore, pxHigherPriorityTaskWoken )	xQueueGiveFromISR( ( QueueHandle_t ) ( xSemaphore ), ( pxHigherPriorityTaskWoken ) )



#define xSemaphoreTakeFromISR( xSemaphore, pxHigherPriorityTaskWoken )	xQueueReceiveFromISR( ( QueueHandle_t ) ( xSemaphore ), NULL, ( pxHigherPriorityTaskWoken ) )



#define xSemaphoreCreateMutex() xQueueCreateMutex( queueQUEUE_TYPE_MUTEX )




#define xSemaphoreCreateRecursiveMutex() xQueueCreateMutex( queueQUEUE_TYPE_RECURSIVE_MUTEX )



#define xSemaphoreCreateCounting( uxMaxCount, uxInitialCount ) xQueueCreateCountingSemaphore( ( uxMaxCount ), ( uxInitialCount ) )



#define vSemaphoreDelete( xSemaphore ) vQueueDelete( ( QueueHandle_t ) ( xSemaphore ) )



#define xSemaphoreGetMutexHolder( xSemaphore ) xQueueGetMutexHolder( ( xSemaphore ) )






#define IMUTEXCOMMON_INCLUDED
namespace pearlrt {

   class MutexCommon {
   protected:

      const char * id;
   public:

      MutexCommon();

      void name(const char * s);

      virtual void lock() = 0;

      virtual void unlock() = 0;
   };
}
namespace pearlrt {

   class Mutex : public MutexCommon {
   private:
      xSemaphoreHandle mutex;
   public:

      Mutex();

      ~Mutex();

      void lock();

      void unlock();
   };
}

namespace pearlrt {

   class Log {
   public:

      enum LogLevel {DEBUG = 1, INFO = 2, WARN = 4, ERROR = 8};
   private:
      Log();

      static Log* getInstance();
      static int logFileHandle;
      static int logLevel;
      static bool initialized;
      static Mutex mutex;

      static void doit(const Character<7>& type, const char * format,
                       va_list args);
   public:

      static void info(const char * format, ...)
      __attribute__((format(printf, 1, 2)));

      static void error(const char * format, ...)
      __attribute__((format(printf, 1, 2)));

      static void warn(const char * format, ...)
      __attribute__((format(printf, 1, 2)));

      static void debug(const char * format, ...)
      __attribute__((format(printf, 1, 2)));

      static void exit();

      static void setLevel(int level);
   };
}
namespace pearlrt {

   template<int S> class Bits;

   template<> class Bits<1> {
   public:

      typedef uint8_t BitType;
   };

   template<> class Bits<2> {
   public:

      typedef uint16_t BitType;
   };

   template<> class Bits<4> {
   public:

      typedef uint32_t BitType;
   };

   template<> class Bits<8> {
   public:

      typedef uint64_t BitType;
   };


   template<int S> class BitString {
   private:

      static const int len = NumberOfBytes<S>::resultBitString;
   public:

      typedef typename Bits<len>::BitType DataType;

      DataType x;
   private:
      static const int shiftSize = sizeof(x) * 8 - S;
      static const DataType mask = ((DataType)(-1) >> shiftSize)
                                   << shiftSize;
   public:

      BitString() {
         x = 0;
      }

      BitString(DataType y) {
         x = y;
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
         if (l.x > 0) {
            retval.x <<= l.x;
            retval.x |= (x >> (S - l.x));
         } else {
            retval.x >>= -l.x;
            retval.x |= x << (S + l.x);
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
         BitString < S + P > z;
         z.x = x << ((sizeof(z) - sizeof(x)) * 8);
         z.x |= y.x << ((sizeof(z) - sizeof(x)) * 8 - S);
         return z;
      }

      template <int P> BitString<P>
      getSlice(const Fixed<15>start, BitString<P> dummy) {
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

      Fixed<S> toFixed() const {
         Fixed<S> returnValue;
         returnValue.x = x >> shiftSize;
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
#error "Float.h needs IEEE753 support"
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
   };
}

#define TASKCOMMON_INCLUDED
namespace pearlrt {
   class TaskCommon;
   class Semaphore;

   enum BlockReason {
      NOTBLOCKED,
      REQUEST
   };

   struct BlockData {
      BlockReason reason;

      union BlockReasons {

         struct BlockSema {
            int nsemas;
            Semaphore **semas;
         } sema;
      } u;
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
      static const UBaseType_t max = 32767;
      SemaphoreHandle_t sem;
      const char * id;
   public:

      CSema(int preset = 0);

      void name(const char * s);

      ~CSema();

      void request();

      void release();
   };
}

namespace pearlrt {
   class UserDation;
}
#define USERDATION_INCLUDED


#define SYSTEMDATION_INCLUDED

#define DATION_INCLUDED

#define DEVICE_INCLUDED
namespace pearlrt {

   class Device {
   };
}


namespace pearlrt {

   class Dation: public Device {
   public:

      enum DationParams {

         IN = 0x0001,
         OUT = 0x0002,
         INOUT = 0x0004,

         IDF = 0x0008,
         ANY = 0x0010, NEW = 0x0020, OLD = 0x0040,

         PRM = 0x0080, CAN = 0x0100,

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
         OPENED = 1, CLOSED = 2
      };

       int dationParams;

      DationStatus dationStatus;

      virtual void dationRead(void * destination, size_t size) = 0;

      virtual void dationWrite(void * destination, size_t size) = 0;

      virtual void dationSeek(const Fixed<31> & p, const int dationParam) {
         Log::error("Dation: no seek implemented");
         throw theDationSeekNotRealizedSignal;
      }
   };
}



namespace pearlrt {


   class SystemDation: public Dation {
   public:

      virtual int capabilities() = 0;

      virtual void dationClose(int closeParam) = 0;
   };
}



#define REFCHAR_INCLUDED



namespace pearlrt {

   class RefCharacter {
   public:
      size_t max;
      size_t current;
      char * data;
   public:

      RefCharacter();

      template<size_t S>
      RefCharacter(Character<S> & rhs) {
         max = rhs.upb().x;
         current = max;
         data = &rhs.data[0];
      }

      template<size_t S>
      void setWork(Character<S> & rhs) {
         max = rhs.upb().x;
         current = max;
         data = &rhs.data[0];
      }

      char getCharAt(size_t index) ;

      char * getCstring() ;

      void clear();

      void fill();

      void add(const RefCharacter & rhs);

      void add(const char * rhs);

      size_t getMax();

      size_t getCurrent();

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

   class UserDation : public Dation {
   private:
      Mutex mutex;
      Fixed<15> * rstValue;
   protected:

      TaskCommon* currentTask;
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
                  throw theIllegalParamSignal;
               }
            }
            if ((!!(p & Dation::IDF)) != (idf != 0)) {
               Log::error("UserDation: ether both or non of IDF and filename");
               throw theIllegalParamSignal;
            }
            if (S > 64) {
               Log::error("filename exceeds 64 characters");
               throw theIllegalParamSignal;
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
                  throw theIllegalParamSignal;
               }
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

      void beginSequence(TaskCommon * me);

      void endSequence();

      void rst(Fixed<15> & rstVar);

      bool updateRst(Signal * s);
   public:

      void suspend();

      void cont();
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
      struct {
         void *cb;
         void *th;
      	  }timer_callback;
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

 




 




#define INC_TASK_H



 







#define LIST_H




#define configLIST_VOLATILE





#if( configUSE_LIST_DATA_INTEGRITY_CHECK_BYTES == 0 )
	

#define listFIRST_LIST_ITEM_INTEGRITY_CHECK_VALUE

#define listSECOND_LIST_ITEM_INTEGRITY_CHECK_VALUE

#define listFIRST_LIST_INTEGRITY_CHECK_VALUE

#define listSECOND_LIST_INTEGRITY_CHECK_VALUE

#define listSET_FIRST_LIST_ITEM_INTEGRITY_CHECK_VALUE( pxItem )

#define listSET_SECOND_LIST_ITEM_INTEGRITY_CHECK_VALUE( pxItem )

#define listSET_LIST_INTEGRITY_CHECK_1_VALUE( pxList )

#define listSET_LIST_INTEGRITY_CHECK_2_VALUE( pxList )

#define listTEST_LIST_ITEM_INTEGRITY( pxItem )

#define listTEST_LIST_INTEGRITY( pxList )
#else
	

#define listFIRST_LIST_ITEM_INTEGRITY_CHECK_VALUE  TickType_t xListItemIntegrityValue1;

#define listSECOND_LIST_ITEM_INTEGRITY_CHECK_VALUE  TickType_t xListItemIntegrityValue2;

#define listFIRST_LIST_INTEGRITY_CHECK_VALUE  	TickType_t xListIntegrityValue1;

#define listSECOND_LIST_INTEGRITY_CHECK_VALUE  	TickType_t xListIntegrityValue2;

	

#define listSET_FIRST_LIST_ITEM_INTEGRITY_CHECK_VALUE( pxItem ) ( pxItem )->xListItemIntegrityValue1 = pdINTEGRITY_CHECK_VALUE

#define listSET_SECOND_LIST_ITEM_INTEGRITY_CHECK_VALUE( pxItem )	( pxItem )->xListItemIntegrityValue2 = pdINTEGRITY_CHECK_VALUE

#define listSET_LIST_INTEGRITY_CHECK_1_VALUE( pxList ) ( pxList )->xListIntegrityValue1 = pdINTEGRITY_CHECK_VALUE

#define listSET_LIST_INTEGRITY_CHECK_2_VALUE( pxList ) ( pxList )->xListIntegrityValue2 = pdINTEGRITY_CHECK_VALUE

	

#define listTEST_LIST_ITEM_INTEGRITY( pxItem ) configASSERT( ( ( pxItem )->xListItemIntegrityValue1 == pdINTEGRITY_CHECK_VALUE ) && ( ( pxItem )->xListItemIntegrityValue2 == pdINTEGRITY_CHECK_VALUE ) )

#define listTEST_LIST_INTEGRITY( pxList ) 	configASSERT( ( ( pxList )->xListIntegrityValue1 == pdINTEGRITY_CHECK_VALUE ) && ( ( pxList )->xListIntegrityValue2 == pdINTEGRITY_CHECK_VALUE ) )
#endif 



struct xLIST_ITEM
{
	listFIRST_LIST_ITEM_INTEGRITY_CHECK_VALUE  
	configLIST_VOLATILE TickType_t xItemValue; 	
	struct xLIST_ITEM * configLIST_VOLATILE pxNext; 
	struct xLIST_ITEM * configLIST_VOLATILE pxPrevious;	
	void * pvOwner;     
	void * configLIST_VOLATILE pvContainer;  
	listSECOND_LIST_ITEM_INTEGRITY_CHECK_VALUE  
};
typedef struct xLIST_ITEM ListItem_t;  	

struct xMINI_LIST_ITEM
{
	listFIRST_LIST_ITEM_INTEGRITY_CHECK_VALUE  
	configLIST_VOLATILE TickType_t xItemValue;
	struct xLIST_ITEM * configLIST_VOLATILE pxNext;
	struct xLIST_ITEM * configLIST_VOLATILE pxPrevious;
};
typedef struct xMINI_LIST_ITEM MiniListItem_t;


typedef struct xLIST
{
	listFIRST_LIST_INTEGRITY_CHECK_VALUE  
	configLIST_VOLATILE UBaseType_t uxNumberOfItems;
	ListItem_t * configLIST_VOLATILE pxIndex; 
	MiniListItem_t xListEnd;   
	listSECOND_LIST_INTEGRITY_CHECK_VALUE  
} List_t;



#define listSET_LIST_ITEM_OWNER( pxListItem, pxOwner ) ( ( pxListItem )->pvOwner = ( void * ) ( pxOwner ) )



#define listGET_LIST_ITEM_OWNER( pxListItem )	( ( pxListItem )->pvOwner )



#define listSET_LIST_ITEM_VALUE( pxListItem, xValue )	( ( pxListItem )->xItemValue = ( xValue ) )



#define listGET_LIST_ITEM_VALUE( pxListItem )	( ( pxListItem )->xItemValue )



#define listGET_ITEM_VALUE_OF_HEAD_ENTRY( pxList )	( ( ( pxList )->xListEnd ).pxNext->xItemValue )



#define listGET_HEAD_ENTRY( pxList )	( ( ( pxList )->xListEnd ).pxNext )



#define listGET_NEXT( pxListItem )	( ( pxListItem )->pxNext )



#define listGET_END_MARKER( pxList )	( ( ListItem_t const * ) ( &( ( pxList )->xListEnd ) ) )



#define listLIST_IS_EMPTY( pxList )	( ( BaseType_t ) ( ( pxList )->uxNumberOfItems == ( UBaseType_t ) 0 ) )



#define listCURRENT_LIST_LENGTH( pxList )	( ( pxList )->uxNumberOfItems )



#define listGET_OWNER_OF_NEXT_ENTRY( pxTCB, pxList )     \
{           	\
List_t * const pxConstList = ( pxList );      	\
  	\
    \
	( pxConstList )->pxIndex = ( pxConstList )->pxIndex->pxNext;   	\
	if( ( void * ) ( pxConstList )->pxIndex == ( void * ) &( ( pxConstList )->xListEnd ) )	\
	{           \
 ( pxConstList )->pxIndex = ( pxConstList )->pxIndex->pxNext;   \
	}           \
	( pxTCB ) = ( pxConstList )->pxIndex->pvOwner;     	\
}




#define listGET_OWNER_OF_HEAD_ENTRY( pxList )  ( (&( ( pxList )->xListEnd ))->pxNext->pvOwner )



#define listIS_CONTAINED_WITHIN( pxList, pxListItem ) ( ( BaseType_t ) ( ( pxListItem )->pvContainer == ( void * ) ( pxList ) ) )



#define listLIST_ITEM_CONTAINER( pxListItem ) ( ( pxListItem )->pvContainer )



#define listLIST_IS_INITIALISED( pxList ) ( ( pxList )->xListEnd.xItemValue == portMAX_DELAY )


void vListInitialise( List_t * const pxList );


void vListInitialiseItem( ListItem_t * const pxItem );


void vListInsert( List_t * const pxList, ListItem_t * const pxNewListItem );


void vListInsertEnd( List_t * const pxList, ListItem_t * const pxNewListItem );


UBaseType_t uxListRemove( ListItem_t * const pxItemToRemove );












#define tskKERNEL_VERSION_NUMBER "V8.2.1"

#define tskKERNEL_VERSION_MAJOR 8

#define tskKERNEL_VERSION_MINOR 2

#define tskKERNEL_VERSION_BUILD 1


typedef void * TaskHandle_t;


typedef BaseType_t (*TaskHookFunction_t)( void * );


typedef enum
{
	eRunning = 0,	
	eReady, 	
	eBlocked, 
	eSuspended, 
	eDeleted 
} eTaskState;


typedef enum
{
	eNoAction = 0,  
	eSetBits,  	
	eIncrement,  	
	eSetValueWithOverwrite, 
	eSetValueWithoutOverwrite	
} eNotifyAction;


typedef struct xTIME_OUT
{
	BaseType_t xOverflowCount;
	TickType_t xTimeOnEntering;
} TimeOut_t;


typedef struct xMEMORY_REGION
{
	void *pvBaseAddress;
	uint32_t ulLengthInBytes;
	uint32_t ulParameters;
} MemoryRegion_t;


typedef struct xTASK_PARAMETERS
{
	TaskFunction_t pvTaskCode;
	const char * const pcName;	
	uint16_t usStackDepth;
	void *pvParameters;
	UBaseType_t uxPriority;
	StackType_t *puxStackBuffer;
	MemoryRegion_t xRegions[ portNUM_CONFIGURABLE_REGIONS ];
} TaskParameters_t;


typedef struct xTASK_STATUS
{
	TaskHandle_t xHandle; 	
	const char *pcTaskName; 	 
	UBaseType_t xTaskNumber; 
	eTaskState eCurrentState; 
	UBaseType_t uxCurrentPriority;	
	UBaseType_t uxBasePriority; 
	uint32_t ulRunTimeCounter; 
	uint16_t usStackHighWaterMark;	
} TaskStatus_t;


typedef enum
{
	eAbortSleep = 0, 
	eStandardSleep, 	
	eNoTasksWaitingTimeout	
} eSleepModeStatus;




#define tskIDLE_PRIORITY 	( ( UBaseType_t ) 0U )



#define taskYIELD()  	portYIELD()



#define taskENTER_CRITICAL() portENTER_CRITICAL()

#define taskENTER_CRITICAL_FROM_ISR( x ) portSET_INTERRUPT_MASK_FROM_ISR( x )



#define taskEXIT_CRITICAL() 	portEXIT_CRITICAL()

#define taskEXIT_CRITICAL_FROM_ISR() portCLEAR_INTERRUPT_MASK_FROM_ISR()


#define taskDISABLE_INTERRUPTS()	portDISABLE_INTERRUPTS()



#define taskENABLE_INTERRUPTS() portENABLE_INTERRUPTS()



#define taskSCHEDULER_SUSPENDED ( ( BaseType_t ) 0 )

#define taskSCHEDULER_NOT_STARTED	( ( BaseType_t ) 1 )

#define taskSCHEDULER_RUNNING ( ( BaseType_t ) 2 )






#define xTaskCreate( pvTaskCode, pcName, usStackDepth, pvParameters, uxPriority, pxCreatedTask ) xTaskGenericCreate( ( pvTaskCode ), ( pcName ), ( usStackDepth ), ( pvParameters ), ( uxPriority ), ( pxCreatedTask ), ( NULL ), ( NULL ) )



#define xTaskCreateRestricted( x, pxCreatedTask ) xTaskGenericCreate( ((x)->pvTaskCode), ((x)->pcName), ((x)->usStackDepth), ((x)->pvParameters), ((x)->uxPriority), (pxCreatedTask), ((x)->puxStackBuffer), ((x)->xRegions) )


void vTaskAllocateMPURegions( TaskHandle_t xTask, const MemoryRegion_t * const pxRegions ) PRIVILEGED_FUNCTION;


void vTaskDelete( TaskHandle_t xTaskToDelete ) PRIVILEGED_FUNCTION;




void vTaskDelay( const TickType_t xTicksToDelay ) PRIVILEGED_FUNCTION;


void vTaskDelayUntil( TickType_t * const pxPreviousWakeTime, const TickType_t xTimeIncrement ) PRIVILEGED_FUNCTION;


UBaseType_t uxTaskPriorityGet( TaskHandle_t xTask ) PRIVILEGED_FUNCTION;


UBaseType_t uxTaskPriorityGetFromISR( TaskHandle_t xTask ) PRIVILEGED_FUNCTION;


eTaskState eTaskGetState( TaskHandle_t xTask ) PRIVILEGED_FUNCTION;


void vTaskPrioritySet( TaskHandle_t xTask, UBaseType_t uxNewPriority ) PRIVILEGED_FUNCTION;


void vTaskSuspend( TaskHandle_t xTaskToSuspend ) PRIVILEGED_FUNCTION;


void vTaskResume( TaskHandle_t xTaskToResume ) PRIVILEGED_FUNCTION;


BaseType_t xTaskResumeFromISR( TaskHandle_t xTaskToResume ) PRIVILEGED_FUNCTION;




void vTaskStartScheduler( void ) PRIVILEGED_FUNCTION;


void vTaskEndScheduler( void ) PRIVILEGED_FUNCTION;


void vTaskSuspendAll( void ) PRIVILEGED_FUNCTION;


BaseType_t xTaskResumeAll( void ) PRIVILEGED_FUNCTION;




TickType_t xTaskGetTickCount( void ) PRIVILEGED_FUNCTION;


TickType_t xTaskGetTickCountFromISR( void ) PRIVILEGED_FUNCTION;


UBaseType_t uxTaskGetNumberOfTasks( void ) PRIVILEGED_FUNCTION;


char *pcTaskGetTaskName( TaskHandle_t xTaskToQuery ) PRIVILEGED_FUNCTION; 


UBaseType_t uxTaskGetStackHighWaterMark( TaskHandle_t xTask ) PRIVILEGED_FUNCTION;



	#if configUSE_APPLICATION_TASK_TAG == 1
 
 void vTaskSetApplicationTaskTag( TaskHandle_t xTask, TaskHookFunction_t pxHookFunction ) PRIVILEGED_FUNCTION;

 
 TaskHookFunction_t xTaskGetApplicationTaskTag( TaskHandle_t xTask ) PRIVILEGED_FUNCTION;
	#endif 


#if( configNUM_THREAD_LOCAL_STORAGE_POINTERS > 0 )

	
	void vTaskSetThreadLocalStoragePointer( TaskHandle_t xTaskToSet, BaseType_t xIndex, void *pvValue );
	void *pvTaskGetThreadLocalStoragePointer( TaskHandle_t xTaskToQuery, BaseType_t xIndex );

#endif


BaseType_t xTaskCallApplicationTaskHook( TaskHandle_t xTask, void *pvParameter ) PRIVILEGED_FUNCTION;


TaskHandle_t xTaskGetIdleTaskHandle( void );


UBaseType_t uxTaskGetSystemState( TaskStatus_t * const pxTaskStatusArray, const UBaseType_t uxArraySize, uint32_t * const pulTotalRunTime );


void vTaskList( char * pcWriteBuffer ) PRIVILEGED_FUNCTION; 


void vTaskGetRunTimeStats( char *pcWriteBuffer ) PRIVILEGED_FUNCTION; 


BaseType_t xTaskGenericNotify( TaskHandle_t xTaskToNotify, uint32_t ulValue, eNotifyAction eAction, uint32_t *pulPreviousNotificationValue );

#define xTaskNotify( xTaskToNotify, ulValue, eAction ) xTaskGenericNotify( ( xTaskToNotify ), ( ulValue ), ( eAction ), NULL )

#define xTaskNotifyAndQuery( xTaskToNotify, ulValue, eAction, pulPreviousNotifyValue ) xTaskGenericNotify( ( xTaskToNotify ), ( ulValue ), ( eAction ), ( pulPreviousNotifyValue ) )


BaseType_t xTaskNotifyFromISR( TaskHandle_t xTaskToNotify, uint32_t ulValue, eNotifyAction eAction, BaseType_t *pxHigherPriorityTaskWoken );


BaseType_t xTaskNotifyWait( uint32_t ulBitsToClearOnEntry, uint32_t ulBitsToClearOnExit, uint32_t *pulNotificationValue, TickType_t xTicksToWait );



#define xTaskNotifyGive( xTaskToNotify ) xTaskNotify( ( xTaskToNotify ), 0, eIncrement );


void vTaskNotifyGiveFromISR( TaskHandle_t xTaskToNotify, BaseType_t *pxHigherPriorityTaskWoken );


uint32_t ulTaskNotifyTake( BaseType_t xClearCountOnExit, TickType_t xTicksToWait );




BaseType_t xTaskIncrementTick( void ) PRIVILEGED_FUNCTION;


void vTaskPlaceOnEventList( List_t * const pxEventList, const TickType_t xTicksToWait ) PRIVILEGED_FUNCTION;
void vTaskPlaceOnUnorderedEventList( List_t * pxEventList, const TickType_t xItemValue, const TickType_t xTicksToWait ) PRIVILEGED_FUNCTION;


void vTaskPlaceOnEventListRestricted( List_t * const pxEventList, const TickType_t xTicksToWait ) PRIVILEGED_FUNCTION;


BaseType_t xTaskRemoveFromEventList( const List_t * const pxEventList ) PRIVILEGED_FUNCTION;
BaseType_t xTaskRemoveFromUnorderedEventList( ListItem_t * pxEventListItem, const TickType_t xItemValue ) PRIVILEGED_FUNCTION;


void vTaskSwitchContext( void ) PRIVILEGED_FUNCTION;


TickType_t uxTaskResetEventItemValue( void ) PRIVILEGED_FUNCTION;


TaskHandle_t xTaskGetCurrentTaskHandle( void ) PRIVILEGED_FUNCTION;


void vTaskSetTimeOutState( TimeOut_t * const pxTimeOut ) PRIVILEGED_FUNCTION;


BaseType_t xTaskCheckForTimeOut( TimeOut_t * const pxTimeOut, TickType_t * const pxTicksToWait ) PRIVILEGED_FUNCTION;


void vTaskMissedYield( void ) PRIVILEGED_FUNCTION;


BaseType_t xTaskGetSchedulerState( void ) PRIVILEGED_FUNCTION;


void vTaskPriorityInherit( TaskHandle_t const pxMutexHolder ) PRIVILEGED_FUNCTION;


BaseType_t xTaskPriorityDisinherit( TaskHandle_t const pxMutexHolder ) PRIVILEGED_FUNCTION;


BaseType_t xTaskGenericCreate( TaskFunction_t pxTaskCode, const char * const pcName, const uint16_t usStackDepth, void * const pvParameters, UBaseType_t uxPriority, TaskHandle_t * const pxCreatedTask, StackType_t * const puxStackBuffer, const MemoryRegion_t * const xRegions ) PRIVILEGED_FUNCTION; 


UBaseType_t uxTaskGetTaskNumber( TaskHandle_t xTask ) PRIVILEGED_FUNCTION;


void vTaskSetTaskNumber( TaskHandle_t xTask, const UBaseType_t uxHandle ) PRIVILEGED_FUNCTION;


void vTaskStepTick( const TickType_t xTicksToJump ) PRIVILEGED_FUNCTION;


eSleepModeStatus eTaskConfirmSleepModeStatus( void ) PRIVILEGED_FUNCTION;


void *pvTaskIncrementMutexHeldCount( void );







namespace pearlrt {

   class Task : public TaskCommon {
   public:
      TaskTimer activateTimer;
      TaskTimer continueTimer;
      TaskHandle_t xth;
      unsigned portSHORT stackDepth;
   private:

      Task();
   public:

      Task(char* n, Prio prio, BitString<1> ismain);

      void init();

      void directActivate(const Fixed<15>& prio);

      void continueFromOtherTask(int condition,
                                 Prio prio);
   private:
      void continueSuspended();
   public:

      void resume2();

      void terminateMySelf();

      void terminateFromOtherTask();

      void suspendMySelf();

      void suspendFromOtherTask();

      virtual void task(Task * me) = 0;

      static void tskfunc(void* parm);

      void changeThreadPrio(const Fixed<15>& prio);

      int switchToThreadPrioMax() ;

      void switchToThreadPrioCurrent(int cp) ;

      static void disableScheduling();

      static void enableScheduling();
   };
}

#define GENERICTASK_H_

#define SPCTASK(x) \
    class C_##x : public pearlrt::Task {  \
    public:    \
       C_##x(char *xx, pearlrt::Prio pp, pearlrt::BitString<1> ii):	\
 pearlrt::Task(xx,pp,ii){};   \
       void task(pearlrt::Task * me);  \
    };     	\
    extern class C_##x x
#define DCLTASK(x, prio, ismain) 	\
 C_##x x((char*)#x,prio, ismain); 	\
 void C_##x::task(pearlrt::Task * me)




#define TASKMONITOR_INCLUDED

namespace pearlrt {

   class TaskMonitor {
   private:
      volatile int nbrPendingTasks;
      pid_t pid;
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
      int r = compareChar(lhs, rhs);
      return BitString<1>(r == 0);
   }

   template<size_t S, size_t P>
   BitString<1> operator!= (const Character<S> & lhs,
                            const Character<P> & rhs) {
      int r = compareChar(lhs, rhs);
      return BitString<1>(r != 0);
   }

   template<size_t S, size_t P>
   BitString<1> operator< (const Character<S> & lhs,
                           const Character<P> & rhs) {
      int r = compareChar(lhs, rhs);
      return BitString<1>(r < 0);
   }

   template<size_t S, size_t P>
   BitString<1> operator<= (const Character<S> & lhs,
                            const Character<P> & rhs) {
      int r = compareChar(lhs, rhs);
      return BitString<1>(r <= 0);
   }

   template<size_t S, size_t P>
   BitString<1> operator> (const Character<S> & lhs,
                           const Character<P> & rhs) {
      int r = compareChar(lhs, rhs);
      return BitString<1>(r > 0);
   }

   template<size_t S, size_t P>
   BitString<1> operator>= (const Character<S> & lhs,
                            const Character<P> & rhs) {
      int r = compareChar(lhs, rhs);
      return BitString<1>(r >= 0);
   }
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
         Fixed<31> s,
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
Log::info("value= %d d=%d w= %d s= %d", fixedValue.x, w.x, d.x, s.x);
         postPointDigits = d.x;
         prePointDigits = digits - s.x;
         if (prePointDigits  < 1) {
            prePointDigits = 1;
         }
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

      GetHelper(const Fixed<31> w,
                Source * s);

      int getRemainingWidth();

      int skipSpaces();

      void discardRemaining();

      int readInteger(int * x, const int digits);

      int readFixedInteger(int * x, const int digits);

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

      void readB1(uint64_t * value, const int nbrOfBitsToSample);

      void readFixedByF(Fixed63 * value, int d, int p);
   };
}
namespace pearlrt {

   template <int S>
   class GetFixed {
   public:

      static void fromF(Fixed<S> &f,
                        const Fixed<31> w,
                        const Fixed<31> d,
                        const Fixed<31> p,
                        Source & source) {
         Fixed63 value;
         if (w.x <= 0 || d.x < 0 || w.x < d.x) {
            throw theFixedFormatSignal;
         }
         GetHelper helper(w, &source);
         helper.setDelimiters(GetHelper::EndOfLine);

         helper.readFixedByF(&value, d.x, p.x);
         f = Fixed<S>(value.get());
         return;
      }
   };
}



#define PUTCHARACTER_H_INCLUDED






#define PUTHELPER_H_INCLUDED



namespace pearlrt {

   class PutHelper {
   public:

      static void doPutChar(int length, RefCharacter* rc, Sink * s);
   };
}
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

using namespace std;
namespace pearlrt {

   template<int S> class PutBitString {
   private:

      PutBitString() {}
   public:

      static void toB4(
         BitString<S> &bitstring,
         int w,
         Sink & sink) {
         if (w <= 0) {
            throw theBitFormatSignal;
         }
         typename BitString<S>::DataType h, m;
         int i;
         int shifts;
         char s;
         m = 0x0f;
         shifts = sizeof(bitstring.x) * 8 - 4;
         h = bitstring.x;
         m  <<= shifts;
         shifts = (shifts / 4) * 4;
         for (i = 0; i < w; i++) {
            if (shifts >= 0) {
               s = ((h & m) >> shifts) + '0';
               if (s > '9') {
                  s += 'A' - '9' - 1;
               }
               sink.putChar(s);
               m >>= 4;
               shifts -= 4;
            } else {
               sink.putChar('0');
            }
         }
         return;
      }

      static void toB4(
         BitString<S> &bitstring,
         Sink & sink) {
         int w = (S + 3) / 4;
         toB4(bitstring, w, sink);
      }

      static void toB1(
         BitString<S> &bitstring,
         int w,
         Sink & sink) {
         if (w <= 0) {
            throw theBitFormatSignal;
         }
         typename BitString<S>::DataType h, m;
         int i;
         h = bitstring.x;
         m = 0x01;
         m <<= (sizeof(bitstring.x) * 8 - 1);
         for (i = 0; i < w; i++) {
            if (h) {
               sink.putChar('0' + !!(h & m));
               m >>= 1;
            } else {
               sink.putChar('0');
            }
         }
         return;
      }

      static void toB1(
         BitString<S> &bitstring,
         Sink & sink) {
         toB1(bitstring, S, sink);
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

      static void fromB1(
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
         uint64_t fixedValue;
         helper.readB1(&fixedValue, S);
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
         uint64_t fixedValue;
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
   };
}



#define SYSTEMDATIONNB_INCLUDED




namespace pearlrt {


   class SystemDationNB: public SystemDation {
   public:

      virtual SystemDationNB* dationOpen(const char * idfValue,
                                         int openParam) = 0;

      virtual void dationUnGetChar(const char c) = 0;
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
   };
}

namespace pearlrt {

   class UserDationNB: public UserDation {
   protected:

      enum DationType { ALPHIC, TYPE};
   private:
      static const Fixed<31> one;
      const DationType dationType;
      void fill(const Fixed<31> n, char fillChar);
      void skipX(const Fixed<31> n);
      void skipUntil(const Fixed<31> n, char fillChar);
      void skipAny(const Fixed<31> n);
   protected:

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

      virtual void internalOpen() = 0;

      virtual void internalClose() = 0;
   private:

      void internalDationOpen(int p,
                              RefCharacter* rc);
   public:

      void closeSystemDation(int dationParams);
   protected:

      void assertOpenDirect();

      void assertOpenDirectOrForward();
   };
}



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




namespace pearlrt {


   class DationPG: public UserDationNB {
   private:

      void internalOpen();

      void internalClose();
      SystemDationNBSink  sink;
      SystemDationNBSource   source;

      void doPutChar(int length, RefCharacter * rc);
   public:

      DationPG(SystemDationNB* parent,
               int dationParams,
               DationDim * dimensions);

      void dationRead(void * destination, size_t size);

      void dationWrite(void * destination, size_t size);

      void dationSeek(const Fixed<31> & p, const int dationParam);

      void dationUnGetChar(const char c);

      template<size_t S>
      void toA(Character<S> & s) {
         toA(s, S);
      };

      template<size_t S>
      void fromA(Character<S> & s) {
         fromA(s, S);
      };

      template<size_t S>
      void toA(Character<S> & s, Fixed<31> w) {
         RefCharacter rc;
         rc.setWork(s);

         return doPutChar(w.x, &rc);
      }

      template<size_t S>
      void fromA(Character<S> & s, Fixed<31> w) {
         GetCharacter<S>::fromA(s, w, source);
         return;
      }

      template<int S>
      void toF(Fixed<S> & f,
               const Fixed<31> w,
               const Fixed<31> d = 0,
               const Fixed<31> s = 0) {
         PutFixed<S>::toF(f, w, d, s, sink);
      };

      template<int S>
      void fromF(Fixed<S> & f,
                 const Fixed<31> w,
                 const Fixed<31> d = 0,
                 const Fixed<31> s = 0) {
         GetFixed<S>::fromF(f, w, d, s, source);
      };

      void toT(const Clock f,
               const Fixed<31> w,
               const Fixed<31> d = 0);

      void toD(const Duration& f,
               const Fixed<31> w,
               const Fixed<31> d = 0);

      void fromT(Clock & f,
                 const Fixed<31> w,
                 const Fixed<31> d = 0) ;
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
               const Fixed<15> stepsize);

      void dationRead(void* data, size_t size);

      void dationWrite(void* data, size_t size);
      void dationSeek(const Fixed<31> & p, const int dationParam);

      void dationUnGetChar(const char c);
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
#define SPCSEMA(x) \
   extern pearlrt::Semaphore x
#define DCLSEMA(x,preset) \
   pearlrt::Semaphore x(preset,#x)



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
