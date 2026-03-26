#include "types.h"
#include "x86.h"
#include "defs.h"
#include "date.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"

pte_t* walkpgdir(pde_t *pgdir, const void *va, int alloc);

int
sys_fork(void)
{
  return fork();
}

int
sys_exit(void)
{
  exit();
  return 0;  // not reached
}

int
sys_wait(void)
{
  return wait();
}

int
sys_kill(void)
{
  int pid;

  if(argint(0, &pid) < 0)
    return -1;
  return kill(pid);
}

int
sys_getpid(void)
{
  return myproc()->pid;
}

int
sys_sbrk(void)
{
  int addr;
  int n;

  if(argint(0, &n) < 0)
    return -1;
  addr = myproc()->sz;
  if(growproc(n) < 0)
    return -1;
  return addr;
}

int
sys_sleep(void)
{
  int n;
  uint ticks0;

  if(argint(0, &n) < 0)
    return -1;
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(myproc()->killed){
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
int
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}

int
sys_mprotect(void)
{
  void *addr;
  int len;
  if(argptr(0, (void*)&addr, sizeof(void*)) < 0 || argint(1, &len) < 0)
    return -1;
  if(len <= 0 || (uint)addr % PGSIZE != 0)
    return -1;

  struct proc *p = myproc();
  for(int i = 0; i < len; i++){
    pte_t *pte = walkpgdir(p->pgdir, (char*)addr + i*PGSIZE, 0);
    if(!pte || !(*pte & PTE_P))
      return -1;
    *pte &= ~PTE_W;  // clear write bit
  }
  lcr3(V2P(p->pgdir));  // flush TLB
  return 0;
}

int
sys_munprotect(void)
{
  void *addr;
  int len;
  if(argptr(0, (void*)&addr, sizeof(void*)) < 0 || argint(1, &len) < 0)
    return -1;
  if(len <= 0 || (uint)addr % PGSIZE != 0)
    return -1;

  struct proc *p = myproc();
  for(int i = 0; i < len; i++){
    pte_t *pte = walkpgdir(p->pgdir, (char*)addr + i*PGSIZE, 0);
    if(!pte || !(*pte & PTE_P))
      return -1;
    *pte |= PTE_W;  // restore write bit
  }
  lcr3(V2P(p->pgdir));  // flush TLB
  return 0;
}