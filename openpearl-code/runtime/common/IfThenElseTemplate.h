#ifndef IFTHENELSETEMPLATE_INCLUDED
#define IFTHENELSETEMPLATE_INCLUDED
namespace pearlrt {
   /** helper template for conditions compilation
   depending on the value of the condition, one of two
   given speciliations will be used.
   Both specializations define the type SELECT_CLASS, on own concern.
   */
   template< bool Condition, class THEN, class ELSE > struct IF { };

   /** helper template for conditions compilation.
   This part defines the behavior in case of true of the condition parameter
   */
   template<class THEN, class ELSE>
   struct IF<true, THEN, ELSE > {
      /**
      selected class

      \returns choosen alternative of the class selection
      */
      typedef THEN SELECT_CLASS;
   };

   /** helper template for conditions compilation.
   This part defines the behavior in case of false of the condition parameter.
   */
   template<class THEN, class ELSE >
   struct IF<false, THEN, ELSE > {
      /**
      selected class

      \returns choosen alternative of the class selection
      */
      typedef ELSE SELECT_CLASS;
   };
}
#endif

