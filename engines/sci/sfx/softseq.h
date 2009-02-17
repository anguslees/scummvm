/* ScummVM - Graphic Adventure Engine
 *
 * ScummVM is the legal property of its developers, whose names
 * are too numerous to list here. Please refer to the COPYRIGHT
 * file distributed with this source distribution.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 * $URL$
 * $Id$
 *
 */

#ifndef SFX_SOFTSEQ_H_
#define SFX_SOFTSEQ_H_

#include "sci/include/sfx_core.h"
#include "sci/include/sfx_pcm.h"
#include "sci/sfx/sequencer.h"
#include "sci/include/resource.h"


/* Software sequencer */
typedef struct sfx_softseq {
	const char *name;
	const char *version;

	int
	(*set_option)(struct sfx_softseq *self, const char *name, const char *value);
	/* Sets an option for the sequencer
	** Parameters: (sfx_softseq_t *) self: Self reference
	**             (const char *) name: Name of the option to set
	**             (const char *) value: Value to set the option to
	** Returns   : (int) GFX_OK on success, or GFX_ERROR if not supported
	*/

	int
	(*init)(struct sfx_softseq *self, byte *res_data, int res_size,
	        byte *res2_data, int res2_size);
	/* Initialises the sequencer
	** Parameters: (sfx_softseq_t *) self: Self reference
	**             (byte *) res_data: Resource data for 'patch_nr' (see below)
	**             (int) res_size: Number of bytes in 'res_data'
	**             (byte *) res2_data: Resource data for 'patch2_nr' (see below)
	**             (int) res2_size: Number of bytes in 'res2_data'
	** Returns   : (int) SFX_OK on success, SFX_ERROR otherwise
	** Note that 'res_data' is only a valid pointer for this call. If the
	** data is needed later during execution, it should be backed up internally.
	** If the requested resource is not available, res_data will be NULL
	** /even if/ patch_nr is set.
	*/

	void
	(*exit)(struct sfx_softseq *self);
	/* Uninitialises the sequencer and frees all used resources
	** Parameters: (sfx_softseq_t *) self: Self reference
	*/

	void
	(*set_volume)(struct sfx_softseq *self, int new_volume);
	/* Sets the sequencer volume
	** Parameters: (sfx_softseq_t *) self: Self reference
	**             (int) new_volume: A volume, between 0 (quiet) and 127 (max)
	*/

	void
	(*handle_command)(struct sfx_softseq *self, byte cmd, int argc, byte *argv);
	/* Handle a MIDI command
	** Parameters: (sfx_softseq_t *) self: Self reference
	**             (byte) cmd: Basic MIDI command, always includes command and channel
	**             (int) argc: Number of additional arguments to this command
	**             (byte *) argv: Additional arguments to 'cmd'
	*/

	void
	(*poll)(struct sfx_softseq *self, byte *dest, int len);
	/* Asks the software sequencer to fill in parts of a buffer
	** Parameters: (sfx_softseq_t *) self: Self reference
	**             (int) len: Number of _frames_ to write
	** Returns   : (byte) *dest: 'len' frames must be written to this buffer
	*/

	void
	(*allstop)(struct sfx_softseq *self);
	/* Stops all sound generation
	** Parameters: (sfx_softseq_t *) self: Self reference
	*/

	void *internal; /* Internal data, may be used by sfx_softseq_t inmplementors */

	int patch_nr; /* Number of the first patch file associated with this sequencer,
		      ** or SFX_SEQ_PATCHFILE_NONE  */
	int patch2_nr; /* Number of the second patch file associated with this sequencer,
		      ** or SFX_SEQ_PATCHFILE_NONE  */
	int playmask; /* playflag identifying the device emulated */
	/* 0x01	-- MT-32
	** 0x02	-- Yamaha FB-01
	** 0x04	-- CMS or Game Blaster
	** 0x08	-- Casio MT540 or CT460
	** 0x10	-- Tandy 3-voice
	** 0x20 -- PC speaker
	*/
	int play_rhythm; /* Whether the rhythm channel (9) should be played */
	int polyphony; /* Number of voices played */

	sfx_pcm_config_t pcm_conf; /* Setup of the channel the sequencer writes to */

} sfx_softseq_t;


sfx_softseq_t *
sfx_find_softseq(const char *name);
/* Finds a given or default software sequencer
** Parameters: (const char *) name: Name of the sequencer to look up, or NULL for default
** Returns   : (sfx_softseq_t *) The requested sequencer, or NULL if not found
*/

#endif /* !defined(SFX_SOFTSEQ_H_) */
