#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "sprites.h"

void sprite_init(void)
{
	//al_set_new_bitmap_flags(ALLEGRO_NO_PREMULTIPLIED_ALPHA);
	//al_set_blender(ALLEGRO_ADD, ALLEGRO_INVERSE_CONST_COLOR, ALLEGRO_CONST_COLOR);
	//al_set_blender(ALLEGRO_ADD, ALLEGRO_INVERSE_ALPHA, ALLEGRO_ALPHA);
	//al_set_blend_color(al_map_rgba(0x00, 0x00, 0x00, 0x00));
}

SPRITE *sprite_load(SPRITE *s, char *file)
{
	ALLEGRO_BITMAP *b;
	float w, h;

	/* load bitmap */
	b = al_load_bitmap(file);
	w = al_get_bitmap_width(b);
	h = al_get_bitmap_height(b);

	/* check dimensions match with the rest */
	if(!s->w) s->w = w;
	if(!s->h) s->h = h;
	assert(s->w == w && s->h == h);

	/* push sprite in bitmaps array */
	assert(NULL != (s->bitmaps = realloc(
				s->bitmaps,
				sizeof(ALLEGRO_BITMAP *) * (s->bitmaps_n + 1))));
	s->bitmaps[s->bitmaps_n] = b;
	s->bitmaps_n++;

	printf("sprite_load(%s), size %.2fx%.2f\n", file, s->w, s->h);

	return s;
}

SPRITE *sprite_new(void)
{
	SPRITE *s;

	assert((s = malloc(sizeof(SPRITE))) != NULL);

	s->bitmaps = NULL;
	s->angle = 0.0;
	s->alpha = 1.0;
	s->hidden = false;
	s->bitmap   = s->bitmaps_n = 0;
	s->x        = s->y         = 0.0;
	s->center_x = s->center_y  = 0.0;
	s->w        = s->h         = 0.0;
	s->zoom_x   = s->zoom_y    = 1.0;
	s->flip_x   = s->flip_y    = false;

	return s;
}

SPRITE *sprite_new_from_file(char *file)
{
	return sprite_load(sprite_new(), file);
}

SPRITE *sprite_set_angle(SPRITE *s, float angle)
{
	s->angle = angle;

	return s;
}

SPRITE *sprite_set_alpha(SPRITE *s, float alpha)
{
	assert(0.0 <= alpha && alpha <= 1.0);
	s->alpha = alpha;

	return s;
}

SPRITE *sprite_set_align(SPRITE *s, int align)
{
	assert((align & (SPRITE_ALIGN_LEFT | SPRITE_ALIGN_RIGHT))
	        	!= (SPRITE_ALIGN_LEFT | SPRITE_ALIGN_RIGHT));
	assert((align & (SPRITE_ALIGN_BOTTOM | SPRITE_ALIGN_TOP))
			!= (SPRITE_ALIGN_BOTTOM | SPRITE_ALIGN_TOP));
	if(align & SPRITE_ALIGN_LEFT)   s->center_x =  s->w / 2.0;
	if(align & SPRITE_ALIGN_RIGHT)  s->center_x = -s->w / 2.0;
	if(align & SPRITE_ALIGN_TOP)    s->center_y =  s->h / 2.0;
	if(align & SPRITE_ALIGN_BOTTOM) s->center_y = -s->h / 2.0;

	return s;
}

SPRITE *sprite_set_bitmap(SPRITE *s, int bitmap)
{
	assert(bitmap >= 0);
	assert(bitmap <  s->bitmaps_n);
	s->bitmap = bitmap;

	return s;
}

SPRITE *sprite_set_flip_x(SPRITE *s, bool flip_x)
{
	s->flip_x = flip_x;

	return s;
}

SPRITE *sprite_set_hidden(SPRITE *s, bool hidden)
{
	s->hidden = hidden;

	return s;
}

void sprite_draw(SPRITE *s)
{
	int flags =
		(s->flip_x ? ALLEGRO_FLIP_HORIZONTAL : 0) |
		(s->flip_y ? ALLEGRO_FLIP_VERTICAL   : 0) ;
	//int op, src, dst;

	if(s->hidden)
		return;

	if(s->alpha != 1.0) {
		//printf("set alpha %.2f\n", s->alpha);
		//float a = 0.9;

		//al_get_blender(&op, &src, &dst);
		//al_set_blender(
		//	ALLEGRO_ADD,
		//	ALLEGRO_ALPHA,
		//	ALLEGRO_INVERSE_ALPHA);
		//al_set_blender(ALLEGRO_ADD, ALLEGRO_ONE, ALLEGRO_INVERSE_ALPHA);
		//al_set_blender(ALLEGRO_ADD, ALLEGRO_ALPHA, ALLEGRO_ONE);
		//al_set_blend_color(al_map_rgb_f(a, a, a 1));
		//al_set_blend_color(al_map_rgb_f(a, a, a));
		//al_set_blend_color(al_map_rgba(0xff, 0xff, 0xff, 0xff));
		//al_set_blender(ALLEGRO_ADD, ALLEGRO_CONST_COLOR, ALLEGRO_ONE);
		//al_set_blender(ALLEGRO_ADD, ALLEGRO_ONE, ALLEGRO_ONE);
		//al_set_blender(ALLEGRO_ADD, ALLEGRO_CONST_COLOR, ALLEGRO_ONE);
		//al_set_blend_color(al_map_rgb(0, 0, 128)); /* nice Chrysler blue */
	}

	al_draw_scaled_rotated_bitmap(
		s->bitmaps[s->bitmap],
		s->w / 2.0, s->h / 2.0,
		s->x + (s->zoom_x * s->center_x),
		s->y + (s->zoom_y * s->center_y),
		s->zoom_x, s->zoom_y,
		s->angle,
		flags);

	if(s->alpha != 1.0) {
		//printf("unset alpha\n");

		//al_set_blend_color(al_map_rgba(0xff, 0xff, 0xff, 0xff));
		//al_set_blender(op, src, dst);
	}
}

SPRITE *sprite_add_xy(SPRITE *s, float dx, float dy)
{
	s->x += dx;
	s->y += dy;

	return s;
}

SPRITE *sprite_set_xy(SPRITE *s, float x, float y)
{
	s->x = x;
	s->y = y;

	return s;
}

SPRITE *sprite_set_x(SPRITE *s, float x)
{
	s->x = x;

	return s;
}

SPRITE *sprite_set_y(SPRITE *s, float y)
{
	s->y = y;

	return s;
}

SPRITE *sprite_set_zoom(SPRITE *s, float zoom)
{
	s->zoom_x = s->zoom_y = zoom;

	return s;
}

SPRITE *sprite_set_zoom_x(SPRITE *s, float zoom_x)
{
	s->zoom_x = zoom_x;

	return s;
}

SPRITE *sprite_set_zoom_xy(SPRITE *s, float zoom_x, float zoom_y)
{
	s->zoom_x = zoom_x;
	s->zoom_y = zoom_y;

	return s;
}

SPRITE *sprite_set_zoom_y(SPRITE *s, float zoom_y)
{
	s->zoom_y = zoom_y;

	return s;
}

float sprite_get_width(SPRITE *s)
{
	return s->w;
}

float sprite_get_height(SPRITE *s)
{
	return s->h;
}

float sprite_get_max_length(SPRITE *s)
{
	return fmax(s->w, s->h);
}

float sprite_get_max_real_length(SPRITE *s)
{
	return fmax(s->w * s->zoom_x, s->h * s->zoom_y);
}

float sprite_get_real_width(SPRITE *s)
{
	return fmax(
		fabs(s->w * s->zoom_x * cos(s->angle)),
		fabs(s->h * s->zoom_y * sin(s->angle)));
}

float sprite_get_real_height(SPRITE *s)
{
	return fmax(
		fabs(s->w * s->zoom_x * sin(s->angle)),
		fabs(s->h * s->zoom_y * cos(s->angle)));
}

float sprite_get_x(SPRITE *s)
{
	return s->x;
}

float sprite_get_y(SPRITE *s)
{
	return s->y;
}

float sprite_get_angle(SPRITE *s)
{
	return s->angle;
}

int sprite_get_bitmap(SPRITE *s)
{
	return s->bitmap;
}

int sprite_get_bitmap_n(SPRITE *s)
{
	return s->bitmaps_n;
}

bool sprite_collide_basic(SPRITE *a, SPRITE *b)
{
	float aw = sprite_get_real_width(a);
	float bw = sprite_get_real_width(b);
	float ah = sprite_get_real_height(a);
	float bh = sprite_get_real_height(b);
	float al = a->x + (a->center_x * a->zoom_x - aw / 2.0); float ar = al + aw;
	float bl = b->x + (b->center_x * b->zoom_x - bw / 2.0); float br = bl + bw;
	float ab = a->y + (a->center_y * a->zoom_y - ah / 2.0); float at = ab + ah;
	float bb = b->y + (b->center_y * b->zoom_y - bh / 2.0); float bt = bb + bh;

//	al_draw_filled_rectangle(al, at, ar, ab, al_map_rgba_f(1.0, 0, 0, 1.0));
//	al_draw_filled_rectangle(bl, bt, br, bb, al_map_rgba_f(0, 0, 1.0, 1.0));

	if(a->hidden || b->hidden)
		return false;

	if(ar < bl || al > br || at < bb || ab > bt)
		return false;

	if((((bb <= ab && ab <= bt) || (bb <= at && at <= bt))
		&& ((bl <= al && al <= br) || (bl <= ar && ar <= br)))
	|| (((ab <= bb && bb <= at) || (ab <= bt && bt <= at))
		&& ((al <= bl && bl <= ar) || (al <= br && br <= ar)))) {
		return true;
	}

	return false;
}

