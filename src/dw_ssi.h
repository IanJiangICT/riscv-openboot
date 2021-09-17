#ifndef DW_SSI_H
#define DW_SSI_H

void dw_ssi_init(volatile void *ssi_base);
void dw_ssi_read_byte(volatile void *ssi_base, unsigned int offset, unsigned char *buf);

#endif
