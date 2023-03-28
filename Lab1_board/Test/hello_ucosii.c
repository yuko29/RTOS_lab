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
#include "stdio.h"
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
void Task(void *data);
// void DispTaskStat(INT8U id);
void printCtxSwMessage();

/*$PAGE*/
/*
*********************************************************************************************************
*                                                  MAIN
*********************************************************************************************************
*/

void main(void)
{
//    PC_DispClrScr(DISP_BGND_BLACK); /* Clear the screen                         */

    OSInit(); /* Initialize uC/OS-II                      */

//    PC_DOSSaveReturn(); /* Save environment to return to DOS        */

//    PC_VectSet(uCOS, OSCtxSw); /* Install uC/OS-II's context switch vector */

//    PC_ElapsedInit(); /* Initialized elapsed time measurement     */

    OSTaskCreate(TaskStart, (void *)0, &TaskStartStk[TASK_STK_SIZE - 1], 0);

    OSStart(); /* Start multitasking                       */
}

/*$PAGE*/
/*
*********************************************************************************************************
*                                               STARTUP TASK
*********************************************************************************************************
*/

void TaskStart(void *pdata)
{
#if OS_CRITICAL_METHOD == 3 /* Allocate storage for CPU status register */
    OS_CPU_SR cpu_sr;
#endif
    INT16S key;

    pdata = pdata; /* Prevent compiler warning                 */

    // TaskStartDispInit(); /* Setup the display                        */

//    OS_ENTER_CRITICAL(); /* Install uC/OS-II's clock tick ISR        */
//    PC_VectSet(0x08, OSTickISR);
//    PC_SetTickRate(OS_TICKS_PER_SEC); /* Reprogram tick rate                      */
//    OS_EXIT_CRITICAL();

    OSStatInit(); /* Initialize uC/OS-II's statistics         */

//    MsgQueue = OSQCreate(&MsgQueueTbl[0], MSG_QUEUE_SIZE); /* Create a message queue                   */

    OSTimeSet(0);

#if TEST_SET_ID == 1
    TaskStartCreateTaskSet1();
#else if TEST_SET_ID == 2
    TaskStartCreateTaskSet2();
#endif

    for (;;)
    {
        // TaskStartDisp(); /* Update the display                       */

        printCtxSwMessage(); /* Print the log of context switch */

//        if (PC_GetKey(&key))
//        { /* See if key has been pressed              */
//            if (key == 0x1B)
//            {                   /* Yes, see if it's the ESCAPE key          */
//                PC_DOSReturn(); /* Yes, return to DOS                       */
//            }
//        }

        OSCtxSwCtr = 0;              /* Clear the context switch counter         */
        OSTimeDly(300); /* Wait one second                          */
    }
}
/*$PAGE*/
/*
*********************************************************************************************************
*                                      CREATE APPLICATION TASKS
*********************************************************************************************************
*/

void TaskStartCreateTaskSet1(void)
{
    TaskInfoBuf[0].compTime = 1;
    TaskInfoBuf[0].period = 3;
    TaskInfoBuf[1].compTime = 3;
    TaskInfoBuf[1].period = 6;
    OSTaskCreate(Task,
                 (void *)&TaskInfoBuf[0],
                 &TaskStk[0][TASK_STK_SIZE - 1],
                 (INT8U)(1));

    OSTaskCreate(Task,
                 (void *)&TaskInfoBuf[1],
                 &TaskStk[1][TASK_STK_SIZE - 1],
                 (INT8U)(2));

}

void TaskStartCreateTaskSet2(void)
{
    TaskInfoBuf[0].compTime = 1;
    TaskInfoBuf[0].period = 3;
    TaskInfoBuf[1].compTime = 3;
    TaskInfoBuf[1].period = 6;
    TaskInfoBuf[2].compTime = 4;
    TaskInfoBuf[2].period = 9;
    OSTaskCreate(Task,
                 (void *)&TaskInfoBuf[0],
                 &TaskStk[0][TASK_STK_SIZE - 1],
                 (INT8U)(1));

    OSTaskCreate(Task,
                 (void *)&TaskInfoBuf[1],
                 &TaskStk[1][TASK_STK_SIZE - 1],
                 (INT8U)(2));

    OSTaskCreate(Task,
                 (void *)&TaskInfoBuf[2],
                 &TaskStk[2][TASK_STK_SIZE - 1],
                 (INT8U)(3));

}
/*$PAGE*/

/*$PAGE*/
/*
*********************************************************************************************************
*                                               TASK
*********************************************************************************************************
*/

void Task(void *pdata)
{
#if OS_CRITICAL_METHOD == 3                                /* Allocate storage for CPU status register     */
    OS_CPU_SR  cpu_sr = 0;
#endif
    char *msg;
    INT8U err;

    int start;
    int end;
    int toDelay;

    TASK_INFO *taskInfo = (TASK_INFO *)pdata;

    OS_ENTER_CRITICAL();
    OSTCBCur->compTime = taskInfo->compTime;
    OSTCBCur->period = taskInfo->period;
    OS_EXIT_CRITICAL();


    start = OSTimeGet();

    for (;;)
    {
        while (OSTCBCur->compTime > 0)
        {
            /* Computing */
        }
        end = OSTimeGet();
        toDelay = (OSTCBCur->period) - (end - start);

        OS_ENTER_CRITICAL();
		OSTCBCur->compTime = taskInfo->compTime;        // Reset the counter
		if (toDelay < 0)
		{
//			sprintf(&CtxSwMsgBuf[CtxSwMsgCursor++], "start: %d, period: %d, prio: %d\n", start, (int)OSTCBCur->period, (int)OSTCBCur->OSTCBPrio);
			sprintf(&CtxSwMsgBuf[CtxSwMsgCursor++], "%5d\t Task%d exceed the deadline.\n", ((int)(start/OSTCBCur->period)+1)*OSTCBCur->period , (int)OSTCBCur->OSTCBPrio);
		}
		start = start + (OSTCBCur->period);             // Next start time
		OS_EXIT_CRITICAL();

        OSTimeDly(toDelay);
    }
}


void printCtxSwMessage()
{
    static int i = 0;

    for(; i < CtxSwMsgCursor; i++)
        printf("%s",CtxSwMsgBuf[i]);

    if (i > CTXSW_MSG_BUF_SIZE)
        while(1);
}
