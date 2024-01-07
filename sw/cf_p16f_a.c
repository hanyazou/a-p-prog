#include "pp3.h"
#include <stdlib.h>
#include <string.h>

int cf_p16f_a_enter_progmode(void)
{
    uint8_t buf[300];

    pp_ops_init();

    // acquire_isp_dat_clk();
    pp_ops(io_dat_out(0));
    pp_ops(io_clk_out(0));

    pp_ops(io_mclr(0));
    pp_ops(delay_us(300));

    pp_ops(isp_send(0b01010000, 8));
    pp_ops(isp_send(0b01001000, 8));
    pp_ops(isp_send(0b01000011, 8));
    pp_ops(isp_send(0b01001101, 8));
    pp_ops(isp_send(0, 1));
    pp_ops(reply(0x81));

    int n = sizeof(buf);
    pp_ops(exec(buf, &n));
    debug_print("%s: n=%d, reply=0x%02x\n", __func__, n, buf[0]);

    return 0;
}

int cf_p16f_a_exit_progmode(void)
{
    uint8_t buf[300];

    debug_print( "Exiting programming mode\n");

    pp_ops_init();

    // release_isp_dat_clk();
    pp_ops(io_dat_in());
    pp_ops(io_clk_in());

    pp_ops(io_mclr(1));
    pp_ops(delay_ms(30));
    pp_ops(io_mclr(0));
    pp_ops(delay_ms(30));
    pp_ops(io_mclr(1));
    pp_ops(reply(0x82));

    int n = sizeof(buf);
    pp_ops(exec(buf, &n));
    debug_print("%s: n=%d, reply=0x%02x\n", __func__, n, buf[0]);

    return 0;
}

int cf_p16f_a_mass_erase(void)
{
    uint8_t buf[300];

    debug_print("Mass erase\n");

    cf_p16f_a_send_config(0);
    pp_ops_init();
    pp_ops(isp_send(0x09, 6));
    pp_ops(delay_ms(10));
    pp_ops_reply(0x87);

    int n = sizeof(buf);
    pp_ops_exec(buf, &n);
    debug_print("%s: n=%d, reply=0x%02x\n", __func__, n, buf[0]);

    return 0;
}

int cf_p16f_a_reset_pointer(void)
{
    uint8_t buf[300];

    debug_print("Resetting PC\n");

    pp_ops_init();
    pp_ops(isp_send(0x16, 6));
    pp_ops(reply(0x83));

    int n = sizeof(buf);
    pp_ops(exec(buf, &n));
    debug_print("%s: n=%d, reply=0x%02x\n", __func__, n, buf[0]);

    return 0;
}

int cf_p16f_a_send_config(uint16_t data)
{
    uint8_t buf[300];

    debug_print("Load config\n");

    pp_ops_init();
    pp_ops(isp_send(0x00, 6));
    pp_ops(isp_send(data, 16));
    pp_ops(reply(0x84));

    int n = sizeof(buf);
    pp_ops(exec(buf, &n));
    debug_print("%s: n=%d, reply=0x%02x\n", __func__, n, buf[0]);

    return 0;
}

int cf_p16f_a_increase_pointer(int num)
{
    uint8_t buf[300];

    num /= 2;
    debug_print( "Inc pointer %d\n", num);

    pp_ops_init();
    pp_ops(isp_send_multi(0x06, 6, num));
    pp_ops(reply(0x85));

    int n = sizeof(buf);
    pp_ops(exec(buf, &n));
    debug_print("%s: n=%d, reply=0x%02x\n", __func__, n, buf[0]);

    return 0;
}

int cf_p16f_a_read_page(uint8_t *data, int address, int num)
{
    uint8_t buf[300];

    debug_print("Reading a page of %d bytes at 0x%06x\n", num, address);

    pp_ops_init();
    pp_ops(reply(0x86));
    pp_ops_param_reset();
    pp_ops_param_set(PP_PARAM_CLK_DELAY, 1);
    pp_ops_param_set(PP_PARAM_CMD1, pp_util_revert_bit_order(0x04, 8));
    pp_ops_param_set(PP_PARAM_CMD1_LEN, 6);
    pp_ops_param_set(PP_PARAM_PREFIX_LEN, 1);
    pp_ops_param_set(PP_PARAM_DATA_LEN, 14);
    pp_ops_param_set(PP_PARAM_POSTFIX_LEN, 1);
    pp_ops_param_set(PP_PARAM_CMD2, pp_util_revert_bit_order(0x06, 8));
    pp_ops_param_set(PP_PARAM_CMD2_LEN, 6);
    pp_ops_read_isp_bits(num);

    int n = sizeof(buf);
    pp_ops(exec(buf, &n));
    debug_print("%s: n=%d, reply=0x%02x\n", __func__, n, buf[0]);

    for (int i = 0; i < num; i += 2) {
        *data++ = pp_util_revert_bit_order(buf[i + 1], 8);
        *data++ = pp_util_revert_bit_order(buf[i + 2], 8);
    }

    return 0;
}

int cf_p16f_a_write_page(uint8_t *data, int address, int num)
{
    uint8_t buf[300];

    debug_print("Writing a page of %d bytes at 0x%06x\n", num, address);

    pp_ops_init();
    pp_ops_param_reset();
    pp_ops_param_set(PP_PARAM_CLK_DELAY, 1);
    pp_ops_param_set(PP_PARAM_CMD1, pp_util_revert_bit_order(0x02, 8));
    pp_ops_param_set(PP_PARAM_CMD1_LEN, 6);

    pp_ops_param_set(PP_PARAM_PREFIX_LEN, 1);
    pp_ops_param_set(PP_PARAM_DATA_LEN, 14);
    pp_ops_param_set(PP_PARAM_POSTFIX_LEN, 1);

    pp_ops_param_set(PP_PARAM_CMD2, pp_util_revert_bit_order(0x06, 8));
    pp_ops_param_set(PP_PARAM_CMD2_LEN, 6);
    pp_ops_reply(0x88);

    int n = sizeof(buf);
    pp_ops(exec(buf, &n));
    debug_print("%s: n=%d, reply=0x%02x\n", __func__, n, buf[0]);

    int i;
    for (i = 0; i < num - 2; i += 2) {
        buf[i + 0] = pp_util_revert_bit_order(data[i + 0], 8);
        buf[i + 1] = pp_util_revert_bit_order(data[i + 1], 8);
    }
    pp_ops_init();
    pp_ops(write_isp_bits(buf, num - 2));

    pp_ops(param_set(PP_PARAM_CMD2, pp_util_revert_bit_order(0x08, 8)));
    buf[0] = pp_util_revert_bit_order(data[i + 0], 8);
    buf[1] = pp_util_revert_bit_order(data[i + 1], 8);
    pp_ops(write_isp_bits(buf, 2));
    pp_ops(delay_ms(5));
    pp_ops(isp_send(0x06, 6));
    pp_ops_reply(0x88);

    n = sizeof(buf);
    pp_ops(exec(buf, &n));
    debug_print("%s: n=%d, reply=0x%02x\n", __func__, n, buf[0]);

    return 0;
}

int cf_p16f_a_read_config(uint8_t *data, int num)
{
    memcpy(data, config_bytes, num);

    cf_p16f_a_reset_pointer();
    cf_p16f_a_send_config(0);
    cf_p16f_a_increase_pointer(7 * 2);
    cf_p16f_a_read_page(&data[7 * 2], 0, 4);

    debug_print("%s: num=%d\n", __func__, num);
    if (verbose > 2) {  // equivalent to debug_print() condition
        pp_util_hexdump("config_bytes: ", 0, config_bytes, num);
        pp_util_hexdump(" Read config: ", 0, data, num);
    }

    return 0;
}

int cf_p16f_a_write_config(uint8_t *data, int size)
{
    debug_print("%s: num=%d\n", __func__, size);
    if (verbose > 2) {  // equivalent to debug_print() condition
        pp_util_hexdump("Write config: ", 0, data, size);
    }

    cf_p16f_a_reset_pointer();
    cf_p16f_a_send_config(0);
    cf_p16f_a_increase_pointer(7 * 2);
    cf_p16f_a_write_page(&config_bytes[7 * 2], 0, 2);
    cf_p16f_a_write_page(&config_bytes[8 * 2], 0, 2);

    return 0;
}

int cf_p16f_a_get_devid(void)
{
    uint8_t buf[4];
    unsigned int devid;

    cf_p16f_a_reset_pointer();
    cf_p16f_a_send_config(0);
    cf_p16f_a_increase_pointer(6 * 2);
    cf_p16f_a_read_page(buf, 0, 4);
    devid = (((unsigned int)(buf[1]))<<8) + (((unsigned int)(buf[0]))<<0);
    devid &= devid_mask;

    return devid;
}

chip_family_t cf_p16f_a = {
    .name = "CF_P16F_A",
    .id = CF_P16F_A,
    .config_address = 0x10000,
    .config_size = 18,
    .odd_mask = 0xc0,
    .enter_progmode     = cf_p16f_a_enter_progmode,
    .exit_progmode      = cf_p16f_a_exit_progmode,
    .mass_erase         = cf_p16f_a_mass_erase,
    .reset_pointer      = cf_p16f_a_reset_pointer,
    .increase_pointer   = cf_p16f_a_increase_pointer,
    .read_page          = cf_p16f_a_read_page,
    .write_page         = cf_p16f_a_write_page,
    .read_config        = cf_p16f_a_read_config,
    .write_config       = cf_p16f_a_write_config,
    .get_device_id      = cf_p16f_a_get_devid,
};
