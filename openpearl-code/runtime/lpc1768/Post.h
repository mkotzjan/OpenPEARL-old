#ifndef POST_INCLUDED
#define POST_INCLUDED
namespace pearlrt {
   /**
   \brief self diagnostic of the system at power on
   */
   class Post {
      static int postStatus;
   public:
      /** ctor for dummy device in system part

      \param mode 0=only print<br>
                  1=print + config
      */
      Post(int mode);

      /**
      check, whether POST is set

      \returns the value of the parameter of the ctor (or -1), if
          no or was usd
      */
      static int getPostStatus();
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
};
#endif
