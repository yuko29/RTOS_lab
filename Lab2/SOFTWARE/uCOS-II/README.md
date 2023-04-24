# Lab2 for uC/OS-II: EDF Scheduler

換掉 uC/OS-II 自帶的 Fixed-priority Scheduling (Rate-Monotonic Scheduling)，改用 EDF (Earliest-Deadline-First)。

## Objective

- To implement an EDF scheduler in uC/OS-II

## Implementation

- `OS_CORE.C`: `OS_Sched`, `OSIntExit()`, `OSStart()` 內的 scheduling 都換成 EDF，採 linear search 實作
- `uCOS_II.H`: 加上印 log 需要的 cursor 變數 
- `TEST.C`: 捨棄 `TaskStart()`，由 `Task1` 負責 install uC/OS-II 的 clock tick ISR 以及印出 context switch log

Note: 每個 task 的 start time 都是從 Tick 0 開始。