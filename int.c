#include "bootpack.h"

struct FIFO8 keyfifo;

void init_pic(void) {
    io_out8(PIC0_IMR, 0xff); /* 割り込みさせない */
    io_out8(PIC1_IMR, 0xff); /* 割り込みさせない */

    io_out8(PIC0_ICW1, 0x11); /* エッジトリガモード */
    io_out8(PIC0_ICW2, 0x20); /* IRQ0-7 -> INT20-27 */
    io_out8(PIC0_ICW3, 1 << 2); /* PIC1 to IRQ2 */
    io_out8(PIC0_ICW4, 0x01); /* ノンバッファモード */

    io_out8(PIC1_ICW1, 0x11); /* エッジトリガモード */
    io_out8(PIC1_ICW2, 0x28); /* IRQ8-f -> INT28-2f */
    io_out8(PIC1_ICW3, 2); /* PIC1 to IRQ2 */
    io_out8(PIC1_ICW4, 0x01); /* ノンバッファモード */

    io_out8(PIC0_IMR, 0xfb); /* 11111011 PIC1以外は全てダメ */
    io_out8(PIC1_IMR, 0xff); /* 割り込みさせない */

    return;
}

void fifo8_init(struct FIFO8 *fifo, int size, unsigned char *buf) {
    fifo->size = size;
    fifo->buf = buf;
    fifo->free = size; // free
    fifo->flags = 0;
    fifo->p = 0; // write
    fifo->q = 0; // read
    return;
}

int fifo8_put(struct FIFO8 *fifo, unsigned char data) {
    if (fifo->free == 0) {
        fifo->flags |= FRAGS_OVERRUN;
        return -1;
    }
    fifo->buf[fifo->p] = data;
    fifo->p++;
    if (fifo->p == fifo->size) {
        fifo->p = 0;
    }
    fifo->free--;
    return 0;
}

int fifo8_get(struct FIFO8 *fifo) {
    int data;
    if (fifo->free == fifo->size) {
        return -1;
    }
    data = fifo->buf[fifo->q];
    fifo->q++;
    if (fifo->q == fifo->size) {
        fifo->q = 0;
    }
    fifo->free++;
    return data;
}

int fifo8_status(struct FIFO8 *fifo) {
    return fifo->size - fifo->free;
}

void inthandler21(int *esp) {
    unsigned char data;
    io_out8(PIC0_OCW2, 0x61);
    data = io_in8(PORT_KEYDAT);
    fifo8_put(&keyfifo, data);
    return;
}

void inthandler2c(int *esp) {
    struct BOOTINFO *binfo = (struct BOOTINFO *) ADR_BOOTINFO;
    boxfill8(binfo->vram, binfo->scrnx, COL8_000000, 0, 0, 32 * 8 - 1, 15);
    putfonts8_asc(binfo->vram, binfo->scrnx, 0, 0, COL8_FFFFFF, "INT 2C (IRQ-12) : PS/2 mouse");
    for (;;) {
        io_hlt();
    }
}

void inthandler27(int *esp) {
	io_out8(PIC0_OCW2, 0x67); /* IRQ-07受付完了をPICに通知(7-1参照) */
	return;
    /* PIC0からの不完全割り込み対策 */
/* Athlon64X2機などではチップセットの都合によりPICの初期化時にこの割り込みが1度だけおこる */
/* この割り込み処理関数は、その割り込みに対して何もしないでやり過ごす */
/* なぜ何もしなくていいの？
	→  この割り込みはPIC初期化時の電気的なノイズによって発生したものなので、
		まじめに何か処理してやる必要がない。									*/
}

