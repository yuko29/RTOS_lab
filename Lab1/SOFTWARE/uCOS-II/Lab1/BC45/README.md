# Lab1 for uC/OS-II: Periodic Task Emulation 

## Objectives

- To implement periodic tasks
- To observe the scheduling behaviors

## Task Sets

Two sets of periodic tasks
- Task set 1 = { t1(1,3), t2(3,6)}
- Task set 2 = { t1(1,3), t2(3,6), t3(4,9)}
- Show context switch behaviors
- Show deadline violations if there is any

## Implementation

* `OS_CORE.C`: 
    * 在 `OS_Sched`, `OSIntExit` 內分別印 Complete 和 Preempt 訊息  
    * `OS_TimeTick` 內把執行的 task 的 compute time 減一
* `uCOS_II.H`:  
    * 在 `os_tcb` 內多記錄 task 的 compute time 和 period  
* `TEST.C`:
    * Create tasks，印 deadline violation 訊息 