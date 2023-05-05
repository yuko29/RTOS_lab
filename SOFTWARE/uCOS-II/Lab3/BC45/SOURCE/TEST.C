/*
*********************************************************************************************************
*                                                uC/OS-II
*                                          The Real-Time Kernel
*
*                          (c) Copyright 1992-2002, Jean J. Labrosse, Weston, FL
*                                           All Rights Reserved
*
*                                               EXAMPLE #3
*********************************************************************************************************
*/

#include "includes.h"

/*
*********************************************************************************************************
*                                              CONSTANTS
*********************************************************************************************************
*/

#define TASK_STK_SIZE 512 /* Size of each task's stacks (# of WORDs)       */

#define TASK_START_ID 0 /* Application tasks                             */

#define MSG_QUEUE_SIZE 20 /* Size of message queue used in example         */

#ifndef CTXSW_MSG_BUF_SIZE
#define CTXSW_MSG_BUF_SIZE 20
#endif

#define NUM_TASK 5

#define TEST_SET_ID 2
/*
*********************************************************************************************************
*                                             DATA TYPES
*********************************************************************************************************
*/

// typedef struct {
//     char    TaskName[30];
//     INT16U  TaskCtr;
//     INT16U  TaskExecTime;
//     INT32U  TaskTotExecTime;
// } TASK_USER_DATA;

typedef struct
{
    int compTime;
    int period;
} TASK_INFO;

/*
*********************************************************************************************************
*                                              VARIABLES
*********************************************************************************************************
*/

OS_STK TaskStartStk[TASK_STK_SIZE];      /* Startup    task stack                         */
OS_STK TaskStk[NUM_TASK][TASK_STK_SIZE]; /* Task stacks                                   */

// TASK_USER_DATA  TaskUserData[7];

OS_EVENT *MsgQueue;    /* Message queue pointer                         */
void *MsgQueueTbl[20]; /* Storage for messages                          */
char CtxSwMsgBuf[CTXSW_MSG_BUF_SIZE][50];
int CtxSwMsgCursor = 0;
int PrintCursor = 0;
TASK_INFO TaskInfoBuf[NUM_TASK];

OS_EVENT *R1;
OS_EVENT *R2;

/*
*********************************************************************************************************
*                                         FUNCTION PROTOTYPES
*********************************************************************************************************
*/

void TaskStart(void *data); /* Function prototypes of tasks                  */
static void TaskStartCreateTaskSet1(void);
static void TaskStartCreateTaskSet2(void);
// static void TaskStartDispInit(void);
// static void TaskStartDisp(void);
void Task1(void *data);
void Task2(void *data);
void Task3(void *data);
// void DispTaskStat(INT8U id);
void printCtxSwMessage();
void TaskArgumentSetting();
void PrintTasksInfo();

/*$PAGE*/
/*
*********************************************************************************************************
*                                                  MAIN
*********************************************************************************************************
*/

void main(void)
{
    INT8U err1;
    INT8U err2;

    OSInit(); /* Initialize uC/OS-II                      */

    PC_DOSSaveReturn(); /* Save environment to return to DOS        */

    PC_VectSet(uCOS, OSCtxSw); /* Install uC/OS-II's context switch vector */

    PC_ElapsedInit(); /* Initialized elapsed time measurement     */

    // OSTaskCreate(TaskStart, (void *)0, &TaskStartStk[TASK_STK_SIZE - 1], 0);
#if TEST_SET_ID == 1
    TaskStartCreateTaskSet1();
#else if TEST_SET_ID == 2
    TaskStartCreateTaskSet2();
#endif
    R1 = OSMutexCreate(1, &err1);
    R2 = OSMutexCreate(2, &err1);
    OSStart(); /* Start multitasking                       */
}

/*$PAGE*/
/*
*********************************************************************************************************
*                                               STARTUP TASK
*********************************************************************************************************
*/

// void TaskStart(void *pdata)
// {
// #if OS_CRITICAL_METHOD == 3 /* Allocate storage for CPU status register */
//     OS_CPU_SR cpu_sr;
// #endif
//     INT16S key;
//     int i=0;

//     pdata = pdata; /* Prevent compiler warning                 */

//     OS_ENTER_CRITICAL(); /* Install uC/OS-II's clock tick ISR        */
//     PC_VectSet(0x08, OSTickISR);
//     PC_SetTickRate(OS_TICKS_PER_SEC); /* Reprogram tick rate                      */

//     OS_EXIT_CRITICAL();

// #if TEST_SET_ID == 1
//     TaskStartCreateTaskSet1();
// #else if TEST_SET_ID == 2
//     TaskStartCreateTaskSet2();
// #endif
//     OSTimeSet(0);

//     for (;;)
//     {
//         // TaskStartDisp(); /* Update the display                       */
//         printCtxSwMessage(); /* Print the log of context switch */

//         if (PC_GetKey(&key))
//         { /* See if key has been pressed              */
//             if (key == 0x1B)
//             {                   /* Yes, see if it's the ESCAPE key          */
//                 PC_DOSReturn(); /* Yes, return to DOS                       */
//             }
//         }

//         OSCtxSwCtr = 0;                                    /* Clear context switch counter             */
//         OSTimeDlyHMSM(0, 0, 1, 0);                         /* Wait one second                          */
//     }
// }
/*$PAGE*/
/*
*********************************************************************************************************
*                                      CREATE APPLICATION TASKS
*********************************************************************************************************
*/
#if TEST_SET_ID == 1
void TaskStartCreateTaskSet1(void)
{
    OSTaskCreate(Task1,
                 (void *)0,
                 &TaskStk[0][TASK_STK_SIZE - 1],
                 (INT8U)(3));

    OSTaskCreate(Task2,
                 (void *)0,
                 &TaskStk[1][TASK_STK_SIZE - 1],
                 (INT8U)(4));

    OSTaskCreate(Task3,
                 (void *)0,
                 &TaskStk[2][TASK_STK_SIZE - 1],
                 (INT8U)(5));
}
#else if TEST_SET_ID == 2
void TaskStartCreateTaskSet2(void)
{
    OSTaskCreate(Task1,
                 (void *)0,
                 &TaskStk[0][TASK_STK_SIZE - 1],
                 (INT8U)(3));

    OSTaskCreate(Task2,
                 (void *)0,
                 &TaskStk[1][TASK_STK_SIZE - 1],
                 (INT8U)(4));
}
#endif
/*$PAGE*/

/*$PAGE*/
/*
*********************************************************************************************************
*                                               TASK
*********************************************************************************************************
*/

#if TEST_SET_ID == 1

void Task1(void *pdata)
{

    char *msg;
    INT16S key;

    int start;
    int todelay;
    INT8U err;

    // TASK_INFO *taskInfo = (TASK_INFO *)pdata;
    OS_ENTER_CRITICAL();
    PC_VectSet(0x08, OSTickISR); /* Install uC/OS-II's clock tick ISR        */
    PC_SetTickRate(1u);          /* Reprogram tick rate                      */
    OS_EXIT_CRITICAL();

    start = 8;
    todelay = start - OSTime;
    if (todelay > 0)
        OSTimeDly(todelay);

    for (;;)
    {
        if (PC_GetKey(&key))
        { /* See if key has been pressed              */
            if (key == 0x1B)
            {                   /* Yes, see if it's the ESCAPE key          */
                PC_DOSReturn(); /* Yes, return to DOS                       */
            }
        }
        printCtxSwMessage();

        OSTCBCur->compTime = 2;
        while (OSTCBCur->compTime > 0);
        OSMutexPend(R1, 0, &err);   // Lock R1
        

        OSTCBCur->compTime = 2;
        while (OSTCBCur->compTime > 0);
        OSMutexPend(R2, 0, &err);   // Lock R2

        OSTCBCur->compTime = 2;
        while (OSTCBCur->compTime > 0);
        OSMutexPost(R2);            // Unlock R2
        OSMutexPost(R1);            // Unlock R1
        
        OSTimeDly(100);
    }
}

void Task2(void *pdata)
{
    int start;
    int todelay;
    INT8U err;

    // // TASK_INFO *taskInfo = (TASK_INFO *)pdata;

    start = 4;
    todelay = start - OSTime;
    if(todelay > 0) 
        OSTimeDly(todelay);
    
    for (;;)
    {
        OSTCBCur->compTime = 2;
        while(OSTCBCur->compTime > 0);
        OSMutexPend(R2,0,&err);
        
        OSTCBCur->compTime = 4;
        while(OSTCBCur->compTime > 0);
        OSMutexPost(R2);
       
       OSTimeDly(100);
    }

    // OSTaskDel(OS_PRIO_SELF);
}

void Task3(void *pdata)
{
    int start;
    int todelay;
    INT8U err;

    // // TASK_INFO *taskInfo = (TASK_INFO *)pdata;

    start = 0;
    todelay = start - OSTime;
    if(todelay > 0) OSTimeDly(todelay);

    for (;;)
    {
        OSTCBCur->compTime = 2;
        while(OSTCBCur->compTime > 0);
        OSMutexPend(R1,0,&err);

        OSTCBCur->compTime = 7;
        while(OSTCBCur->compTime > 0);
        OSMutexPost(R1);
        
        OSTimeDly(100);

    }

    // OSTaskDel(OS_PRIO_SELF);
}

#else if TEST_SET_ID == 2

void Task1(void *pdata)
{
    char *msg;
    INT16S key;

    int start;
    int todelay;
    INT8U err;

    // TASK_INFO *taskInfo = (TASK_INFO *)pdata;
    OS_ENTER_CRITICAL();
    PC_VectSet(0x08, OSTickISR); /* Install uC/OS-II's clock tick ISR        */
    PC_SetTickRate(1u);          /* Reprogram tick rate                      */
    OS_EXIT_CRITICAL();

    start = 5;
    todelay = start - OSTime;
    if (todelay > 0)
        OSTimeDly(todelay);

    for (;;)
    {
        if (PC_GetKey(&key))
        { /* See if key has been pressed              */
            if (key == 0x1B)
            {                   /* Yes, see if it's the ESCAPE key          */
                PC_DOSReturn(); /* Yes, return to DOS                       */
            }
        }
        printCtxSwMessage();

        OSTCBCur->compTime = 2;
        while (OSTCBCur->compTime > 0);
        OSMutexPend(R2, 0, &err);       // Lock R2
        

        OSTCBCur->compTime = 3;
        while (OSTCBCur->compTime > 0);
        OSMutexPend(R1, 0, &err);       // Lock R1

        OSTCBCur->compTime = 3;
        while (OSTCBCur->compTime > 0);
        OSMutexPost(R1);                // Unlock R1

        OSTCBCur->compTime = 3;
        while (OSTCBCur->compTime > 0);
        OSMutexPost(R2);                // Unlock R2
        
        OSTimeDly(100);
    }
}

void Task2(void *pdata)
{
    int start;
    int todelay;
    INT8U err;

    start = 0;
    todelay = start - OSTime;
    if (todelay > 0)
        OSTimeDly(todelay);

    for (;;)
    {
        OSTCBCur->compTime = 2;
        while (OSTCBCur->compTime > 0);
        OSMutexPend(R1, 0, &err);       // Lock R1
        

        OSTCBCur->compTime = 6;
        while (OSTCBCur->compTime > 0);
        OSMutexPend(R2, 0, &err);       // Lock R2

        OSTCBCur->compTime = 2;
        while (OSTCBCur->compTime > 0);
        OSMutexPost(R2);                // Unlock R2

        OSTCBCur->compTime = 2;
        while (OSTCBCur->compTime > 0);
        OSMutexPost(R1);                // Unlock R1

        OSTimeDly(100);
    }

}
#endif

void printCtxSwMessage()
{
    // static int i = 0;

    // printf("PrintCursor: %d\n", PrintCursor);
    for (; PrintCursor < CtxSwMsgCursor; PrintCursor++)
        printf("%s", CtxSwMsgBuf[PrintCursor]);

    if (PrintCursor > CTXSW_MSG_BUF_SIZE)
    {
        while (1)
            ;
    }
}

void TaskArgumentSetting(TASK_INFO *TaskInfoBuf)
{
    OS_TCB *ptcb;
    ptcb = OSTCBList;
    while (ptcb->OSTCBPrio == 1 || ptcb->OSTCBPrio == 2 || ptcb->OSTCBPrio == 3)
    {
        if (ptcb->OSTCBPrio == 1)
        {
            ptcb->compTime = TaskInfoBuf[1].compTime;
            ptcb->period = TaskInfoBuf[1].period;
            ptcb->deadline = TaskInfoBuf[1].period;
        }
        else if (ptcb->OSTCBPrio == 2)
        {
            ptcb->compTime = TaskInfoBuf[2].compTime;
            ptcb->period = TaskInfoBuf[2].period;
            ptcb->deadline = TaskInfoBuf[2].period;
        }
        else if (ptcb->OSTCBPrio == 3)
        {
            ptcb->compTime = TaskInfoBuf[3].compTime;
            ptcb->period = TaskInfoBuf[3].period;
            ptcb->deadline = TaskInfoBuf[3].period;
        }
        ptcb = ptcb->OSTCBNext;
    }
}

void PrintTasksInfo()
{
    OS_TCB *ptcb;
    ptcb = OSTCBList;
    while (ptcb->OSTCBPrio == 0 || ptcb->OSTCBPrio == 1 || ptcb->OSTCBPrio == 2 || ptcb->OSTCBPrio == 3)
    {
        printf("Task prio: %d, deadline: %d\n", ptcb->OSTCBPrio, ptcb->deadline);
        ptcb = ptcb->OSTCBNext;
    }
}