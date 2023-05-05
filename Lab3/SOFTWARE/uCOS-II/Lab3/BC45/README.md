# Lab 3 for uC/OS-II Ceiling Priority Protocol

uc/OS-II 自帶 Priority Inversion Protocol (PIP)，改成 Ceiling Priority Protocol (CPP)

## Objective

To implement Ceiling Priority Protocol for ucOS's mutex locks

## Implementation

Modify the two function:

* `OSMutexPend`
    * Boost the locker's priority to the mutex priority
* `OSMutexPost`
    * Restore the original priority of the locker
