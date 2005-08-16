/* ScummVM - Scumm Interpreter
 * Copyright (C) 2005 The ScummVM project
 * Copyright (C) 2005 Joost Peters PSP Backend
 * Copyright (C) 2005 Thomas Mayer PSP Backend
 * Copyright (C) 2005 Paolo Costabel PSP Backend
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
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 * $Header$
 *
 */

#include "common/stdafx.h"
#include "common/scummsys.h"

#include "common/rect.h"
#include "backends/psp/osys_psp.h"

#include "common/config-manager.h"

#include <pspgu.h>

#include "./trace.h"

#define	SCREEN_WIDTH	480
#define	SCREEN_HEIGHT	272

extern bool g_quit;

unsigned short *DrawBuffer = (unsigned short *)0x44044000;
unsigned short *DisplayBuffer = (unsigned short *)0x44000000;

unsigned long RGBToColour(unsigned long r, unsigned long g, unsigned long b) {
	return (((b >> 3) << 10) | ((g >> 3) << 5) | ((r >> 3) << 0)) | 0x8000;
}

void putPixel(uint16 x, uint16 y, unsigned long colour) {
	*(unsigned short *)(DrawBuffer + (y << 9) + x ) = colour;
}

const OSystem::GraphicsMode OSystem_PSP::s_supportedGraphicsModes[] = {
	{ "320x200 (centered)", "320x200 16-bit centered", CENTERED_320X200 },
	{ "353x272 (best-fit, centered)", "353x272 16-bit centered", CENTERED_435X272 },
	{ "480x272 (full screen)", "480x272 16-bit stretched", STRETCHED_480X272 },
	{ "362x272 (4:3, centered)", "362x272 16-bit centered", CENTERED_362X272 },
	{0, 0, 0}
};


OSystem_PSP::OSystem_PSP() : _screenWidth(0), _screenHeight(0), _offscreen(0), _overlayBuffer(0), _overlayVisible(false), _shakePos(0), _mouseBuf(0), _prevButtons(0), _lastPadCheck(0), _padAccel(0) {

	memset(_palette, 0, sizeof(_palette));

	_samplesPerSec = 0;

	//init SDL
	uint32	sdlFlags = SDL_INIT_AUDIO | SDL_INIT_TIMER;
	SDL_Init(sdlFlags);

	sceDisplaySetMode(0, SCREEN_WIDTH, SCREEN_HEIGHT);
	sceDisplaySetFrameBuf((char *)DisplayBuffer, 512, 1, 1);
	sceDisplayWaitVblankStart();
}

OSystem_PSP::~OSystem_PSP() {
	if (_offscreen) free(_offscreen);
	if (_overlayBuffer) free(_overlayBuffer);
	if (_mouseBuf)	free(_mouseBuf);
}

bool OSystem_PSP::hasFeature(Feature f)
{
	return false;
}

void OSystem_PSP::setFeatureState(Feature f, bool enable)
{
}

bool OSystem_PSP::getFeatureState(Feature f)
{
	return false;
}

const OSystem::GraphicsMode* OSystem_PSP::getSupportedGraphicsModes() const
{
	return s_supportedGraphicsModes;
}


int OSystem_PSP::getDefaultGraphicsMode() const
{
	return -1;
}

bool OSystem_PSP::setGraphicsMode(int mode)
{
	return true;
}

bool OSystem_PSP::setGraphicsMode(const char *name)
{
	return true;
}

int OSystem_PSP::getGraphicsMode() const
{
	return -1;
}

void OSystem_PSP::initSize(uint width, uint height, int overlayScale) {
	_screenWidth = width;
	_screenHeight = height;
	_offscreen = (byte *)malloc(width * height);

	if(overlayScale == -1)
		overlayScale = 1;
	
	_overlayScale = overlayScale;
	_overlayBuffer = (OverlayColor *)malloc(width * overlayScale * height * overlayScale * sizeof(OverlayColor));
	bzero(_offscreen, width * height);
	bzero(_overlayBuffer, width * overlayScale * height * overlayScale);

	_mouseVisible = false;
}

int16 OSystem_PSP::getHeight() {
	return _screenHeight;
}

int16 OSystem_PSP::getWidth() {
	return _screenWidth;
}

void OSystem_PSP::setPalette(const byte *colors, uint start, uint num) {
	const byte *b = colors;

	for (uint i = 0; i < num; ++i) {
		_palette[start + i] = RGBToColour(b[0], b[1], b[2]);
		b += 4;
	}
}

void OSystem_PSP::copyRectToScreen(const byte *buf, int pitch, int x, int y, int w, int h) {
	//Clip the coordinates
	if (x < 0) {
		w += x;
		buf -= x;
		x = 0;
	}

	if (y < 0) {
		h += y;
		buf -= y * pitch;
		y = 0;
	}

	if (w > _screenWidth - x) {
		w = _screenWidth - x;
	}

	if (h > _screenHeight - y) {
		h = _screenHeight - y;
	}

	if (w <= 0 || h <= 0)
		return;
	
	
	byte *dst = _offscreen + y * _screenWidth + x;
	if (_screenWidth == pitch && pitch == w) {
		memcpy(dst, buf, h * w);
	} else {
		do {
			memcpy(dst, buf, w);
			buf += pitch;
			dst += _screenWidth;
		} while (--h);
	}
}

void OSystem_PSP::updateScreen() {
	unsigned short *temp;

	uint xStart = (SCREEN_WIDTH >> 1) - (_screenWidth >> 1);
	uint yStart = (SCREEN_HEIGHT >> 1) - (_screenHeight >> 1);
	
	for (int i = 0; i < _screenHeight; ++i) {
		for (int j = 0; j < _screenWidth; ++j) {
			putPixel(xStart + j, yStart + i, _palette[_offscreen[i * _screenWidth +j]]);
		}
	}

	if(_overlayVisible) {
		for (int i = 0; i < _screenHeight * _overlayScale; ++i) {
			for (int j = 0; j < _screenWidth * _overlayScale; ++j) {

				OverlayColor pixel = _overlayBuffer[(i * _screenWidth +j)];

				if(pixel & 0x8000)
					putPixel(xStart + j, yStart + i, pixel);
			}
		}
	}

	//draw mouse on top
	if (_mouseVisible) {
		for (int y = 0; y < _mouseHeight; ++y) {
			for (int x = 0; x < _mouseWidth; ++x) {
				if (_mouseBuf[y * _mouseHeight + x] != _mouseKeyColour) {
					int my = _mouseY + y; // + _mouseHotspotY;
					int mx = _mouseX + x; // + _mouseHotspotX;

					if (mx >= 0 && mx < _screenWidth && my >= 0 && my < _screenHeight)
						putPixel(xStart + mx, yStart + my, _palette[_mouseBuf[y * _mouseHeight + x]]);
				}
			}
		}
	}


	// switch buffers
	temp = DrawBuffer;
	DrawBuffer = DisplayBuffer;
	DisplayBuffer = temp;
	sceDisplayWaitVblankStart();
	sceDisplaySetFrameBuf((char *)DisplayBuffer, 512, 1, 1);

}

void OSystem_PSP::setShakePos(int shakeOffset) {
	_shakePos = shakeOffset;
}

void OSystem_PSP::showOverlay ()
{
	_overlayVisible = true;
}

void OSystem_PSP::hideOverlay ()
{
	PSPDebugTrace("OSystem_PSP::hideOverlay called\n");
	_overlayVisible = false;
}

void OSystem_PSP::clearOverlay ()
{
	PSPDebugTrace("clearOverlay\n");
	bzero(_overlayBuffer, _screenWidth * _overlayScale * _screenHeight * _overlayScale * sizeof(OverlayColor));
}

void OSystem_PSP::grabOverlay (OverlayColor *buf, int pitch)
{
}

void OSystem_PSP::copyRectToOverlay (const OverlayColor *buf, int pitch, int x, int y, int w, int h)
{
	PSPDebugTrace("copyRectToOverlay\n");
	
	//Clip the coordinates
	if (x < 0) {
		w += x;
		buf -= x;
		x = 0;
	}

	if (y < 0) {
		h += y;
		buf -= y * pitch;
		y = 0;
	}

	if (w > _screenWidth*_overlayScale - x) {
		w = _screenWidth*_overlayScale - x;
	}

	if (h > _screenHeight*_overlayScale - y) {
		h = _screenHeight*_overlayScale - y;
	}

	if (w <= 0 || h <= 0)
		return;

	
	OverlayColor *dst = _overlayBuffer +( y * _screenWidth * _overlayScale + x);
	if (_screenWidth == pitch && pitch == w) {
		memcpy(dst, buf, h * _overlayScale * w * _overlayScale * sizeof(OverlayColor));
	} else {
		do {
			memcpy(dst, buf, w * _overlayScale * sizeof(OverlayColor));
			buf += pitch;
			dst += _screenWidth * _overlayScale;
		} while (--h);
	}
}

int16 OSystem_PSP::getOverlayHeight()
{
	return getHeight() * _overlayScale;
}

int16 OSystem_PSP::getOverlayWidth()
{
	return getWidth() * _overlayScale;
}

OverlayColor OSystem_PSP::RGBToColor(uint8 r, uint8 g, uint8 b)
{
	return (((r >> 3) & 0x1F) | (((g >> 3) & 0x1F) << 5) | (((b >> 3) & 0x1F) << 10 ) ) | 0x8000;
}

void OSystem_PSP::colorToRGB(OverlayColor color, uint8 &r, uint8 &g, uint8 &b)
{
		r = ((color & 0x1F) << 3);
		g = (((color >> 5) & 0x1F) << 3);
		b = (((color >> 10) & 0x1F) << 3);
}
	
OverlayColor OSystem_PSP::ARGBToColor(uint8 a, uint8 r, uint8 g, uint8 b) 
{
	OverlayColor color = RGBToColor(r, g, b);

	if(a == 255)
		color |= 0x8000;

	return color;
}

void OSystem_PSP::colorToARGB(OverlayColor color, uint8 &a, uint8 &r, uint8 &g, uint8 &b) 
{
		colorToRGB(color, r, g, b);
		if(color & 0x8000)
			a = 255;
		else
			a = 0;
}

void OSystem_PSP::grabPalette(byte *colors, uint start, uint num)
{
	uint i;
	uint16 color;

	for(i=start; i < start+num; i++)
	{
		color = _palette[i];
		*colors++ = ((color & 0x1F) << 3);
		*colors++ = (((color >> 5) & 0x1F) << 3);
		*colors++ = (((color >> 10) & 0x1F) << 3);
		*colors++ = (color & 0x8000 ? 255 : 0); 
	}
}

bool OSystem_PSP::showMouse(bool visible) { 
	bool last = _mouseVisible;
	_mouseVisible = visible;
	return last;
}

void OSystem_PSP::warpMouse(int x, int y) {
	//assert(x > 0 && x < _screenWidth);
	//assert(y > 0 && y < _screenHeight);
	_mouseX = x;
	_mouseY = y;
}

void OSystem_PSP::setMouseCursor(const byte *buf, uint w, uint h, int hotspotX, int hotspotY, byte keycolor, int cursorTargetScale) {
	//TODO: handle cursorTargetScale
	_mouseWidth = w;
	_mouseHeight = h;

	_mouseHotspotX = hotspotX;
	_mouseHotspotY = hotspotY;

	_mouseKeyColour = keycolor;

	if (_mouseBuf)
		free(_mouseBuf);

	_mouseBuf = (byte *)malloc(w * h);
	memcpy(_mouseBuf, buf, w * h);
}

#define PAD_CHECK_TIME	40
#define PAD_DIR_MASK	(PSP_CTRL_UP | PSP_CTRL_DOWN | PSP_CTRL_LEFT | PSP_CTRL_RIGHT)

bool OSystem_PSP::pollEvent(Event &event) {
	s8 analogStepAmountX = 0;
	s8 analogStepAmountY = 0;
/*	
	SceCtrlData pad;

	sceCtrlSetSamplingCycle(0);
	sceCtrlSetSamplingMode(1);
	sceCtrlReadBufferPositive(&pad, 1);
*/
	uint32 buttonsChanged = pad.Buttons ^ _prevButtons;

	/* check if user exited using the Home button */
	if (g_quit) {
		g_quit = false; //set g_quit back to false or else pollEvent keeps looping..

		warning("g_quit signal caught, sending EVENT_QUIT to VM engine");
		event.type = OSystem::EVENT_QUIT;
		return true;
	}
	
	if (buttonsChanged & (PSP_CTRL_CROSS | PSP_CTRL_CIRCLE | PSP_CTRL_LTRIGGER | PSP_CTRL_RTRIGGER | PSP_CTRL_START | PSP_CTRL_SELECT | PSP_CTRL_SQUARE)) {
		if (buttonsChanged & PSP_CTRL_CROSS) {
			event.type = (pad.Buttons & PSP_CTRL_CROSS) ? OSystem::EVENT_LBUTTONDOWN : OSystem::EVENT_LBUTTONUP;
		}
		else if (buttonsChanged & PSP_CTRL_CIRCLE) {
			event.type = (pad.Buttons & PSP_CTRL_CIRCLE) ? OSystem::EVENT_RBUTTONDOWN : OSystem::EVENT_RBUTTONUP;
		}
		else {
			//any of the other buttons.
			event.type = buttonsChanged & pad.Buttons ? OSystem::EVENT_KEYDOWN : OSystem::EVENT_KEYUP;
			event.kbd.flags = 0;
			
			if (buttonsChanged & PSP_CTRL_LTRIGGER) {
				event.kbd.keycode = SDLK_ESCAPE;
				event.kbd.ascii = 27;
			} else if (buttonsChanged & PSP_CTRL_RTRIGGER) {
				event.kbd.keycode = SDLK_RETURN;
				event.kbd.ascii = 13;
			} else if (buttonsChanged & PSP_CTRL_START) {
				event.kbd.keycode = SDLK_F5;
				event.kbd.ascii = 319;
/*			} else if (buttonsChanged & PSP_CTRL_SELECT) {
				event.kbd.keycode = SDLK_0;
				event.kbd.ascii = '0';
*/			} else if (buttonsChanged & PSP_CTRL_SQUARE) {
				event.kbd.keycode = SDLK_PERIOD;
				event.kbd.ascii = '.';
			}
			
		}
			
		event.mouse.x = _mouseX;
		event.mouse.y = _mouseY;
		_prevButtons = pad.Buttons;
		return true;
	}
	
	uint32 time = getMillis();
	if (time - _lastPadCheck > PAD_CHECK_TIME) {
		_lastPadCheck = time;
		int16 newX = _mouseX;
		int16 newY = _mouseY;
	
		if (pad.Lx < 100) {
			analogStepAmountX = pad.Lx - 100;
		}
		else if (pad.Lx > 155) {
			analogStepAmountX = pad.Lx - 155;
		}
		
		if (pad.Ly < 100) {
			analogStepAmountY = pad.Ly - 100;
		}
		else if (pad.Ly > 155) {
			analogStepAmountY = pad.Ly - 155;
		}
	
		if (pad.Buttons & PAD_DIR_MASK ||
		    analogStepAmountX != 0 || analogStepAmountY != 0) {
			if (_prevButtons & PAD_DIR_MASK) {
				if (_padAccel < 16) _padAccel++;
			} else _padAccel = 0;

			_prevButtons = pad.Buttons;

			if (pad.Buttons & PSP_CTRL_LEFT)
				newX -= _padAccel >> 2;
			if (pad.Buttons & PSP_CTRL_RIGHT)
				newX += _padAccel >> 2;
			if (pad.Buttons & PSP_CTRL_UP)
				newY -= _padAccel >> 2;
			if (pad.Buttons & PSP_CTRL_DOWN)
				newY += _padAccel >> 2;

			// If no movement then this has no effect	
			if (pad.Buttons & PSP_CTRL_TRIANGLE) {
				// Fine control mode for analog
					if (analogStepAmountX != 0)
						if (analogStepAmountX > 0)
							newX += analogStepAmountX - (analogStepAmountX - 1);
						else
							newX -= -analogStepAmountX - (-analogStepAmountX - 1);
						
					if (analogStepAmountY != 0)
						if (analogStepAmountY > 0)
							newY += analogStepAmountY - (analogStepAmountY - 1);
						else
							newY -= -analogStepAmountY - (-analogStepAmountY - 1);
			}
			else {
				newX += analogStepAmountX >> ((_screenWidth == 640 || (_overlayVisible && _overlayScale > 1)) ? 2 : 3);
				newY += analogStepAmountY >> ((_screenWidth == 640 || (_overlayVisible && _overlayScale > 1)) ? 2 : 3);
			}

			if (newX < 0) newX = 0;
			if (newY < 0) newY = 0;
			if(_overlayVisible)
			{
				if (newX >= _screenWidth*_overlayScale) newX = _screenWidth*_overlayScale - 1;
				if (newY >= _screenHeight*_overlayScale) newY = _screenHeight*_overlayScale - 1;		
			}
			else
			{
				if (newX >= _screenWidth) newX = _screenWidth - 1;
				if (newY >= _screenHeight) newY = _screenHeight - 1;		
			}
			
			if ((_mouseX != newX) || (_mouseY != newY)) {
				event.type = OSystem::EVENT_MOUSEMOVE;
				event.mouse.x = _mouseX = newX;
				event.mouse.y = _mouseY = newY;
				return true;
			}
		} else { 
			//reset pad acceleration
			_padAccel = 0;
		}
	}

	return false;
}

uint32 OSystem_PSP::getMillis() {
	return SDL_GetTicks();
}

void OSystem_PSP::delayMillis(uint msecs) {	
	sceKernelDelayThread(1);        // Sleep thread for 1 millionth of a second to allow callback threads to run
	SDL_Delay(msecs);
}

void OSystem_PSP::setTimerCallback(TimerProc callback, int interval) {
	SDL_SetTimer(interval, (SDL_TimerCallback)callback);
}

OSystem::MutexRef OSystem_PSP::createMutex(void) {
	return (MutexRef)SDL_CreateMutex();
}

void OSystem_PSP::lockMutex(MutexRef mutex) {
	SDL_mutexP((SDL_mutex *)mutex);
}

void OSystem_PSP::unlockMutex(MutexRef mutex) {
	SDL_mutexV((SDL_mutex *)mutex);
}

void OSystem_PSP::deleteMutex(MutexRef mutex) {
	SDL_DestroyMutex((SDL_mutex *)mutex);
}

bool OSystem_PSP::setSoundCallback(SoundProc proc, void *param) {
	SDL_AudioSpec desired;
	SDL_AudioSpec obtained;

	memset(&desired, 0, sizeof(desired));

	if (ConfMan.hasKey("output_rate"))
		_samplesPerSec = ConfMan.getInt("output_rate");
	else
		_samplesPerSec = SAMPLES_PER_SEC;

	// Originally, we always used 2048 samples. This loop will produce the
	// same result at 22050 Hz, and should hopefully produce something
	// sensible for other frequencies. Note that it must be a power of two.

	uint16 samples = 0x8000;

	for (;;) {
		if (samples / (_samplesPerSec / 1000) < 100)
			break;
		samples >>= 1;
	}

	desired.freq = _samplesPerSec;
	desired.format = AUDIO_S16SYS;
	desired.channels = 2;
	desired.samples = samples;
	desired.callback = proc;
	desired.userdata = param;
	if (SDL_OpenAudio(&desired, &obtained) != 0) {
		return false;
	}
	// Note: This should be the obtained output rate, but it seems that at
	// least on some platforms SDL will lie and claim it did get the rate
	// even if it didn't. Probably only happens for "weird" rates, though.
	_samplesPerSec = obtained.freq;
	SDL_PauseAudio(0);
	return true;
}

void OSystem_PSP::clearSoundCallback() {
	SDL_CloseAudio();
}

int OSystem_PSP::getOutputSampleRate() const {
	return _samplesPerSec; 
}

bool OSystem_PSP::openCD(int drive) {
	return false;
}

bool OSystem_PSP::pollCD() {
	return false;
}

void OSystem_PSP::playCD(int track, int num_loops, int start_frame, int duration) {
}

void OSystem_PSP::stopCD() {
}

void OSystem_PSP::updateCD() {
}

void OSystem_PSP::quit() {
	clearSoundCallback();
	SDL_Quit();
	sceGuTerm();
	sceKernelExitGame();
}

void OSystem_PSP::setWindowCaption(const char *caption) {
}

void OSystem_PSP::displayMessageOnOSD(const char *msg) {
}


/* moved to psp-gu.cpp */
/*
OSystem *OSystem_PSP_create() {
	return new OSystem_PSP();
}
*/
// Correct pixel format ABBBBBGGGGGRRRRR
int gBitFormat = 1555;

