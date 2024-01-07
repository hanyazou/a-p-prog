#include <stdio.h>
#include <stdint.h>
#include "../fw/pp/fw_pp_ops.h"

#define	PROGMEM_LEN	260000
#define	CONFIG_LEN	35

enum {
    CF_P16F_A,
    CF_P18F_A,
    CF_P16F_B,
    CF_P18F_B,
    CF_P18F_C,
    CF_P18F_D,
    CF_P18F_E,
    CF_P16F_C,
    CF_P16F_D,
    CF_P18F_F,
    CF_P18F_G,
    CF_P18F_Q,
    CF_P18F_Q43,
    CF_P18F_Q8x,
    CF_P18F_Qxx,

    CF_NO_LEGACY  // use this to prevent legacy if (chip_family == xxx) processing
};

typedef struct {
    char *name;
    int id;
    uint32_t config_address;
    int config_size;
    uint8_t odd_mask, even_mask;
    int (*enter_progmode)(void);
    int (*exit_progmode)(void);
    int (*mass_erase)(void);
    int (*reset_pointer)(void);
    int (*increase_pointer)(int num);
    int (*read_page)(uint8_t *data, int address, int num);
    int (*write_page)(uint8_t *data, int address, int num);
    int (*read_config)(uint8_t *data, int size);
    int (*write_config)(uint8_t *data, int size);
    int (*get_device_id)(void);
} chip_family_t;

extern int verbose;
extern int devid_mask, flash_size, page_size, chip_family, config_size;
extern unsigned char file_image[70000], progmem[PROGMEM_LEN], config_bytes[CONFIG_LEN];

extern void sleep_us(int num);
extern void putByte(int byte);
extern int getByte(void);
extern void flsprintf(FILE* f, char *fmt, ...);

extern void pp_ops_init(void);
extern int pp_ops_io_mclr(int v);
extern int pp_ops_io_dat_in(void);
extern int pp_ops_io_dat_out(int v);
extern int pp_ops_io_clk_in(void);
extern int pp_ops_io_clk_out(int v);
extern int pp_ops_read_isp(int n);
extern int pp_ops_write_isp(uint8_t *v, int n);
#if defined(PP_EXEC_OPS_RW_BITS)
extern int pp_ops_read_isp_bits(int n);
extern int pp_ops_write_isp_bits(uint8_t *v, int n);
extern int pp_ops_isp_send_msb_multi(uint32_t v, int len, int n);
extern int pp_ops_isp_send_multi(uint32_t v, int len, int n);
static inline int pp_ops_isp_send_msb(uint32_t v, int len) {
    return pp_ops_isp_send_msb_multi(v, len, 1);
}
static inline int pp_ops_isp_send(uint32_t v, int len) {
    return pp_ops_isp_send_multi(v, len, 1);
}
#endif
extern int pp_ops_delay_us(int n);
extern int pp_ops_delay_ms(int n);
extern int pp_ops_reply(uint8_t v);
extern int pp_ops_param_set(int param, int value);
extern int pp_ops_param_reset(void);
extern int pp_ops_exec(uint8_t *v, int *n);
extern int pp_ops_write_isp_8(uint8_t v);
extern int pp_ops_write_isp_24(uint32_t v);

extern uint32_t pp_util_revert_bit_order(uint32_t v, int n);
extern void pp_util_hexdump(const char *header, uint32_t addr_offs, const void *data, int size);

#define pp_ops(f) do { int res = pp_ops_ ## f; if (res != 0) { \
        return res; \
    } } while (0)

#define info_print(fmt ...) do { if (verbose > 0) \
            flsprintf(stdout, fmt); } while (0)
#define detail_print(fmt ...) do { if (verbose > 1) \
            flsprintf(stdout, fmt); } while (0)
#define debug_print(fmt ...) do { if (verbose > 2) \
            flsprintf(stdout, fmt); } while (0)
#define verbose_print(fmt ...) do { if (verbose > 3) \
            flsprintf(stdout, fmt); } while (0)
#define dump_print(fmt ...) do { if (verbose > 4) \
            flsprintf(stdout, fmt); } while (0)

// CF_P16F_A
extern chip_family_t cf_p16f_a;
int cf_p16f_a_enter_progmode(void);
int cf_p16f_a_exit_progmode(void);
int cf_p16f_a_mass_erase(void);
int cf_p16f_a_reset_pointer(void);
int cf_p16f_a_send_config(uint16_t data);
int cf_p16f_a_increase_pointer(int num);
int cf_p16f_a_read_page(uint8_t *data, int address, int num);
int cf_p16f_a_write_page(uint8_t *data, int address, int num);
int cf_p16f_a_read_config(uint8_t *data, int num);
int cf_p16f_a_write_config(uint8_t *data, int size);
int cf_p16f_a_get_devid(void);

// CF_P16F_B
extern chip_family_t cf_p16f_b;
int cf_p16f_b_read_config(uint8_t *data, int num);
int cf_p16f_b_write_config(uint8_t *data, int size);

// CF_P16F_C
int cf_p16f_c_set_pc(unsigned long pc);
int cf_p16f_c_enter_progmode(void);
int cf_p16f_c_read_page(uint8_t *data, int address, int num);

// CF_P18F_Q
int cf_p18f_q_write_page(uint8_t *data, int address, int num);
int cf_p18f_q_read_config(uint8_t *data, int num);
int cf_p18f_q_write_config(uint8_t *data, int size);
int cf_p18f_q_get_device_id(void);

// CF_P18F_Qxx
extern chip_family_t cf_p18q43;
extern chip_family_t cf_p18q8x;
int cf_p18f_qxx_mass_erase(void);
