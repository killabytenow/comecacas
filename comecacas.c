#include <math.h>
#include <stdio.h>
#include <limits.h>	// PATH_MAX

#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>

#include "sprites.h"

#define WINDOW		0
#define FPS		50

#define SCREEN_LIMIT	16

#define COMECACAS_SPRITES		4
#define COMECACAS_GRAVITY		2.5
#define COMECACAS_JUMP			50.0
#define COMECACAS_ACC			2.0
#define COMECACAS_BRAKE_ACC 		4.0
#define COMECACAS_SPEED_MAX		20.0
#define COMECACAS_SWIPE_LEN		20
#define COMECACAS_SWIPE_STEP_1		(1.0 / 4.0)
#define COMECACAS_SWIPE_STEP_2		(3.0 / 4.0)
#define COMECACAS_SWIPE_MAX_ACC		5.0

#define CACAS_FREQ_MIN			20
#define CACAS_FREQ_MAX			50
#define CACAS_SPRITES			9
#define CACAS_MAX			CACAS_SPRITES * 4
#define CACAS_ZOOM_UP_MAX		0.7
#define CACAS_ZOOM_UP_MIN		0.1
#define CACAS_SPEED_UP 			6
#define CACAS_SPEED_DOWN		20
#define CACAS_FRAMES_OUT		20
#define CACAS_FRAMES_CHOF		100
#define CACA_FRAMES_NOMNOMNOM		20

#define LANZACACAS_SPRITES		4
#define LANZACACAS_FRAMES_LAUNCH	4
#define LANZACACAS_DESP_MAX		600

#define FLIES_MAX			3
#define FLIES_SPRITES			2
#define FLIES_SPEED_MAX			10.0

typedef struct {
	SPRITE		*s;

	float		floor;
	float		speed_x;
	float		speed_y;
	float		x;
	float		y;
	int		state;
#define COMECACAS_INIT		0
#define COMECACAS_TITLE_LEN	FPS * 2
#define COMECACAS_TITLE		1
#define COMECACAS_HELLO_LEN	25
#define COMECACAS_HELLO		2
#define COMECACAS_NORMAL	3
#define COMECACAS_SWIPE		4
#define COMECACAS_DIE		5
#define COMECACAS_DIE_LEN	100
#define COMECACAS_GAMEOVER	6
#define COMECACAS_GAMEOVER_LEN	FPS * 5
	int		status;
} COMECACAS;

typedef struct {
	SPRITE		*s;

	int		counter;
	int		state;
#define CACA_OFF	0
#define CACA_UP		1
#define CACA_OUT	2
#define CACA_DOWN	3
#define CACA_CHOF	4
#define CACA_NOMNOMNOM	5
} CACAS;

typedef struct {
	SPRITE		*s;
} LANZACACAS;

typedef struct _tag_FLIES {
	SPRITE		*s;

	void (*update)(struct _tag_FLIES *f);
	float		speed;
	float		angle;
	float		rotipoti;
	float		cocototo;
} FLIES;

typedef struct {
	ALLEGRO_DISPLAY *screen;
	float		screen_width;
	float		screen_height;
	float		screen_limit_left;
	float		screen_limit_right;
	float		screen_floor;
	float		screen_horizont;

	unsigned long	frame;
	unsigned long	next_caca;
	float		next_caca_x;
	unsigned long	last_caca;
	float		last_caca_x;

	int		lifes;
	int		score;

	SPRITE		*background;
	LANZACACAS	lanzacacas;
	COMECACAS	comecacas;
	CACAS		cacas[CACAS_MAX];
	FLIES		flies[FLIES_MAX];

	ALLEGRO_FONT	*font_poo;
	ALLEGRO_FONT	*font_paper;

	bool		control_up;
	bool		control_down;
	bool		control_left;
	bool		control_right;
	bool		control_fire;
} GAME;

void game_update_flies_dividi(FLIES *f);
void game_update_flies_lokipoki(FLIES *f);
void game_update_flies_middlemierdi(FLIES *f);

GAME game;

void game_init(void)
{
	char tmp[PATH_MAX];
	float max_w, max_h;
	void *ff[] = {
		game_update_flies_middlemierdi,
		game_update_flies_lokipoki,
		game_update_flies_dividi,
	};

	// max_w/h will have the biggest sprite dimensions!
	max_w = 0.0,
	max_h = 0.0;

	// configure screen
	if(WINDOW) {
		al_set_new_display_flags(ALLEGRO_WINDOWED);
		game.screen = al_create_display(800, 600);
		game.screen_width  = al_get_display_width(game.screen);
		game.screen_height = al_get_display_height(game.screen);
	} else {
		al_set_new_display_flags(ALLEGRO_FULLSCREEN_WINDOW);
		game.screen = al_create_display(1920, 1080);
		game.screen_width  = al_get_display_width(game.screen);
		game.screen_height = al_get_display_height(game.screen);
		al_resize_display(
			game.screen,
			al_get_display_width(game.screen),
			al_get_display_height(game.screen));
		al_acknowledge_resize(game.screen);
	}

	// init
	game.frame = 0;
	game.next_caca = game.last_caca = 0;
	game.next_caca_x = game.last_caca_x = game.screen_width / 2.0;

	// init sprite library
	sprite_init();

	// load background
	game.background = sprite_new_from_file("res/background.jpg");
	sprite_set_align(
		game.background,
		SPRITE_ALIGN_CENTER | SPRITE_ALIGN_MIDDLE);
	sprite_set_xy(
		game.background,
		game.screen_width / 2.0,
		game.screen_height / 2.0);
	sprite_set_zoom(
		game.background,
		game.screen_height / sprite_get_height(game.background));
	game.screen_floor    = game.screen_height * 0.9;
	game.screen_horizont = game.screen_height * 0.5;

	// load comecacas
	game.comecacas.s = sprite_new();
	for(int i = 0; i < COMECACAS_SPRITES; i++) {
		snprintf(tmp, sizeof(tmp), "res/comecacas-%02d.png", i + 1);
		sprite_load(game.comecacas.s, tmp);
	}
	for(int i = COMECACAS_SPRITES - 1; i > 0; i--) {
		snprintf(tmp, sizeof(tmp), "res/comecacas-%02d.png", i + 1);
		sprite_load(game.comecacas.s, tmp);
	}
	sprite_set_align(
		game.comecacas.s,
		SPRITE_ALIGN_CENTER | SPRITE_ALIGN_BOTTOM);

	game.comecacas.state = COMECACAS_INIT;

	max_w = fmax(max_w, sprite_get_width(game.comecacas.s));
	max_h = fmax(max_h, sprite_get_height(game.comecacas.s));

	// load cacas
	for(int i = 0; i < CACAS_MAX; i++) {
		snprintf(tmp, sizeof(tmp), "res/poo-%02d.png", (i % CACAS_SPRITES) + 1);
		game.cacas[i].s = sprite_new_from_file(tmp);
		max_w = fmax(max_w, sprite_get_width(game.cacas[i].s));
		max_h = fmax(max_h, sprite_get_height(game.cacas[i].s));
	}

	// load lanzacacas
	game.lanzacacas.s = sprite_new();
	for(int i = 0; i < LANZACACAS_SPRITES; i++) {
		snprintf(tmp, sizeof(tmp), "res/catapult-%02d.png", i + 1);
		sprite_load(game.lanzacacas.s, tmp);
		max_w = fmax(max_w, sprite_get_width(game.lanzacacas.s));
		max_h = fmax(max_h, sprite_get_height(game.lanzacacas.s));
	}
	
	// load flies
	for(int j = 0, ffi = 0; j < FLIES_MAX; j++, ffi++) {
		game.flies[j].s = sprite_new();
		game.flies[j].update   = ff[ffi % (sizeof(ff) / sizeof(void *))];
		game.flies[j].angle    = ((float) (random() & 0xff) / 255.0) * 4 * M_PI;
		game.flies[j].rotipoti = 0.0;
		game.flies[j].cocototo = 0.0;
		game.flies[j].speed    = (FLIES_SPEED_MAX - 1.0) * ((float) (random() & 0xff) / 255.0) + 1.0;
		sprite_set_xy(game.flies[j].s, game.screen_width / 2.0, 0);
		sprite_set_angle(game.flies[j].s, game.flies[j].angle);
		for(int i = 0; i < FLIES_SPRITES; i++) {
			snprintf(tmp, sizeof(tmp), "res/fly-%02d.png", i + 1);
			sprite_load(game.flies[j].s, tmp);
		}
		max_w = fmax(max_w, sprite_get_width(game.flies[j].s));
		max_h = fmax(max_h, sprite_get_height(game.flies[j].s));
	}

	// decide screen limits
	game.screen_limit_left = game.screen_width / 2.0
					- sprite_get_real_width(game.background) / 2.0
					+ max_w / 4.0;
	game.screen_limit_right = game.screen_width / 2.0
					+ sprite_get_real_width(game.background) / 2.0
					- max_w / 4.0;
	
	// load fonts
	game.font_poo   = al_load_ttf_font("res/poocorny.ttf",    150, 0);
	//game.font_poo   = al_load_ttf_font("res/poosmooth.ttf",   150, 0);
	game.font_paper = al_load_ttf_font("res/paper4urass.ttf", 100, 0);

	game.score = 0;
	game.lifes = 3;
}

int game_update_comecacas()
{
	int dir = 0;
	float acc_x = 0.0;
	float zoom_x = 1.0;
	float zoom_y = 1.0;
	float angle = -M_PI / 2.0;
	bool flip_x = false;
	bool hidden = false;

	// controls
	switch(game.comecacas.state) {
	case COMECACAS_INIT:
		game.comecacas.x = game.screen_width / 2.0;
		game.comecacas.y = game.screen_floor;
		game.comecacas.state = COMECACAS_TITLE;
		game.comecacas.status = COMECACAS_TITLE_LEN;
		hidden = true;
		break;

	case COMECACAS_TITLE:
		hidden = true;
		game.comecacas.status--;
		if(game.comecacas.status < 0) {
			game.comecacas.state = COMECACAS_HELLO;
			game.comecacas.status = COMECACAS_HELLO_LEN;
		}
		break;

	case COMECACAS_DIE:
		zoom_x = zoom_y = (float) (game.comecacas.status) / (float) COMECACAS_DIE_LEN;
		game.comecacas.status--;
		if(game.comecacas.status < 0) {
			if(game.lifes > 0) {
				game.comecacas.state = COMECACAS_HELLO;
				game.comecacas.status = COMECACAS_HELLO_LEN;
			} else {
				game.comecacas.state = COMECACAS_GAMEOVER;
				game.comecacas.status = COMECACAS_GAMEOVER_LEN;
			}
		}
		break;

	case COMECACAS_GAMEOVER:
		if(game.comecacas.status < 0)
			exit(1);
		game.comecacas.status--;
		hidden = true;
		break;

	case COMECACAS_HELLO:
		zoom_x = zoom_y = (float) (game.comecacas.status);
		game.comecacas.status--;
		if(game.comecacas.status < 0)
			game.comecacas.state = COMECACAS_NORMAL;
		break;

	case COMECACAS_NORMAL:
		if(game.comecacas.floor) {
			if(game.control_up || game.control_fire) {
				dir = 0;
				game.comecacas.speed_y = -COMECACAS_JUMP;
			} else
			if(game.control_down && (game.control_left || game.control_right)) {
				dir = game.control_left ? -1 : 1;
				game.comecacas.status = dir * COMECACAS_SWIPE_LEN;
				game.comecacas.state = COMECACAS_SWIPE;
			}
		}

		if((game.control_left || game.control_right)
		&& !(game.control_left && game.control_right)) {
			dir = game.control_left ? -1 : 1;
			acc_x = (float) dir * COMECACAS_ACC;
		} else {
			acc_x = game.comecacas.speed_x > 0.0
				? -COMECACAS_BRAKE_ACC
				: game.comecacas.speed_x < 0.0
				? COMECACAS_BRAKE_ACC
				: 0.0;
		}

		if(dir) {
			angle = 0.0;
			flip_x = dir < 0;
		}

		break;

	case COMECACAS_SWIPE:
		dir = game.comecacas.status < 0 ? -1 : 1;
		float swipe_percent = (float) (COMECACAS_SWIPE_LEN - abs(game.comecacas.status)) / (float) COMECACAS_SWIPE_LEN;

		if(swipe_percent < COMECACAS_SWIPE_STEP_1) {
			zoom_y = 0.3 + 0.7 * (1.0 - (swipe_percent / COMECACAS_SWIPE_STEP_1));
		} else
		if(swipe_percent < COMECACAS_SWIPE_STEP_2) {
			zoom_y = 0.3;
		} else {
			zoom_y = 0.3 + 0.7 *
				((swipe_percent - COMECACAS_SWIPE_STEP_2)
					/ (1.0 - COMECACAS_SWIPE_STEP_2));
		}
		acc_x = COMECACAS_SWIPE_MAX_ACC
			* (float) dir
			* sin(M_PI * swipe_percent);
		zoom_x = sin(M_PI * swipe_percent) * 0.5 + 1.0;

		game.comecacas.status -= dir;

		angle = 0.0;
		flip_x = dir < 0;

		if(!game.comecacas.status)
			game.comecacas.state = COMECACAS_NORMAL;
		break;
	
	default:
		printf("Comecacas bad state %d\n", game.comecacas.state);
	}

	// apply horizontal acceleration, limit speed
	game.comecacas.speed_x += acc_x;
	if(game.comecacas.state == COMECACAS_NORMAL
	&& fabsf(game.comecacas.speed_x) > COMECACAS_SPEED_MAX)
		game.comecacas.speed_x =
			game.comecacas.speed_x > 0
				? COMECACAS_SPEED_MAX
				: -COMECACAS_SPEED_MAX;

	game.comecacas.x += game.comecacas.speed_x;

	// apply gravity
	if(game.comecacas.y + game.comecacas.speed_y > game.screen_floor) {
		game.comecacas.y = game.screen_floor;
		game.comecacas.floor = true;
	} else {
		game.comecacas.floor = game.comecacas.y >= game.screen_floor;
	}

	game.comecacas.y += game.comecacas.speed_y;
	game.comecacas.speed_y =
		game.comecacas.floor
			? 0.0
			: game.comecacas.speed_y + COMECACAS_GRAVITY;

	// apply zoom
	sprite_set_zoom_xy(game.comecacas.s, zoom_x, zoom_y);

	// apply screen limit
	if(game.comecacas.x < game.screen_limit_left)
		game.comecacas.x = game.screen_limit_left;
	if(game.comecacas.x > game.screen_limit_right)
		game.comecacas.x = game.screen_limit_right;

	// set x, y, zoom, flip and bitmap
	sprite_set_hidden(
		game.comecacas.s,
		hidden);
	sprite_set_xy(
		game.comecacas.s,
		game.comecacas.x,
		game.comecacas.y);
	sprite_set_angle(
		game.comecacas.s,
		angle);
	sprite_set_flip_x(
		game.comecacas.s,
		flip_x);
	sprite_set_bitmap(
		game.comecacas.s,
		(game.frame >> 2) % game.comecacas.s->bitmaps_n);

	return 0;
}

void game_update_lanzacacas(void)
{
	float dx;
	if(game.next_caca < game.frame)
		return;
	
	if(game.frame < game.next_caca - LANZACACAS_FRAMES_LAUNCH * 4) {
		sprite_set_bitmap(game.lanzacacas.s, 0);
		dx = (game.next_caca_x - game.last_caca_x)
			* ((float) (game.frame - game.last_caca) / (float) (game.next_caca - game.last_caca - LANZACACAS_FRAMES_LAUNCH * 4))
			+ game.last_caca_x;
		sprite_set_flip_x(game.lanzacacas.s, game.next_caca_x < game.last_caca_x);
	} else {
		int s = 3 - (game.next_caca - game.frame) / LANZACACAS_FRAMES_LAUNCH;
		if(s <= 0)
			s = 3;
		sprite_set_bitmap(game.lanzacacas.s, s);
		dx = game.next_caca_x;
	}
	sprite_set_xy(game.lanzacacas.s, dx, game.screen_horizont);
}

void game_update_cacas(void)
{
	float r = game.screen_limit_right;
	float l = game.screen_limit_left;

	if(game.next_caca < game.frame) {
		game.last_caca   = game.next_caca;
		game.last_caca_x = game.next_caca_x;

		game.next_caca = (random() % (CACAS_FREQ_MAX - CACAS_FREQ_MIN))
			+ game.frame
			+ CACAS_FREQ_MIN;
		game.next_caca_x +=
			(random() & 1 ? -1 : 1) * random() % LANZACACAS_DESP_MAX;
		if(game.next_caca_x < l)
			game.next_caca_x = l + (l - game.next_caca_x);
		if(game.next_caca_x > r)
			game.next_caca_x = r - (game.next_caca_x - r);
	}

	if(game.next_caca == game.frame)
	{
		for(int i = 0; i < CACAS_MAX; i++) {
			if(!game.cacas[i].state) {
				game.cacas[i].state = CACA_UP;
				sprite_set_xy(
					game.cacas[i].s,
					game.next_caca_x,
					game.screen_horizont);
				sprite_set_hidden(game.cacas[i].s, false);
				sprite_set_alpha(game.cacas[i].s, 1.0);
				break;
			}
		}
	}

	for(int i = 0; i < CACAS_MAX; i++) {
		float y;

		switch(game.cacas[i].state) {
		default:
		case CACA_OFF:
			break;

		case CACA_CHOF:
			if(game.cacas[i].counter > 0) {
				game.cacas[i].counter--;
			} else {
				game.cacas[i].state = CACA_OFF;
				sprite_set_hidden(game.cacas[i].s, true);
			}
			break;

		case CACA_OUT:
			if(game.cacas[i].counter > 0) {
				game.cacas[i].counter--;
			} else {
				game.cacas[i].state = CACA_DOWN;
			}
			break;

		case CACA_UP:
		case CACA_DOWN:
			y = sprite_get_y(game.cacas[i].s)
					+ (game.cacas[i].state == CACA_UP ? -CACAS_SPEED_UP : CACAS_SPEED_DOWN);
			if(y < 0) {
				game.cacas[i].state = CACA_OUT;
				game.cacas[i].counter = CACAS_FRAMES_OUT;
			} else
			if(y > game.screen_floor) {
				game.cacas[i].state = CACA_CHOF;
				game.cacas[i].counter = CACAS_FRAMES_CHOF;
				sprite_set_y(game.cacas[i].s, game.screen_floor);
				sprite_set_angle(game.cacas[i].s, 0.0);
				sprite_set_zoom_x(game.cacas[i].s, 1.2);
				sprite_set_zoom_y(game.cacas[i].s, 0.5);
				sprite_set_alpha(game.cacas[i].s, 0.5);
			} else {
				sprite_set_zoom(
					game.cacas[i].s,
					game.cacas[i].state == CACA_UP
					? (CACAS_ZOOM_UP_MAX - CACAS_ZOOM_UP_MIN)
						* (y - game.screen_floor) / game.screen_floor
					+ CACAS_ZOOM_UP_MIN
					: 1.0);
				sprite_set_y(game.cacas[i].s, y);
				sprite_set_angle(
					game.cacas[i].s,
					sprite_get_angle(game.cacas[i].s) + 0.1);
			}
			break;

		case CACA_NOMNOMNOM:
			if(game.cacas[i].counter > 0) {
				game.cacas[i].counter--;
				sprite_set_zoom(
					game.cacas[i].s,
					((float) game.cacas[i].counter) / (float) CACA_FRAMES_NOMNOMNOM);
				sprite_set_angle(
					game.cacas[i].s,
					sprite_get_angle(game.cacas[i].s) + 0.5);
				sprite_set_xy(
					game.cacas[i].s,
					game.comecacas.x,
					game.comecacas.y
						- sprite_get_real_height(game.comecacas.s) / 2.0);
			} else {
				game.cacas[i].state = CACA_OFF;
			}
		}
	}
}

void game_update_flies_dividi(FLIES *f)
{
	if(f->s->x <= game.screen_limit_left
	|| f->s->x >= game.screen_limit_right)
		f->angle = (M_PI - f->angle);
	if(f->s->y <= 0
	|| f->s->y >= game.screen_height)
		f->angle = -f->angle;

	sprite_add_xy(
		f->s,
		f->speed * cos(f->angle),
		f->speed * sin(f->angle));
	sprite_set_angle(
		f->s,
		f->angle + M_PI);
}

void game_update_flies_lokipoki(FLIES *f)
{
	if((game.frame & 0xff) == 0x01) {
		f->angle = ((float) (random() & 0xff) / 255.0) * M_PI * 2;
		f->rotipoti = M_PI * 8.0;
	} else {
		f->rotipoti -= (f->rotipoti * 0.01);
	}

	game_update_flies_dividi(f);

	sprite_set_angle(
		f->s,
		f->angle + f->rotipoti + M_PI);
}

void game_update_flies_middlemierdi(FLIES *f)
{
	float w = 0.2;

	f->rotipoti += 0.01;
	f->cocototo = ((float) ((game.frame & 0x100) ? 0xff - (game.frame & 0xff) : game.frame & 0xff) / 255.0)
			* game.screen_height / 2.0
			+ 100.0;
	
	float x = sprite_get_x(f->s);
	x = x * (1.0 - w) + (f->cocototo * cos(f->rotipoti) + game.screen_width / 2.0) * w;
	float y = sprite_get_y(f->s);
	y = y * (1.0 - w) + (f->cocototo * sin(f->rotipoti) + game.screen_height / 2.0) * w;
	sprite_set_xy(f->s, x, y);
	sprite_set_angle(f->s, tanf(-f->rotipoti));
}

void game_update_flies(void)
{
	float w = 0.01;

	for(int i = 0; i < FLIES_MAX; i++) {
		switch(game.comecacas.state) {
		case COMECACAS_NORMAL:
		case COMECACAS_SWIPE:
			game.flies[i].update(&(game.flies[i]));
			break;

		case COMECACAS_DIE:
			sprite_set_xy(game.flies[i].s,
				sprite_get_x(game.flies[i].s) * (1.0 - w) + w * game.screen_width / 2.0,
				sprite_get_y(game.flies[i].s) * (1.0 - w) + w * 0.0);
			break;
		}
		sprite_set_bitmap(game.flies[i].s, (game.frame >> 1) & 0x1);
	}
}

void game_caca_collisions(void)
{
	for(int i = 0; i < CACAS_MAX; i++) {
		if(game.cacas[i].state == CACA_DOWN
		&& sprite_collide_basic(game.comecacas.s, game.cacas[i].s)) {
			game.cacas[i].state = CACA_NOMNOMNOM;
			game.cacas[i].counter = CACA_FRAMES_NOMNOMNOM;
			game.score++;
		}
	}
}

void game_flies_collisions(void)
{
	if(game.comecacas.state != COMECACAS_NORMAL
	&& game.comecacas.state != COMECACAS_SWIPE)
		return;

	for(int i = 0; i < FLIES_MAX; i++) {
		if(sprite_collide_basic(game.comecacas.s, game.flies[i].s)) {
			game.comecacas.state = COMECACAS_DIE;
			game.comecacas.status = COMECACAS_DIE_LEN;
			game.lifes--;
		}
	}
}


void game_render_toilet_paper(char *str, float x, float y, int align)
{
	al_draw_text(game.font_paper, al_map_rgba_f(.0, .0, .0, 1.), x + 3, y + 3, align, str);
	al_draw_text(game.font_paper, al_map_rgba_f(.5, .5, .5, .5), x - 1, y - 1, align, str);
	al_draw_text(game.font_paper, al_map_rgba_f(1., 1., 1., 1.), x,     y,     align, str);
}

void game_render_title(char *str, float y)
{
	float x = game.screen_width / 2.0;
	al_draw_text(game.font_poo, al_map_rgb_f(0, 0, 0), x + 3, y + 3, ALLEGRO_ALIGN_CENTER, str);
	al_draw_text(game.font_poo, al_map_rgb_f(1, 1, 1), x,     y,     ALLEGRO_ALIGN_CENTER, str);
}

void game_render(void)
{
	char tmp[255];

	al_clear_to_color(al_map_rgb(0, 0, 0));
	sprite_draw(game.background);

	sprite_draw(game.lanzacacas.s);
	for(int i = 0; i < CACAS_MAX; i++) {
		if(game.cacas[i].state != CACA_OFF
		&& game.cacas[i].state != CACA_OUT)
			sprite_draw(game.cacas[i].s);
	}

	for(int i = 0; i < FLIES_MAX; i++) {
		sprite_draw(game.flies[i].s);
	}


	sprite_draw(game.comecacas.s);

	// score
	snprintf(tmp, sizeof(tmp), "<SCORE %03d>", game.score);
	game_render_toilet_paper(tmp, game.screen_limit_right * 0.95, 10, ALLEGRO_ALIGN_RIGHT);

	// score
	snprintf(tmp, sizeof(tmp), "<LIFES %d>", game.lifes);
	game_render_toilet_paper(tmp, game.screen_limit_left * 1.05, 10, ALLEGRO_ALIGN_LEFT);

	// gameover
	if(game.comecacas.state == COMECACAS_GAMEOVER)
		game_render_title("GAME OVER", game.screen_height / 2.0);
	if(game.comecacas.state == COMECACAS_TITLE)
		game_render_title("- COMECACAS -", game.screen_height / 2.0);
}

int game_update()
{
	// update actors
	game_update_comecacas();
	game_update_flies();
	game_update_cacas();
	game_update_lanzacacas();

	game_render();

	// look for collisions
	game_caca_collisions();
	game_flies_collisions();

	game.frame++;

	return 0;
}

int main(void)
{
	ALLEGRO_TIMER *timer;
	ALLEGRO_EVENT_QUEUE *queue;
	ALLEGRO_EVENT event;
	bool key_pressed;

	al_init();
	al_install_keyboard();
	al_init_image_addon();
	al_init_primitives_addon();
	al_init_font_addon();
	al_init_ttf_addon();

	game_init();

	timer = al_create_timer(1.0 / FPS);
	al_start_timer(timer);

	queue = al_create_event_queue();
	al_register_event_source(queue, al_get_display_event_source(game.screen));
	al_register_event_source(queue, al_get_keyboard_event_source());
	al_register_event_source(queue, al_get_timer_event_source(timer));

	while(1) {
		al_wait_for_event(queue, &event);

		key_pressed = false;
		switch(event.type) {
		case ALLEGRO_EVENT_TIMER:
			game_update();
			al_flip_display();
			break;

		case ALLEGRO_EVENT_KEY_DOWN:
			key_pressed = true;

		case ALLEGRO_EVENT_KEY_UP:
			switch(event.keyboard.keycode) {
			case ALLEGRO_KEY_SPACE:  game.control_fire  = key_pressed; break;
			case ALLEGRO_KEY_UP:     game.control_up    = key_pressed; break;
			case ALLEGRO_KEY_DOWN:   game.control_down  = key_pressed; break;
			case ALLEGRO_KEY_LEFT:   game.control_left  = key_pressed; break;
			case ALLEGRO_KEY_RIGHT:  game.control_right = key_pressed; break;
			case ALLEGRO_KEY_ESCAPE: exit(0); break;
			}
			break;


		case ALLEGRO_EVENT_DISPLAY_CLOSE:
			return 0;

		default:
			;
		}
	}

	return 0;
}
