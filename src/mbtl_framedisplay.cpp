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
  // HA6, HA6_9, PAL, CG
  int sizes[4];
  int offsets[4];
};

static const MBTL_Character_Info mbtl_character_info[] = {
  {"Arcueid", "ARC",	"chr000", {0x59ba3, 0x14267, 0xf010, 0x19dc760}, {0x19fdab7, 0x413cebf, 0x1a6bb82, 0x21357 } },
  {"Hisui",   "HIS",	"chr001", {0x5fd81, 0xdb1c, 0xf010, 0x2834d70}, {0x69f9b61, 0x9a071d7, 0x6a6e60a, 0x41c4df1 } },
  {"S.Akiha", "SAK",	"chr002", {0x459dc, 0xa82c, 0xf010, 0x1e161e8}, {0xb8b3687, 0xda6c3b7, 0xb90ce8b, 0x9a9d49f } },
  {"Shiki", 	"SIK",	"chr003", {0x4f943, 0xf5a5, 0xf010, 0x1d0a8a8}, {0xf7f5f0b, 0x11f235ee, 0xf859c36, 0xdaeb663 } },
  {"Kohaku",  "KOH",	"chr004", {0x63903, 0xd23d, 0xf010, 0x2a1dfe8}, {0x149c346d, 0x170ffa73, 0x14a3c6d8, 0x11fa5485 } },
  {"Roa",     "ROA",	"chr005", {0x4d8b3, 0x4ee9, 0xf010, 0x2098ac0}, {0x1922b763, 0x1b99188e, 0x1928cdfe, 0x17192ca3 } },
  {"Kouma",   "KIS",	"chr006", {0x4f960, 0x3ef5, 0xf010, 0x1edc550}, {0x1d8c72bd, 0x1ff8f070, 0x1d92a805, 0x1b9ead6d } },
  //{"Maids",		"HNK",	"chr007", { 
  {"Noel",    "NOE",	"chr008", {0x560ec, 0xed31, 0xf010, 0x2943580}, {0x2292673a, 0x2493f5aa, 0x22990c6e, 0x1ffe31ba } },
  {"VLOV",		"VLO",	"chr009", {0x5a829, 0xcfda, 0xf010, 0x2f1bd20}, {0x278e6b6e, 0x29f874e3, 0x279551ff, 0x249cae4e } },
  {"Warcueid","BOA",	"chr010", {0x4e500, 0xdb24, 0xf010, 0x235f570}, {0x2c3717a8, 0x2fbe3086, 0x2c3d3ff0, 0x2a012238 } },
  {"Ciel",		"CIE",	"chr011",	{0x608fc, 0x41ff, 0xf010, 0x28814c0}, {0x324e54cb, 0x350e5d03, 0x3255a90f, 0x2fc6400b } },
  {"Saber",		"ALT",	"chr012", {0x6caee, 0x4ac2, 0xf010, 0x3cab138}, {0x38e05b93, 0x3b9021ab, 0x38e8bc89, 0x3515aa5b } },
  {"Miyako",  "MIY",	"chr013", {0x4ca5d, 0x3499, 0xf010, 0x19278b8}, {0x3d28dc8e, 0x3fae42b7, 0x3d2eeed3, 0x3b9663d6 } },
  {"DA Noel",  "DNO",	"chr014", {0x5d8b4, 0x33d6, 0xf010, 0x2dff550}, {0x42935c7b, 0x44c619a5, 0x429a7a97, 0x3fb3672b } },
  {"Aoko",  "AOK",	"chr015", {0x649ff, 0x493f, 0xf010, 0x2b735c0}, {0x47820fca, 0x4a420785, 0x4789af11, 0x44cada0a } },
  {"P.Ciel",  "PCI",	"chr016", {0x6bf04, 0x3425, 0xf010, 0x3847bf8}, {0x4dcb6826, 0x4fa56bb3, 0x4dd38932, 0x4a46ec2e } },
  {"Mario",  "MAR",	"chr017", {0x68379, 0x4513, 0xf010, 0x32eb5e0}, {0x52d98867, 0x54bf80e4, 0x52e184e8, 0x4faad287 } },
  {"Nun",  "NUN",	"chr018", {0x26381, 0x135, 0xf010, 0xe97de8}, {0x55afbd7f, 0x55f36839, 0x55b33548, 0x54c63f97 } },
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

	return m_character_data.save_current_sprite(filename, m_sequence, m_frame, properties);
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
