/* ScummVM - Scumm Interpreter
 * Copyright (C) 2004 The ScummVM project
 *
 * The ReInherit Engine is (C)2000-2003 by Daniel Balsom.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 * $Header$
 *
 */

// Palette animation module
#include "saga/saga.h"
#include "saga/gfx.h"

#include "saga/events_mod.h"
#include "saga/game_mod.h"

#include "saga/palanim_mod.h"
#include "saga/palanim.h"

namespace Saga {

static PALANIM_DATA PAnimData;

int PALANIM_Load(const byte *resdata, size_t resdata_len) {
	void *test_p;

	uint16 i;

	if (PAnimData.loaded) {
		PALANIM_Free();
	}

	if (resdata == NULL) {
		return R_FAILURE;
	}

	MemoryReadStream readS(resdata, resdata_len);

	if (GAME_GetGameType() == R_GAMETYPE_IHNM) {
		return R_SUCCESS;
	}

	PAnimData.entry_count = readS.readUint16LE();

	debug(0, "PALANIM_Load(): Loading %d PALANIM entries.", PAnimData.entry_count);

	test_p = calloc(PAnimData.entry_count, sizeof(PALANIM_ENTRY));
	if (test_p == NULL) {
		warning("PALANIM_Load(): Allocation failure");
		return R_MEM;
	}

	PAnimData.entries = (PALANIM_ENTRY *)test_p;

	for (i = 0; i < PAnimData.entry_count; i++) {
		int color_count;
		int pal_count;
		int p, c;

		color_count = readS.readUint16LE();
		pal_count = readS.readUint16LE();

		PAnimData.entries[i].pal_count = pal_count;
		PAnimData.entries[i].color_count = color_count;

		debug(2, "PALANIM_Load(): Entry %d: Loading %d palette indices.\n", i, pal_count);

		test_p = calloc(1, sizeof(char) * pal_count);
		if (test_p == NULL) {
			warning("PALANIM_Load(): Allocation failure");
			return R_MEM;
		}

		PAnimData.entries[i].pal_index = (byte *)test_p;

		debug(2, "PALANIM_Load(): Entry %d: Loading %d SAGA_COLOR structures.", i, color_count);

		test_p = calloc(1, sizeof(R_COLOR) * color_count);
		if (test_p == NULL) {
			warning("PALANIM_Load(): Allocation failure");
			return R_MEM;
		}

		PAnimData.entries[i].colors = (R_COLOR *)test_p;

		for (p = 0; p < pal_count; p++) {
			PAnimData.entries[i].pal_index[p] = readS.readByte();
		}

		for (c = 0; c < color_count; c++) {
			PAnimData.entries[i].colors[c].red = readS.readByte();
			PAnimData.entries[i].colors[c].green = readS.readByte();
			PAnimData.entries[i].colors[c].blue = readS.readByte();
		}
	}

	PAnimData.loaded = 1;
	return R_SUCCESS;
}

int PALANIM_CycleStart() {
	R_EVENT event;

	if (!PAnimData.loaded) {
		return R_FAILURE;
	}

	event.type = R_ONESHOT_EVENT;
	event.code = R_PALANIM_EVENT;
	event.op = EVENT_CYCLESTEP;
	event.time = PALANIM_CYCLETIME;

	EVENT_Queue(&event);

	return R_SUCCESS;
}

int PALANIM_CycleStep(int vectortime) {
	R_SURFACE *back_buf;

	static PALENTRY pal[256];
	uint16 pal_index;
	uint16 col_index;

	uint16 i, j;
	uint16 cycle;
	uint16 cycle_limit;

	R_EVENT event;

	if (!PAnimData.loaded) {
		return R_FAILURE;
	}

	_vm->_gfx->getCurrentPal(pal);
	back_buf = _vm->_gfx->getBackBuffer();

	for (i = 0; i < PAnimData.entry_count; i++) {
		cycle = PAnimData.entries[i].cycle;
		cycle_limit = PAnimData.entries[i].color_count;
		for (j = 0; j < PAnimData.entries[i].pal_count; j++) {
			pal_index = (unsigned char)PAnimData.entries[i].pal_index[j];
			col_index = (cycle + j) % cycle_limit;
			pal[pal_index].red = (byte) PAnimData.entries[i].colors[col_index].red;
			pal[pal_index].green = (byte) PAnimData.entries[i].colors[col_index].green;
			pal[pal_index].blue = (byte) PAnimData.entries[i].colors[col_index].blue;
		}

		PAnimData.entries[i].cycle++;

		if (PAnimData.entries[i].cycle == cycle_limit) {
			PAnimData.entries[i].cycle = 0;
		}
	}

	_vm->_gfx->setPalette(back_buf, pal);

	event.type = R_ONESHOT_EVENT;
	event.code = R_PALANIM_EVENT;
	event.op = EVENT_CYCLESTEP;
	event.time = vectortime + PALANIM_CYCLETIME;

	EVENT_Queue(&event);

	return R_SUCCESS;
}

int PALANIM_Free() {
	uint16 i;

	if (!PAnimData.loaded) {
		return R_FAILURE;
	}

	for (i = 0; i < PAnimData.entry_count; i++) {
		debug(2, "PALANIM_Free(): Entry %d: Freeing colors.", i);
		free(PAnimData.entries[i].colors);
		debug(2, "PALANIM_Free(): Entry %d: Freeing indices.", i);
		free(PAnimData.entries[i].pal_index);
	}

	debug(0, "PALANIM_Free(): Freeing entries.");

	free(PAnimData.entries);

	PAnimData.loaded = 0;

	return R_SUCCESS;
}

} // End of namespace Saga
