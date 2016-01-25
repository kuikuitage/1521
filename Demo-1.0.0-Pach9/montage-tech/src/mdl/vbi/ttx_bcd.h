/******************************************************************************/
/******************************************************************************/

#ifndef __TTX_BCD_H__
#define __TTX_BCD_H__



static inline u32 vbi_dec2bcd(u32 dec)
{
    return (dec % 10) + ((dec / 10) % 10) * 16 + ((dec / 100) % 10) * 256;
}


static inline u32 vbi_bcd2dec(u32 bcd)
{
    return (bcd & 15) + ((bcd >> 4) & 15) * 10 + ((bcd >> 8) & 15) * 100;
}


static inline u32 vbi_add_bcd(u32 a, u32 b)
{
    u32 t = 0;

    a += 0x06666666;
    t  = a + b;
    b ^= a ^ t;
    b  = (~b & 0x11111110) >> 3;
    b |= b * 2;

    return t - b;
}


static inline BOOL vbi_is_bcd(u32 bcd)
{
    return 0 == (((bcd + 0x06666666) ^ (bcd ^ 0x06666666)) & 0x11111110);
}


static inline BOOL vbi_bcd_digits_greater(u32 bcd, u32 maximum)
{
    maximum ^= ~0;

    return 0 != (((bcd + maximum) ^ bcd ^ maximum) & 0x11111110);
}


#endif



