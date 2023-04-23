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

#define TEST_SET_ID 1
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
    PC_DispClrScr(DISP_BGND_BLACK); /* Clear the screen                         */
    
    OSInit(); /* Initialize uC/OS-II                      */
    OSTCBPrioTbl[OS_IDLE_PRIO]->deadline = 1002;
    
    PC_DOSSaveReturn(); /* Save environment to return to DOS        */

    PC_VectSet(uCOS, OSCtxSw); /* Install uC/OS-II's context switch vector */

    PC_ElapsedInit(); /* Initialized elapsed time measurement     */

    // OSTaskCreate(TaskStart, (void *)0, &TaskStartStk[TASK_STK_SIZE - 1], 0);
#if TEST_SET_ID == 1
    TaskStartCreateTaskSet1();
#else if TEST_SET_ID == 2
    TaskStartCreateTaskSet2();
#endif
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

//     // TaskStartDispInit(); /* Setup the display                        */
//     OS_ENTER_CRITICAL(); /* Install uC/OS-II's clock tick ISR        */
//     PC_VectSet(0x08, OSTickISR);
//     PC_SetTickRate(OS_TICKS_PER_SEC); /* Reprogram tick rate                      */
//     // OSTCBCur->deadline = TASKSTART_DEADLINE;
    
//     OS_EXIT_CRITICAL();
//     // OSStatInit(); /* Initialize uC/OS-II's statistics         */

//     // MsgQueue = OSQCreate(&MsgQueueTbl[0], MSG_QUEUE_SIZE); /* Create a message queue                   */


//     OSTimeSet(0);
//     OSTimeDly(MAX_EXECTIME - 1);

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

//         // OSCtxSwCtr = 0;              /* Clear the context switch counter         */
//         // OSTimeDly(MAX_EXECTIME);
//     }
// }
/*$PAGE*/
/*
*********************************************************************************************************
*                                      CREATE APPLICATION TASKS
*********************************************************************************************************
*/

void TaskStartCreateTaskSet1(void)
{
    TaskInfoBuf[1].compTime = 1;
    TaskInfoBuf[1].period = 3;
    TaskInfoBuf[2].compTime = 3;
    TaskInfoBuf[2].period = 5;
    OSTaskCreate(Task1,
                 (void *)&TaskInfoBuf[1],
                 &TaskStk[0][TASK_STK_SIZE - 1],
                 (INT8U)(1));

    OSTaskCreate(Task2,
                 (void *)&TaskInfoBuf[2],
                 &TaskStk[1][TASK_STK_SIZE - 1],
                 (INT8U)(2));

    TaskArgumentSetting(TaskInfoBuf);
}

void TaskStartCreateTaskSet2(void)
{
    TaskInfoBuf[1].compTime = 1;
    TaskInfoBuf[1].period = 4;
    TaskInfoBuf[2].compTime = 2;
    TaskInfoBuf[2].period = 5;
    TaskInfoBuf[3].compTime = 2;
    TaskInfoBuf[3].period = 10;
    OSTaskCreate(Task1,
                 (void *)&TaskInfoBuf[1],
                 &TaskStk[0][TASK_STK_SIZE - 1],
                 (INT8U)(1));

    OSTaskCreate(Task2,
                 (void *)&TaskInfoBuf[2],
                 &TaskStk[1][TASK_STK_SIZE - 1],
                 (INT8U)(2));

    OSTaskCreate(Task3,
                 (void *)&TaskInfoBuf[3],
                 &TaskStk[2][TASK_STK_SIZE - 1],
                 (INT8U)(3));

    TaskArgumentSetting(TaskInfoBuf);
}
/*$PAGE*/

/*$PAGE*/
/*
*********************************************************************************************************
*                                               TASK
*********************************************************************************************************
*/

void Task1(void *pdata)
{
    char *msg;
    INT8U err;
    INT16S key;

    int start;
    int end;
    int toDelay;

    TASK_INFO *taskInfo = (TASK_INFO *)pdata;

    OS_ENTER_CRITICAL();
    PC_VectSet(0x08, OSTickISR);                           /* Install uC/OS-II's clock tick ISR        */
    PC_SetTickRate(1u);                      /* Reprogram tick rate                      */
    OS_EXIT_CRITICAL();

    start = 0;

    // printf("Task running: %d\n", OSTCBCur->OSTCBPrio);

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
        while (OSTCBCur->compTime > 0)
        {
            /* Computing */
        }
        
        // printf("Task complete: %d\n", OSTCBCur->OSTCBPrio);
        
        OS_ENTER_CRITICAL();
        end = OSTimeGet();
        toDelay = (OSTCBCur->period) - (end - start);
        start = start + (OSTCBCur->period);      // Next start time
        OSTCBCur->compTime = taskInfo->compTime; // Reset the counter
        OSTCBCur->deadline = start + OSTCBCur->period;
        OS_EXIT_CRITICAL();
        // printf("------In Task------\n");
        // printf("Tick: %d, Task prio: %d, start: %d, end: %d, compTime: %d, toDelay: %d\n",
        //        OSTimeGet(), OSTCBCur->OSTCBPrio, start - (OSTCBCur->period), end, OSTCBCur->compTime, toDelay);
        if(toDelay < 0) {
            if (CtxSwMsgCursor < CTXSW_MSG_BUF_SIZE)
                sprintf(&CtxSwMsgBuf[CtxSwMsgCursor++], "Tick: %d: Task1 over deadline.", OSTimeGet());
        }
        else {
            OSTimeDly(toDelay);
        }  
    }
}

void Task2(void *pdata)
{
    char *msg;
    INT8U err;
    INT16S key;

    int start;
    int end;
    int toDelay;

    TASK_INFO *taskInfo = (TASK_INFO *)pdata;

    start = 0;

    // printf("Task running: %d\n", OSTCBCur->OSTCBPrio);

    for (;;)
    {
        while (OSTCBCur->compTime > 0)
        {
            /* Computing */
        }
        
        // printf("Task complete: %d\n", OSTCBCur->OSTCBPrio);
        
        OS_ENTER_CRITICAL();
        end = OSTimeGet();
        toDelay = (OSTCBCur->period) - (end - start);
        start = start + (OSTCBCur->period);      // Next start time
        OSTCBCur->compTime = taskInfo->compTime; // Reset the counter
        OSTCBCur->deadline = start + OSTCBCur->period;
        OS_EXIT_CRITICAL();
        // printf("------In Task------\n");
        // printf("Tick: %d, Task prio: %d, start: %d, end: %d, compTime: %d, toDelay: %d\n",
        //        OSTimeGet(), OSTCBCur->OSTCBPrio, start - (OSTCBCur->period), end, OSTCBCur->compTime, toDelay);
        if(toDelay < 0) {
            if (CtxSwMsgCursor < CTXSW_MSG_BUF_SIZE)
                sprintf(&CtxSwMsgBuf[CtxSwMsgCursor++], "Tick: %d: Task2 over deadline.", OSTimeGet());
        }
        else {
            OSTimeDly(toDelay);
        }  
    }
}

void Task3(void *pdata)
{
    char *msg;
    INT8U err;
    INT16S key;

    int start;
    int end;
    int toDelay;

    TASK_INFO *taskInfo = (TASK_INFO *)pdata;

    start = 0;

    // printf("Task running: %d\n", OSTCBCur->OSTCBPrio);

    for (;;)
    {
        while (OSTCBCur->compTime > 0)
        {
            /* Computing */
        }
        
        // printf("Task complete: %d\n", OSTCBCur->OSTCBPrio);
        
        OS_ENTER_CRITICAL();
        end = OSTimeGet();
        toDelay = (OSTCBCur->period) - (end - start);
        start = start + (OSTCBCur->period);      // Next start time
        OSTCBCur->compTime = taskInfo->compTime; // Reset the counter
        OSTCBCur->deadline = start + OSTCBCur->period;
        OS_EXIT_CRITICAL();
        // printf("------In Task------\n");
        // printf("Tick: %d, Task prio: %d, start: %d, end: %d, compTime: %d, toDelay: %d\n",
        //        OSTimeGet(), OSTCBCur->OSTCBPrio, start - (OSTCBCur->period), end, OSTCBCur->compTime, toDelay);
        if(toDelay < 0) {
            if (CtxSwMsgCursor < CTXSW_MSG_BUF_SIZE)
                sprintf(&CtxSwMsgBuf[CtxSwMsgCursor++], "Tick: %d: Task3 over deadline.", OSTimeGet());
        }
        else {
            OSTimeDly(toDelay);
        }  
    }
}




void printCtxSwMessage()
{
    // static int i = 0;

    // printf("PrintCursor: %d\n", PrintCursor);
    for (; PrintCursor < CtxSwMsgCursor; PrintCursor++)
        printf("%s", CtxSwMsgBuf[PrintCursor]);

    if (PrintCursor > CTXSW_MSG_BUF_SIZE) {
        while (1);
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