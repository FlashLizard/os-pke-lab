#ifndef _TYPES_H_
#define _TYPES_H_

typedef unsigned char uint8;
typedef unsigned short uint16;
typedef unsigned int uint32;
typedef unsigned long long uint64;

typedef signed char int8;
typedef signed short int16;
typedef signed int int32;
typedef signed long long int64;

typedef int bool;

typedef signed long ssize_t;
typedef unsigned long size_t;

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

#define NULL ((void *)0)
#define TRUE 1
#define FALSE 0

#endif
