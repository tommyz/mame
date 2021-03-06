// license:BSD-3-Clause
// copyright-holders:Nicola Salmoria

#include "machine/74259.h"
#include "sound/tms5110.h"

class bagman_state : public driver_device
{
public:
	bagman_state(const machine_config &mconfig, device_type type, const char *tag)
		: driver_device(mconfig, type, tag),
		m_maincpu(*this, "maincpu"),
		m_gfxdecode(*this, "gfxdecode"),
		m_palette(*this, "palette"),
		m_tmsprom(*this, "tmsprom"),
		m_tmslatch(*this, "tmslatch"),
		m_videoram(*this, "videoram"),
		m_colorram(*this, "colorram"),
		m_spriteram(*this, "spriteram") { }

	required_device<cpu_device> m_maincpu;
	required_device<gfxdecode_device> m_gfxdecode;
	required_device<palette_device> m_palette;
	optional_device<tmsprom_device> m_tmsprom;
	optional_device<ls259_device> m_tmslatch;

	required_shared_ptr<uint8_t> m_videoram;
	required_shared_ptr<uint8_t> m_colorram;
	required_shared_ptr<uint8_t> m_spriteram;

	bool m_irq_mask;
	bool m_video_enable;
	uint8_t m_p1_res;
	uint8_t m_p1_old_val;
	uint8_t m_p2_res;
	uint8_t m_p2_old_val;

	/*table holds outputs of all ANDs (after AND map)*/
	uint8_t m_andmap[64];

	/*table holds inputs (ie. not x, x, not q, q) to the AND map*/
	uint8_t m_columnvalue[32];

	/*8 output pins (actually 6 output and 2 input/output)*/
	uint8_t m_outvalue[8];

	tilemap_t *m_bg_tilemap;

	// common
	DECLARE_WRITE_LINE_MEMBER(coin_counter_w);
	DECLARE_WRITE_LINE_MEMBER(irq_mask_w);
	DECLARE_WRITE8_MEMBER(videoram_w);
	DECLARE_WRITE8_MEMBER(colorram_w);
	DECLARE_WRITE_LINE_MEMBER(flipscreen_x_w);
	DECLARE_WRITE_LINE_MEMBER(flipscreen_y_w);
	DECLARE_WRITE_LINE_MEMBER(video_enable_w);

	// bagman
	DECLARE_WRITE8_MEMBER(ls259_w);
	DECLARE_WRITE_LINE_MEMBER(tmsprom_bit_w);
	DECLARE_WRITE_LINE_MEMBER(tmsprom_csq0_w);
	DECLARE_WRITE_LINE_MEMBER(tmsprom_csq1_w);
	DECLARE_WRITE8_MEMBER(pal16r6_w);
	DECLARE_READ8_MEMBER(pal16r6_r);

	// squaitsa
	DECLARE_READ8_MEMBER(dial_input_p1_r);
	DECLARE_READ8_MEMBER(dial_input_p2_r);

	TILE_GET_INFO_MEMBER(get_bg_tile_info);

	virtual void machine_start() override;
	DECLARE_MACHINE_START(squaitsa);
	virtual void machine_reset() override;
	virtual void video_start() override;
	DECLARE_PALETTE_INIT(bagman);

	INTERRUPT_GEN_MEMBER(vblank_irq);

	uint32_t screen_update(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect);
	void draw_sprites(bitmap_ind16 &bitmap, const rectangle &cliprect);
	void update_pal();
	void botanic(machine_config &config);
	void squaitsa(machine_config &config);
	void sbagman(machine_config &config);
	void bagman(machine_config &config);
	void pickin(machine_config &config);
	void sbagmani(machine_config &config);
	void main_map(address_map &map);
	void main_portmap(address_map &map);
	void pickin_map(address_map &map);
};

/*----------- timings -----------*/

#define BAGMAN_MAIN_CLOCK   XTAL(18'432'000)
#define BAGMAN_HCLK         (BAGMAN_MAIN_CLOCK / 3)
#define BAGMAN_H0           (BAGMAN_HCLK / 2)
#define BAGMAN_H1           (BAGMAN_H0   / 2)
#define HTOTAL              ((0x100-0x40)*2)
#define HBEND               (0x00)
#define HBSTART             (0x100)
#define VTOTAL              ((0x100-0x7c)*2)

/* the following VBEND/VBSTART are used for compsync
 * #define VBEND                (0x08)
 * #define VBSTART              (0x100)
 *
 * However VBSYQ (and INTQ) is generated using the following values:
 */
#define VBEND               (0x10)
#define VBSTART             (0xf0)
