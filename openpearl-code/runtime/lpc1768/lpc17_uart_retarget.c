
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
    while(Chip_UART_ReadLineStatus(UART_SELECTION)==0);
    Chip_UART_SendByte(UART_SELECTION, cc);
}

int  __attribute__((used)) _write (int fd, char *ptr, int len)
{
   int i;
  /* Write "len" of char from "ptr" to file id "fd"
   * Return number of char written.
   * Need implementing with UART here. */
if (!initiated) {
	retarget_init();
}
for (i=0; i<len; i++) {
	if (*(ptr+i) == '\n') {
		sendChar(cr);
	}
	sendChar(*(ptr+i));
}
  return len;
}

int __attribute__((used)) _read (int fd, char *ptr, int len)
{
  /* Read "len" of char to "ptr" from file id "fd"
   * Return number of char read.
   * Need implementing with UART here. */
	if (!initiated) {
		retarget_init();
	}
    int received=0; 
    while (received<len) { 
       while((Chip_UART_ReadLineStatus(UART_SELECTION) & UART_LSR_RDR) == 0);
       *ptr = Chip_UART_ReadByte(UART_SELECTION);
        if (*ptr == '\b') {
            if (received > 0) {
              received --;
	      sendChar(*ptr); // local echo
              ptr --;
            }
        } else if (*ptr == '\r') {
	      sendChar(*ptr); // local echo
          *ptr = '\n';
	      sendChar(*ptr); // local echo
	  *(ptr+1) = 0;
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

static void retarget_init()
{
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

/*
void _exit(int x) {
_write(1,(char*)"*** end. ***\n", 13);
}
*/
