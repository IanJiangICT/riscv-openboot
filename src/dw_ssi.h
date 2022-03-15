#ifndef DW_SSI_H
#define DW_SSI_H

void dw_ssi_init(volatile unsigned char *ssi_base, unsigned int freq_div);
void dw_ssi_jedec_id(volatile unsigned char *ssi_base, unsigned char *manufacturer, unsigned char *mem_type, unsigned char *capacity);
void dw_ssi_read_byte(volatile unsigned char *ssi_base, unsigned int offset, unsigned char *buf, unsigned char addr_4bytes);

#endif
