#include "bootpack.h"

unsigned int memtest(unsigned int start, unsigned int end) {
    char flg486 = 0;
    unsigned int eflg, cr0, i;

    // check 386 or 486
    eflg = io_load_eflags();
    eflg |= EFLAGS_AC_BIT; // AC-bit = 1
    io_store_eflags(eflg);
    eflg = io_load_eflags();
    if ((eflg & EFLAGS_AC_BIT) != 0) { // 386ではAC-bitが自動でリセットされてしまうのでそれを見る
        flg486 = 1;
    }
    eflg &= ~EFLAGS_AC_BIT; // AC-bit = 0
    io_store_eflags(eflg);

    if (flg486 != 0) {
        cr0 = load_cr0();
        cr0 |= CR0_CACHE_DISABLE; // cr0キャッシュ無効化
        store_cr0(cr0);
    }

    i = memtest_main(start, end);

    if (flg486 != 0) {
        cr0 = load_cr0();
        cr0 &= ~CR0_CACHE_DISABLE; // cr0キャッシュ有効化
        store_cr0(cr0);
    }

    return i;
}

/*
unsigned int memtest_main(unsigned int start, unsigned int end) {
    unsigned int i, *p, old, pat0 = 0xaa55aa55, pat1 = 0x55aa55aa;
    for (i = start; i <= end; i += 0x1000) {
        p = (unsigned int *) (i += 0xffc);
        old = *p; // もとの値を保存
        *p = pat0; // 試しに書いてみる
        *p ^= 0xffffffff; // 反転
        if (*p != pat1) {
            *p = old;
            break;
        }
        *p ^= 0xffffffff; // 反転２
        if (*p != pat0) {
            *p = old;
            break;
        }
        *p = old;
    }
    return i;
}
*/