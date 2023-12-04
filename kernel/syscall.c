/*
 * contains the implementation of all syscalls.
 */

#include <stdint.h>
#include <errno.h>

#include "util/types.h"
#include "syscall.h"
#include "string.h"
#include "process.h"
#include "elf.h"
#include "util/functions.h"

#include "spike_interface/spike_utils.h"

#define SYM_INFO_TYPE_MASK 0x0f

char buf[2 << 20];
elf_header ehdr;
Elf64_Shdr *shdr;
char *shstrtab;
char *strtab;
Elf64_Sym *symtab;
int sym_cnt;
int has_load = 0;

// load the elf info of the current program
void sys_load_elf_info()
{
  if (has_load)
    return;
  has_load = 1;
  spike_file_t *file;
  file = spike_file_open(current->file_name, O_RDONLY, 0);
  spike_file_pread(file, &ehdr, sizeof(ehdr), 0);
  spike_file_pread(file, &buf, ehdr.shoff + ehdr.shnum * ehdr.shentsize, 0);
  shdr = (Elf64_Shdr *)(buf + ehdr.shoff);
  shstrtab = buf + shdr[ehdr.shstrndx].sh_offset;
  for (int i = 0; i < ehdr.shnum; i++)
  {
    if (strcmp(shstrtab + shdr[i].sh_name, ".strtab") == 0)
    {
      strtab = buf + shdr[i].sh_offset;
    }
    if (strcmp(shstrtab + shdr[i].sh_name, ".symtab") == 0)
    {
      symtab = (Elf64_Sym *)(buf + shdr[i].sh_offset);
      sym_cnt = shdr[i].sh_size / sizeof(Elf64_Sym);
    }
  }
  // close the host spike file
  spike_file_close(file);
}

// print the sections info
uint64 sys_print_section_info()
{
  sys_load_elf_info();
  sprint("section table info: num: %d size: %d, string table addr: 0x%lx, offset: 0x%lx\n",
         ehdr.shnum, ehdr.shentsize, ehdr.shstrndx, ehdr.shoff);
  for (int i = 0; i < ehdr.shnum; i++)
  {
    sprint("section %d: name: %s, offset: 0x%lx, size: 0x%lx\n",
           i, shstrtab + shdr[i].sh_name, shdr[i].sh_offset, shdr[i].sh_size);
  }
  return 0;
}

// return the addr of the function that contains the given address
uint64 sys_which_func(uint64 addr)
{
  sys_load_elf_info();
  for (int i = 0; i < sym_cnt; i++)
  {
    if ((symtab[i].st_info & SYM_INFO_TYPE_MASK) == 2 &&
        symtab[i].st_value <= addr && addr < symtab[i].st_value + symtab[i].st_size)
    {
      // sprint("function: %s\n", strtab + symtab[i].st_name);
      return symtab[i].st_name;
    }
  }
  return -1;
}

//
// implement the SYS_user_print syscall
//
ssize_t sys_user_print(const char *buf, size_t n)
{
  sprint(buf);
  return 0;
}

//
// implement the SYS_user_exit syscall
//
ssize_t sys_user_exit(uint64 code)
{
  sprint("User exit with code:%d.\n", code);
  // in lab1, PKE considers only one app (one process).
  // therefore, shutdown the system when the app calls exit()
  shutdown(code);
}

ssize_t sys_user_print_backtrace(uint64 n)
{
  uint64 fp = current->trapframe->regs.s0; // 上一个栈帧的顶
  uint64 ra = current->trapframe->regs.ra; // 要返回的地址
  int fl = 0;
  n++; // to exit do_syscall frame
  //sys_print_section_info();
  while (n--)
  {
    // sprint("ra: 0x%lx fp: 0x%lx\n", ra, fp);
    uint64 func_addr = sys_which_func(ra);
    if (fl)
    {
      if (func_addr != -1)
      {
        char *func_name = strtab + func_addr;
        sprint("%s\n", strtab + func_addr);
        if (strcmp(func_name, "main") == 0)
          break;
      }
      fp = *(uint64 *)(fp - 16);
      ra = *(uint64 *)(fp - 8);
    }
    else // 第一个只有fp没有ra
    {
      fp = *(uint64 *)(fp - 8);
      ra = *(uint64 *)(fp - 8);
    }
    fl = 1;
  }
  return 0;
}

//
// [a0]: the syscall number; [a1] ... [a7]: arguments to the syscalls.
// returns the code of success, (e.g., 0 means success, fail for otherwise)
//
long do_syscall(long a0, long a1, long a2, long a3, long a4, long a5, long a6, long a7)
{
  switch (a0)
  {
  case SYS_user_print:
    return sys_user_print((const char *)a1, a2);
  case SYS_user_exit:
    return sys_user_exit(a1);
  case SYS_user_print_backtrace:
    return sys_user_print_backtrace(a1);
  default:
    panic("Unknown syscall %ld \n", a0);
  }
}
