#ifndef _LCD_H_
#define _LCD_H_


//#define LCD_LINE_1  ((1 << 7) | 0x00)
//#define LCD_LINE_2  ((1 << 7) | 0x40)

#define LCD_LINE_1  0x80
#define LCD_LINE_2  0xc0


int lcd_init(void);



int lcd_write_cmd(uint8 cmd);


int lcd_write_data(uint8 data);


void lcd_test_for_nad(void);

#endif




