#ifndef SERVICE_INCLUDE
#define SERVICE_INCLUDE

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
   void (*job)(void *param);
   void * param;
} ServiceJob;

void add_service_from_ISR(ServiceJob * j);
void add_service(ServiceJob * j);
void init_service(void);

#ifdef __cplusplus
};
#endif

#endif
