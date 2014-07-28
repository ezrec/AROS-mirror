/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#include "manager.h"
#include "../game/game.h"
#include "file.h"
#include "chart.h"
#include "event.h"
#include "config.h"
#include "../gui/stk.h"
#include "theme.h"
#include "slot.h"

/** Basic information about levelset and screenshot of first level */
typedef struct {
	char		*name;
	struct {
		int major;
		int minor;
	} 		version;
	char		*author;
	int		num_levels;
	SDL_Surface	*thumbnail;
} set_info_t;

/** Button displaying a levelsetname or up/down */
typedef struct {
	SDL_Rect	region;
#define SELECTID_UNUSED -4
#define	SELECTID_PREV -3
#define SELECTID_NEXT -2
#define SELECTID_EXIT -1
	int		id; /* special id or index in set_infos */
#define MAXLABELLEN 24
	char		label[MAXLABELLEN];
	int		focus;
	uint32_t	last_focus_time; /* for animation; not used now */
} select_button_t;

/** Select dialog object */
typedef struct {
	int		initialized; /* resources loaded */
	SDL_Surface	*background; /* background of dialog */
	StkFont		*standard_font;
	StkFont		*caption_font;
	StkFont		*highlight_font;
	int		num_set_infos;
	set_info_t	*set_infos; /* information about all levelsets */
#define NUMSELECTBUTTONS 22
#define SETBUTTON_START_ID 1
#define SETBUTTON_END_ID 19
	select_button_t	select_buttons[NUMSELECTBUTTONS];
	char		*selected_set; /* pointer to name in set_infos */
	SDL_Surface	*thumbnail_background; /* bkgnd + frame + paddle */
	SDL_Surface	*thumbnail_bricks; /* scaled down bricks */
} setselect_dlg_t;
setselect_dlg_t ssd;

extern char **levelset_names_local;
extern int levelset_count_local;
extern SDL_Surface *stk_display;
extern SDL_Surface *extra_pic;
extern SDL_Surface *brick_pic;
extern int stk_quit_request;
extern Config config;
extern SDL_Surface *brick_pic;
extern Brick_Conv brick_conv_table[BRICK_COUNT];
extern SDL_Surface *frame_left, *frame_top, *frame_right;
extern SDL_Surface **bkgnds, *paddle_pic, *ball_pic, *lamps;
extern int paddle_cw, paddle_ch, ball_w, ball_h;
extern int cw, ch;
#ifdef AUDIO_ENABLED
extern StkSound *wav_menu_click, *wav_menu_motion;
#endif

/** Return new surface half the width*height than @surf. */
static SDL_Surface * shrink_surface_half(SDL_Surface *surf)
{
	int i, j, nw = surf->w / 2, nh = surf->h / 2;
	SDL_Surface *newsurf = NULL;
	
	if ((newsurf = stk_surface_create(SDL_SWSURFACE,nw,nh)) == NULL) {
		fprintf(stderr,_("Out of memory"));
		return NULL;
	}
	
        for ( j = 0; j < nh; j++ )
		for ( i = 0; i < nw; i++ )
			stk_surface_set_pixel(newsurf, i, j, 
					stk_surface_get_pixel(surf,i<<1,j<<1));
	return newsurf;
}


/** Create an empty level (with frame, paddle, etc) and scale to half size so
 * it can be used for levelset previews. */
static SDL_Surface* create_thumbnail_background()
{
	int i, px, py;
	SDL_Surface *bkgnd = NULL, *thumb = NULL;
	
	/* wallpaper */
	bkgnd = stk_surface_create(SDL_SWSURFACE,stk_display->w,stk_display->h);
	bkgnd_draw(bkgnd,-1,0);
	
	/* frame */
	stk_surface_blit(frame_left,0,0,-1,-1,bkgnd,0,0);
	stk_surface_blit(frame_top,0,0,-1,-1,bkgnd,frame_left->w,0);
	stk_surface_blit(frame_right,0,0,-1,-1,bkgnd,stk_display->w-frame_right->w,0);
	
	/* lifes */
	for (i = 0; i < 5; i++)
		stk_surface_blit(lamps,0,BRICK_HEIGHT,BRICK_WIDTH,BRICK_HEIGHT,
					bkgnd, 0, bkgnd->h-(i+1)*BRICK_HEIGHT);
	
	/* paddle + ball */
	px = (bkgnd->w - paddle_cw * 3) / 2;
	py = bkgnd->h - 2 * BRICK_HEIGHT;
	stk_surface_blit(paddle_pic,0,0,paddle_cw * 3, paddle_ch, bkgnd, px,py);
	stk_surface_blit(ball_pic,0,0,ball_w,ball_h,bkgnd,
				(bkgnd->w - ball_w) / 2, py - ball_h);
	
	thumb = shrink_surface_half(bkgnd);
	SDL_FreeSurface(bkgnd);
	SDL_SetColorKey(thumb, 0, 0);
	return thumb;
}

/** Render an image of the first level. If @level is NULL render empty level. */
static void render_level_thumbnail(set_info_t *si, const Level *level)
{
	int tw = ssd.thumbnail_background->w;
	int th = ssd.thumbnail_background->h;
	int bw = BRICK_WIDTH / 2, bh = BRICK_HEIGHT / 2;
	int i, j, k, bx, by, xoff, yoff;
	
	if (si->thumbnail)
		return;

	/* set background */
	if ((si->thumbnail = stk_surface_create(SDL_SWSURFACE,tw,th)) == NULL)
		return;
	SDL_SetColorKey(si->thumbnail, 0, 0);
	stk_surface_blit(ssd.thumbnail_background, 0, 0, tw, th, 
							si->thumbnail, 0, 0);
	
	if (level == NULL)
		return;

	/* add bricks */
	xoff = bx = ((MAP_WIDTH - EDIT_WIDTH)/2) * bw;
	yoff = by = bh;
	for (j = 0; j < EDIT_HEIGHT; j++, by += bh) {
		for (i = 0; i < EDIT_WIDTH; i++, bx += bw) {
			for (k = 0; k < BRICK_COUNT; k++)
				if (brick_conv_table[k].c == level->bricks[i][j])
					break;
			if (k == BRICK_COUNT)
				continue; /* oops, unknown id? */
			if (brick_conv_table[k].id != INVIS_BRICK_ID)
				stk_surface_blit(ssd.thumbnail_bricks,
						bw * brick_conv_table[k].id, 0,
						bw, bh, si->thumbnail, bx, by);
		}
		bx = xoff;
	}
}

/** Load basic information of levelset @sname (preceded by ~ for set in 
 * home directory) into struct @si. Also generate a small preview thumbnail
 * of first level. */
static void load_set_info( set_info_t *si, const char *sname )
{
	FILE *file = NULL;
	Level *level = NULL;
	int num_levels = 0, i, is_def_set = 0;
	char *default_sets[] = {
		TOURNAMENT,
		_("!JUMPING_JACK!"),
		_("!OUTBREAK!"),
		_("!BARRIER!"),
		_("!SITTING_DUCKS!"),
		_("!HUNTER!"),
		_("!INVADERS!")
	};
	
	memset(si, 0, sizeof( set_info_t ));
	
	/* set name */
	si->name = strdup(sname);
	
	/* open file, if this fails the rest of the info can't be set */
	/* TODO: what to do with lengthy description of special levelsets? */
	for (i = 0; i < 7; i++)
		if (strcmp(sname,default_sets[i]) == 0) {
			is_def_set = 1;
			break;
		}
	if (is_def_set || (file = levelset_open(sname, "rb")) == NULL) {
		si->version.major = 1;
		si->version.minor = 0;
		si->author = strdup("???");
		si->num_levels = 0;
		render_level_thumbnail(si, NULL);
		if (is_def_set) {
			ssd.caption_font->align = STK_FONT_ALIGN_CENTER_X | 
							STK_FONT_ALIGN_CENTER_Y;
			stk_font_write(ssd.caption_font, si->thumbnail, 
					si->thumbnail->w/2, si->thumbnail->h/2,
					STK_OPAQUE, _("Special Game"));
		}
		return; /* error is printed by levelset_open() */
	}

	/* get version */
	levelset_get_version(file, &si->version.major, &si->version.minor);
	
	/* get author name and thumbnail from first level */
	if ((level = level_load(file)) == NULL) {
		fprintf(stderr,"Could not load first level of set %s\n",sname);
		si->author = strdup("???");
		si->num_levels = 0;
		render_level_thumbnail(si, NULL);
		return;
	}
	num_levels = 1;
	si->author = strdup(level->author);
	render_level_thumbnail(si,level);
	level_delete(level);
	
	/* count remaining levels 
	 * FIXME: don't parse levels but just count number */
	while ((level = level_load(file)) != NULL) {
		num_levels++;
		level_delete(level);
	}
	si->num_levels = num_levels;

	fclose( file );	
}

/** Update set select buttons using set infos starting at index @set_id.
 * The buttons outside range (prev, next, exit) are not touched. */
static void update_select_buttons( int set_id )
{
	int i;
	
	for (i = SETBUTTON_START_ID; i <= SETBUTTON_END_ID; i++) {
		select_button_t *btn = &ssd.select_buttons[i];
		
		btn->focus = 0;
		btn->last_focus_time = 0;
		
		if (set_id >= ssd.num_set_infos) {
			btn->id = SELECTID_UNUSED;
			strcpy(btn->label,"");
			continue;
		}
		
		btn->id = set_id;
		snprintf(btn->label, MAXLABELLEN, "%s",
					ssd.set_infos[set_id].name);
		set_id++;
	}
}

/** Return new id for select buttons. @dir is either -1 for scrolling up
 * or 1 for scrolling down. */
static int get_new_select_button_start_id( int dir )
{
	int id;
	int num_setbuttons = SETBUTTON_END_ID - SETBUTTON_START_ID + 1;
	
	if (dir == -1) {
		id = ssd.select_buttons[SETBUTTON_START_ID].id;
		if (id == 0) {
			/* go to end of list */
			id = ssd.num_set_infos - num_setbuttons;
		} else {
			id -= num_setbuttons;
			if (id < 0)
				id = 0;
		}
		return id;
	}
	if (dir == 1) {
		id = ssd.select_buttons[SETBUTTON_START_ID].id;
		if (id == ssd.num_set_infos - num_setbuttons) {
			/* go to begin of list */
			id = 0;
		} else {
			id += num_setbuttons;
			if (id > ssd.num_set_infos - num_setbuttons)
				id = ssd.num_set_infos - num_setbuttons;
		}
		return id;
	}
	return 0;
}

/** Load/Free resources. */
void setselect_create()
{
	int i, x, y;
	
	if (ssd.initialized)
		return; /* already done */
	
	memset(&ssd, 0, sizeof(ssd));
	
	ssd.standard_font = stk_font_load( SDL_SWSURFACE, "f_small_yellow.png" );
	SDL_SetColorKey( ssd.standard_font->surface, SDL_SRCCOLORKEY, 
		stk_surface_get_pixel( ssd.standard_font->surface, 0,0 ) );	
	ssd.highlight_font = stk_font_load( SDL_SWSURFACE, "f_small_white.png" );
	SDL_SetColorKey( ssd.highlight_font->surface, SDL_SRCCOLORKEY, 
		stk_surface_get_pixel( ssd.highlight_font->surface, 0,0 ) );	
	ssd.caption_font = stk_font_load( SDL_SWSURFACE, "f_yellow.png" );
	SDL_SetColorKey( ssd.caption_font->surface, SDL_SRCCOLORKEY, 
		stk_surface_get_pixel( ssd.caption_font->surface, 0,0 ) );
	
	/* background -- will be filled when running dialog */
	ssd.background = stk_surface_create( SDL_SWSURFACE, stk_display->w, 
							stk_display->h );
	SDL_SetColorKey( ssd.background, 0, 0 );
	
	/* position select buttons */
	x = 50; y = 50;
	for (i = 0; i < NUMSELECTBUTTONS; i++) {
		select_button_t *sb = &ssd.select_buttons[i];
		
		sb->region.x = x;
		sb->region.y = y;
		sb->region.w = 180;
		sb->region.h = ssd.standard_font->height + 5;
		
		if (i == 0) {
			sb->id = SELECTID_PREV;
			snprintf(sb->label,MAXLABELLEN,_("...Previous Page..."));
		} else if (i == NUMSELECTBUTTONS - 2) {
			sb->id = SELECTID_NEXT;
			snprintf(sb->label,MAXLABELLEN,_("...Next Page..."));
			y += 10; /* some distance to back button */
		} else if (i == NUMSELECTBUTTONS - 1) {
			sb->id = SELECTID_EXIT;
			snprintf(sb->label,MAXLABELLEN,_("Back To Menu"));
			
		} else {
			sb->id = SELECTID_UNUSED;
			snprintf(sb->label,MAXLABELLEN,_("<not set>"));
		}
		
		sb->focus = 0;
		sb->last_focus_time = 0;
		
		y += ssd.standard_font->height + 5;
	}
	
	/* thumbnail background and bricks */
	ssd.thumbnail_background = create_thumbnail_background();
	ssd.thumbnail_bricks = shrink_surface_half(brick_pic);
	/* by default bricks have no color key but there may be an extra 
	 * column at the end to specify one */
	if (brick_pic->w & 1)
		SDL_SetColorKey( ssd.thumbnail_bricks, SDL_SRCCOLORKEY,
					stk_surface_get_pixel( brick_pic,
						brick_pic->w - 1, 0 ) );

	/* levelset infos */
	ssd.num_set_infos = levelset_count_local;
	ssd.set_infos = calloc( ssd.num_set_infos, sizeof(set_info_t) );
	for (i = 0; i < ssd.num_set_infos; i++) {
		set_info_t *si = &ssd.set_infos[i];
		load_set_info( si, levelset_names_local[i] );
	}
	update_select_buttons(0);
	
	ssd.initialized = 1;
}
void setselect_delete()
{
	int i;
	
	if (!ssd.initialized)
		return;
	
	stk_font_free( &ssd.standard_font );
	stk_font_free( &ssd.caption_font );
	stk_font_free( &ssd.highlight_font );
	stk_surface_free( &ssd.background );
	
	if (ssd.set_infos) {
		for (i = 0; i < ssd.num_set_infos; i++) {
			set_info_t *si = &ssd.set_infos[i];
			if (si->name)
				free(si->name);
			if (si->author)
				free(si->author);
			if (si->thumbnail)
				SDL_FreeSurface(si->thumbnail);
		}
		free(ssd.set_infos);
		ssd.set_infos = NULL;
	}
	
	stk_surface_free( &ssd.thumbnail_background );
	stk_surface_free( &ssd.thumbnail_bricks );
	
	ssd.initialized = 0;
}

/** Set background from current screen */
static void set_background()
{
	SDL_Surface *buffer = stk_surface_create(SDL_SWSURFACE,
					stk_display->w, stk_display->h);
	
	stk_surface_blit( stk_display, 0,0,-1,-1, buffer, 0,0 );
	SDL_SetColorKey(buffer, 0, 0);
	stk_surface_gray( stk_display, 0,0,-1,-1, 2 );
	stk_surface_blit( stk_display, 0,0,-1,-1, ssd.background, 0,0 );
	
	ssd.caption_font->align = STK_FONT_ALIGN_CENTER_X | STK_FONT_ALIGN_TOP;
	stk_font_write(ssd.caption_font,ssd.background,ssd.background->w/2,20,
				STK_OPAQUE,_("Select Custom Levelset"));
	
	SDL_FreeSurface( buffer );
	
}

/** Draw buttons list. If @refresh is True update screen. */
static void draw_buttons( int refresh )
{
	int i;
	StkFont *font = NULL;
	
	ssd.standard_font->align = STK_FONT_ALIGN_LEFT | STK_FONT_ALIGN_TOP;
	for (i = 0; i < NUMSELECTBUTTONS; i++) {
		select_button_t *btn = &ssd.select_buttons[i];
		stk_surface_blit( ssd.background, btn->region.x, btn->region.y,
				btn->region.w, btn->region.h,
				stk_display, btn->region.x, btn->region.y);
		font = ssd.standard_font;
		if (btn->focus)
			font = ssd.highlight_font;
		stk_font_write(font, stk_display, btn->region.x, btn->region.y,
						STK_OPAQUE, btn->label);
	}
	
	if (refresh) {
		SDL_Rect region = { 
			ssd.select_buttons[0].region.x, 
			ssd.select_buttons[0].region.y,
			ssd.select_buttons[NUMSELECTBUTTONS-1].region.w,
			ssd.select_buttons[NUMSELECTBUTTONS-1].region.y +
			ssd.select_buttons[NUMSELECTBUTTONS-1].region.h - 
			ssd.select_buttons[0].region.y };
		stk_display_store_rect( &region );
		stk_display_update( STK_UPDATE_RECTS );
	}
}

/** Draw set info. If @si is NULL just clear region. */
static void draw_set_info(set_info_t *si, int refresh)
{
	SDL_Rect ir = {270, 50, 320, 250+ch};
	int x, y;
	char buf[64];
	
	/* clear background */
	stk_surface_blit(ssd.background,ir.x,ir.y,ir.w,ir.h,
							stk_display,ir.x,ir.y);
	if (si == NULL) {
		stk_display_store_rect( &ir );
		stk_display_update( STK_UPDATE_RECTS );
		return;
	}
	
	/* thumbnail */
	stk_surface_blit(si->thumbnail,0,0,si->thumbnail->w,si->thumbnail->h,
				stk_display, ir.x, ir.y);

	/* info */
	x = ir.x + si->thumbnail->w / 2; y = ir.y + si->thumbnail->h + 2;
	ssd.standard_font->align = STK_FONT_ALIGN_CENTER_X | STK_FONT_ALIGN_TOP;
	snprintf(buf, 64, _("by %s, %d levels"), si->author, si->num_levels);
	stk_font_write(ssd.standard_font, stk_display, x, y, STK_OPAQUE, buf);
	x = ir.x + si->thumbnail->w / 2; y = ir.y + si->thumbnail->h - 40;
	ssd.caption_font->align = STK_FONT_ALIGN_CENTER_X | STK_FONT_ALIGN_TOP;
	snprintf(buf, 64, _("%s v%d.%02d"), si->name, si->version.major,
							si->version.minor);
	stk_font_write(ssd.caption_font, stk_display, x, y, STK_OPAQUE, buf);
	
	/* highscores */
	chart_show_compact(chart_set_query(si->name), 
					ir.x + (ir.w-cw)/2, ir.y + 264, cw, ch);
	
	if (refresh) {
		stk_display_store_rect( &ir );
		stk_display_update( STK_UPDATE_RECTS );
	}
}

/** Draw everything. */
static void draw_all()
{
	stk_surface_blit( ssd.background, 0,0,-1,-1, stk_display, 0,0 );
	draw_buttons(0);
	stk_display_update( STK_UPDATE_ALL );
}

/** Handle mouse motion to position @x,@y. Redraw all buttons if either
 * the focus has changed or @force_redraw is set. */
static void handle_motion( int x, int y, int force_redraw )
{
	int i;
	select_button_t *focus_sb = NULL;
	static select_button_t *old_focus_sb = NULL;
	
	/* check button focus */
	for (i = 0; i < NUMSELECTBUTTONS; i++) {
		select_button_t *sb = &ssd.select_buttons[i];
		
		if (FOCUS_RECT(x,y,sb->region)) {
			sb->focus = 1;
			focus_sb = sb;
#ifdef AUDIO_ENABLED
			if (focus_sb != old_focus_sb)
				stk_sound_play(wav_menu_motion);
#endif			
		} else
			sb->focus = 0;
	}
	/* redraw */
	if (force_redraw || old_focus_sb != focus_sb) {
		draw_buttons(1);
		old_focus_sb = focus_sb;
		if (focus_sb && focus_sb->id >= 0)
			draw_set_info(&ssd.set_infos[focus_sb->id],1);
		else 
			draw_set_info(NULL,1);
	}
}

/** Handle mouse button click on position @x,@y. Return 1 if either Quit
 * button or levelset has been clicked, 0 otherwise. If set has been 
 * selected store it in ssd::selected_set. */
static int handle_click(int x, int y)
{
	int i;
	select_button_t *sb = NULL;
	
	/* get clicked button */
	for (i = 0; i < NUMSELECTBUTTONS; i++) {
		if (FOCUS_RECT(x,y,ssd.select_buttons[i].region)) {
			sb = &ssd.select_buttons[i];
			break;
		}
	}
	if (sb == NULL)
		return 0; /* no button clicked */
	
#ifdef AUDIO_ENABLED
	stk_sound_play(wav_menu_click);
#endif		
	
	if (sb->id == SELECTID_EXIT)
		return 1;
	if (sb->id == SELECTID_PREV || sb->id == SELECTID_NEXT) {
		int id;
		if (sb->id == SELECTID_PREV)
			id = get_new_select_button_start_id(-1);
		else
			id = get_new_select_button_start_id(1);
		update_select_buttons(id);
		draw_buttons(1);
		return 0;
	}
	ssd.selected_set = ssd.set_infos[sb->id].name;
	return 1;
}

/** Check if button is a mouse wheel. If so fake up/down button click and
 * return 1, otherwise 0. */
static int handle_scrolling( const SDL_Event *ev )
{
	int id = -1;

	if (ev->button.button == 4 /* up */)
		id = get_new_select_button_start_id(-1);
	else if (ev->button.button == 5 /* down */)
		id = get_new_select_button_start_id(1);
	if (id == -1)
		return 0;
	update_select_buttons(id);
	
#ifdef AUDIO_ENABLED
	stk_sound_play(wav_menu_motion);
#endif			
	
	handle_motion(ev->button.x, ev->button.y, 1);
	return 1;
}

/** Main loop, run dialog and return pointer to name of selected set or
 * NULL if none selected. */
const char * setselect_run()
{
	SDL_Event event;
	int leave = 0;
	SDL_EventFilter old_filter;
	
	/* backup current filter (which takes out motion events) as we work	
	 * with WaitEvent() for the moment; see manager_run() how it is done
	 * for polling events. */
	old_filter = SDL_GetEventFilter();
	SDL_SetEventFilter( 0 );
	
	/* TODO: reload set infos if necessary */
	
	ssd.selected_set = NULL;
	set_background();
	handle_motion(0,0,0); /* clear old highlighting */
	draw_all();
		
	while ( !leave && !stk_quit_request ) {
		SDL_WaitEvent( &event );
		switch ( event.type ) {
			case SDL_QUIT: 
				stk_quit_request = 1;
				break;
			case SDL_MOUSEMOTION:
				handle_motion( event.motion.x,
							event.motion.y, 0 );
				break;
			case SDL_MOUSEBUTTONUP:
				if (handle_scrolling(&event))
					break;
				if (handle_click(event.button.x,event.button.y))
					leave = 1;
				break;
			case SDL_KEYUP:
				if (event.key.keysym.sym == SDLK_ESCAPE)
					leave = 1;
				break;
		}
	}
	
	/* restore event filter */
	SDL_SetEventFilter(old_filter);
	
	return ssd.selected_set; /* is a pointer to ssd.set_infos */
}
