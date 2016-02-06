
#include <chip.h>

#define UART_SELECTION  LPC_UART0
static const PINMUX_GRP_T pinmuxing[] = {
   {0,  2,   IOCON_MODE_INACT | IOCON_FUNC1},	/* TXD0 */
   {0,  3,   IOCON_MODE_INACT | IOCON_FUNC1}	/* RXD0 */
};

static const char hello[] = "retarget initiated\n";
static const char cr = '\r';

static bool initiated = false;

static void retarget_init();

static void sendChar(char cc) {
   while ((Chip_UART_ReadLineStatus(UART_SELECTION) & UART_LSR_TEMT) == 0);

   Chip_UART_SendByte(UART_SELECTION, cc);
}

int  __attribute__((used)) _write(int fd, char *ptr, int len) {
   int i;

   /* Write "len" of char from "ptr" to file id "fd"
    * Return number of char written.
    * Need implementing with UART here. */
   if (!initiated) {
      retarget_init();
   }

   for (i = 0; i < len; i++) {
      if (*(ptr + i) == '\n') {
         sendChar(cr);
      }

      sendChar(*(ptr + i));
   }

   return len;
}

int __attribute__((used)) _read(int fd, char *ptr, int len) {
   /* Read "len" of char to "ptr" from file id "fd"
    * Return number of char read.
    * Need implementing with UART here. */
   int received = 0;

   if (!initiated) {
      retarget_init();
   }

   while (received < len) {
      while ((Chip_UART_ReadLineStatus(UART_SELECTION) & UART_LSR_RDR) == 0);

      *ptr = Chip_UART_ReadByte(UART_SELECTION) & 0x7f;  // discard 8th-bit

      // backspace and del
      if (*ptr == '\b' || *ptr == 0x7f) {
         if (received > 0) {
            received --;
            sendChar('\b'); // delete char from line
            sendChar(' ');
            sendChar('\b');
            ptr --;
         }

         // test for newline -- \r is not transmitted by lpc21isp in terminal mode
      } else if (*ptr == '\n') {
         sendChar(*ptr); // local echo
         *ptr = '\n';
         sendChar(*ptr); // local echo
         *(ptr + 1) = 0;
         received ++;
         break;
      } else {
         sendChar(*ptr); // local echo
         ptr ++;
         received ++;
      }
   }

   return received;
}

void _ttywrch(int ch) {
   /* Write one char "ch" to the default console
    * Need implementing with UART here. */
   char c = ch;
   sendChar(c);

}

static void retarget_init() {
   // Initialize UART
   Chip_Clock_EnablePeriphClock(SYSCTL_CLOCK_GPIO);
   Chip_IOCON_SetPinMuxing(LPC_IOCON,
                           pinmuxing,
                           sizeof(pinmuxing) / sizeof(pinmuxing[0]));
   Chip_UART_Init(LPC_UART0); //UART_SELECTION);
   Chip_UART_SetBaud(UART_SELECTION, 115200);
   Chip_UART_ConfigData(UART_SELECTION, (UART_LCR_WLEN8 | UART_LCR_SBS_1BIT));

   Chip_UART_TXEnable(UART_SELECTION);
   initiated = true;
   _write(1, (char*)hello, sizeof(hello));
}


void _exit(int x) {
   _write(1, (char*)"*** end. ***\n", 13);
}

static volatile uint32_t r0, r1, r2, r3, r12, lr, pc, psr;
static volatile exceptNumber = 0;

void prvGetRegistersFromStack(uint32_t* faultStack) {
   r0 = faultStack[0];
   r1 = faultStack[1];
   r2 = faultStack[2];
   r3 = faultStack[3];
   r12 = faultStack[4];
   lr = faultStack[5];
   pc = faultStack[6];
   psr = faultStack[7];
}

void lpc17_default_handler(void) __attribute__((naked));
void lpc17_default_handler(void) {
// NVIC_INT_CTRL_CONSTis 0xe000ed04		\n"

   /*
      asm volatile (
           " ldr r3, #e000ed04			\n"
   	" ldr r2, [r3, #0]			\n"
   	" ldr r3, exceptNumber			\n"
   	" str r2, [r3,#0]			\n"
    );
   */
   asm volatile(
      " tst lr,#4			\n"
      " ite eq			\n"
      " mrseq r0, msp			\n"
      " mrsne r0, psp			\n"
      " ldr r1, [r0, #24]		\n"
      " ldr r2, handler2_address_const	\n"
      " bx r2				\n"
      " handler2_address_const: .word prvGetRegistersFromStack\n"
   );
   _write(1, "DEFAULT HANDLER", 15);
   printf(" Except number = %d\n", exceptNumber & 0x0f);
   printf(" Registers: \n r0: %08x r1: %08x r2: %08x r3: %08x\n"
          "r12: %08x lr: %08x pc: %08x psr: %08x\n",
          r0, r1, r2, r3, r12, lr, pc, psr);

   while (1);  // never return
}

