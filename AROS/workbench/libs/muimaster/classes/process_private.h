#ifndef _PROCESS_PRIVATE_H_
#define _PROCESS_PRIVATE_H_

/*** Instance data **********************************************************/
struct Process_DATA
{
    ULONG autolaunch;
    ULONG name;
    ULONG priority;
    ULONG sourceclass;
    ULONG sourceobject;
    ULONG stacksize;
    ULONG task;
};

#endif /* _PROCESS_PRIVATE_H_ */
