#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <dingoo/slcd.h>
#include <dingoo/cache.h>
#include <sml/control.h>
#include <sml/timer.h>
#include <sml/graphics.h>
#include <sml/display.h>

#define VERSION	1.2

extern int _sys_judge_event(void *);

display	*gameDisplay  = NULL;
bool game_running = true;
bool field[240][320];
unsigned short field_cnt[240][320];
unsigned char SCALE = 0;


int rnd( int max ) {
    return (rand() % max) + 1;
  }

void field_random()
{
    unsigned int j;
    unsigned short i;

    for(i = 0; i < 240; i++)
    {
        for(j = 0; j < 320; j++)
        {
            if (rnd(9) > 8) field[i][j] = TRUE;
        }
    }

}

void field_clear()
{
    unsigned int j;
    unsigned short i;

    for(i = 0; i < 240; i++)
    {
        for(j = 0; j < 320; j++)
        {
            field[i][j] = FALSE;
        }
    }
}

void display_life()
{
    unsigned int j,j1,j2, color;
    unsigned short i,i1,i2,w;

    switch(SCALE)
    {
    	default:
    		i1 = 0;
    		i2 = 239;
	    	j1 = 0;
    	    j2 = 319;
    	    w = 1;
    	    break;
    	case 1:
    		i1 = 60;
    		i2 = 179;
	    	j1 = 80;
    	    j2 = 239;
    	    w = 2;
    	    break;
    	case 2:
    		i1 = 90;
    		i2 = 149;
	    	j1 = 120;
    	    j2 = 199;
    	    w = 4;
    	    break;
    	case 3:
    		i1 = 105;
    		i2 = 134;
	    	j1 = 140;
    	    j2 = 179;
    	    w = 8;
    		break;
    }


    for(i = i1; i <= i2; i++) {
        for(j = j1; j <= j2; j++) {
            if (field[i][j])
            {
                color = COLOR_WHITE;
            }
            else
            {
                color = COLOR_BLACK;
            }
           	gfx_rect_fill_draw((j-j1)*w,(i-i1)*w,w,w,color);
        }
    }

}

void field_calculator()
{
    unsigned int j,jj;
    unsigned short i,ii;

    for(i = 0; i < 240; i++)
    {
        for(j = 0; j < 320; j++)
        {
            field_cnt[i][j] = 0;
        }
    }
    for(i = 0; i < 240; i++)
    {
        for(j = 0; j < 320; j++)
        {
            ii = i;
            jj = j;
            // N
            if (ii == 0)
                ii = 239;
            else
                ii--;
            if (field[ii][jj]) field_cnt[i][j]++;
            // NO
            if (jj == 319)
                jj = 0;
            else
                jj++;
            if (field[ii][jj]) field_cnt[i][j]++;
            // O
            if (ii == 239)
                ii = 0;
            else
                ii++;
            if (field[ii][jj]) field_cnt[i][j]++;
            // SO
            if (ii == 239)
                ii = 0;
            else
                ii++;
            if (field[ii][jj]) field_cnt[i][j]++;
            // S
            if (jj == 0)
                jj = 319;
            else
                jj--;
            if (field[ii][jj]) field_cnt[i][j]++;
            // SW
            if (jj == 0)
                jj = 319;
            else
                jj--;
            if (field[ii][jj]) field_cnt[i][j]++;
            // W
            if (ii == 0)
                ii = 239;
            else
                ii--;
            if (field[ii][jj]) field_cnt[i][j]++;
            // NW
            if (ii == 0)
                ii = 239;
            else
                ii--;
            if (field[ii][jj]) field_cnt[i][j]++;
        }
    }
    for(i = 0; i < 240; i++)
    {
        for(j = 0; j < 320; j++)
        {
            if (field_cnt[i][j] == 3) field[i][j] = TRUE;
            if ((field_cnt[i][j] < 2) || (field_cnt[i][j] > 3)) field[i][j] = FALSE;
        }
    }
}

int main(void)
{
	unsigned int j;
	unsigned short i;
	int ref = 1;
	bool auto_calc = TRUE, temp, need_update_display = TRUE;

	gameDisplay = display_create(320, 240, 320, (DISPLAY_FORMAT_RGB565|DISPLAY_BUFFER_STATIC), (uint32_t*)_lcd_get_frame(), NULL);
	if(gameDisplay == NULL) {
		return ref;
	}

	gfx_init(gameDisplay);
	gfx_render_target_clear(gfx_color_rgb(0x00, 0x00, 0x00));
	display_flip(gameDisplay);

	srand(OSTimeGet());

        control_init();
        //control_lock(timer_resolution / 4);

        field_random();

	int sysref;
	while(game_running) {
		sysref = _sys_judge_event(NULL);
		if(sysref < 0) {
			ref = sysref;
			break;
		}

                if (auto_calc)
                {
                    field_calculator();
                    need_update_display = TRUE;
                }

                control_poll();
                if (control_just_pressed(CONTROL_TRIGGER_LEFT))
                {
                	if (SCALE) SCALE--;
                	need_update_display = TRUE;
                }
                if (control_just_pressed(CONTROL_TRIGGER_RIGHT))
                {
                	if (SCALE<3) SCALE++;
                	need_update_display = TRUE;
                }
                // random fill
                if (control_just_pressed(CONTROL_BUTTON_A))
                {
                    field_random();
                    need_update_display = TRUE;
                }
                // step by step
                if (control_just_pressed(CONTROL_BUTTON_B))
                {
                    auto_calc = FALSE;
                    field_calculator();
                    need_update_display = TRUE;
                }
                // automatic ON/OFF
                if(control_just_pressed(CONTROL_BUTTON_X)) {
                    auto_calc = !auto_calc;
		}
		//
                if(control_check(CONTROL_DPAD_RIGHT).pressed)
                {
                    for(i = 0; i < 240; i++)
                    {
                        temp = field[i][0];
                        for(j = 0; j < 319; j++)
                        {
                            field[i][j] = field[i][j+1];
                        }
                        field[i][319] = temp;
                    }
                    need_update_display = TRUE;
                }
                //
                if(control_check(CONTROL_DPAD_LEFT).pressed)
                {
                    for(i = 0; i < 240; i++)
                    {
                        temp = field[i][319];
                        for(j = 319; j > 0; j--)
                        {
                            field[i][j] = field[i][j-1];
                        }
                        field[i][0] = temp;
                    }
                    need_update_display = TRUE;
                }
		//
                if(control_check(CONTROL_DPAD_DOWN).pressed)
                {
                    for(j = 0; j < 320; j++)
                    {
                        temp = field[0][j];
                        for(i = 0; i < 239; i++)
                        {
                            field[i][j] = field[i+1][j];
                        }
                        field[239][j] = temp;
                    }
                    need_update_display = TRUE;
                }
		//
                if(control_check(CONTROL_DPAD_UP).pressed)
                {
                    for(j = 0; j < 320; j++)
                    {
                        temp = field[239][j];
                        for(i = 239; i > 0; i--)
                        {
                            field[i][j] = field[i-1][j];
                        }
                        field[0][j] = temp;
                    }
                    need_update_display = TRUE;
                }
                // clear
                if(control_just_pressed(CONTROL_BUTTON_Y)) {
                    auto_calc = FALSE;
                    field_clear();
                    need_update_display = TRUE;
		}
		//exit
                if(control_check(CONTROL_BUTTON_SELECT).pressed)
			break;
                //
                if (need_update_display)
                {
                    display_life();
        			__dcache_writeback_all();
        			_lcd_set_frame();
                    //display_flip(gameDisplay);
                    need_update_display = FALSE;
                }
        }

	gfx_term();
	display_delete(gameDisplay);
	control_term();

	return ref;
}
