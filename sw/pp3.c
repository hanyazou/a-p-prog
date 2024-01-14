#include "pp3.h"
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include <string.h>

static unsigned char file_image[70000];
static int chip_family, config_size;

/*
 * programming routines
 */
static int setCPUtype(char* cpu)
{
    int name_len, i, read;
    name_len = strlen(cpu);
    for(i = 0; i < name_len; i++)
        cpu[i] = tolower(cpu[i]);
    char * line = NULL;
    char * filename = "pp3_devices.dat";
    char read_cpu_type[20], read_algo_type[20];
    int read_flash_size, read_page_size, read_id, read_mask;

    size_t len = 0;
    debug_print("Opening filename %s \n", filename);
    FILE* sf = fopen(filename, "r");
#if defined(__linux__) || defined(__APPLE__)
    if (sf == 0) {
        filename = "/etc/pp3/pp3_devices.dat";
        sf = fopen(filename, "r");
    }
#endif
    if (sf == 0) {
        info_print("Can't open device database file '%s'\n",filename);
        exit(1);
    }
    debug_print("File open\n");
    while ((read = pp_util_getline(&line, &len, sf)) != -1) {
        dump_print("\nRead %d chars: %s",read,line);
        if (line[0] != '#') {
            sscanf(line,"%s %d %d %x %x %s", (char*)&read_cpu_type,
                    &read_flash_size,&read_page_size,&read_id,&read_mask,(char*)&read_algo_type);
            dump_print("\n*** %s,%d,%d,%x,%x,%s", read_cpu_type,
                       read_flash_size, read_page_size, read_id, read_mask, read_algo_type);
            if (strcmp(read_cpu_type,cpu) == 0) {
                flash_size = read_flash_size;
                page_size = read_page_size;
                devid_expected = read_id;
                devid_mask = read_mask;
                info_print("Found database match %s,%d,%d,%x,%x,%s\n", read_cpu_type,
                           read_flash_size, read_page_size, read_id, read_mask, read_algo_type);

                if (strcmp("CF_P16F_A",   read_algo_type) == 0) chip_family = CF_P16F_A;
                if (strcmp("CF_P16F_B",   read_algo_type) == 0) chip_family = CF_P16F_B;
                if (strcmp("CF_P16F_C",   read_algo_type) == 0) chip_family = CF_P16F_C;
                if (strcmp("CF_P16F_D",   read_algo_type) == 0) chip_family = CF_P16F_D;
                if (strcmp("CF_P18F_A",   read_algo_type) == 0) chip_family = CF_P18F_A;
                if (strcmp("CF_P18F_B",   read_algo_type) == 0) chip_family = CF_P18F_B;
                if (strcmp("CF_P18F_C",   read_algo_type) == 0) chip_family = CF_P18F_C;
                if (strcmp("CF_P18F_D",   read_algo_type) == 0) chip_family = CF_P18F_D;
                if (strcmp("CF_P18F_E",   read_algo_type) == 0) chip_family = CF_P18F_E;
                if (strcmp("CF_P18F_F",   read_algo_type) == 0) chip_family = CF_P18F_F;
                if (strcmp("CF_P18F_G",   read_algo_type) == 0) chip_family = CF_P18F_G;
                if (strcmp("CF_P18F_Q",   read_algo_type) == 0) chip_family = CF_P18F_Q;
                if (strcmp("CF_P18F_Q43", read_algo_type) == 0) chip_family = CF_P18F_Q43;
                if (strcmp("CF_P18F_Q8x", read_algo_type) == 0) chip_family = CF_P18F_Q8x;

                if (chip_family == CF_P18F_A)
                    config_size = 16;
                if (chip_family == CF_P18F_B)
                    config_size = 8;
                if (chip_family == CF_P18F_C) {
                    config_size = 16;
                    chip_family = CF_P18F_B;
                }
                if (chip_family == CF_P18F_D)
                    config_size = 16;
                if (chip_family == CF_P18F_E)
                    config_size = 16;
                if (chip_family == CF_P18F_F)
                    config_size = 12;
                if (chip_family == CF_P18F_Q)
                    config_size = 12;
                if (chip_family == CF_P18F_G) {
                    config_size = 10;
                    chip_family = CF_P18F_F;
                }
                if (chip_family == CF_P18F_Q43) {
                    config_size = 10;
                    chip_family = CF_P18F_Qxx;
                }
                if (chip_family == CF_P18F_Q8x) {
                    config_size = 35;
                    chip_family = CF_P18F_Qxx;
                }
                debug_print("chip family:%d, config size:%d\n", chip_family, config_size);
            }
        }
    }
    fclose(sf);

    // means specified cpu not found in 'pp3_devices.dat'
    if (flash_size==0) {
        printf("PIC model '%s' not supported, please consider to add it to 'pp3_devices.dat'\n",
               cpu);
        exit(1);
    }
    return 0;
}

int p16a_rst_pointer(void)
{
    debug_print("Resetting PC\n");
    if (chip_family == CF_P16F_D)
        putByte(0x09);  // operation number
    else
        putByte(0x03);  // operation number
    putByte(0x00);  // number of bytes remaining
    getByte();  // return result - no check for its value
    return 0;
    }

int p16a_mass_erase(void)
{
    debug_print( "Mass erase\n");
    putByte(0x07);
    putByte(0x00);
    getByte();
    return 0;
}

int p16a_load_config(void)
{
    debug_print("Load config\n");
    putByte(0x04);
    putByte(0x00);
    getByte();
    return 0;
}

int p16a_inc_pointer(unsigned char num)
{
    debug_print( "Inc pointer %d\n", num);
    putByte(0x05);
    putByte(0x01);
    putByte(num);
    getByte();
    return 0;
}

int p16a_program_page(unsigned int ptr, unsigned char num, unsigned char slow)
{
    //	unsigned char i;
    debug_print("Programming page of %d bytes at 0x%4.4x\n", num, ptr);
    putByte(0x08);
    putByte(num+2);
    putByte(num);
    putByte(slow);
    /*
    for (i = 0; i < num; i++)
        putByte(file_image[ptr+i]);
    */
    putBytes(&file_image[ptr], num);
    getByte();
    return 0;
}

int p16a_read_page(unsigned char * data, unsigned char num)
{
    unsigned char i;
    debug_print( "Reading page of %d bytes\n", num);
    putByte(0x06);
    putByte(0x01);
    putByte(num / 2);
    getByte();
    for (i = 0; i < num; i++)
        *data++ = getByte();
    return 0;
}

int p16a_get_devid(void)
{
    unsigned char tdat[20], devid_lo, devid_hi;
    unsigned int retval;
    p16a_rst_pointer();
    p16a_load_config();
    p16a_inc_pointer(6);
    p16a_read_page(tdat, 4);
    devid_hi = tdat[(2*0)+1];
    devid_lo = tdat[(2*0)+0];
    debug_print( "Getting devid - lo:%2.2x,hi:%2.2x\n", devid_lo, devid_hi);
    retval = (((unsigned int)(devid_lo))<<0) + (((unsigned int)(devid_hi))<<8);
    retval = retval & devid_mask;
    return retval;
}

int p16a_get_config(unsigned char n)
{
    unsigned char tdat[20], devid_lo, devid_hi;
    unsigned int retval;
    p16a_rst_pointer();
    p16a_load_config();
    p16a_inc_pointer(n);
    p16a_read_page(tdat, 4);
    devid_hi = tdat[(2*0)+1];
    devid_lo = tdat[(2*0)+0];
    retval = (((unsigned int)(devid_lo))<<0) + (((unsigned int)(devid_hi))<<8);
    debug_print( "Getting config +%d - lo:%2.2x,hi:%2.2x = %4.4x\n", n, devid_lo,
                  devid_hi, retval);
    return retval;
}

int p16a_program_config(void)
{
    p16a_rst_pointer();
    p16a_load_config();
    p16a_inc_pointer(7);
    p16a_program_page(2*0x8007,2,1);
    p16a_program_page(2*0x8008,2,1);
    if ((chip_family==CF_P16F_B)|(chip_family==CF_P16F_D))
        p16a_program_page(2*0x8009,2,1);
    if (chip_family==CF_P16F_D)
        p16a_program_page(2*0x800A,2,1);
    return 0;
}

int p18a_read_page(unsigned char * data, int address, unsigned char num)
{
    unsigned char i;
    debug_print( "Reading page of %d bytes at 0x%6.6x\n", num, address);
    putByte(0x11);
    putByte(0x04);
    putByte(num/2);
    putByte((address>>16)&0xFF);
    putByte((address>>8)&0xFF);
    putByte((address>>0)&0xFF);
    getByte();
    for (i = 0; i < num; i++) {
        *data++ = getByte();
    }
    return 0;
}

int p18a_mass_erase(void)
{
    debug_print( "Mass erase\n");
    putByte(0x13);
    putByte(0x00);
    getByte();
    return 0;
}

int p18b_mass_erase(void)
{
    debug_print( "Mass erase\n");
    putByte(0x23);
    putByte(0x00);
    getByte();
    return 0;
}

int p18d_mass_erase_part(unsigned long data)
{
    debug_print( "Mass erase part of 0x%6.6x\n", data);
    putByte(0x30);
    putByte(0x03);
    putByte((data>>16)&0xFF);
    putByte((data>>8)&0xFF);
    putByte((data>>0)&0xFF);
    getByte();
    return 0;
}

int p18d_mass_erase(void)
{
    debug_print("Mass erase\n");
    p18d_mass_erase_part(0x800104);
    p18d_mass_erase_part(0x800204);
    p18d_mass_erase_part(0x800404);
    p18d_mass_erase_part(0x800804);
    /*
    p18d_mass_erase_part(0x801004);
    p18d_mass_erase_part(0x802004);
    p18d_mass_erase_part(0x804004);
    p18d_mass_erase_part(0x808004);
    */
    p18d_mass_erase_part(0x800004);
    p18d_mass_erase_part(0x800005);
    p18d_mass_erase_part(0x800002);
    return 0;
}

int p18e_mass_erase(void)
{
    debug_print("Mass erase\n");
    p18d_mass_erase_part(0x800104);
    p18d_mass_erase_part(0x800204);
    p18d_mass_erase_part(0x800404);
    p18d_mass_erase_part(0x800804);
    p18d_mass_erase_part(0x801004);
    p18d_mass_erase_part(0x802004);
    p18d_mass_erase_part(0x804004);
    p18d_mass_erase_part(0x808004);
    p18d_mass_erase_part(0x800004);
    p18d_mass_erase_part(0x800005);
    p18d_mass_erase_part(0x800002);
    return 0;
}

int p18a_write_page(unsigned char * data, int address, unsigned char num)
{
    unsigned char i,empty;
    empty = 0;
    for (i = 0; i < num; i++) {
        if (data[i]!=0xFF) {
            empty = 0;
        }
    }
    if (empty) {
        verbose_print("~");
        return 0;
    }
    debug_print("Writing A page of %d bytes at 0x%6.6x\n", num, address);
    putByte(0x12);
    putByte(4+num);
    putByte(num);
    putByte((address>>16)&0xFF);
    putByte((address>>8)&0xFF);
    putByte((address>>0)&0xFF);
    for (i = 0; i < num; i++) {
        putByte(data[i]);

        // Without this delay Arduino Leonardo's USB serial might be stalled
        sleep_us(5);
    }
    getByte();
    return 0;
}

int p18d_write_page(unsigned char * data, int address, unsigned char num)
{
    unsigned char i,empty;
    empty = 0;
    for (i = 0; i < num; i++) {
        if (data[i]!=0xFF)
            empty = 0;
    }
    if (empty) {
        verbose_print("~");
        return 0;
    }
    debug_print("Writing D page of %d bytes at 0x%6.6x\n", num, address);
    putByte(0x31);
    putByte(4+num);
    putByte(num);
    putByte((address>>16)&0xFF);
    putByte((address>>8)&0xFF);
    putByte((address>>0)&0xFF);
    for (i = 0; i < num; i++) {
        putByte(data[i]);

        // Without this delay Arduino Leonardo's USB serial might be stalled
        sleep_us(5);
    }
    getByte();
    return 0;
}

int p18a_write_cfg(unsigned char data1, unsigned char data2, int address)
{
    debug_print( "Writing cfg 0x%2.2x 0x%2.2x at 0x%6.6x\n", data1, data2, address);
    putByte(0x14);
    putByte(6);
    putByte(0);
    putByte((address>>16)&0xFF);
    putByte((address>>8)&0xFF);
    putByte((address>>0)&0xFF);
    putByte(data1);
    putByte(data2);
    getByte();
    return 0;
}

int p18d_write_cfg(unsigned char data1, unsigned char data2, int address)
{
    debug_print("Writing cfg 0x%2.2x 0x%2.2x at 0x%6.6x\n", data1, data2, address);
    putByte(0x32);
    putByte(6);
    putByte(0);
    putByte((address>>16)&0xFF);
    putByte((address>>8)&0xFF);
    putByte((address>>0)&0xFF);
    putByte(data1);
    putByte(data2);
    getByte();
    return 0;
}


int p16c_mass_erase(void)
{
    debug_print("Mass erase\n");
    putByte(0x43);
    putByte(0x00);
    getByte();
    return 0;
}

int p16c_read_page(unsigned char * data, int address, unsigned char num)
{
    unsigned char i;
    address = address / 2;
    debug_print("Reading page of %d bytes at 0x%6.6x\n", num, address);
    putByte(0x41);
    putByte(0x04);
    putByte(num/2);
    putByte((address>>16)&0xFF);
    putByte((address>>8)&0xFF);
    putByte((address>>0)&0xFF);
    getByte();
    for (i = 0; i < num; i++) {
        *data++ = getByte();
    }
    /*
    for (i = 0; i < num; i++) {
        debug_print("%2.2x ", data[i]);
    }
    */

    return 0;
}

int p16c_write_page(unsigned char * data, int address, unsigned char num)
{
    unsigned char i, empty;
    address = address / 2;
    empty = 1;
    for (i = 0; i < num; i = i + 2) {
        if	((data[i]!=0xFF)|(data[i+1]!=0xFF))
            empty = 0;
    }
    debug_print("Writing A page of %d bytes at 0x%6.6x\n", num, address);
    if (empty) {
        verbose_print("~");
        return 0;
    }
    putByte(0x42);
    putByte(4+num);
    putByte(num);
    putByte((address>>16)&0xFF);
    putByte((address>>8)&0xFF);
    putByte((address>>0)&0xFF);
    for (i = 0; i < num; i++) {
        putByte(data[i]);

        // Without this delay Arduino Leonardo's USB serial might be stalled
        sleep_us(5);
    }
    getByte();
    return 0;
}

int p16c_get_devid(void)
{
    unsigned char tdat[20],devid_lo,devid_hi;
    unsigned int retval;
    p16c_read_page(tdat, 0x8006*2,4);
    devid_hi = tdat[(2*0)+1];
    devid_lo = tdat[(2*0)+0];
    debug_print("Getting devid - lo:%2.2x,hi:%2.2x\n",devid_lo,devid_hi);
    retval = (((unsigned int)(devid_lo))<<0) + (((unsigned int)(devid_hi))<<8);
    retval = retval & devid_mask;
    return retval;
}

int p16c_write_single_cfg(unsigned char data1, unsigned char data2, int address)
{
    debug_print( "Writing cfg 0x%2.2x 0x%2.2x at 0x%6.6x\n", data1, data2, address);
    putByte(0x44);
    putByte(6);
    putByte(0);
    putByte((address>>16)&0xFF);
    putByte((address>>8)&0xFF);
    putByte((address>>0)&0xFF);
    putByte(data1);
    putByte(data2);
    getByte();
    return 0;
}

int p18qxx_mass_erase (void)
{
    debug_print( "Mass erase\n");
    putByte(0x49);
    putByte(0x00);
    getByte();
    return 0;
}

int p18q_write_single_cfg(unsigned char data1, unsigned char data2, int address)
{
    debug_print( "Writing cfg 0x%2.2x 0x%2.2x at 0x%6.6x\n", data1, data2, address);
    putByte(0x45);
    putByte(6);
    putByte(0);
    putByte((address>>16)&0xFF);
    putByte((address>>8)&0xFF);
    putByte((address>>0)&0xFF);
    putByte(data1);
    putByte(data2);
    getByte();
    return 0;
}

int p18q_write_byte_cfg(unsigned char data, int address)
{
    debug_print( "Writing cfg 0x%2.2x at 0x%6.6x\n", data, address);
    putByte(0x48);
    putByte(5);
    putByte(0);
    putByte((address>>16)&0xFF);
    putByte((address>>8)&0xFF);
    putByte((address>>0)&0xFF);
    putByte(data);
    getByte();
    return 0;
}

int p18q_write_page(unsigned char * data, int address, unsigned char num)
{
    unsigned char i, empty;
    address = address / 2;
    empty = 1;
    for (i = 0; i < num; i = i + 2) {
        if	((data[i]!=0xFF)|(data[i+1]!=0xFF))
            empty = 0;
    }
    debug_print( "Writing A page of %d bytes at 0x%6.6x\n", num, address);
    if (empty) {
        verbose_print("~");
        return 0;
    }
    putByte(0x46);
    putByte(4 + num);
    putByte(num);
    putByte((address>>16)&0xFF);
    putByte((address>>8)&0xFF);
    putByte((address>>0)&0xFF);
    for (i = 0; i < num; i++) {
        putByte(data[i]);

        // Without this delay Arduino Leonardo's USB serial might be stalled
        sleep_us(5);
    }
    getByte();
    return 0;
}

int p16c_write_cfg(void)
{
    p16c_write_single_cfg(config_bytes[1],config_bytes[0],0x8007);
    p16c_write_single_cfg(config_bytes[3],config_bytes[2],0x8008);
    p16c_write_single_cfg(config_bytes[5],config_bytes[4],0x8009);
    p16c_write_single_cfg(config_bytes[7],config_bytes[6],0x800A);
    p16c_write_single_cfg(config_bytes[9],config_bytes[8],0x800B);
    return 0;
}

int p18q_read_cfg(unsigned char * data, int address, unsigned char num)
{
    unsigned char i;
    debug_print( "Reading config of %d bytes at 0x%6.6x\n", num, address);
    putByte(0x47);
    putByte(0x04);
    putByte(num);
    putByte((address>>16)&0xFF);
    putByte((address>>8)&0xFF);
    putByte((address>>0)&0xFF);
    getByte();
    for (i = 0; i < num; i++) {
        *data++ = getByte();
    }
    // for (i = 0; i < num; i++)
    //     debug_print("%2.2x ", data[i]);

    return 0;
}

int prog_enter_progmode(void)
{
    debug_print("Entering programming mode\n");
    if (chip_family==CF_P16F_A) putByte(0x01);
    else if (chip_family==CF_P16F_B) putByte(0x01);
    else if (chip_family==CF_P16F_D) putByte(0x01);
    else if (chip_family==CF_P18F_A) putByte(0x10);
    else if (chip_family==CF_P18F_B) putByte(0x10);
    else if (chip_family==CF_P18F_D) putByte(0x10);
    else if (chip_family==CF_P18F_E) putByte(0x10);
    else if (chip_family==CF_P16F_C) putByte(0x40);
    else if (chip_family==CF_P18F_F) putByte(0x40);
    else if (chip_family==CF_P18F_Q) putByte(0x40);
    else if (chip_family==CF_P18F_Qxx) putByte(0x40);
    putByte(0x00);
    getByte();
    return 0;
}

int prog_exit_progmode(void)
{
    debug_print( "Exiting programming mode\n");
    putByte(0x02);
    putByte(0x00);
    getByte();
    return 0;
}

int prog_reset(void)
{
    return prog_exit_progmode();
}

int prog_get_device_id(void)
{
    unsigned char mem_str[10];
    unsigned int devid;
    debug_print("getting ID for family %d\n",chip_family);

    if ((chip_family==CF_P16F_A)|(chip_family==CF_P16F_B)|(chip_family==CF_P16F_D))
        return p16a_get_devid();
    if (chip_family==CF_P16F_C)
        return p16c_get_devid();
    if ((chip_family==CF_P18F_A)|(chip_family==CF_P18F_B)|(chip_family==CF_P18F_D)|(chip_family==CF_P18F_E)) {
        p18a_read_page((unsigned char *)&mem_str, 0x3FFFFE, 2);
        devid = (((unsigned int)(mem_str[1]))<<8) + (((unsigned int)(mem_str[0]))<<0);
        devid = devid & devid_mask;
        return devid;
    }
    if ((chip_family==CF_P18F_F)|(chip_family==CF_P18F_Q)|(chip_family==CF_P18F_Qxx)) {
        p16c_read_page(mem_str, 0x3FFFFE*2,2);
        devid = (((unsigned int)(mem_str[1]))<<8) + (((unsigned int)(mem_str[0]))<<0);
        devid = devid & devid_mask;
        return devid;
    }

    return 0;
}

/*
 * hex parse and main function
 */

static int parseHex(char * filename, unsigned char * progmem, unsigned char * config)
{
    char * line = NULL;
    unsigned char line_content[128];
    size_t len = 0;
    int i,temp, read,line_len, line_type, line_address, line_address_offset,effective_address;
    int p16_cfg = 0;
    debug_print("Opening filename %s \n", filename);
    FILE* sf = fopen(filename, "r");
    if (sf == 0) {
        fprintf (stderr, "Can't open hex file %s\n", filename);
        return -1;
    }
    line_address_offset = 0;
    if (chip_family==CF_P16F_A) p16_cfg = 1;
    if (chip_family==CF_P16F_B) p16_cfg = 1;
    if (chip_family==CF_P16F_C) p16_cfg = 1;
    if (chip_family==CF_P16F_D) p16_cfg = 1;

    debug_print("File open\n");
    while ((read = pp_util_getline(&line, &len, sf)) != -1) {
        dump_print("\nRead %d chars: %s",read,line);
        if (line[0] != ':') {
            info_print("--- : invalid\n");
            return -1;
        }
        sscanf(line+1, "%2X", &line_len);
        sscanf(line+3, "%4X", &line_address);
        sscanf(line+7, "%2X", &line_type);
        effective_address = line_address+(65536*line_address_offset);
        dump_print("Line len %d B, type %d, address 0x%4.4x offset 0x%4.4x, EFF 0x%6.6x\n",
                   line_len,line_type,line_address,line_address_offset,effective_address);
        if (line_type == 0) {
            for (i = 0; i < line_len; i++) {
                sscanf(line+9+i*2, "%2X", &temp);
                line_content[i] = temp;
            }
            if (effective_address < flash_size) {
                dump_print("PM ");
                for (i = 0; i < line_len; i++)
                    progmem[effective_address+i] = line_content[i];
            }
            if ((line_address_offset == 0x30) &&
                ((chip_family==CF_P18F_A)|(chip_family==CF_P18F_D)|(chip_family==CF_P18F_E)|
                 (chip_family==CF_P18F_F)|(chip_family==CF_P18F_Q)|(chip_family==CF_P18F_Qxx))) {
                dump_print("CB ");
                for (i = 0; i < line_len; i++)
                    config[i] = line_content[i];
            }
            if ((chip_family == CF_P18F_B) && (effective_address == (flash_size-config_size))) {
                dump_print("CB ");
                for (i = 0; i < line_len; i++)
                    config[i] = line_content[i];
            }
            if ((line_address_offset == 0x01) && (p16_cfg == 1)) {
                dump_print("CB ");
                for (i = 0; i < line_len; i++) {
                    if (0 <= line_address + i - 0x0E)
                        config[line_address+i-0x0E] = line_content[i];
                }
            }
        }
        if (line_type == 4) {
            sscanf(line+9, "%4X", &line_address_offset);
        }
        for (i = 0; i < line_len; i++)
            dump_print("%2.2X", line_content[i]);
        dump_print("\n");
    }
    fclose(sf);
    return 0;
}

int legacy_pp3(void)
{
    int i, j, pages_performed, config, econfig;
    unsigned char *pm_point, *cm_point;
    unsigned char tdat[200];

    setCPUtype(cpu_type_name);
    // check setCPUtype works or not.
    if (flash_size == 0) {
          printf("Please use '-t MODEL' to specify correct PIC model, such as '16f1824'\n");
          exit(1);
    }

    if (sleep_time > 0) {
        info_print("Sleeping for %d ms while arduino bootloader expires\n", sleep_time);
        fflush(stdout);
        sleep_ms(sleep_time);
    }

    pm_point = (unsigned char *)(&progmem);
    cm_point = (unsigned char *)(&config_bytes);
    if ((program || verify) && parseHex(input_file_name, pm_point, cm_point)) {
        // parse and write content of hex file into buffers
        fprintf(stderr,"Failed to read input file.\n");
        abort();
    }

    // now this is ugly kludge
    // my original programmer expected only file_image holding the image of memory to be programmed
    // for PIC18, it is divided into two regions, program memory and config. to glue those two
    // different approaches, I made this. not particulary proud of having this mess
    for (i = 0; i < 70000; i++)
        file_image[i] = progmem[i];
    for (i = 0; i < 10; i++)
        file_image[2*0x8007 + i] = config_bytes[i];
    // for (i = 0; i < 10; i++)
    //     printf("%2.2x", config_bytes[i]);
    for (i = 0; i < 70000; i++) {
        if (i % 2)
            file_image[i] = 0x3F & file_image[i];
    }

    if (reset) {
        printf("Reset the target and wait for %d ms\n", reset_time);
        prog_reset();
        sleep_ms(reset_time);
    }

    prog_enter_progmode();  // enter programming mode and probe the target
    i = prog_get_device_id();
    if (i == devid_expected) {
        info_print("Device ID: %4.4x \n", i);
    } else {
        printf("Wrong device ID: %4.4x, expected: %4.4x\n", i, devid_expected);
        printf("Check for connection to target MCU, exiting now\n");
        prog_exit_progmode();
        exit(1);
    }

    // ah, I need to unify programming interfaces for PIC16 and PIC18
    if ((chip_family==CF_P18F_A)|(chip_family==CF_P18F_B)|(chip_family==CF_P18F_D)|
        (chip_family==CF_P18F_E)|(chip_family==CF_P18F_F)|(chip_family==CF_P18F_Q)|
        (chip_family==CF_P18F_Qxx)) {
        //
        // PIC 18F
        //
        if (program) {
            pages_performed = 0;
            if (chip_family==CF_P18F_A)						//erase whole device
                p18a_mass_erase();
            if (chip_family==CF_P18F_B)
                p18b_mass_erase();
            if (chip_family==CF_P18F_D)
                p18d_mass_erase();
            if (chip_family==CF_P18F_E)
                p18e_mass_erase();
            if ((chip_family==CF_P18F_F)|(chip_family==CF_P18F_Q))
                p16c_mass_erase();
            if (chip_family==CF_P18F_Qxx)
                p18qxx_mass_erase();
            info_print("Programming FLASH (%d B in %d pages per %d bytes): \n", flash_size,
                       flash_size/page_size,page_size);
            fflush(stdout);
            for (i = 0; i < flash_size; i=i+page_size) {
                if (!is_empty(progmem+i, page_size)) {
                    if ((chip_family==CF_P18F_D)|(chip_family==CF_P18F_E))
                        p18d_write_page(progmem + i, i, page_size);
                    else
                    if (chip_family==CF_P18F_F)
                        p16c_write_page(progmem + i, i * 2, page_size);
                    else
                    if ((chip_family==CF_P18F_Q)|(chip_family==CF_P18F_Qxx))
                        p18q_write_page(progmem + i, i * 2, page_size);
                    else
                        p18a_write_page(progmem + i, i, page_size);
                    pages_performed++;
                    info_print("#");
                } else {
                    detail_print(".");
                }
            }

            info_print("\n%d pages programmed\n",pages_performed);
            info_print("Programming config\n");
            for (i = 0; i < config_size; i = i + 2) {
                // write config bytes for PIC18Fxxxx and 18FxxKxx devices
                if (chip_family==CF_P18F_A)
                    p18a_write_cfg(config_bytes[i], config_bytes[i+1], 0x300000+i);
                if (chip_family==CF_P18F_D)
                    p18d_write_cfg(config_bytes[i], config_bytes[i+1], 0x300000+i);
                if (chip_family==CF_P18F_E)
                    p18d_write_cfg(config_bytes[i], config_bytes[i+1], 0x300000+i);
                if (chip_family==CF_P18F_F)
                    p16c_write_single_cfg(config_bytes[i+1], config_bytes[i], 0x300000+i);
                if (chip_family==CF_P18F_Q)
                    p18q_write_single_cfg(config_bytes[i+1], config_bytes[i], 0x300000+i);
                if (chip_family==CF_P18F_Qxx) {
                    p18q_write_byte_cfg(config_bytes[i], 0x300000+i);
                    if (i + 1 < config_size)
                        p18q_write_byte_cfg(config_bytes[i+1], 0x300000+i+1);
                }
            }
            // for PIC18FxxJxx, config bytes are at the end of FLASH memory
        }
        if (verify) {
            pages_performed = 0;
            info_print("Verifying FLASH (%d B in %d pages per %d bytes): \n",flash_size,
                       flash_size/page_size,page_size);
            for (i = 0; i < flash_size; i = i + page_size) {
                if (is_empty(progmem+i,page_size)) {
                    detail_print(".");
                } else {
                    if ((chip_family==CF_P18F_F)|(chip_family==CF_P18F_Q)|
                        (chip_family==CF_P18F_Qxx))
                        p16c_read_page(tdat, i*2, page_size);
                    else
                        p18a_read_page(tdat, i, page_size);
                    pages_performed++;
                    verbose_print("Verifying page at 0x%4.4X\n", i);
                    info_print("#");
                    for (j = 0; j < page_size; j++) {
                        if (progmem[i+j] != tdat[j]) {
                            printf("\nError at program address 0x%06X E:0x%02X R:0x%02X\n", i + j,
                                    progmem[i + j], tdat[j]);
                            printf("Exiting now\n");
                            prog_exit_progmode();
                            exit(1);
                        }
                    }
                }
            }
            info_print("\n%d pages verified\n", pages_performed);
            if ((chip_family==CF_P18F_F)|(chip_family==CF_P18F_Q))
                p16c_read_page(tdat,0x300000*2,page_size);
            else if (chip_family==CF_P18F_Qxx)
                p18q_read_cfg(tdat,0x300000,config_size);
            else
                p18a_read_page(tdat,0x300000,page_size);

            info_print("Verifying config...");
            for (i = 0; i < config_size; i++) {
                if (config_bytes[i] != tdat[i]) {
                    printf("Error at config address 0x%02X E:0x%02X R:0x%02X\n", i,
                           config_bytes[i], tdat[i]);
                    printf("Exiting now\n");
                    prog_exit_progmode();
                    exit(1);
                }
            }
            info_print("OK\n");
        }
    } else {
        //
        // PIC 16F
        //
        if (program) {
            if ((chip_family==CF_P16F_A)|(chip_family==CF_P16F_B)|(chip_family==CF_P16F_D))
                p16a_mass_erase();
            if (chip_family==CF_P16F_C)
                p16c_mass_erase();
            if ((chip_family==CF_P16F_A)|(chip_family==CF_P16F_B)|(chip_family==CF_P16F_D))
                p16a_rst_pointer();  // pointer reset is needed before every "big" operation
            info_print("Programming FLASH (%d B in %d pages per %d bytes): \n", flash_size,
                       flash_size / page_size, page_size);
            fflush(stdout);
            for (i = 0; i < flash_size; i = i + page_size) {
                info_print("#");
                if ((chip_family==CF_P16F_A)|(chip_family==CF_P16F_B)|(chip_family==CF_P16F_D))
                    p16a_program_page(i,page_size,0);
                if (chip_family==CF_P16F_C)
                    p16c_write_page(progmem+i,i,page_size);
            }
            info_print("\n");
            info_print("Programming config\n");
            if ((chip_family==CF_P16F_A)|(chip_family==CF_P16F_B)|(chip_family==CF_P16F_D))
                p16a_program_config();
            if (chip_family==CF_P16F_C)
                p16c_write_cfg();
        }
        if (verify) {
            info_print("Verifying FLASH (%d B in %d pages per %d bytes): \n", flash_size,
                       flash_size / page_size, page_size);
            if ((chip_family==CF_P16F_A)|(chip_family==CF_P16F_B)|(chip_family==CF_P16F_D))
                p16a_rst_pointer();
            for (i = 0; i < flash_size; i = i + page_size) {
                info_print("#");
                if ((chip_family==CF_P16F_A)|(chip_family==CF_P16F_B)|(chip_family==CF_P16F_D))
                    p16a_read_page(tdat, page_size);
                if (chip_family==CF_P16F_C)
                    p16c_read_page(tdat, i, page_size);
                for (j = 0; j < page_size; j++) {
                    if (file_image[i+j] != tdat[j]) {
                        printf("\nError at program address 0x%06X E:0x%02X R:0x%02X\n",
                               i + j, file_image[i + j], tdat[j]);
                        if (verbose > 2) {  // equivalent to debug_print() condition
                            pp_util_hexdump("   file_image: ", i, &progmem[i], page_size);
                            pp_util_hexdump(" Read program: ", i, tdat, page_size);
                        }
                        prog_exit_progmode();
                        exit(1);
                    }
                }
            }
            info_print("\n");
            info_print("Verifying config...");
            if ((chip_family==CF_P16F_A)|(chip_family==CF_P16F_B)|(chip_family==CF_P16F_D)) {
                info_print("\n");
                config = p16a_get_config(7);
                econfig = (((unsigned int)(file_image[2*0x8007]))<<0) +
                    (((unsigned int)(file_image[2*0x8007+1]))<<8);
                if (config == econfig) {
                    info_print("config 1 OK: %4.4X\n",config);
                } else {
                    printf("config 1 error: E:0x%4.4X R:0x%4.4X\n", econfig, config);
                    prog_exit_progmode();
                    exit(1);
                }

                config = p16a_get_config(8);
                econfig = (((unsigned int)(file_image[2*0x8008]))<<0) +
                    (((unsigned int)(file_image[2*0x8008+1]))<<8);
                if (config == econfig) {
                    info_print("config 2 OK: %4.4X\n", config);
                } else {
                    printf("config 2 error: E:0x%4.4X R:0x%4.4X\n", econfig, config);
                    prog_exit_progmode();
                    exit(1);
                }
            }
            if (chip_family==CF_P16F_C) {
                p16c_read_page(tdat,0x8007*2,page_size);
                for (j = 0; j < 10; j++) {
                    if (config_bytes[j] != tdat[j]) {
                        printf("Error at config address 0x%02X E:0x%02X R:0x%02X\n",
                                7*2 + j, config_bytes[j], tdat[j]);
                        if (verbose > 2) {  // equivalent to debug_print() condition
                            pp_util_hexdump(" config_bytes: ", i, config_bytes, 10);
                            pp_util_hexdump("  Read config: ", i, tdat, 10);
                        }
                        prog_exit_progmode();
                        exit(1);
                    }
                }
                info_print("OK\n");
            }
        }
    }
    prog_exit_progmode();
    exit(0);
}
