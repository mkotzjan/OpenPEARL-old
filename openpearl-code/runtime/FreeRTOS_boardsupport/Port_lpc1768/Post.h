#ifndef POST_INCLUDED
#define POST_INCLUDED

/**
\brief self diagnostic of the system at power on
*/
class Post {
public:
   /**
   print POST screen to retarget console

   <ul>
   <li>show the memory usage in FLASH and RAM
   <li>show RTC values
   </ul>
   */
   static void print(void);

   /**
   start config dialog in POST mode

   supported tools:
   <ul>
   <li>set RTC
   <li>start application
   </ul>
   */
   static void config(void);
};

#endif
