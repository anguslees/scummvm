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

#ifndef AGI_SOUND_PCJR_H
#define AGI_SOUND_PCJR_H

namespace Agi {

#define CHAN_MAX 4

#define SAMPLE_RATE 22050

enum GenType {
	kGenSilence,
	kGenTone,
	kGenPeriod,
	kGenWhite
};

struct SndGenChan {
	const byte *data;
	uint16 duration;
	uint16 avail;	// turned on (1)  but when the channel's data runs out, it's set to (0)
	uint16 dissolveCount;
	byte attenuation;
	byte attenuationCopy;
	
	GenType genType;
	
	// for the sample mixer
	int freqCount;
};

struct ToneChan {
	int avail;

	int noteCount; // length of tone.. duration

	int freqCount;
	int freqCountPrev;
	int atten;  // volume

	GenType genType;
	int genTypePrev;

	int count;
	int scale;
	int sign;
	unsigned int noiseState;		/* noise generator      */
	int feedback;						/* noise feedback mask */
};

struct Tone {
	int freqCount;
	int atten;
	GenType type;
};

class SoundGenPCJr : public SoundGen {
public:
	SoundGenPCJr(AgiEngine *vm);
	~SoundGenPCJr();

	void play(int resnum, int flag);
	void stop(void);

	void premixerCall(int16 *stream, int len);

private:
	int getNextNote(int ch, Tone *tone);
	int volumeCalc(SndGenChan *chan);

	int chanGen(int chan, int16 *stream, int len);

	int fillNoise(ToneChan *t, int16 *buf, int len);
	int fillSquare(ToneChan *t, int16 *buf, int len);

private:
	AgiEngine *_vm;
	SndGenChan _channel[CHAN_MAX];
	ToneChan _tchannel[CHAN_MAX];
	int16 *_chanData;
	int _chanAllocated;

	int _dissolveMethod;
};

} // End of namespace Agi

#endif /* AGI_SOUND_PCJR_H */