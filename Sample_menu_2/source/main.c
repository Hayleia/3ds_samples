#include <stdio.h>
#include <string.h>
#include <math.h>
#include <3ds.h>
#include <sf2d.h>
#include <sftd.h>
#include "FreeSans_ttf.h"

u64 umax(u64 a, u64 b) {
	return a>b ? a:b;
}

int main()
{
	char* names[] = {
		"nul/20",
		"item1",
		"second item",
		"the third one",
		"item4",
		"item5",
		"item6",
		"item7",
		"item8",
		"item9",
		"item10",
		"item11",
		"item12",
		"item13",
		"item14",
		"item15",
		"item16",
		"item17",
		"item18",
		"item19",
		"item20",
		"item21",
		"item22",
		"item23",
		"item24",
		"item25",
		"item26",
		"item27",
		"item28",
		"item29"
	};

	int nbitems = 30;
	uint8_t owneditem[nbitems];
	for (int i=0; i<nbitems; i++) owneditem[i] = 0;

	sf2d_init();
	sf2d_set_clear_color(RGBA8(255, 255, 255, 255));
	sf2d_set_vblank_wait(0);

	// Font loading
	sftd_init();
	sftd_font* font = sftd_load_font_mem(FreeSans_ttf, FreeSans_ttf_size);

	int fontSize = 20;

	int row = 0;
	int nbrow = 10;
	int firstrow = 0;

#define LONG_TIMEOUT 500
#define SHORT_TIMEOUT_MAX 100
#define SHORT_TIMEOUT_MIN 20

	u64 oldTime = osGetTime();
	u64 timer = 0;
	u64 timeout = LONG_TIMEOUT;

	while (aptMainLoop()) {

		hidScanInput();
		if (hidKeysDown() & KEY_START) break;

		if ((hidKeysHeld() & KEY_UP) && timer == 0) row--;
		if (row == -1) {
			row++;
			firstrow--;
			if (firstrow == -1) {
				row = nbrow-1;
				firstrow = nbitems-nbrow;
			}
		}

		if ((hidKeysHeld() & KEY_DOWN) && timer == 0) row++;
		if (row == nbrow) {
			row--;
			firstrow++;
			if (firstrow+nbrow == nbitems+1) {
				firstrow = 0;
				row = 0;
			}
		}

		int index = firstrow+row;
		owneditem[index] += 100;
		if (hidKeysDown() & KEY_LEFT) owneditem[index]--;
		if (hidKeysDown() & KEY_RIGHT) owneditem[index]++;
		owneditem[index] %= 100;

		// use osGetTime to have key repetition
		u64 newTime = osGetTime();
		u64 delay = newTime-oldTime;
		oldTime = newTime;
		if (hidKeysHeld()) {
			timer += delay;
			if (timer>timeout) {
				timer = 0;
				if (timeout == LONG_TIMEOUT) {
					timeout = SHORT_TIMEOUT_MAX;
				} else {
					timeout = umax(timeout-2, SHORT_TIMEOUT_MIN);
				}
			}
		} else {
			timer = 0;
			timeout = LONG_TIMEOUT;
		}

		sf2d_start_frame(GFX_TOP, GFX_LEFT);
		{
			for (int i=0; i<nbrow; i++) {
				unsigned int color = RGBA8(0, 0, 0, 255);
				if (i == row) {
					sf2d_draw_rectangle(0, i*fontSize, 400, fontSize, RGBA8(0, 0, 0, 255));
					color = RGBA8(255, 255, 255, 255);
				}
				sftd_draw_textf(font, 010, i*fontSize, color, fontSize, names[firstrow+i]);
				sftd_draw_textf(font, 210, i*fontSize, color, fontSize, "%i", owneditem[firstrow+i]);
			}
		}
		sf2d_end_frame();

		sf2d_start_frame(GFX_BOTTOM, GFX_LEFT);
		{
			sftd_draw_textf(font, 0, 20, RGBA8(0, 255, 0, 255), 20, "Press START to exit");
			sftd_draw_textf(font, 0, 40, RGBA8(0, 255, 0, 255), 20, "Timeout: %i", timeout);
		}
		sf2d_end_frame();

		sf2d_swapbuffers();
	}

	sftd_free_font(font);
	sftd_fini();

	sf2d_fini();
	return 0;
}
