#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"
#include "vm.h"

uint64
sys_exit(void)
{
  int n;
  argint(0, &n);
  kexit(n);
  return 0;  // not reached
}

uint64
sys_getpid(void)
{
  return myproc()->pid;
}

uint64
sys_fork(void)
{
  return kfork();
}

uint64
sys_wait(void)
{
  uint64 p;
  argaddr(0, &p);
  return kwait(p);
}

uint64
sys_sbrk(void)
{
  uint64 addr;
  int t;
  int n;

  argint(0, &n);
  argint(1, &t);
  addr = myproc()->sz;

  if(t == SBRK_EAGER || n < 0) {
    if(growproc(n) < 0) {
      return -1;
    }
  } else {
    // Lazily allocate memory for this process: increase its memory
    // size but don't allocate memory. If the processes uses the
    // memory, vmfault() will allocate it.
    if(addr + n < addr)
      return -1;
    myproc()->sz += n;
  }
  return addr;
}

uint64
sys_pause(void)
{
  int n;
  uint ticks0;

  argint(0, &n);
  if(n < 0)
    n = 0;
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(killed(myproc())){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

uint64
sys_kill(void)
{
  int pid;

  argint(0, &pid);
  return kkill(pid);
}

// return how many clock tick interrupts have occurred
// since start.
uint64
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}

uint64
sys_getnice(void)
{
  int pid;
  argint(0, &pid);
  return getnice(pid);
}

uint64
sys_setnice(void)
{
  int pid, value;
  argint(0, &pid);
  argint(1, &value);
  return setnice(pid, value);
}

uint64
sys_ps(void)
{
  int pid;
  argint(0, &pid);
  return ps(pid);
}

// The wrapper function of the meminfo system call.
// It serves as a bridge between the meminfo() call of the user program and the actual kernel functionality.
uint64
sys_meminfo(void)
{
  // Call the count_freemem() function to get the number of free memory pages, multiply it by the page size (PGSIZE) to convert it to bytes, and return the result to the user program.
  return count_freemem() * PGSIZE;
}

// A wrapper function of a waitpid system call.
// Safely import the parameters (pid, addr) passed by the user program into the kernel space and pass them to the kwaitpid() function that performs the actual function.
uint64
sys_waitpid(void)
{
  int pid;
  uint64 addr;
  
  // argint(0, &pid) securely copies the first factor delivered by the user program to the pid variable in the kernel.
  argint(0, &pid);
  // argaddr(1, &addr) copies the second factor (the address to store the child's termination state) to the addr variable in the kernel.
  argaddr(1, &addr);

  // Hand over the imported factors to the kwaitpid() function and return the result.
  return kwaitpid(pid, addr);
}