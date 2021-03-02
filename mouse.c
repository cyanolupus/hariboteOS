#include "bootpack.h"

void enable_mouse(struct MOUSE_DEC *mdec) {
    wait_KBC_sendready();
    io_out8(PORT_KEYCMD, KEYCMD_SENDTO_MOUSE); // マウスに送信準備
    wait_KBC_sendready();
    io_out8(PORT_KEYDAT, MOUSECMD_ENABLE); // 送信するもの
    mdec->phase = 0;
    return;
}

int mouse_decode(struct MOUSE_DEC *mdec, unsigned char dat) {
    if (mdec->phase == 0) {
        if (dat == 0xfa) { // 11111010
            mdec->phase = 1;
        }
        return 0;
    }
    if (mdec->phase == 1) {
        if ((dat & 0xc8) == 0x08) { // 11001000 と AND をとることで0-3,8-fに絞る
            mdec->buf[0] = dat;
            mdec->phase = 2;
        }
        return 0;
    }
    if (mdec->phase == 2) {
        mdec->buf[1] = dat;
        mdec->phase = 3;
        return 0;
    }
    if (mdec->phase == 3) {
        mdec->buf[2] = dat;
        mdec->phase = 1;
        mdec->btn = mdec->buf[0] & 0x07; // 00000111 と AND をとることで 下3桁だけ取る
        mdec->x = mdec->buf[1];
        mdec->y = mdec->buf[2];
        if ((mdec->buf[0] & 0x10) != 0) { // 00010000
            mdec->x |= 0xffffff00; // 1fill
        }
        if ((mdec->buf[0] & 0x20) != 0) { // 00100000
            mdec->y |= 0xffffff00; // 1fill
        }
        mdec->y = - mdec->y; // 縦の向きが逆なため
        return 1;
    }
    return -1; // 異常コード
}