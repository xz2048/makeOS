// 初始化一些东西

#include "bootpack.h"

// PIC 初始化
void init_pic(void){
    io_out8(PIC0_IMR,  0xff  ); /* 禁止所有中断 */
	io_out8(PIC1_IMR,  0xff  ); /* 禁止所有中断 */

	io_out8(PIC0_ICW1, 0x11  ); /* 边缘触发模式（edge trigger mode） */
	io_out8(PIC0_ICW2, 0x20  ); /* IRQ0-7由INT20-27接收 */
	io_out8(PIC0_ICW3, 1 << 2); /* PIC1由IRQ2相连 */
	io_out8(PIC0_ICW4, 0x01  ); /* 无缓冲区模式 */

	io_out8(PIC1_ICW1, 0x11  ); /* 边缘触发模式（edge trigger mode） */
	io_out8(PIC1_ICW2, 0x28  ); /* IRQ8-15由INT28-2f接收 */
	io_out8(PIC1_ICW3, 2     ); /* PIC1由IRQ2连接 */
	io_out8(PIC1_ICW4, 0x01  ); /* 无缓冲区模式 */

	io_out8(PIC0_IMR,  0xfb  ); /* 11111011 PIC1以外全部禁止 */
	io_out8(PIC1_IMR,  0xff  ); /* 11111111 禁止所有中断 */

	return;
}



// 21号中断的处理程序,键盘中断
#define PORT_KEYDAT 0x0060
struct FIFO8 keyfifo;

void inthandler21(int *esp){
	unsigned char data;
	io_out8(PIC0_OCW2, 0x61);		// 通知PIC的"IRQ-01已经受理完毕
	data = io_in8(PORT_KEYDAT);
	fifo8_put(&keyfifo, data);
    return;
}

// 鼠标中断
struct FIFO8 mousefifo;
void inthandler2c(int *esp){
    unsigned char data;
	io_out8(PIC1_OCW2, 0x64);		// 通知PIC的"IRQ-12已经受理完毕
	io_out8(PIC0_OCW2, 0x62);		// 通知PIC的"IRQ-02已经受理完毕
	data = io_in8(PORT_KEYDAT);
	fifo8_put(&mousefifo, data);
    return;
}


void inthandler27(int *esp)
/* PIC0中断的不完整策略 */
/* 这个中断在Athlon64X2上通过芯片组提供的便利，只需执行一次 */
/* 这个中断只是接收，不执行任何操作 */
/* 为什么不处理？
	→  因为这个中断可能是电气噪声引发的、只是处理一些重要的情况。*/
{
	io_out8(PIC0_OCW2, 0x67); /* 通知PIC的IRQ-07（参考7-1） */
	return;
}
