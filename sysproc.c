#include "types.h"
#include "x86.h"
#include "defs.h"
#include "date.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"
#include "procinfo.h" // HA2

extern pte_t* walkpgdir(pde_t *pgdir, const void *va, int alloc); //LAB5-CA4

int sys_fork(void)
{
  return fork();
}

int sys_exit(void)
{
  exit();
  return 0; // not reached
}

int sys_wait(void)
{
  return wait();
}

int sys_kill(void)
{
  int pid;

  if (argint(0, &pid) < 0)
    return -1;
  return kill(pid);
}

int sys_getpid(void)
{
  return myproc()->pid;
}

int sys_sbrk(void)
{
  int addr;
  int n;

  if (argint(0, &n) < 0)
    return -1;
  addr = myproc()->sz;
  if (growproc(n) < 0)
    return -1;
  return addr;
}

int sys_sleep(void)
{
  int n;
  uint ticks0;

  if (argint(0, &n) < 0)
    return -1;
  acquire(&tickslock);
  ticks0 = ticks;
  while (ticks - ticks0 < n)
  {
    if (myproc()->killed)
    {
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

// return how many clock tick interrupts have occurred
// since start.
int sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}

// CA1->step3
int sys_strrev(void)
{
  char *str;
  int len, i;
  if (argstr(0, &str) < 0 || argint(1, &len) < 0)
    return -1;
  // Simple in-place reversal
  for (i = 0; i < len / 2; i++)
  {
    char tmp = str[i];
    str[i] = str[len - i - 1];
    str[len - i - 1] = tmp;
  }
  return 0;
}

// HA1->step4
int sys_setflag(void)
{
  int arg;
  if (argint(0, &arg) < 0)
    return -1;
  myproc()->perProcess = arg;
  return 0;
}

// HA1->step4
int sys_getflag(void)
{
  return myproc()->perProcess;
}

// LAB4-TA6-gmeet
int sys_getstats(void)
{
  int *user_stats_ptr;

  if (argptr(0, (void *)&user_stats_ptr, 2 * sizeof(int)) < 0)
    return -1;
  struct proc *p = myproc();
  int kernel_stats[2];
  kernel_stats[0] = p->shed_count;
  kernel_stats[1] = p->run_ticks;

  if (copyout(p->pgdir, (uint)user_stats_ptr, (char *)kernel_stats,
              sizeof(kernel_stats)) < 0)
    return -1;

  return 0;
}

// HA2
int sys_get_proc_info(void)
{
  int pid;
  struct proc_info *info;

  // Fetch the first argument (int pid) from the user stack.
  if (argint(0, &pid) < 0)
    return -1;

  // Fetch the second argument (struct proc_info*) from the user stack.
  // argptr also verifies that the pointer is valid.
  if (argptr(1, (char **)&info, sizeof(*info)) < 0)
    return -1;

  // Now call the actual implementation with the validated arguments.
  return get_proc_info(pid, info);
}

//LAB5-step3
int sys_numvp(void)
{
  struct proc *p = myproc();
  return (p->sz + PGSIZE - 1) / PGSIZE + 1;
}

//LAB5-step3
int sys_numpp(void)
{
  struct proc *p = myproc();
  pte_t *pte;
  int count = 0;
  for (uint a = 0; a < p->sz; a += PGSIZE)
  {
    pte = walkpgdir(p->pgdir, (void *)a, 0);
    if (pte && (*pte & PTE_P))
      count++;
  }
  return count;
}

//LAB5-step3
int sys_getptsize(void)
{
  struct proc *p = myproc();
  int count = 1; // outer page directory
  for (int i = 0; i < NPDENTRIES; i++)
    if (p->pgdir[i] & PTE_P)
      count++;
  return count;
}

//HA3->step2c
int sys_setpriority(void)
{
  int priority;
  if (argint(0, &priority) < 0)
  {
    return -1;
  }
  // Assign the given priority value to the calling process ’s priority field 
  struct proc * p = myproc();
  p->priorityMark = priority;
  return 0;
}