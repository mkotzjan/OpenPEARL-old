
#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "task.h"
#include "FakeTypes.h"

typedef char UBT [ (sizeof(FakePortSHORT) == sizeof(portSHORT)) -1];
typedef char STKT [(sizeof(FakeUBaseType_t) == sizeof(UBaseType_t)) -1];
typedef char TCB [ (sizeof(FakeTCB_t) == sizeof(TCB_t)) -1 ];

