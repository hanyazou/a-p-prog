#ifndef __FW_PP_OPS_H__
#define __FW_PP_OPS_H__

#define PP_EXEC_OPS
#define PP_EXEC_OPS_RW_BITS

#if defined(PP_EXEC_OPS)
enum {
    OP_NONE,
    OP_IO_MCLR,
    OP_IO_DAT,
    OP_IO_CLK,
    OP_READ_ISP,
    OP_WRITE_ISP,
#if defined(PP_EXEC_OPS_RW_BITS)
    OP_READ_ISP_BITS,
    OP_WRITE_ISP_BITS,
#endif
    OP_DELAY_US,
    OP_DELAY_10US,
    OP_DELAY_MS,
    OP_REPLY,
    OP_PARAM_SET,
    OP_PARAM_RESET,
};

enum {
    PP_PARAM_CLK_DELAY,
    PP_PARAM_CMD1,
    PP_PARAM_CMD1_LEN,
    PP_PARAM_DELAY1,
    PP_PARAM_CMD2,
    PP_PARAM_CMD2_LEN,
    PP_PARAM_DELAY2,
    PP_PARAM_PREFIX_LEN,
    PP_PARAM_DATA_LEN,
    PP_PARAM_POSTFIX_LEN,
    PP_PARAM_DELAY3,
    PP_PARAM_NUM_PARAMS  // number of parameters
};
#endif  // PP_EXEC_OPS
#endif  // __FW_PP_OPS_H__
