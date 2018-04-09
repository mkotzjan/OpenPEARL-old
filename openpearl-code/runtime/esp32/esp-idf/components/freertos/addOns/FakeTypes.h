// fake for FreeRTOS types for the usage in OpenPEARL API
// this filke is auto generated -- do not edit

#ifndef FAKETYPES_H_INCLUDED
#define FAKETYPES_H_INCLUDED
#ifdef _cplusplus
extern "C" {
#endif

#include <stdint.h>


// FreeRTOS stuff
#define FakePortSHORT int16_t
typedef unsigned long FakeUBaseType_t;
typedef uint8_t FakeStackType_t;
typedef void* FakeSemaphoreHandle_t;
#define FakexSemaphoreHandle FakeSemaphoreHandle_t

typedef void* FakeTaskHandle_t;
typedef struct {uint32_t assertAlign; char data[352];} FakeTCB_t;
#ifdef _cplusplus
}
#endif

#endif
