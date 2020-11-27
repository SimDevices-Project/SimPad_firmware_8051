/**
 * NyaSama Core Virtualize Module
 * NSDN (C) 2014 - 2020
 * Version: 0.01a
 **/
#ifndef __CVM_H_
#define __CVM_H_


#include <stdint.h>

#define CVM_VER "0.01a"

/* ---------------- 配置区域开始 ---------------- */

#define CVM_FUNC_MAX     0xFF
//#define CVM_FUNC_MAX     0x1FF
//#define CVM_FUNC_MAX     0x3FF
//#define CVM_FUNC_MAX     0x7FF

/* 使用超过 256 种指令的情况下, 需开启长指令支持, 并关闭变长指令支持 */
/* 比如: 编号为 0x100 ~ 0x1FF 的指令省略目标操作数的时候会引发错误 */
#define CVM_VARLEN_OP
//#define CVM_LONGLEN_OP

#define cvm_ret        uint8_t
#define CVM_RET_END    0xFF
#define CVM_RET_RST    0xFE

#define CVM_RET_OK     0x00
#define CVM_RET_ERR    0x01

#define cvm_addr       uint16_t

/* ---------------- 配置区域结束 ---------------- */


/* ------------------------------------------------- */
/* ---------------- 以下内容不应修改 ---------------- */
/* ------------------------------------------------- */

// 目标操作数类型便捷访问宏定义
#define type_dst type_dst_expr

typedef struct {
    uint8_t     op_index;       // 1 byte, 指令编号低 8 位
    uint8_t     type_dst_expr;  // 1 byte, 低 7 位为目标操作数类型, 或者为双字节指令的操作数
             // type_dst & 0x80    1 bit, 指令编号第 9 位, 目前作为保留位
    uint32_t    dst;            // 4 bytes, 立即数或虚拟地址
    uint8_t     type_src;       // 1 byte, 低 7 位为源操作数类型
             // type_src & 0x80    1 bit, 指令编号第 10 位, 目前作为保留位
    uint32_t    src;            // 4 bytes, 立即数或虚拟地址
    uint8_t     type_ext;       // 1 byte, 低 7 位为附加操作数类型
             // type_ext & 0x80    1 bit, 指令编号第 11 位, 目前作为保留位
    uint32_t    ext;            // 4 bytes, 立即数或虚拟地址
} CVM_OP_L;                    // 16 bytes

typedef struct {
    uint8_t     op_index;       // 1 byte, 指令编号
    uint8_t     type_dst_expr;  // 1 byte, 目标操作数类型, 或者为双字节指令的操作数
    uint16_t    dst;            // 2 bytes, 立即数或虚拟地址
    uint8_t     type_src;       // 1 byte, 源操作数类型
    uint16_t    src;            // 2 bytes, 立即数或虚拟地址
    uint8_t     reversed;       // 1 byte, 保留
} CVM_OP_S;                    // 8 bytes

#ifdef CVM_VARLEN_OP
    #define CVM_OP_NARG                1
    #define CVM_OP_DST8                2
    #ifdef CVM_LONGLEN_OP
        #define CVM_OP_DST             6
        #define CVM_OP_DST_SRC         11
        #define CVM_OP_DST_SRC_EXT     16
    #else
        #define CVM_OP_DST             4
        #define CVM_OP_DST_SRC         8
    #endif
#endif

#ifdef CVM_LONGLEN_OP
#define CVM_OP CVM_OP_L
typedef uint32_t cvm_arg;
#else
#define CVM_OP CVM_OP_S
typedef uint16_t cvm_arg;
#endif

typedef struct {
    cvm_ret (*func)(CVM_OP* op);
#ifdef CVM_VARLEN_OP
    uint8_t length;
#endif
} CVM_FUNC;

#ifndef CVM_FUNC_MAX
#define CVM_FUNC_MAX 0x80
#endif

#if CVM_FUNC_MAX > 0xFF
#ifndef CVM_LONGLEN_OP
#error CVM_LONGLEN_OP should be defined.
#endif
#ifdef CVM_VARLEN_OP
#error CVM_VARLEN_OP should not be defined.
#endif
#endif

#if CVM_FUNC_MAX <= 0xFF
typedef uint8_t cvm_opi;
#else
typedef uint16_t cvm_opi;
#endif

typedef struct {
    char msg;
    cvm_addr addr;
    CVM_OP op;
} CVM_ERR_INFO;

CVM_ERR_INFO* cvm_err_info();

void cvm_end();
void cvm_jmp(cvm_addr addr);
cvm_ret cvm_exe(CVM_OP* op);
cvm_ret cvm_run(uint8_t* code, cvm_addr length);

void cvm_wdt(cvm_ret (*callback)());


#endif
