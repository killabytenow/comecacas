#ifndef __SPRITES_H__
#define __SPRITES_H__

#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>

#define SPRITE_ALIGN_LEFT	1
#define SPRITE_ALIGN_RIGHT	2
#define SPRITE_ALIGN_BOTTOM	4
#define SPRITE_ALIGN_TOP	8
#define SPRITE_ALIGN_CENTER	0
#define SPRITE_ALIGN_MIDDLE	0

typedef struct {
	ALLEGRO_BITMAP	**bitmaps;

	int		bitmaps_n;
	int		bitmap;

	float		x;
	float		y;
	float		w;
	float		h;
	float		center_x;
	float		center_y;
	float		zoom_x;
	float		zoom_y;
	float		alpha;
	float		angle;
	bool		hidden;
	bool		flip_x;
	bool		flip_y;
} SPRITE;

void sprite_init(void);

SPRITE *sprite_load(SPRITE *sprite, char *file);
SPRITE *sprite_new(void);
SPRITE *sprite_new_from_file(char *file);
SPRITE *sprite_set_alpha(SPRITE *s, float alpha);
SPRITE *sprite_set_angle(SPRITE *s, float angle);
SPRITE *sprite_set_bitmap(SPRITE *s, int bitmap);
SPRITE *sprite_set_flip_x(SPRITE *s, bool flip_x);
SPRITE *sprite_set_hidden(SPRITE *s, bool hidden);
SPRITE *sprite_set_align(SPRITE *s, int align);
SPRITE *sprite_set_center_x(SPRITE *s, float x);
SPRITE *sprite_set_center_y(SPRITE *s, float y);
SPRITE *sprite_set_center_xy(SPRITE *s, float x, float y);
SPRITE *sprite_set_x(SPRITE *s, float x);
SPRITE *sprite_set_xy(SPRITE *s, float x, float y);
SPRITE *sprite_set_y(SPRITE *s, float y);
SPRITE *sprite_set_zoom(SPRITE *s, float zoom);
SPRITE *sprite_set_zoom_x(SPRITE *s, float zoom);
SPRITE *sprite_set_zoom_xy(SPRITE *s, float zoom_x, float zoom_y);
SPRITE *sprite_set_zoom_y(SPRITE *s, float zoom);
SPRITE *sprite_add_xy(SPRITE *s, float dx, float dy);
float sprite_get_x(SPRITE *s);
float sprite_get_y(SPRITE *s);
float sprite_get_center_x(SPRITE *s);
float sprite_get_center_y(SPRITE *s);
float sprite_get_angle(SPRITE *s);
float sprite_get_max_length(SPRITE *s);
float sprite_get_max_real_length(SPRITE *s);
float sprite_get_width(SPRITE *s);
float sprite_get_height(SPRITE *s);
float sprite_get_real_width(SPRITE *s);
float sprite_get_real_height(SPRITE *s);
void sprite_hide(SPRITE *s);
void sprite_show(SPRITE *s);

bool sprite_collide_basic(SPRITE *a, SPRITE *b);

void sprite_draw(SPRITE *s);

#endif
