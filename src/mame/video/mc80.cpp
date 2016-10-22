// license:BSD-3-Clause
// copyright-holders:Miodrag Milanovic
/***************************************************************************

        MC-80.xx video by Miodrag Milanovic

        15/05/2009 Initial implementation
        12/05/2009 Skeleton driver.

****************************************************************************/

#include "includes/mc80.h"

// This is not a content of U402 510
// but order is fine

static const uint8_t prom[] = {
	0x0c,0x11,0x13,0x15,0x17,0x10,0x0e,0x00, // @
	0x04,0x0a,0x11,0x11,0x1f,0x11,0x11,0x00, // A
	0x1e,0x11,0x11,0x1e,0x11,0x11,0x1e,0x00, // B
	0x0e,0x11,0x10,0x10,0x10,0x11,0x0e,0x00, // C
	0x1e,0x09,0x09,0x09,0x09,0x09,0x1e,0x00, // D
	0x1f,0x10,0x10,0x1e,0x10,0x10,0x1f,0x00, // E
	0x1f,0x10,0x10,0x1e,0x10,0x10,0x10,0x00, // F
	0x0e,0x11,0x10,0x10,0x13,0x11,0x0f,0x00, // G

	0x11,0x11,0x11,0x1f,0x11,0x11,0x11,0x00, // H
	0x0e,0x04,0x04,0x04,0x04,0x04,0x0e,0x00, // I
	0x01,0x01,0x01,0x01,0x11,0x11,0x0e,0x00, // J
	0x11,0x12,0x14,0x18,0x14,0x12,0x11,0x00, // K
	0x10,0x10,0x10,0x10,0x10,0x10,0x1f,0x00, // L
	0x11,0x1b,0x15,0x15,0x11,0x11,0x11,0x00, // M
	0x11,0x11,0x19,0x15,0x13,0x11,0x11,0x00, // N
	0x0e,0x11,0x11,0x11,0x11,0x11,0x0e,0x00, // O

	0x1e,0x11,0x11,0x1e,0x10,0x10,0x10,0x00, // P
	0x0e,0x11,0x11,0x11,0x15,0x12,0x0d,0x00, // Q
	0x1e,0x11,0x11,0x1e,0x14,0x12,0x11,0x00, // R
	0x0e,0x11,0x10,0x0e,0x01,0x11,0x0e,0x00, // S
	0x1f,0x04,0x04,0x04,0x04,0x04,0x04,0x00, // T
	0x11,0x11,0x11,0x11,0x11,0x11,0x0e,0x00, // U
	0x11,0x11,0x11,0x0a,0x0a,0x04,0x04,0x00, // V
	0x11,0x11,0x11,0x15,0x15,0x15,0x0a,0x00, // W

	0x11,0x11,0x0a,0x04,0x0a,0x11,0x11,0x00, // X
	0x11,0x11,0x0a,0x04,0x04,0x04,0x04,0x00, // Y
	0x1f,0x01,0x02,0x04,0x08,0x10,0x1f,0x00, // Z
	0x1c,0x10,0x10,0x10,0x10,0x10,0x1c,0x00, // [
	0x00,0x10,0x08,0x04,0x02,0x01,0x00,0x00, // backslash
	0x07,0x01,0x01,0x01,0x01,0x01,0x07,0x00, // ]
	0x0e,0x11,0x00,0x00,0x00,0x00,0x00,0x00, // ^
	0x00,0x00,0x00,0x00,0x00,0x00,0x1f,0x00, // _

	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, //
	0x04,0x04,0x04,0x04,0x04,0x00,0x04,0x00, // !
	0x0a,0x0a,0x0a,0x00,0x00,0x00,0x00,0x00, // "
	0x0a,0x0a,0x1f,0x0a,0x1f,0x0a,0x0a,0x00, // #
	0x00,0x11,0x0e,0x0a,0x0e,0x11,0x00,0x00, // []
	0x18,0x19,0x02,0x04,0x08,0x13,0x03,0x00, // %
	0x04,0x0a,0x0a,0x0c,0x15,0x12,0x0d,0x00, // &
	0x04,0x04,0x08,0x00,0x00,0x00,0x00,0x00, // '

	0x02,0x04,0x08,0x08,0x08,0x04,0x02,0x00, // (
	0x08,0x04,0x02,0x02,0x02,0x04,0x08,0x00, // )
	0x00,0x04,0x15,0x0e,0x15,0x04,0x00,0x00, // *
	0x00,0x04,0x04,0x1f,0x04,0x04,0x00,0x00, // +
	0x00,0x00,0x00,0x00,0x08,0x08,0x10,0x00, // ,
	0x00,0x00,0x00,0x1f,0x00,0x00,0x00,0x00, // -
	0x00,0x00,0x00,0x00,0x00,0x00,0x08,0x00, // .
	0x00,0x01,0x02,0x04,0x08,0x10,0x00,0x00, // /

	0x0e,0x11,0x13,0x15,0x19,0x11,0x0e,0x00, // 0
	0x04,0x0c,0x04,0x04,0x04,0x04,0x0e,0x00, // 1
	0x0e,0x11,0x01,0x06,0x08,0x10,0x1f,0x00, // 2
	0x1f,0x01,0x02,0x06,0x01,0x11,0x0e,0x00, // 3
	0x02,0x06,0x0a,0x12,0x1f,0x02,0x02,0x00, // 4
	0x1f,0x10,0x1e,0x01,0x01,0x11,0x0e,0x00, // 5
	0x07,0x08,0x10,0x1e,0x11,0x11,0x0e,0x00, // 6
	0x1f,0x01,0x02,0x04,0x08,0x08,0x08,0x00, // 7

	0x0e,0x11,0x11,0x0e,0x11,0x11,0x0e,0x00, // 8
	0x0e,0x11,0x11,0x0f,0x01,0x02,0x1c,0x00, // 9
	0x00,0x00,0x00,0x00,0x08,0x00,0x08,0x00, // :
	0x00,0x00,0x04,0x00,0x04,0x04,0x08,0x00, // ;
	0x02,0x04,0x08,0x10,0x08,0x04,0x02,0x00, // <
	0x00,0x00,0x1f,0x00,0x1f,0x00,0x00,0x00, // =
	0x08,0x04,0x02,0x01,0x02,0x04,0x08,0x00, // >
	0x0e,0x11,0x01,0x02,0x04,0x00,0x04,0x00  // ?
};

/*****************************************************************************/
/*                            Implementation for MC80.2x                     */
/*****************************************************************************/



VIDEO_START_MEMBER(mc80_state,mc8020)
{
}

uint32_t mc80_state::screen_update_mc8020(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect)
{
	uint8_t y,ra,chr,gfx;
	uint16_t sy=0,ma=0,x;

	for(y = 0; y < 8; y++ )
	{
		for (ra = 0; ra < 16; ra++)
		{
			uint16_t *p = &bitmap.pix16(sy++);

			for (x = ma; x < ma + 32; x++)
			{
				if (ra > 3 && ra < 12)
				{
					chr = m_p_videoram[x];
					gfx = prom[(chr<<3) | (ra-4)];
				}
				else
					gfx = 0;

				/* Display a scanline of a character */
				*p++ = BIT(gfx, 5);
				*p++ = BIT(gfx, 4);
				*p++ = BIT(gfx, 3);
				*p++ = BIT(gfx, 2);
				*p++ = BIT(gfx, 1);
				*p++ = BIT(gfx, 0);
			}
		}
		ma+=32;
	}
	return 0;
}

/*****************************************************************************/
/*                            Implementation for MC80.3x                     */
/*****************************************************************************/


VIDEO_START_MEMBER(mc80_state,mc8030)
{
	m_p_videoram.set_target(memregion("vram")->base(),m_p_videoram.bytes());
}

uint32_t mc80_state::screen_update_mc8030(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect)
{
	uint8_t gfx;
	uint16_t y=0,ma=0,x;

	for(y = 0; y < 256; y++ )
	{
		uint16_t *p = &bitmap.pix16(y);
		{
			for (x = ma; x < ma + 64; x++)
			{
				gfx = m_p_videoram[x^0x3fff];

				/* Display a scanline of a character */
				*p++ = BIT(gfx, 7);
				*p++ = BIT(gfx, 6);
				*p++ = BIT(gfx, 5);
				*p++ = BIT(gfx, 4);
				*p++ = BIT(gfx, 3);
				*p++ = BIT(gfx, 2);
				*p++ = BIT(gfx, 1);
				*p++ = BIT(gfx, 0);
			}
		}
		ma+=64;
	}
	return 0;
}
