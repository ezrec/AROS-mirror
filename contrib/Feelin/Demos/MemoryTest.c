#include <stdlib.h>

#include <libraries/feelin.h>
#include <exec/execbase.h>

#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/timer.h>
#include <proto/feelin.h>

struct FeelinBase *FeelinBase;
struct Device *TimerBase;

#define SIZE        8
#define NUM         300000
#define RAND_NUM    50000
#define ITEM        200

//#define CODE_USE_SIMPLE
//#define CODE_USE_REVERSE
#define CODE_USE_RANDOM

#define CODE_USE_EXEC
#define CODE_USE_FEELIN

uint32 main(void)
{
    if (F_FEELIN_OPEN)
    {
        APTR pool;
        APTR *array = AllocVec((NUM * sizeof (APTR)) + (RAND_NUM * sizeof (uint32)),0);
        
        TimerBase = (struct Device *) FindName(&FeelinBase->SYS->DeviceList, "timer.device");

        if (array)
        {
            int32 i;
            struct timeval t1,t2;
            uint32 *rand_array = (uint32 *)((uint32)(array) + NUM * sizeof (APTR));
            
#ifdef CODE_USE_SIMPLE

            Printf("This test creates a memory pool with a thresh size of %ld.\n"
                   "Then it allocates %ld items from the pool (%ld in each puddle).\n"
                   "Exec pools and Feelin pools are tested.\n\n"
                   "Keep in mind that F_Dispose() requires only the memory pointer as argument.\n"
                   "F_Dispose() & F_DisposeP() will NEVER dispose a block that does not exists.\n"
                   "More over, memory freed is filled with $ABADF00D.\n\n",SIZE,NUM,ITEM);

#ifdef CODE_USE_EXEC
///exec : simple
        
            pool = CreatePool(MEMF_CLEAR,SIZE * ITEM,SIZE);
            
            if (pool)
            {
                /*** clear *************************************************/
                
                for (i = 0 ; i < NUM ; i++)
                {
                    array[i] = 0;
                }

                /*** alloc *************************************************/
 
                Printf("[33mAllocsPooled()[0m ");

                Forbid();
                GetSysTime(&t1);
                
                for (i = 0 ; i < NUM ; i++)
                {
                    array[i] = AllocPooled(pool,SIZE);
                }
                
                GetSysTime(&t2);
                Permit();
                
                Printf("%ld secs (micro 0x%08lx)\n",t2.tv_secs - t1.tv_secs, 0x000FFFFF & (t2.tv_micro - t1.tv_micro));

                /*** free **************************************************/
                
                Printf("[33mFreePooled()[0m   ");
                
                Forbid();
                GetSysTime(&t1);

                for (i = 0 ; i < NUM ; i++)
                {
                    FreePooled(pool,array[i],SIZE);
                }
                
                GetSysTime(&t2);
                Permit();
                
                Printf("%ld secs (micro 0x%08lx)\n",t2.tv_secs - t1.tv_secs, 0x000FFFFF & (t2.tv_micro - t1.tv_micro));

                /*** delete ************************************************/
                
                DeletePool(pool);
            }
//+
#endif
#ifdef CODE_USE_FEELIN
///feelin: simple

            pool = F_CreatePool(SIZE, FA_Pool_Items, ITEM, TAG_DONE);

            if (pool)
            {
                /*** clear *************************************************/

                for (i = 0 ; i < NUM ; i++)
                {
                    array[i] = 0;
                }

                /*** alloc *************************************************/

                Printf("[33mF_NewP()      [0m ");

                Forbid();
                GetSysTime(&t1);

                for (i = 0 ; i < NUM ; i++)
                {
                    array[i] = F_NewP(pool,SIZE);
                }

                GetSysTime(&t2);
                Permit();

                Printf("%ld secs (micro 0x%08lx)\n",t2.tv_secs - t1.tv_secs, 0x000FFFFF & (t2.tv_micro - t1.tv_micro));

                /*** free **************************************************/

                Printf("[33mF_Dispose() [0m   ");

                Forbid();
                GetSysTime(&t1);

                for (i = 0 ; i < NUM ; i++)
                {
                    F_Dispose(array[i]);
                }

                GetSysTime(&t2);
                Permit();

                Printf("%ld secs (micro 0x%08lx)\n",t2.tv_secs - t1.tv_secs, 0x000FFFFF & (t2.tv_micro - t1.tv_micro));

                /*** delete ************************************************/

                F_DeletePool(pool);
            }
//+
#endif

            Printf("\n");
#endif

#ifdef CODE_USE_REVERSE
            Printf("Allocations will now be freed in reverse order...\n\n");

#ifdef CODE_USE_EXEC
///exec : reverse

            pool = CreatePool(MEMF_CLEAR,SIZE * ITEM,SIZE);

            if (pool)
            {
                /*** clear *************************************************/

                for (i = 0 ; i < NUM ; i++)
                {
                    array[i] = 0;
                }

                /*** alloc *************************************************/

                Printf("[33mAllocsPooled()[0m ");

                Forbid();
                GetSysTime(&t1);

                for (i = 0 ; i < NUM ; i++)
                {
                    array[i] = AllocPooled(pool,SIZE);
                }

                GetSysTime(&t2);
                Permit();

                Printf("%ld secs (micro 0x%08lx)\n",t2.tv_secs - t1.tv_secs, 0x000FFFFF & (t2.tv_micro - t1.tv_micro));

                /*** free **************************************************/

                Printf("[33mFreePooled()[0m   ");

                Forbid();
                GetSysTime(&t1);

                for (i = NUM - 1 ; i > -1 ; i--)
                {
                    FreePooled(pool,array[i],SIZE);
                }

                GetSysTime(&t2);
                Permit();

                Printf("%ld secs (micro 0x%08lx)\n",t2.tv_secs - t1.tv_secs, 0x000FFFFF & (t2.tv_micro - t1.tv_micro));

                /*** delete ************************************************/

                DeletePool(pool);
            }
//+
#endif
#ifdef CODE_USE_FEELIN
///feelin : reverse

            pool = F_CreatePool(SIZE, FA_Pool_Items, ITEM, TAG_DONE);

            if (pool)
            {
                /*** clear *************************************************/

                for (i = 0 ; i < NUM ; i++)
                {
                    array[i] = 0;
                }

                /*** alloc *************************************************/

                Printf("[33mF_NewP()      [0m ");

                Forbid();
                GetSysTime(&t1);

                for (i = 0 ; i < NUM ; i++)
                {
                    array[i] = F_NewP(pool,SIZE);
                }

                GetSysTime(&t2);
                Permit();

                Printf("%ld secs (micro 0x%08lx)\n",t2.tv_secs - t1.tv_secs, 0x000FFFFF & (t2.tv_micro - t1.tv_micro));

                /*** free **************************************************/

                Printf("[33mF_Dispose() [0m   ");

                Forbid();
                GetSysTime(&t1);

                for (i = NUM - 1 ; i > -1 ; i--)
                {
                    F_Dispose(array[i]);
                }

                GetSysTime(&t2);
                Permit();

                Printf("%ld secs (micro 0x%08lx)\n",t2.tv_secs - t1.tv_secs, 0x000FFFFF & (t2.tv_micro - t1.tv_micro));

                /*** delete ************************************************/

                F_DeletePool(pool);
            }
//+
#endif

            Printf("\n");
#endif
    
#ifdef CODE_USE_RANDOM
            Printf("%ld allocations will now be allocated and freed in random order.\n"
                   "EXEC's FreePooled() is a mess with this test it may take a while to process.\n\n",RAND_NUM);

            for (i = 0 ; i < RAND_NUM ; i++)
            {
                rand_array[i] = i;
            }

            for (i = 0 ; i < RAND_NUM ; i++)
            {
                size_t j = i + (unsigned) rand () / (RAND_MAX / (RAND_NUM - i) + 1);
                
                int32 t = rand_array[j];
                rand_array[j] = rand_array[i];
                rand_array[i] = t;
                
                //Printf("%ld ",rand_array[i]);
            }

#ifdef CODE_USE_EXEC
///exec : random

            pool = CreatePool(MEMF_CLEAR,SIZE * ITEM,SIZE);

            if (pool)
            {
                /*** clear *************************************************/

                for (i = 0 ; i < RAND_NUM ; i++)
                {
                    array[i] = 0;
                }

                /*** alloc *************************************************/

                Printf("[33mAllocsPooled()[0m ");

                Forbid();
                GetSysTime(&t1);

                for (i = 0 ; i < RAND_NUM ; i++)
                {
                    array[i] = AllocPooled(pool,SIZE);
                }

                GetSysTime(&t2);
                Permit();

                Printf("%ld secs (micro 0x%08lx)\n",t2.tv_secs - t1.tv_secs, 0x000FFFFF & (t2.tv_micro - t1.tv_micro));

                /*** free **************************************************/

                Printf("[33mFreePooled()[0m   ");

                Forbid();
                GetSysTime(&t1);

                for (i = 0 ; i < RAND_NUM ; i++)
                {
                    FreePooled(pool,array[rand_array[i]],SIZE);
                }

                GetSysTime(&t2);
                Permit();

                Printf("%ld secs (micro 0x%08lx)\n",t2.tv_secs - t1.tv_secs, 0x000FFFFF & (t2.tv_micro - t1.tv_micro));

                /*** delete ************************************************/

                DeletePool(pool);
            }
//+
#endif
#ifdef CODE_USE_FEELIN
///feelin : random

            pool = F_CreatePool(SIZE, FA_Pool_Items, ITEM, TAG_DONE);

            if (pool)
            {
                /*** clear *************************************************/

                for (i = 0 ; i < RAND_NUM ; i++)
                {
                    array[i] = 0;
                }

                /*** alloc *************************************************/

                Printf("[33mF_NewP()      [0m ");

                Forbid();
                GetSysTime(&t1);

                for (i = 0 ; i < RAND_NUM ; i++)
                {
                    array[i] = F_NewP(pool,SIZE);
                }

                GetSysTime(&t2);
                Permit();

                Printf("%ld secs (micro 0x%08lx)\n",t2.tv_secs - t1.tv_secs, 0x000FFFFF & (t2.tv_micro - t1.tv_micro));

                /*** free **************************************************/

                Printf("[33mF_Dispose() [0m   ");

                Forbid();
                GetSysTime(&t1);

                for (i = 0 ; i < RAND_NUM ; i++)
                {
                    F_Dispose(array[rand_array[i]]);
                }

                GetSysTime(&t2);
                Permit();

                Printf("%ld secs (micro 0x%08lx)\n",t2.tv_secs - t1.tv_secs, 0x000FFFFF & (t2.tv_micro - t1.tv_micro));

                /*** delete ************************************************/

                F_DeletePool(pool);
            }
//+
#endif
            Printf("\n");
#endif

            FreeVec(array);
        } 

        F_FEELIN_CLOSE;
    }

    return 0;
}
