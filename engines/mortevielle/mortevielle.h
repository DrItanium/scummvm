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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 */

#ifndef MORTEVIELLE_H
#define MORTEVIELLE_H

#include "common/events.h"
#include "common/file.h"
#include "common/random.h"
#include "common/rect.h"
#include "common/stack.h"
#include "engines/advancedDetector.h"
#include "engines/engine.h"
#include "common/error.h"
#include "graphics/surface.h"
#include "mortevielle/graphics.h"
#include "mortevielle/menu.h"
#include "mortevielle/saveload.h"
#include "mortevielle/sound.h"

namespace Mortevielle {

// Debug channels
enum {
	kMortevielleCore = 1 << 0,
	kMortevielleGraphics = 1 << 1
};

// Game languages
enum {
	LANG_FRENCH = 0,
	LANG_ENGLISH = 1
};

// Static string list
enum {
	S_YES_NO = 0, S_GO_TO = 1, S_SOMEONE_ENTERS = 2, S_COOL = 3, S_LOURDE = 4, 
	S_MALSAINE = 5, S_IDEM = 6,	S_YOU = 7, S_ARE = 8, S_ALONE = 9,
	S_HEAR_NOISE = 10, S_SHOULD_HAVE_NOTICED = 11, S_NUMBER_OF_HINTS = 12,
	S_WANT_TO_WAKE_UP = 13, S_OK = 14, S_SAVE_LOAD = 15, S_RESTART = 18, S_F3 = 19, 
	S_F8 = 20, S_HIDE_SELF = 21, S_TAKE = 22, S_PROBE = 23, S_RAISE = 24, S_SUITE = 25, 
	S_STOP = 26, S_USE_DEP_MENU = 27, S_LIFT = 28, S_READ = 29, 
	S_LOOK = 30, S_SEARCH = 31, S_OPEN = 32, S_PUT = 33, S_TURN = 34, S_TIE = 35, S_CLOSE = 36,
	S_HIT = 37, S_POSE = 38, S_SMASH = 39,

	S_SMELL = 40, S_SCRATCH = 41, S_PROBE2 = 42, S_BEFORE_USE_DEP_MENU = 43, S_DAY = 44
};

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 400
#define SCREEN_ORIG_HEIGHT 200
#define MORT_DAT_REQUIRED_VERSION 1
#define MORT_DAT "mort.dat"
#define GAME_FRAME_DELAY (1000 / 50)

class MortevielleEngine : public Engine {
private:
	const ADGameDescription *_gameDescription;
	Common::Stack<int> _keypresses;
	uint32 _lastGameFrame;
	bool _mouseClick;
	Common::Point _mousePos;
	bool _inMainGameLoop;	// Flag when the main game loop is active
	Common::StringArray _staticStrings;

	Common::ErrorCode initialise();
	Common::ErrorCode loadMortDat();
	void readStaticStrings(Common::File &f, int dataSize);
	void loadFont(Common::File &f);
	bool handleEvents();
	void addKeypress(Common::Event &evt);
	void initMouse();
	void showIntroduction();
	void mainGame();
	void playGame();
	void handleAction();
	void divers(int np, bool b);
public:
	ScreenSurface _screenSurface;
	PaletteManager _paletteManager;
	GfxSurface _backgroundSurface;
	Common::RandomSource _randomSource;
	SoundManager _soundManager;
	SavegameManager _savegameManager;
	Menu _menu;

	bool _quitGame;			// Quit game flag. Originally called 'arret'
	bool _endGame;			// End game flag. Originally called 'solu'
	bool _loseGame;			// Lose game flag. Originally called 'perdu'
public:
	MortevielleEngine(OSystem *system, const ADGameDescription *gameDesc);
	~MortevielleEngine();
	virtual bool hasFeature(EngineFeature f) const;
	virtual bool canLoadGameStateCurrently();
	virtual bool canSaveGameStateCurrently();
	virtual Common::Error loadGameState(int slot);
	virtual Common::Error saveGameState(int slot, const Common::String &desc);
	virtual Common::Error run();
	uint32 getGameFlags() const;
	Common::Language getLanguage() const;

	bool keyPressed();
	int getChar();
	Common::Point getMousePos() const { return _mousePos; }
	void setMousePos(const Common::Point &pt);
	bool getMouseClick() const { return _mouseClick; }
	void setMouseClick(bool v) { _mouseClick = v; }
	void delay(int amount);
	Common::String getString(int idx) const { return _staticStrings[idx]; }

	void endGame();
	void loseGame();
	void gameLoaded();
};

extern MortevielleEngine *g_vm;

#define CHECK_QUIT if (g_vm->shouldQuit()) { return; }
#define CHECK_QUIT0 if (g_vm->shouldQuit()) { return 0; }

} // End of namespace Mortevielle

#endif
