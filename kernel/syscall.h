/*
 * define the syscall numbers of PKE OS kernel.
 */
#ifndef _SYSCALL_H_
#define _SYSCALL_H_

// syscalls of PKE OS kernel. append below if adding new syscalls.
#define SYS_user_base 64
#define SYS_user_print (SYS_user_base + 0)
#define SYS_user_exit (SYS_user_base + 1)
#define SYS_user_print_backtrace (SYS_user_base + 2)

long do_syscall(long a0, long a1, long a2, long a3, long a4, long a5, long a6, long a7);

typedef struct
{
  uint32	sh_name;		/* Section name (string tbl index) */
  uint32	sh_type;		/* Section type */
  uint64	sh_flags;		/* Section flags */
  uint64	sh_addr;		/* Section virtual addr at execution */
  uint64	sh_offset;		/* Section file offset */
  uint64	sh_size;		/* Section size in bytes */
  uint32	sh_link;		/* Link to another section */
  uint32	sh_info;		/* Additional section information */
  uint64	sh_addralign;		/* Section alignment */
  uint64	sh_entsize;		/* Entry size if section holds table */
} Elf64_Shdr;

typedef struct {
	uint32	st_name; // 4 B (B for bytes)
	unsigned char	st_info; // 1 B
	unsigned char	st_other; // 1 B
	uint16	st_shndx; // 2 B
	uint64	st_value; // 8 B
	uint64	st_size; // 8 B
} Elf64_Sym; 

#endif
