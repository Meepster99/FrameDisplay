// MBTL Frame Display:
//
// Core manager.

#include "mbtl_framedisplay.h"

#include <cstdio>
#include <cstring>
#include <iostream>
#include <fstream>

struct MBTL_Character_Info {
	const char	*long_name;
	const char	*short_name;
	const char	*file_name;
  /*
  int		ha6_size;
	int		ha6_offset;
  int		ha6_9_size;
  int		ha6_9_offset;
  int		pal_size;
  int		pal_offset;
  int		cg_size;
  int		cg_offset;
  */
  // CG, HA6, PAL, HA6_9
  int sizes[4];
  int offsets[4];
};

static const MBTL_Character_Info mbtl_character_info[] = {
  {"Arcueid",		"ARC",	"chr000", { 0x00059D66,0x0143C3, 0xF010, 0x019D0848}, {0x019F1AFF,0x0412767D, 0x01A5FD4D, 0x0212B7 }	},
  {"Hisui",		"HIS",	"chr001", { 0x5f672, 0xdc38, 0xf010, 0x27a7128 }, {0x6951d99, 0x995cf51, 0x69c5f93, 0x41aac71}	},
  {"S.Akiha",		"SAK",	"chr002", { 0x4636b, 0xa8a0, 0xf010, 0x1e13d60}, {0xb801ae6, 0xd9bb185, 0xb85bc59, 0x99edd86 } },
  {"Shiki",		"SIK",	"chr003", { 0x4f87a, 0xf699, 0xf010, 0x1cfa5a8}, {0xf72e7d3, 0x11e5b86d, 0xf7923f5, 0xda3422b } },
  {"Kohaku",		"KOH",	"chr004", { 0x62d8b, 0xd40d, 0xf010, 0x2a0ee98}, {0x148eb626, 0x17026fde, 0x14963cd9, 0x11edc78e } },
  {"Roa",		"ROA",	"chr005", { 0x4dbea, 0x4ed8, 0xf010, 0x2087808}, {0x1913bbb5, 0x1b8a1fda, 0x1919d547, 0x170b43ad } },
  {"Kouma",		"KIS",	"chr006", { 0x5005f, 0x3ee5, 0xf010, 0x1eccb58}, {0x1d7c4a8d, 0x1fe8cee4, 0x1d828694, 0x1b8f7f35 } },
  //{"Maids",		"HNK",	"chr007", { 
  {"Noel",		"NOE",	"chr008", { 0x55cbb, 0xee39, 0xf010, 0x2936a38}, {0x228138f7, 0x268e7143, 0x2287d9ba, 0x1fedcebf } },
  {"VLOV",		"VLO",	"chr009", { 0x5a9d0, 0xd102, 0xf010, 0x2f06c10}, {0x29877cd1, 0x2bf1885d, 0x298e64c9, 0x269710c1 } },
  {"Warcueid",		"BOA",	"chr010", { 0x4da6b, 0xdcc8, 0xf010, 0x2351020}, {0x2e2ecc35, 0x31b5da3e, 0x2e34e9a8, 0x2bf9bc15 } },
  {"Ciel",		"CIE",	"chr011",	{ 0x60867, 0x41ee, 0xf010, 0x2875a38}, {0x34450db6, 0x370514df, 0x344c6125, 0x31bdb37e } },
  {"Saber",		"ALT",	"chr012", { 0x6c59b, 0x4ab1, 0xf010, 0x3c96630}, {0x3ad59372, 0x3d8553d4, 0x3addeeb5, 0x370c2d42 } },
  {"Miyako",		"MIY",	"chr013", { 0x4ce8f, 0x3488, 0xf010, 0x18ef968}, {0x3f19da14, 0x419f431f, 0x3f1ff00b, 0x3d8ae0ac } },
};

static const int mbtl_ncharacter_info = sizeof(mbtl_character_info)/sizeof(mbtl_character_info[0]);

const char *MBTL_FrameDisplay::get_character_long_name(int n) {
	if (n < 0 || n >= mbtl_ncharacter_info) {
		return FrameDisplay::get_character_long_name(n);
	}
	return mbtl_character_info[n].long_name;
}

const char *MBTL_FrameDisplay::get_character_name(int n) {
	if (n < 0 || n >= mbtl_ncharacter_info) {
		return FrameDisplay::get_character_name(n);
	}
	return mbtl_character_info[n].short_name;
}

int MBTL_FrameDisplay::get_sequence_count() {
	if (!m_initialized) {
		return 0;
	}

	return m_character_data.get_sequence_count();
}

bool MBTL_FrameDisplay::has_sequence(int n) {
	if (!m_initialized) {
		return 0;
	}

	return m_character_data.has_sequence(n);
}
const char *MBTL_FrameDisplay::get_sequence_name(int n) {
	if (!m_initialized) {
		return 0;
	}

	return m_character_data.get_sequence_name(n);
}

const char *MBTL_FrameDisplay::get_sequence_move_name(int n, int *dmeter) {
	if (!m_initialized) {
		return 0;
	}

	const char *str = m_character_data.get_sequence_move_name(n, dmeter);

	if (!str) {
		const char *ch_name = get_character_name(m_character);

		if (ch_name && !strstr(ch_name, "EFFECT")) {
			switch(n) {
			case 0:	str = "5"; break;
			case 1: str = "5A"; break;
			case 2: str = "5B"; break;
			case 3: str = "5C"; break;
			case 4: str = "2A"; break;
			case 5: str = "2B"; break;
			case 6: str = "2C"; break;
			case 7: str = "j.A"; break;
			case 8: str = "j.B"; break;
			case 9: str = "j.C"; break;
			case 10: str = "6"; break;
			case 11: str = "4"; break;
			case 12: str = "5->2"; break;
			case 13: str = "2"; break;
			case 14: str = "2->5"; break;
			case 17: str = "4 Guard"; break;
			case 18: str = "3 Guard"; break;
			case 19: str = "j.4 Guard";
			case 35: str = "9"; break;
			case 36: str = "8"; break;
			case 37: str = "7"; break;
			case 38: str = "j.9"; break;
			case 39: str = "j.8"; break;
			case 40: str = "j.7"; break;
			case 50: str = "intro"; break;
			case 52: str = "win pose"; break;
			case 250: str = "heat"; break;
			case 255: str = "circuit spark"; break;
			}
		}
	}

	return str;
}

int MBTL_FrameDisplay::get_frame_count() {
	if (!m_initialized) {
		return 0;
	}

	return m_character_data.get_frame_count(m_sequence);
}

int MBTL_FrameDisplay::get_subframe() {
	return m_subframe + m_subframe_base;
}

int MBTL_FrameDisplay::get_subframe_count() {
	if (!m_initialized) {
		return 0;
	}

	return m_character_data.get_subframe_count(m_sequence);
}

void MBTL_FrameDisplay::set_frame(int n) {
	if (!m_initialized) {
		return;
	}

	m_frame = m_character_data.find_frame(m_sequence, n);

	m_subframe = 0;

	m_subframe_base = m_character_data.count_subframes(m_sequence, m_frame);
	m_subframe_next = m_character_data.get_subframe_length(m_sequence, m_frame);
}

void MBTL_FrameDisplay::set_sequence(int n) {
	if (!m_initialized) {
		return;
	}

	m_sequence = m_character_data.find_sequence(n, n<m_sequence?-1:1);

	m_subframe_base = 0;
	m_subframe = 0;

	set_frame(0);
}

void MBTL_FrameDisplay::set_palette(int n) {
	if (!m_initialized) {
		return;
	}

	m_palette = n%36;

	m_character_data.set_palette(n);
}

void MBTL_FrameDisplay::set_active_character(int n) {
	if (!m_initialized) {
		return;
	}

	if (n == m_character) {
		return;
	}

	if (n < 0 || n >= (mbtl_ncharacter_info)) {
		return;
	}

	bool need_gfx = 1;

	if (m_character >= 0 && m_character < mbtl_ncharacter_info) {
		if (!strcmp(mbtl_character_info[m_character].file_name, mbtl_character_info[n].file_name)) {
			need_gfx = 0;
		}
	}

	if (need_gfx) {
		m_character_data.free();
	} else {
		m_character_data.free_frame_data();
	}

	if (m_character_data.load(&m_pack, mbtl_character_info[n].file_name,
                            mbtl_character_info[n].sizes,
                            mbtl_character_info[n].offsets )) {
		if (need_gfx) {
        m_character_data.load_graphics(&m_pack,
                                       mbtl_character_info[n].sizes[3],
                                       mbtl_character_info[n].offsets[3]
                                       );
		}
	}
	m_character = n;

	set_palette(m_palette);
	set_sequence(0);
	set_frame(0);
}

bool MBTL_FrameDisplay::init(const char *filename) {
	if (m_initialized) {
		return 1;
	}

	if (!m_pack.open_pack(filename)) {
		return 0;
	}

	// finish up
	if (!FrameDisplay::init()) {
		free();

		return 0;
	}

	// set defaults
	m_character = -1;
	set_active_character(0);

	return 1;
}

bool MBTL_FrameDisplay::init() {
	if (m_initialized) {
		return 1;
	}

	if (!init("data0008.bin")) {
		return 0;
	}

	return 1;
}

void MBTL_FrameDisplay::free() {
	m_pack.close_pack();

	m_character_data.free();

	m_character = -1;

	m_subframe = 0;
	m_subframe_base = 0;
	m_subframe_next = 0;

	FrameDisplay::free();
}

void MBTL_FrameDisplay::render(const RenderProperties *properties) {
	if (!m_initialized) {
		return;
	}

	m_character_data.render(properties, m_sequence, m_frame);
}

Clone *MBTL_FrameDisplay::make_clone() {
	if (!m_initialized) {
		return 0;
	}

	return m_character_data.make_clone(m_sequence, m_frame);
}

void MBTL_FrameDisplay::flush_texture() {
	if (!m_initialized) {
		return;
	}

	m_character_data.flush_texture();
}

void MBTL_FrameDisplay::render_frame_properties(bool detailed, int scr_width, int scr_height) {
	if (!m_initialized) {
		return;
	}

	m_character_data.render_frame_properties(detailed, scr_width, scr_height, m_sequence, m_frame);
}

void MBTL_FrameDisplay::command(FrameDisplayCommand command, void *param) {
	if (!m_initialized) {
		return;
	}

	switch(command) {
	case COMMAND_CHARACTER_NEXT:
		if (m_character == -1) {
			set_active_character(0);
		} else {
			set_active_character((m_character+1)%(mbtl_ncharacter_info));
		}
		break;
	case COMMAND_CHARACTER_PREV:
		if (m_character == -1) {
			set_active_character(0);
		} else {
			set_active_character((m_character+(mbtl_ncharacter_info)-1)%mbtl_ncharacter_info);
		}
		break;
	case COMMAND_CHARACTER_SET:
		if (!param) {
			break;
		}
		set_active_character((int)(*(unsigned int *)param)%(mbtl_ncharacter_info));
		break;
	case COMMAND_PALETTE_NEXT:
		set_palette((m_palette+1)%36);
		break;
	case COMMAND_PALETTE_PREV:
		set_palette((m_palette+35)%36);
		break;
	case COMMAND_PALETTE_SET:
		if (!param) {
			break;
		}
		set_palette((int)(*(int *)param)%36);
		break;
	case COMMAND_SEQUENCE_NEXT:
		set_sequence(m_sequence+1);
		break;
	case COMMAND_SEQUENCE_PREV:
		set_sequence(m_sequence-1);
		break;
	case COMMAND_SEQUENCE_SET:
		if (!param) {
			break;
		}
		set_sequence(*(int *)param);
		break;
	case COMMAND_FRAME_NEXT:
		set_frame(m_frame+1);
		break;
	case COMMAND_FRAME_PREV:
		set_frame(m_frame-1);
		break;
	case COMMAND_FRAME_SET:
		if (!param) {
			break;
		}
		set_frame(*(int *)param);
		break;
	case COMMAND_SUBFRAME_NEXT:
		m_subframe++;
		if (m_subframe >= m_subframe_next) {
			set_frame(m_frame+1);
		}
		break;
	case COMMAND_SUBFRAME_PREV:
		--m_subframe;
		if (m_subframe < 0) {
			set_frame(m_frame+1);
			m_subframe = m_subframe_next - 1;
		}
		break;
	case COMMAND_SUBFRAME_SET:
		break;
	}
}

const char *MBTL_FrameDisplay::get_current_sprite_filename() {
	if (!m_initialized) {
		return 0;
	}

	return m_character_data.get_current_sprite_filename(m_sequence, m_frame);
}

bool MBTL_FrameDisplay::save_current_sprite(const char *filename, RenderProperties* properties) {
	if (!m_initialized) {
		return 0;
	}

	return m_character_data.save_current_sprite(filename, m_sequence, m_frame);
}

int MBTL_FrameDisplay::save_all_character_sprites(const char *directory, RenderProperties* properties) {
	if (!m_initialized) {
		return 0;
	}

	return m_character_data.save_all_character_sprites(directory);
}

MBTL_FrameDisplay::MBTL_FrameDisplay() {
	m_character = -1;
	m_subframe = 0;
	m_subframe_base = 0;
	m_subframe_next = 0;
}

MBTL_FrameDisplay::~MBTL_FrameDisplay() {
	// cleanup will do the work
}
