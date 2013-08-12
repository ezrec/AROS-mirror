/* Things declared here do not depend on host OS includes */

#include <aros/config.h>
#include <proto/kernel.h>

struct HostInterface;

extern unsigned int SupervisorCount;
extern struct HostInterface *HostIFace;

unsigned int krnGetPageSize(void *libc);
int core_Setup(void *libc, void *libpthread);
int core_Start(void);

int smp_Start(void);

#define UKB(base) (&(base)->kb_PlatformData->thread[KrnGetCPUNumber()])

#ifdef AROS_NO_ATOMIC_OPERATIONS

#define SUPERVISOR_ENTER UKB(KernelBase)->SupervisorCount++
#define SUPERVISOR_LEAVE UKB(KernelBase)->SupervisorCount--

#else

#define SUPERVISOR_ENTER AROS_ATOMIC_INC(UKB(KernelBase)->SupervisorCount)
#define SUPERVISOR_LEAVE AROS_ATOMIC_DEC(UKB(KernelBase)->SupervisorCount)

#endif

