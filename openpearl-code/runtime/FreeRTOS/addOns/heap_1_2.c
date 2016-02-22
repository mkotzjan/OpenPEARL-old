/*
    FreeRTOS V8.2.1 - Copyright (C) 2015 Real Time Engineers Ltd.
    All rights reserved

    VISIT http://www.FreeRTOS.org TO ENSURE YOU ARE USING THE LATEST VERSION.

    This file is part of the FreeRTOS distribution.

    FreeRTOS is free software; you can redistribute it and/or modify it under
    the terms of the GNU General Public License (version 2) as published by the
    Free Software Foundation >>!AND MODIFIED BY!<< the FreeRTOS exception.

    ***************************************************************************
    >>!   NOTE: The modification to the GPL is included to allow you to     !<<
    >>!   distribute a combined work that includes FreeRTOS without being   !<<
    >>!   obliged to provide the source code for proprietary components     !<<
    >>!   outside of the FreeRTOS kernel.                                   !<<
    ***************************************************************************

    FreeRTOS is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE.  Full license text is available on the following
    link: http://www.freertos.org/a00114.html

    ***************************************************************************
     *                                                                       *
     *    FreeRTOS provides completely free yet professionally developed,    *
     *    robust, strictly quality controlled, supported, and cross          *
     *    platform software that is more than just the market leader, it     *
     *    is the industry's de facto standard.                               *
     *                                                                       *
     *    Help yourself get started quickly while simultaneously helping     *
     *    to support the FreeRTOS project by purchasing a FreeRTOS           *
     *    tutorial book, reference manual, or both:                          *
     *    http://www.FreeRTOS.org/Documentation                              *
     *                                                                       *
    ***************************************************************************

    http://www.FreeRTOS.org/FAQHelp.html - Having a problem?  Start by reading
    the FAQ page "My application does not run, what could be wrong?".  Have you
    defined configASSERT()?

    http://www.FreeRTOS.org/support - In return for receiving this top quality
    embedded software for free we request you assist our global community by
    participating in the support forum.

    http://www.FreeRTOS.org/training - Investing in training allows your team
    to be as productive as possible as early as possible.  Now you can receive
    FreeRTOS training directly from Richard Barry, CEO of Real Time Engineers
    Ltd, and the world's leading authority on the world's leading RTOS.

    http://www.FreeRTOS.org/plus - A selection of FreeRTOS ecosystem products,
    including FreeRTOS+Trace - an indispensable productivity tool, a DOS
    compatible FAT file system, and our tiny thread aware UDP/IP stack.

    http://www.FreeRTOS.org/labs - Where new FreeRTOS products go to incubate.
    Come and try FreeRTOS+TCP, our new open source TCP/IP stack for FreeRTOS.

    http://www.OpenRTOS.com - Real Time Engineers ltd. license FreeRTOS to High
    Integrity Systems ltd. to sell under the OpenRTOS brand.  Low cost OpenRTOS
    licenses offer ticketed support, indemnification and commercial middleware.

    http://www.SafeRTOS.com - High Integrity Systems also provide a safety
    engineered and independently SIL3 certified version for use in safety and
    mission critical applications that require provable dependability.

    1 tab == 4 spaces!
	in vi use 'set tabstop=4'
*/


/*
 * This is a simple implementation of pvPortMalloc() and vPortFree().
 * Note that this implementation is a mixture of the FreeRTOS supplied files
 * heap_1.c and heap_2.c
 * pvPortMalloc returns ether a  previously freed memory segment of the EXACT
 * size (similar to heap_2.c), or a memory segment from the static allocated
 * heap space (like in heap_1), or an error if the heap space is exhausted.
 *
 * See heap_2.c, heap_3.c and heap_4.c for alternative implementations, and the
 * memory management pages of http://www.FreeRTOS.org for more information.
 */
#include <stdlib.h>

/* Defining MPU_WRAPPERS_INCLUDED_FROM_API_FILE prevents task.h from redefining
all the API functions to use the MPU wrappers.  That should only be done when
task.h is included from an application file. */
#define MPU_WRAPPERS_INCLUDED_FROM_API_FILE

#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "task.h"

#undef MPU_WRAPPERS_INCLUDED_FROM_API_FILE

/* A few bytes might be lost to byte aligning the heap start address. */
#define configADJUSTED_HEAP_SIZE	( configTOTAL_HEAP_SIZE - portBYTE_ALIGNMENT )


/*
 * Initialises the heap structures before their first use.
 */
static void prvHeapInit(void);
static int xHeapHasBeenInitialised = pdFALSE;

/* Allocate the memory for the heap. */
static uint8_t ucHeap[ configTOTAL_HEAP_SIZE ]
__attribute__((section("FREERTOS_HEAP")));

static size_t xNextFreeByte = (size_t) 0;

/* Define the linked list structure.  This is used to link free blocks in order
of their size. */
typedef struct A_BLOCK_LINK {
   /** The next free block in the list. */
   struct A_BLOCK_LINK *pxNextFreeBlock;
   /** The size of the free block. */
   size_t xBlockSize;
} BlockLink_t;


static const uint16_t heapSTRUCT_SIZE	=
   ((sizeof(BlockLink_t) +
     (portBYTE_ALIGNMENT - 1)) & ~portBYTE_ALIGNMENT_MASK);

#define heapMINIMUM_BLOCK_SIZE	( ( size_t ) ( heapSTRUCT_SIZE * 2 ) )

/* Create a couple of list links to mark the start and end of the list. */
static	BlockLink_t zeroBlock = {(BlockLink_t*)NULL, 0};
static BlockLink_t *xStart = &zeroBlock;

/* Keeps track of the number of free bytes remaining, but says nothing about
fragmentation. */
static size_t xFreeBytesRemaining = configADJUSTED_HEAP_SIZE;

/* STATIC FUNCTIONS ARE DEFINED AS MACROS TO MINIMIZE THE FUNCTION CALL DEPTH.
*/

/*
 * Insert a block into the list of free blocks - which is ordered by size of
 * the block.  Small blocks at the start of the list and large blocks at the
 * end of the list.
 */
#define prvInsertBlockIntoFreeList( pxBlockToInsert )								\
{																					\
	BlockLink_t *pxIterator;														\
	size_t xBlockSize;																\
																					\
	xBlockSize = pxBlockToInsert->xBlockSize;										\
																					\
	/* Iterate through the list until a block is found that has a larger size */	\
	/* than the block we are inserting. */											\
	for( pxIterator = xStart; 														\
		 pxIterator->pxNextFreeBlock != NULL && pxIterator->xBlockSize < xBlockSize;\
		 pxIterator = pxIterator->pxNextFreeBlock )									\
	{																				\
		/* There is nothing to do here - just iterate to the correct position. */	\
	}																				\
																					\
	/* Update the list to include the block being inserted in the correct */		\
	/* position. */																	\
	pxBlockToInsert->pxNextFreeBlock = pxIterator->pxNextFreeBlock;					\
	pxIterator->pxNextFreeBlock = pxBlockToInsert;									\
}

/*-----------------------------------------------------------*/

void *pvPortMalloc(size_t xWantedSize) {
   void *pvReturn = NULL;
   static uint8_t *pucAlignedHeap = NULL;
   BlockLink_t * 	pxPreviousBlock;
   BlockLink_t * 	pxBlock;
   /* Ensure that blocks are always aligned to the required number of bytes. */
#if portBYTE_ALIGNMENT != 1

   if (xWantedSize & portBYTE_ALIGNMENT_MASK) {
      /* Byte alignment required. */
      xWantedSize += (portBYTE_ALIGNMENT -
                      (xWantedSize & portBYTE_ALIGNMENT_MASK));
   }

#endif

   vTaskSuspendAll();
   {
      /* If this is the first call to malloc then the heap will require
      initialisation to setup the list of free blocks. */
      if (xHeapHasBeenInitialised == pdFALSE) {
         prvHeapInit();
         xHeapHasBeenInitialised = pdTRUE;
      }


      /* Blocks are stored in byte order - traverse the list from the start
      (smallest) block until one of adequate size is found. */
      pxPreviousBlock = xStart;

      /* the 'zero' block need not to be checked */
      pxBlock = xStart->pxNextFreeBlock;
      pvReturn = NULL;

      /* there is at least the 'zero' element in the free list!*/
      while ((pxBlock != NULL) &&
             (pvReturn == NULL) &&
             (pxBlock->xBlockSize <= xWantedSize)) {
         if (pxBlock->xBlockSize == xWantedSize) {
            pvReturn = (void *)(((uint8_t *) pxBlock) + heapSTRUCT_SIZE);
//printf("prvMalloc: reuse of block at %p size=%d\n",
//     pvReturn, pxBlock->xBlockSize);

            /* and remove the found block from the freeBlockList */
            pxPreviousBlock->pxNextFreeBlock = pxBlock->pxNextFreeBlock;
            traceMALLOC(pvReturn, xWantedSize);
            xFreeBytesRemaining -= pxBlock->xBlockSize;
         } else {
            pxPreviousBlock = pxBlock;
            pxBlock = pxBlock->pxNextFreeBlock;
         }
      }

      /* If we found NO EXACTLY fitting block allocate from the static
         heap storage */
      if (pvReturn == NULL) {
         if (pucAlignedHeap == NULL) {
            /* Ensure the heap starts on a correctly aligned boundary. */
            pucAlignedHeap = (uint8_t *)
                             (((portPOINTER_SIZE_TYPE) &ucHeap[ portBYTE_ALIGNMENT ]) &
                              (~((portPOINTER_SIZE_TYPE) portBYTE_ALIGNMENT_MASK))
                             );
         }

         /* Check there is enough room left for the allocation. */
         if (((xNextFreeByte + xWantedSize + heapSTRUCT_SIZE) <
               configADJUSTED_HEAP_SIZE) &&
               ((xNextFreeByte + xWantedSize + heapSTRUCT_SIZE) >
                xNextFreeByte)) {  /* Check for overflow. */
            /* Return the next free byte then increment the index past
             * this block.
             * insert the linked list elements just before the returned pointer
             *  			+-----------------------------------+
             *				|   BlockList.pxNextFreeBlock 		|
             *				|	BlockList.xBlockSize			|
             * 				+-----------------------------------+
             *  pvReturn -> |	data block						|
             *				|	...								|
             *				+-----------------------------------+
             */
            pvReturn = pucAlignedHeap + xNextFreeByte;
            xNextFreeByte += xWantedSize + heapSTRUCT_SIZE;
            ((BlockLink_t*)pvReturn)->xBlockSize = xWantedSize;
            ((BlockLink_t*)pvReturn)->pxNextFreeBlock = (BlockLink_t*)NULL;
            pvReturn += heapSTRUCT_SIZE;
//printf("prvMalloc: new block at %p size=%d\n", pvReturn, xWantedSize);
            xFreeBytesRemaining -= xWantedSize + heapSTRUCT_SIZE;
         }

         traceMALLOC(pvReturn, xWantedSize);
      }
   }
   (void) xTaskResumeAll();

#if( configUSE_MALLOC_FAILED_HOOK == 1 )
   {
      if (pvReturn == NULL) {
         extern void vApplicationMallocFailedHook(void);
         printf("prvMalloc: heap full: wanted size = %d xNextFreeByte=%d\n",
                xWantedSize, xNextFreeByte);
         vApplicationMallocFailedHook();
      }
   }
#endif

   return pvReturn;
}
/*-----------------------------------------------------------*/

void vPortInitialiseBlocks(void) {
   /* Only required when static memory is not cleared. */
   xNextFreeByte = (size_t) 0;
}

/*-----------------------------------------------------------*/
void vPortFree(void *pv) {
   uint8_t *puc = (uint8_t *) pv;
   BlockLink_t *pxLink;

   if (pv != NULL) {
      /* The memory being freed will have an BlockLink_t structure immediately
      before it. */
      puc -= heapSTRUCT_SIZE;

      /* This unexpected casting is to keep some compilers from issuing
      byte alignment warnings. */
      pxLink = (void *) puc;

      vTaskSuspendAll();
      /* Add this block to the list of free blocks. */
      prvInsertBlockIntoFreeList(((BlockLink_t *) pxLink));
      xFreeBytesRemaining += pxLink->xBlockSize;
      traceFREE(pv, pxLink->xBlockSize);
//printf("prvFree: freed block at %p size=%d\n", pxLink, pxLink->xBlockSize);
      xTaskResumeAll();
   }
}

/*-----------------------------------------------------------*/

size_t xPortGetFreeHeapSize(void) {
   return xFreeBytesRemaining;
}
/*-----------------------------------------------------------*/

static void prvHeapInit(void) {
   /* Only required when static memory is not cleared. */
   zeroBlock.xBlockSize = 0;
   zeroBlock.pxNextFreeBlock = (BlockLink_t *) NULL;

   xStart = &zeroBlock;
}
/*-----------------------------------------------------------*/
