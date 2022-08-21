// .CG loader
//
// .CG contains information about sprite mappings from the ENC and PVR tiles.

#include "mbtl_framedisplay.h"
#include "render.h"
#include "misc.h"

#include <cstdlib>
#include <cstring>

static void copy_hitbox_to_rect(rect_t *rect, MBTL_Hitbox *hitbox) {
    rect->x1 = (hitbox->x1);
    rect->y1 = (hitbox->y1);
    rect->x2 = (hitbox->x2);
    rect->y2 = (hitbox->y2);
}

static unsigned char* get_pixel(int x, int y, int width, int height, unsigned char* grid) {
    return &grid[(x+y*width)*4];
}

static void set_pixel(int x, int y, int width, int height, unsigned char* grid, unsigned char* rgba) {
    grid[(x+y*width)*4+0] = rgba[0];
    grid[(x+y*width)*4+1] = rgba[1];
    grid[(x+y*width)*4+2] = rgba[2];
    grid[(x+y*width)*4+3] = rgba[3];
}

static void add_pixel(int x, int y, int width, int height, unsigned char* grid, unsigned char* rgba) {
    double oldR = grid[(x+y*width)*4+0]/255.0;
    double oldG = grid[(x+y*width)*4+1]/255.0;
    double oldB = grid[(x+y*width)*4+2]/255.0;
    double oldA = grid[(x+y*width)*4+3]/255.0;
    double oldA2 = rgba[3]/255.0;
    double newA = oldA2 + oldA - oldA*oldA2;
    grid[(x+y*width)*4+3] = newA * 255;
    grid[(x+y*width)*4+0] = 255*(oldA2 * rgba[0]/255 + oldA*oldR*(1-oldA2))/newA;
    grid[(x+y*width)*4+1] = 255*(oldA2 * rgba[1]/255 + oldA*oldG*(1-oldA2))/newA;
    grid[(x+y*width)*4+2] = 255*(oldA2 * rgba[2]/255 + oldA*oldB*(1-oldA2))/newA;
}

struct MBTL_CG_Alignment {
	int	x;
	int	y;
	int	width;
	int	height;
	short	source_x;
	short	source_y;
	short	source_image;
	short	copy_flag;
};

struct MBTL_CG_Image {
	char		filename[32];
	int		type_id;	// i think this is render mode
	unsigned int	width;
	unsigned int	height;
	unsigned int	bpp;
	int		bounds_x1;
	int		bounds_y1;
	int		bounds_x2;
	int		bounds_y2;
	unsigned int	align_start;
	unsigned int	align_len;
	unsigned char	data[1]; 	// for indexing.
};

const MBTL_CG_Image *MBTL_CG::get_image(unsigned int n) {
	if (n >= m_nimages) {
		return 0;
	}

	unsigned int index = m_indices[n];
	if (index < 0) {
		return 0;
	}

	if (index + sizeof(MBTL_CG_Image) > m_data_size) {
		return 0;
	}

	return (const MBTL_CG_Image *)(m_data + index);
}

const char *MBTL_CG::get_filename(unsigned int n) {
	if (!m_loaded) {
		return 0;
	}

	const MBTL_CG_Image *image = get_image(n);
	if (!image) {
		return 0;
	}

	return image->filename;
}

int MBTL_CG::get_image_count() {
	return m_nimages;
}

void MBTL_CG::copy_cells(const MBTL_CG_Image *image,
			const MBTL_CG_Alignment *align,
			unsigned char *pixels,
			unsigned int x1,
			unsigned int y1,
			unsigned int width,
			unsigned int height,
			unsigned int *palette,
			bool is_8bpp) {
	int w = align->width / 0x10;
	int h = align->height / 0x10;
	int x = align->source_x / 0x10;
	int y = align->source_y / 0x10;
	int cell_n = (y * 0x10) + x;
	Image *im = &m_image_table[align->source_image];

	for (int a = 0; a < h; ++a) {
		for (int b = 0 ; b < w; ++b) {
			ImageCell *cell = &im->cell[cell_n + b];

			if (cell->start == 0) {
				continue;
			}

			unsigned char *dest = pixels;
			unsigned int offset;

			offset = (align->y + (a * 0x10) - y1) * width;
			offset += align->x + (b * 0x10) - x1;

			if (is_8bpp) {
				// 8bpp -> 8bpp
				unsigned char *src = ((unsigned char *)m_data) + cell->start + cell->offset;
				int cellw = cell->width;

				dest += offset;

				for (int c = 0; c < 0x10; ++c) {
					for (int d = 0; d < 0x10; ++d) {
						dest[d] = src[d];
					}

					src += cellw;
					dest += width;
				}
			} else if (image->type_id == 4) {
				// two pass: first 8bit palettized, second 8bit alpha
				unsigned int *ldest = (unsigned int *)dest;
				unsigned char *src = ((unsigned char *)m_data) + cell->start + cell->offset;
				int cellw = cell->width;

				ldest += offset;

				for (int c = 0; c < 0x10; ++c) {
					for (int d = 0; d < 0x10; ++d) {
						ldest[d] = palette[src[d]] & 0xffffff;
					}

					src += cellw;
					ldest += width;
				}

				ldest = (unsigned int *)dest;
				ldest += offset;

				src = ((unsigned char *)m_data) + cell->start + cell->offset;
				src += align->width * align->height;

				for (int c = 0; c < 0x10; ++c) {
					for (int d = 0; d < 0x10; ++d) {
						ldest[d] |= src[d] << 24;
					}

					src += cellw;
					ldest += width;
				}
			} else if (image->type_id == 1) {
				// 32bpp bgr -> rgb
				unsigned int *ldest = (unsigned int *)dest;
				unsigned int *src = (unsigned int *)(m_data + cell->start + cell->offset);
				int cellw = cell->width;

				ldest += offset;

				for (int c = 0; c < 0x10; ++c) {
					for (int d = 0; d < 0x10; ++d) {
						unsigned int v = src[d];
						v = (v & 0xff00ff00) | ((v&0xff) << 16) | ((v&0xff0000) >> 16);
						ldest[d] = v;
					}

					src += cellw;
					ldest += width;
				}
			} else {
				// palettized 8bpp -> 32bpp
				unsigned int *ldest = (unsigned int *)dest;
				unsigned char *src = ((unsigned char *)m_data) + cell->start + cell->offset;
				int cellw = cell->width;

				ldest += offset;

				for (int c = 0; c < 0x10; ++c) {
					for (int d = 0; d < 0x10; ++d) {
						ldest[d] = palette[src[d]];
					}

					src += cellw;
					ldest += width;
				}
			}
		}

		cell_n += 0x10;
	}
}


Texture *MBTL_CG::draw_texture(unsigned int n, unsigned int *palette, bool to_pow2_flg, bool draw_8bpp) {
	const MBTL_CG_Image *image = get_image(n);
	if (!image) {
		return 0;
	}

	if ((image->align_start + image->align_len) > m_nalign) {
		return 0;
	}

	// initialize texture and boundaries
	int x1 = 0;
	int y1 = 0;

	if (!draw_8bpp) {
		x1 = image->bounds_x1;
		y1 = image->bounds_y1;
	}

	int width = image->bounds_x2 - x1;
	int height = image->bounds_y2 - y1;

	if (width == 0 || height == 0) {
		return 0;
	}

	if (to_pow2_flg) {
		width = to_pow2(width);
		height = to_pow2(height);
	}

	// check to see if we need a custom palette
	unsigned int custom_palette[256];
	if (image->bpp == 32) {
		if (image->type_id == 3) {
			unsigned int color = *(unsigned int *)image->data;

			color &= 0xffffff;

			custom_palette[0] = 0;
			for (int i = 1; i < 256; ++i) {
				custom_palette[i] = (i << 24) | color;
			}

			palette = custom_palette;
		} else if (image->type_id == 2 || image->type_id == 4) {
			memcpy(custom_palette, image->data, 1024);

			for (int i = 0; i < 256; ++i) {
				custom_palette[i] = (0xff << 24) | custom_palette[i];
			}
			palette = custom_palette;
		}
	}

	unsigned char *pixels = new unsigned char[width*height*4];
	memset(pixels, 0, width*height*4);

	// run through all tile region data
	const MBTL_CG_Alignment *align;
	int last_image = -32769;

	bool is_8bpp;

	if (draw_8bpp) {
		is_8bpp = 1;

		if (image->bpp > 8) {
			is_8bpp = 0;
		}
	} else {
		is_8bpp = 0;
	}

	align = &m_align[image->align_start];
	last_image = -32769;
	for (unsigned int i = 0; i < image->align_len; ++i, ++align) {
		copy_cells(image, align, pixels, x1, y1, width, height, palette, is_8bpp);
	}

	// finalize in texture
	Texture *texture = new Texture();

	if (!texture->init(pixels, width, height, is_8bpp)) {
		delete texture;
		delete[] pixels;
		texture = 0;
	} else {
		texture->offset(image->bounds_x1*2, image->bounds_y1*2);
	}

	return texture;
}

Texture *MBTL_CG::draw_texture_with_boxes(unsigned int n,
                                          unsigned int *palette, bool to_pow2_flg,
                                          RenderProperties* properties,
                                          MBTL_Frame* frame){
	const MBTL_CG_Image *image = get_image(n);
	if (!image) {
		return 0;
	}

	if ((image->align_start + image->align_len) > m_nalign) {
		return 0;
	}

	// initialize texture and boundaries
	int x1 = 0;
	int y1 = 0;

  bool draw_8bpp = false;
  /*
	if (!draw_8bpp) {
		x1 = image->bounds_x1;
		y1 = image->bounds_y1;
	}
  */
	int width = image->bounds_x2 - x1;
	int height = image->bounds_y2 - y1;

	if (width == 0 || height == 0) {
		return 0;
	}

	if (to_pow2_flg) {
		width = to_pow2(width);
		height = to_pow2(height);
	}

  printf("%d, %d\n", width, height);
	// check to see if we need a custom palette
	unsigned int custom_palette[256];
	if (image->bpp == 32) {
		if (image->type_id == 3) {
			unsigned int color = *(unsigned int *)image->data;

			color &= 0xffffff;

			custom_palette[0] = 0;
			for (int i = 1; i < 256; ++i) {
				custom_palette[i] = (i << 24) | color;
			}

			palette = custom_palette;
		} else if (image->type_id == 2 || image->type_id == 4) {
			memcpy(custom_palette, image->data, 1024);

			for (int i = 0; i < 256; ++i) {
				custom_palette[i] = (0xff << 24) | custom_palette[i];
			}
			palette = custom_palette;
		}
	}

	unsigned char *pixels = new unsigned char[width*height*4];
	memset(pixels, 0, width*height*4);
  if (properties->display_bg) {
      for (int pix=0; pix < width*height; ++pix) {
          pixels[pix*4] = 0;
          pixels[pix*4+1] = 19;
          pixels[pix*4+2] = 19;
          pixels[pix*4+3] = 255;
      }
  }

	// run through all tile region data
	const MBTL_CG_Alignment *align;
	int last_image = -32769;

	bool is_8bpp;

	if (draw_8bpp) {
		is_8bpp = 1;

		if (image->bpp > 8) {
			is_8bpp = 0;
		}
	} else {
		is_8bpp = 0;
	}

	align = &m_align[image->align_start];
	last_image = -32769;
	for (unsigned int i = 0; i < image->align_len; ++i, ++align) {
		copy_cells(image, align, pixels, x1, y1, width, height, palette, is_8bpp);
	}

  // Draw Boxes

	// render collision box
	if (properties->display_collision_box) {
		if (frame->hitboxes[0]) {
			rect_t rect;

			copy_hitbox_to_rect(&rect, frame->hitboxes[0]);

			draw_boxes(BOX_COLLISION, &rect, 1, properties->display_solid_boxes, pixels, width, height, frame);
		}
	}

	// render hitboxes
	rect_t rects[32];
	int nrects;

	if (properties->display_hit_box) {
		nrects = 0;
		for (int i = 1; i < 11; ++i) {
			if (frame->hitboxes[i]) {
				copy_hitbox_to_rect(&rects[nrects], frame->hitboxes[i]);

				++nrects;
			}
		}

		if (nrects > 0) {
        //draw_boxes(BOX_HIT, rects, nrects, properties->display_solid_boxes, pixels, width, height, frame);
		}
	}

	// render damage boxes
	if (properties->display_attack_box) {
		nrects = 0;
		for (int i = 25; i < 33; ++i) {
			if (frame->hitboxes[i]) {
				copy_hitbox_to_rect(&rects[nrects], frame->hitboxes[i]);

				++nrects;
			}
		}

		if (nrects > 0) {
        //draw_boxes(BOX_ATTACK, rects, nrects, properties->display_solid_boxes, pixels, width, height, frame);
		}
	}

	// render clash boxes
	if (properties->display_clash_box) {
		nrects = 0;
		for (int i = 11; i < 25; ++i) {
			if (frame->hitboxes[i]) {
				copy_hitbox_to_rect(&rects[nrects], frame->hitboxes[i]);

				++nrects;
			}
		}

		if (nrects > 0) {
        //draw_boxes(BOX_CLASH, rects, nrects, properties->display_solid_boxes, pixels, width, height, frame);
		}
	}

	// finalize in texture
	Texture *texture = new Texture();

	if (!texture->init(pixels, width, height, is_8bpp)) {
		delete texture;
		delete[] pixels;
		texture = 0;
	} else {
		texture->offset(image->bounds_x1*2, image->bounds_y1*2);
	}

	return texture;

}

void MBTL_CG::draw_boxes(BoxType box_type, rect_t *rects, int nrects, bool solid, unsigned char* pixels, int width, int height, MBTL_Frame* frame) {
  unsigned char R;
  unsigned char G;
  unsigned char B;
  unsigned char A;
  return;
  for (int i = 0; i < width; ++i) {
      pixels[(i+(height-frame->AF.layers[0].offset_y-17)*width)*4] = 0xFF;
      pixels[(i+(height-frame->AF.layers[0].offset_y-17)*width)*4+3] = 0xFF;
      pixels[(width/2 - 1+(i)*width)*4] = 0xFF;
      pixels[(width/2 - 1+(i)*width)*4+3] = 0xFF;
  }
  return;
	if (solid) {
		switch (box_type) {
		case BOX_DEFAULT:
        R = 255;
        G = 76;
        B = 255;
        A = 76;
			break;
		case BOX_COLLISION:
        R = 204;
        G = 204;
        B = 204;
        A = 63;
			break;
		case BOX_HIT:
        R = 76;
        G = 255;
        B = 76;
        A = 76;
			break;
		case BOX_ATTACK:
        R = 255;
        G = 76;
        B = 76;
        A = 76;
			break;
		case BOX_CLASH:
        R = 255;
        G = 255;
        B = 76;
        A = 76;
			break;
		case BOX_SPECIAL:
        R = 51;
        G = 51;
        B = 255;
        A = 76;
        break;
		default:
			return;
		}

    bool marked[width*height];
    for( int i=0; i < width*height; ++i) {
        marked[i] = false;
    }

    printf("%d, %d\n", frame->AF.layers[0].offset_x, frame->AF.layers[0].offset_y);
    unsigned char colors[4] = {R,G,B,A};
    for (int i = 0; i < nrects; ++i) {
        rect_t rect = rects[i];
        printf("%d %d %d %d\n", rect.x1, rect.x2, rect.y1, rect.y2);
        for (int x=rect.x1; x<rect.x2; ++x){
            for (int y = rect.y1; y<rect.y2; ++y) {
                int new_x = x + 256-frame->AF.layers[0].offset_x*2;
                int new_y = y + 512-64-frame->AF.layers[0].offset_y*2;
                if (!marked[new_x+new_y*width]) {
                    add_pixel(new_x, new_y, width, height, pixels, colors);
                    marked[new_x+new_y*width] = true;
                }
            }
        }
    }
	}
  switch (box_type) {
  case BOX_DEFAULT:
      R = 255;
      G = 76;
      B = 255;
      A = 255;
			break;
  case BOX_COLLISION:
      R = 255*0.9;
      G = 255*0.9;
      B = 255*0.9;
      A = 255;
			break;
  case BOX_HIT:
      R = 76;
      G = 255;
      B = 76;
      A = 255;
			break;
  case BOX_ATTACK:
      R = 255;
      G = 76;
      B = 76;
      A = 255;
			break;
  case BOX_CLASH:
      R = 255;
      G = 255;
      B = 76;
      A = 255;
			break;
  case BOX_SPECIAL:
      R = 0;
      G = 0;
      B = 255*0.1;
      A = 76;
      break;
  default:
			return;
	}
  for (int i = 0; i < nrects; ++i) {
      rect_t rect = rects[i];
      for (int x=rect.x1; x<rect.x2; ++x) {
          int new_x = x + 256-frame->AF.layers[0].offset_x*2;
          int new_y1 = rect.y1 + 512-64-frame->AF.layers[0].offset_y*2;
          int new_y2 = rect.y2 + 512-64-frame->AF.layers[0].offset_y*2;
          int base = (new_x+new_y1*width)*4;
          int base2 = (new_x+new_y2*width)*4;
          pixels[base] = R;
          pixels[base+1] = G;
          pixels[base+2] = B;
          pixels[base+3] = A;
          pixels[base2] = R;
          pixels[base2+1] = G;
          pixels[base2+2] = B;
          pixels[base2+3] = A;
          //marked[x+y*width] = true;
      }
      for (int y = rect.y1; y<rect.y2; ++y) {
          int new_x1 = rect.x1 + 256-frame->AF.layers[0].offset_x*2-1;
          int new_x2 = rect.x2 + 256-frame->AF.layers[0].offset_x*2-1;
          int new_y = y + 512-64-frame->AF.layers[0].offset_y*2;
          int base = (new_x1+new_y*width)*4;
          int base2 = (new_x2+new_y*width)*4;
          pixels[base] = R;
          pixels[base+1] = G;
          pixels[base+2] = B;
          pixels[base+3] = A;
          pixels[base2] = R;
          pixels[base2+1] = G;
          pixels[base2+2] = B;
          pixels[base2+3] = A;
      }
      int corner_x = rect.x1 + 256-frame->AF.layers[0].offset_x*2-1;
      int corner_y = rect.y2 + 512-64-frame->AF.layers[0].offset_y*2;
      int base = (corner_x+corner_y*width)*4;
      pixels[base] = R;
      pixels[base+1] = G;
      pixels[base+2] = B;
      pixels[base+3] = A;
  }
}

void MBTL_CG::build_image_table() {
	// Go through the entire align table and figure out
	// how many images there are supposed to be.
	int max_image = 0;

	for (unsigned int i = 0; i < m_nalign; ++i) {
		if (m_align[i].source_image > max_image) {
			max_image = m_align[i].source_image;
		}
	}
	max_image += 1;

	// Create new image table and initialize it.
	m_image_table = new Image[max_image];
	m_image_count = max_image;

	memset(m_image_table, 0, sizeof(Image) * m_image_count);

	// Go through and initialize all the cells.

	for (unsigned int i = 0; i < 0x3000; ++i) {
		const MBTL_CG_Image *image = get_image(i);

		if (!image) {
			continue;
		}

		if ((image->align_start + image->align_len) > m_nalign) {
			continue;
		}

		const MBTL_CG_Alignment *align = &m_align[image->align_start];
		unsigned int address = ((const char *)image->data) - m_data;

		if (image->bpp == 32) {
			if (image->type_id == 3) {
				address += 4;
			} else if (image->type_id == 2 || image->type_id == 4) {
				address += 1024;
			}
		}

		for (unsigned int j = 0; j < image->align_len; ++j, ++align) {
			if (align->copy_flag != 0) {
				continue;
			}

			int w = align->width / 0x10;
			int h = align->height / 0x10;
			int x = align->source_x / 0x10;
			int y = align->source_y / 0x10;
			int cell_n = (y * 0x10) + x;
			Image *im = &m_image_table[align->source_image];

			if (x + w >= 0x10) {
				w = 0x10 - x;
			}
			if (y + h >= 0x10) {
				h = 0x10 - y;
			}

			int mult = 1;
			if (image->type_id == 1) {
				mult = 4;
			}

			for (int a = 0; a < h; ++a) {
				ImageCell *cell = &im->cell[cell_n];
				for (int b = 0; b < w; ++b, ++cell) {
					cell->start = address;
					cell->width = align->width;
					cell->height = align->height;
					cell->offset = (b * 0x10) + (a * align->width * 0x10) * mult;
					cell->type_id = image->type_id;
					cell->bpp = image->bpp;
				}
				cell_n += 0x10;
			}

			if (image->type_id == 4) {
				mult = 2;
			}

			address += align->width * align->height * mult;
		}
	}
}

bool MBTL_CG::load(MBTL_Pack *pack, const char *name, int bsize, int offset) {
	if (m_loaded) {
		return 0;
	}

	char *data;
	unsigned int size;

	if (!pack->read_file(name, &data, &size, bsize, offset)) {
		return 0;
	}

	// verify size and header
	if (size < 0x4f30 || memcmp(data, "BMP Cutter3", 11)) {
		delete[] data;

		return 0;
	}

	// palette data.
	// we always use external palettes, so skip this.
	unsigned int *d = (unsigned int *)(data + 0x10);
	d += 1;		// has palette data?
	d += 0x800;	// palette data - always included.

	// 'parse' header
	unsigned int *indices = d + 12;
	unsigned int image_count = d[3];

	if (image_count >= 2999) {
		delete[] data;

		return 0;
	}

	// alignment data
	int align_count = (size - indices[3000]) / sizeof(MBTL_CG_Alignment);

	if (align_count <= 0) {
		delete[] data;

		return 0;
	}

	// store everything for lookup later
	m_align = (MBTL_CG_Alignment *)(data + indices[3000]);
	m_nalign = align_count;

	m_data = data;
	m_data_size = size;

	m_indices = indices;

	m_nimages = image_count;

	// but wait, there's more!
	// because of the compression added to TL, we need to go create
	// an image table for this crap.
	build_image_table();

	// we're done, so finish up

	m_loaded = 1;

	return 1;
}

void MBTL_CG::free() {
	if (m_data) {
		delete[] m_data;
	}
	m_data = 0;
	m_data_size = 0;

	if (m_image_table) {
		delete[] m_image_table;
	}
	m_image_table = 0;
	m_image_count = 0;

	m_indices = 0;

	m_nimages = 0;

	m_align = 0;
	m_nalign = 0;

	m_loaded = 0;
}

MBTL_CG::MBTL_CG() {
	m_data = 0;
	m_data_size = 0;

	m_indices = 0;

	m_nimages = 0;

	m_image_table = 0;
	m_image_count = 0;

	m_align = 0;
	m_nalign = 0;

	m_loaded = 0;
}

MBTL_CG::~MBTL_CG() {
	free();
}
