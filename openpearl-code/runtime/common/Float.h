// missing in Float.h:
// toFloat(Fixed), ENTIER()--> FIXED, ROUND() -> FIXED
// mit fixed param:SQRT,SIN,cos, exp. ln, tan, atan, tanh,
// mit gemischten parametern  pow



#ifndef FLOAT_H_INCLUDED
#define FLOAT_H_INCLUDED
/*
 [A "BSD license"]
 Copyright (c) 2014-2014 Rainer Mueller
 All rights reserved.

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions
 are met:

 1. Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.
 2. Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in the
    documentation and/or other materials provided with the distribution.
 3. The name of the author may not be used to endorse or promote products
    derived from this software without specific prior written permission.

 THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

/**
\file

\brief The data types Float(x)  only x=24 and x=53 are supported


\author R. Mueller


The data type FLOAT(x) is realized with templates.
Depending on the length of the requested FLOAT-value,
the intrinsic type float (Float<24>) or double (Float<53>) is used.
Other lengths are not supported and will lead to c++-compile errors.

*/
#include <cmath>

#include "FloatHelper.h"
#include "Signals.h"
#include "Fixed.h"

//IncludeCollector::PassThru begin
#ifndef NAN
#error "Float.h needs IEEE754 support"
#endif
//IncludeCollector::PassThru end

namespace pearlrt {

   /**
   the internal type of the Float(X)
   */
   template < int T > struct InternalFloatType;

   /**
   the internal type of the Float(24) is float

   */
   template < > struct InternalFloatType<24> {
      /**
      The internal type of a Float(24) is float
      */
      typedef float InternalType;
   };

   /**
   the internal type of the Float(53) is double
   */
   template < > struct InternalFloatType<53> {
      /**
      The internal type of a Float(24) is double
      */
      typedef double InternalType;
   };

   /**

   \brief the type FLOAT(S)


   Floats are specified with the number of mantissa bits.
   Only 24 bit (float) and 53 bit (double) are provided in this
   implementation.

   \tparam S length of the mantissa. The hidden one is included in this
          calculation!
   */
   template<int S> class Float {
   public:

      /**
      template prototype for the result type 
      of mixed operations
      */
      template<int fixedSize, int floatSize> struct FloatResult;

      /**
      template specialisation for FLOAT(x),FLOAT(24)-operations
      
      - All sizes <= 24 deliver FLOAT(24).
      - All sizes > 24 deliver FLOAT(53).

      */
      template<int fixedSize > struct FloatResult<fixedSize, 24> {
         typedef Float < fixedSize <= 24 ? 24 : 53 > ResultType;
      };

      /**
      template specialisation for FLOAT(x),FLOAT(53)-operations
      delivers always FLOAT(53).
      */
      template<int fixedSize> struct FloatResult<fixedSize, 53> {
         /**
         the result type of mixed operations with Float(53)
         */
         typedef  Float<53>  ResultType;
      };

      /**
      The internal type of the Float(S) value.

      \note If S does not fit to the concrete defined lengths the c++ compiler
      will produce some error messages.
      */
      typedef typename InternalFloatType<S>::InternalType InternalType1;

      /**
      The internal data representation as defined by the templated type
      */
      InternalType1 x;

      /**
      default ctor constructs the Float with the preset NaN.

      Any calculations will leed to NaN-exceptions
      */
      Float<S>() {
         x = NAN;
      }

      /**
      ctor with given preset. The preset value is passed as double,

      Since this is a common floating point type in c++ covering all
      values of both supported Float(x) types.
      If the given value is inf or nan a signal is created.

      \param xx the preset value
      */
      Float<S>(double xx) {
         x = xx;
         FloatHelper::testFloatResult(x);
      }

      /**
      ctor with given preset. The preset value is passed as Fixed<31>,

      Since this is common floating point type in c++ covering all
      values of both supported Float(x) types.
      If the given value does not fit into a Float<24> a signal
      is created.

      \param xx the preset value
      */
      Float<S>(Fixed<31> xx) {
         x = xx.x;  // use the c++ internal type conversion
      }

      /**
      assignment operator works only for larger or same type

      \tparam T size of the target Float()
      \param rhs value to be assignes
      */
      template<int T> Float < S < T ? 0 : S > operator=(const Float<T> &rhs) {
         x = rhs.x;
         FloatHelper::testFloatResult(x);
         return *this;
      }

      /** add: operator +

      The result type is the largest type of the operands
      \tparam T type of the 2nd operand
      \param rhs the second operand
      \returns the sum of both operands, or an exception
      */
      template<int T>Float < S < T ? T : S >
      operator+(const Float<T> & rhs) const {
         Float < S < T ? T : S > result;
         result.x = x + rhs.x;
         FloatHelper::testFloatResult(result.x);
         return result;
      }

      /** add: operator + (float+fixed)

      The result type is the largest type of the operands
      \tparam T type of the 2nd operand
      \param rhs the second operand
      \returns the sum of both operands, or an exception
      */
      template<int T, template<int> class R>
      typename FloatResult<T, S>::ResultType
      operator+(const R<T> & rhs) const {
         typename FloatResult<T, S>::ResultType result;
         result.x = x + rhs.x;
         FloatHelper::testFloatResult(result.x);
         return result;
      }

      /** monadic minus: operator -

      The two's complement of the object's value.
      \returns the differnce of both operands, or an exception
      */
      Float<S> operator-() const {
         Float<S> result;
         result.x = -x;
         FloatHelper::testFloatResult(result.x);
         return result;
      }

      /** subtract: operator -

      The result type is the largest type of the operands
      \tparam T type of the 2nd operand
      \param rhs the second operand
      \returns the difference of both operands, or an exception
      */
      template<int T, template<int> class R>
      typename FloatResult<T, S>::ResultType
      operator-(const R<T> & rhs) const {
         typename FloatResult<T, S>::ResultType result;
         result.x = x - rhs.x;
         FloatHelper::testFloatResult(result.x);
         return result;
      }

      /** multiply: operator *

      The result type is the largest type of the operands
      \tparam T type of the 2nd operand
      \param rhs the second operand
      \returns the product of both operands, or an exception
      */
      template<int T, template<int> class R>
      typename FloatResult<T, S>::ResultType
      operator*(const R<T> & rhs) const {
         typename FloatResult<T, S>::ResultType result;
         result.x = x * rhs.x;
         FloatHelper::testFloatResult(result.x);
         return result;
      }

      /** divide: operator /

      The result type is the largest type of the operands
      \tparam T type of the 2nd operand
      \param rhs the second operand
      \returns the quotient of both operands, or an exception
      */
      template<int T, template<int> class R>
      typename FloatResult<T, S>::ResultType
      operator/(const R<T> & rhs) const {
         typename FloatResult<T, S>::ResultType result;
         result.x = x / rhs.x;
         FloatHelper::testFloatResult(result.x);
         return result;
      }

      /** adjust length: FIT

      This operation may throw expections, if the target type is smaller
      than to current type

       \tparam T type of the 2nd operand, only needed for type specification
       \param rhs the second operand
       \returns the objects value in the type of the 2nd operand
      */
      template<int T> Float<T> fit(const Float<T> &rhs) const {
         Float<T> result;
         result.x = x;
         FloatHelper::testFloatResult(result.x);
         return result;
      }

      /** absulute value: abs()

      Return the absolute value to the current objects value.
      This method may throw exceptions at range boundaries.

      \returns the absolute value of the object
      */
      Float<S> abs() const {
         Float<S> result;
         result.x = std::fabs(x);
         FloatHelper::testFloatResult(result.x);
         return result;
      }

      /** sign value: sign()

      Return the value of the sign of the current objects value.

      \returns the sign of the object
      */
      Fixed<1> sign() const {
         Fixed<1> result(0);

         if (x < 0) {
            result.x = -1;
         } else if (x > 0) {
            result.x = 1;
         }

         return result;
      }

      /** entier value: entier()

      Return the largest integral number less or equal the value of
      the current objects value.
      E.g. 4.5 -> 4.0; 4.0 -> 4.0; -4.5 -> -5; -4.4 -> -5 ; -4.0 -> -4.0

      \returns the absolute value of the object
      */
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
      /** round value: round()

      Return the the rounded value according DIN  of
      the current objects value.
      The absoulte value is rounded.
      E.g. 4.5 -> 5.0; 4.4 -> 4.0; -4.5 -> -5; -4.4 -> -4

      \returns the rounded value of the object
      */
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

      /** sine value: sin()

      Return the sin of the current objects value.
      This method may throw exceptions at range boundaries.

      \returns the sin value of the object
      */
      Float<S> sin() const {
         Float<S> result;
         result.x = std::sin(x);
         FloatHelper::testFloatResult(result.x);
         return result;
      }

      /** cosine value: cos()


      This method may throw exceptions at range boundaries.

      \returns the cosine value of the object
      */
      Float<S> cos() const {
         Float<S> result;
         result.x = std::cos(x);
         FloatHelper::testFloatResult(result.x);
         return result;
      }

      /** tangens value: tan()

      Return the tangens of the current objects value.
      This method may throw exceptions at range boundaries.

      \returns the tan value of the object
      */
      Float<S> tan() const {
         Float<S> result;
         result.x = std::tan(x);
         FloatHelper::testFloatResult(result.x);
         return result;
      }

      /** arc tangens value: atan()

      Return the atan of the current objects value.
      This method may throw exceptions at range boundaries.

      \returns the atan value of the object
      */
      Float<S> atan() const {
         Float<S> result;
         result.x = std::atan(x);
         FloatHelper::testFloatResult(result.x);
         return result;
      }

      /** hyperbolic tangens value: tanh()

      Return the tanhsin of the current objects value.
      This method may throw exceptions at range boundaries.

      \returns the tanh value of the object
      */
      Float<S> tanh() const {
         Float<S> result;
         result.x = std::tanh(x);
         FloatHelper::testFloatResult(result.x);
         return result;
      }

      /** expontial function value: exp()

      Return the sin of the current objects value.
      This method may throw exceptions at range boundaries.

      \returns the exp value of the object
      */
      Float<S> exp() const {
         Float<S> result;
         result.x = std::exp(x);
         FloatHelper::testFloatResult(result.x);
         return result;
      }

      /** logarithm (ln) value: ln()

      Return the ln of the current objects value.
      This method may throw exceptions at range boundaries.

      \returns the exp value of the object
      */
      Float<S> ln() const {
         Float<S> result;
         result.x = std::log(x);
         FloatHelper::testFloatResult(result.x);
         return result;
      }

      /** square root  value: sqrt()

      Return the sqrt of the current objects value.
      This method may throw exceptions with negative values

      \returns the sqrt value of the object
      */
      Float<S> sqrt() const {
         Float<S> result;
         result.x = std::sqrt(x);
         FloatHelper::testFloatResult(result.x);
         return result;
      }

      /** exponentiate

       x ** y; y is Fixed<P>

       <em>x</em> pow 0 returns 1 (as stated in IEE854)<br>

       \param rhs the exponent
       \returns result  of the operation
       \throws FloatIsINFSignal, if numerical overflow
      */
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

            // the calculation may throw FloatIsINFSignal
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

   /**
   add

   \param lhs left hand side of the operator
   \param rhs right hand side of the operator
   \tparam S length of the lhs Float value
   \tparam T length of the rhs Float value
   \returns the sum with the length of the rhs value
   */
   template <int S, int T> Float<T>
   operator+(const Fixed<S> & lhs, const Float<T> & rhs) {
      Float<T> result;
      result.x = lhs.x + rhs.x;
      FloatHelper::testFloatResult(result.x);
      return result;
   }

   /**
   substract

   \param lhs left hand side of the operator
   \param rhs right hand side of the operator
   \tparam S length of the lhs Float value
   \tparam T length of the rhs Float value
   \returns the difference with the length of the rhs value
   */
   template <int S, int T> Float<T>
   operator-(const Fixed<S> & lhs, const Float<T> & rhs) {
      Float<T> result;
      result.x = lhs.x - rhs.x;
      FloatHelper::testFloatResult(result.x);
      return result;
   }

   /**
   multiply

   \param lhs left hand side of the operator
   \param rhs right hand side of the operator
   \tparam S length of the lhs Float value
   \tparam T length of the rhs Float value
   \returns the product with the length of the rhs value
   */
   template <int S, int T> Float<T>
   operator*(const Fixed<S> & lhs, const Float<T> & rhs) {
      Float<T> result;
      result.x = lhs.x * rhs.x;
      FloatHelper::testFloatResult(result.x);
      return result;
   }

   /**
   divide

   \param lhs left hand side of the operator
   \param rhs right hand side of the operator
   \tparam S length of the lhs Float value
   \tparam T length of the rhs Float value
   \returns the quotient with the length of the rhs value
   */
   template <int S, int T> Float<T>
   operator/(const Fixed<S> & lhs, const Float<T> & rhs) {
      Float<T> result;
      result.x = lhs.x / rhs.x;
      FloatHelper::testFloatResult(result.x);
      return result;
   }
}
#endif

