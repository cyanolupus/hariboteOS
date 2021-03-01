#include <stdio.h>
#include "bootpack.h"

void HariMain(void)
{
	struct BOOTINFO *binfo = (struct BOOTINFO *) ADR_BOOTINFO;
	char s[40], mcursor[256];
	int mx, my;

	init_gdtidt();
	init_pic();
	io_sti();

	init_palette();
	init_screen8(binfo->vram, binfo->scrnx, binfo->scrny);

	mx = (binfo->scrnx - 16) / 2; 
	my = (binfo->scrny - 28 - 16) / 2;
	init_mouse_cursor8(mcursor, COL8_008484);
	putblock8_8(binfo->vram, binfo->scrnx, 16, 16, mx, my, mcursor, 16);
	sprintf(s, "(%d,%d)", mx, my);
	putfonts8_asc(binfo->vram, binfo->scrnx, 9, 9, COL8_000000, "Haribote OS");
	putfonts8_asc(binfo->vram, binfo->scrnx, 8, 8, COL8_FFFFFF, "Haribote OS");
	putfonts8_asc(binfo->vram, binfo->scrnx, 100, 10, COL8_FF00FF, "Kawaisou ha kawaii");
	putfonts8_asc(binfo->vram, binfo->scrnx, 8, 40, COL8_FFFFFF, s);

	io_out8(PIC0_IMR, 0xf9); /* pic1とキーボードを有効化 */
	io_out8(PIC1_IMR, 0xef); /* マウスを有効化 */

	for (;;) {
		io_hlt();
	}
}