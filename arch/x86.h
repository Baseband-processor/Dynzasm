#ifndef X86_H
#define X86_H

#include "../include/dis.h"
#include "../common/trie.h"
#include "x86strings.h"
#include "x86load.h"
#include <stdint.h>

/*Macro to check a bit for a flag*/
#define CHECK_FLAG(byte, flag) (!!(byte & flag))

/*Trie Node Flags*/
#define REG_EXT_FLAG 		2
#define PREFIX_FLAG 		4
#define OFFSET_FLAG 		8
#define MOD_EXT_FLAG 		16

/*Instruction Flags*/
#define OPER_SIZE_OVERRIDE 	1 //01
#define ADDR_SIZE_OVERRIDE 	2 //10
#define REX_B 			4 //100
#define REX_X			8 //1000
#define REX_R 			16 //10000
#define REX_W 			32 //100000

/*Defaults*/
#define DEF_OPER_SIZE 		3
#define DEF_ADDR_SIZE 		4

/*Encoding constants*/
#define SIB_RM 			4
#define MODRM_INDIR 		0
#define MODRM_1DISP 		1
#define MODRM_4DISP 		2
#define MODRM_REG 		3
#define MODRM_DISPONLY(mod, rm) ((mod==0) && (rm == 5))
#define SIB_NO_BASE(mod, base) 	(base==5 && (mod != 3))
#define SIB_NO_INDEX(idx) 	(idx==4)

typedef unsigned char u8;

long x86_disassemble(struct trie_node *node, u8 *stream, long max, struct dis *disas);
long x86_decode_operand(struct operand_tree **opt, char *operand, u8 flags, u8 *stream, long max);
int x86_operand_size(int op_size, char size_byte, u8 flags);
long x86_decode_modrm(struct operand_tree **operand, int op_size, int addr_size, u8 *stream, long max, u8 flags);
long x86_decode_sib(struct operand_tree **operand, int op_size, int addr_size, u8 *stream, long max, u8 flags);
long x86_disassemble_operand(struct operand_tree **operand, u8 addr_mode, int op_size, int addr_size, u8 *stream, long max, u8 flags);
struct operand_tree *x86_indir_operand_tree(int op_size, const char *base, const char *index, long scale, unsigned long offset);
long get_integer(uint64_t * val, int size, u8 *stream, long max);
void fmt_offset_str(struct operand_tree *operand, const char *offset);

#endif
