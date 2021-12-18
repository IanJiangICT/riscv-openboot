#ifndef DW_I2C_H
#define DW_I2C_H
void dw_i2c_master_init(volatile unsigned char *uart_base);
void dw_i2c_set_address(volatile unsigned char *base, unsigned char addr);
void dw_i2c_set_frequency(volatile unsigned char *base, unsigned int apb_speed, unsigned int i2c_khz);
int dw_i2c_read_bytes(volatile unsigned char *base, unsigned char dev, unsigned char *buffer, unsigned int len, unsigned int stop);
int dw_i2c_write_bytes(volatile unsigned char *base, unsigned char dev, unsigned char *buffer, unsigned int len, unsigned int stop);

int dw_i2c_read_mem(volatile unsigned char *base, unsigned char dev, unsigned int addr, unsigned int alen, unsigned char *buffer, unsigned int len);
int dw_i2c_write_mem(volatile unsigned char *base, unsigned char dev, unsigned int addr, unsigned int alen, unsigned char *buffer, unsigned int len);

int dw_i2c_write_vip(volatile unsigned char *base, unsigned char dev, unsigned int addr, unsigned char *buffer, unsigned int len);
int dw_i2c_read_vip(volatile unsigned char *base, unsigned char dev, unsigned char *buffer, unsigned len);
#endif
