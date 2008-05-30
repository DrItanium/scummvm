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

#ifndef DRASCULA_H
#define DRASCULA_H

#include "common/scummsys.h"
#include "common/endian.h"
#include "common/util.h"
#include "common/file.h"
#include "common/savefile.h"
#include "common/system.h"
#include "common/hash-str.h"
#include "common/events.h"
#include "common/keyboard.h"

#include "sound/audiostream.h"
#include "sound/mixer.h"
#include "sound/voc.h"
#include "sound/audiocd.h"

#include "engines/engine.h"

namespace Drascula {

enum DrasculaGameFeatures {
};

enum Languages {
	kEnglish = 0,
	kSpanish = 1,
	kGerman = 2,
	kFrench = 3,
	kItalian = 4
};

enum Verbs {
	kVerbDefault = -1,
	kVerbLook = 1,
	kVerbPick = 2,
	kVerbOpen = 3,
	kVerbClose = 4,
	kVerbTalk = 5,
	kVerbMove = 6
};

enum Colors {
	kColorBrown = 1,
	kColorDarkBlue = 2,
	kColorLightGreen = 3,
	kColorDarkGreen = 4,
	kColorYellow = 5,
	kColorOrange = 6,
	kColorRed = 7,
	kColorMaroon = 8,
	kColorPurple = 9,
	kColorWhite = 10,
	kColorPink = 11
};

#define TEXTD_START 68

struct DrasculaGameDescription;
struct RoomTalkAction;

#define NUM_SAVES     10
#define NUM_FLAGS     50
#define ESC          0x01
#define F1           0x3B
#define F2           0x3C
#define F3           0x3D
#define F4           0x3E
#define F5           0x3F
#define F6           0x40
#define F8           0x42
#define F9           0x43
#define F10          0x44
#define DIF_MASK       55
#define OBJWIDTH        40
#define OBJHEIGHT         25

#define DIF_MASK_HARE   72
#define DIF_MASK_ABC    22
#define CHAR_WIDTH     8
#define CHAR_HEIGHT      6

#define Y_ABC            158
#define Y_ABC_ESP        149
#define Y_SIGNOS         169
#define Y_SIGNOS_ESP     160
#define Y_ACENTOS        180

// Normal font, each letter has a space of 9 from the next
// Spanish has a special character, defined as "X_GN"
// after N, so these are split from A-N and O-Z
#define X_A              6
// (...)
#define X_N             123
#define X_GN            132
#define X_O             141
// (...)
#define X_Z             240
#define X_DOT             6
#define X_COMA           15
#define X_HYPHEN          24
#define X_CIERRA_INTERROGACION        33
#define X_ABRE_INTERROGACION          42
#define X_COMILLAS       51
#define X_CIERRA_EXCLAMACION         60
#define X_ABRE_EXCLAMACION              69
#define X_pointY_COMA              78
#define X_GREATER_THAN              87
#define X_LESSER_THAN              96
#define X_DOLAR             105
#define X_PERCENT             114
#define X_DOS_PUNTOS             123
#define X_AND            132
#define X_BARRA             141
#define X_BRACKET_OPEN             150
#define X_BRACKET_CLOSE             159
#define X_ASTERISCO             168
#define X_PLUS             177
// Normal font, each number has a space of 9 from the next
#define X_N1            186
// (...)
#define X_N0            267
#define SPACE           250
#define ALTO_TALK_HARE  25
#define ANCHO_TALK_HARE 23
#define PASO_HARE_X       8
#define PASO_HARE_Y       3
#define CHARACTER_HEIGHT   70
#define CHARACTER_WIDTH  43
#define PIES_HARE        12

#define CHAR_WIDTH_OPC     6
#define CHAR_HEIGHT_OPC      5
#define Y_ABC_OPC_1          6
#define Y_SIGNOS_OPC_1       15
#define Y_ABC_OPC_2          31
#define Y_SIGNOS_OPC_2       40
#define Y_ABC_OPC_3          56
#define Y_SIGNOS_OPC_3       65
// Dialog font, each letter has a space of 7 from the next
// Spanish has a special character, defined as "X_GN_OPC"
// after N, so these are split from A-N and O-Z
#define X_A_OPC              10
// (...)
#define X_N_OPC             101
#define X_GN_OPC            108
#define X_O_OPC             115
// (...)
#define X_Z_OPC             192
#define SPACE_OPC           199
#define X_DOT_OPC            10
#define X_COMA_OPC           17
#define X_HYPHEN_OPC          24
#define X_CIERRA_INTERROGACION_OPC        31
#define X_ABRE_INTERROGACION_OPC          38
#define X_COMILLAS_OPC       45
#define X_CIERRA_EXCLAMACION_OPC         52
#define X_ABRE_EXCLAMACION_OPC              59
#define X_pointY_COMA_OPC              66
#define X_GREATER_THAN_OPC              73
#define X_LESSER_THAN_OPC              80
#define X_DOLAR_OPC             87
#define X_PERCENT_OPC            94
#define X_DOS_PUNTOS_OPC             101
#define X_AND_OPC            108
#define X_BARRA_OPC             115
#define X_BRACKET_OPEN_OPC             122
#define X_BRACKET_CLOSE_OPC             129
#define X_ASTERISCO_OPC             136
#define X_PLUS_OPC             143
// Dialog font, each number has a space of 7 from the next
#define X_N1_OPC             150
// (...)
#define X_N0_OPC            213
#define NO_DOOR              99

#define INIT_FRAME  0
#define CMP_RLE     1
#define CMP_OFF     2
#define END_ANIM    3
#define SET_PAL     4
#define MOUSE_KEY   5
#define EMPTY_FRAME 6

#define COMPLETE_PAL   256
#define HALF_PAL       128

class DrasculaEngine : public ::Engine {
	int _gameId;
	Common::KeyState _keyPressed;

protected:
	int init();
	int go();
//	void shutdown();

public:
	DrasculaEngine(OSystem *syst, const DrasculaGameDescription *gameDesc);
	virtual ~DrasculaEngine();
	int getGameId() {
		return _gameId;
	}

	Common::RandomSource *_rnd;
	const DrasculaGameDescription *_gameDescription;
	uint32 getGameID() const;
	uint32 getFeatures() const;
	uint16 getVersion() const;
	Common::Platform getPlatform() const;
	Common::Language getLanguage() const;
	void updateEvents();

	Audio::SoundHandle _soundHandle;

	void allocMemory();
	void freeMemory();
	void releaseGame();

	void loadPic(const char *);
	void decompressPic(byte *dir_escritura, int plt);

	typedef char DacPalette256[256][3];

	void setRGB(byte *dir_lectura, int plt);
	void paleta_hare();
	void updatePalette();
	void setPalette(byte *PalBuf);
	void copyBackground(int xorg, int yorg, int xdes, int ydes, int width,
				int height, byte *src, byte *dest);
	void copyRect(int xorg, int yorg, int xdes, int ydes, int width,
				int height, byte *src, byte *dest);
	void copyRectClip(int *Array, byte *src, byte *dest);
	void updateScreen(int xorg, int yorg, int xdes, int ydes, int width, int height, byte *buffer);

	DacPalette256 gamePalette;
	DacPalette256 palHare;
	DacPalette256 palHareClaro;
	DacPalette256 palHareOscuro;

	byte *VGA;

	byte *drawSurface1;
	byte *backSurface;
	byte *drawSurface3;
	byte *drawSurface2;
	byte *tableSurface;
	byte *extraSurface;	// not sure about this one, was "dir_hare_dch"
	byte *screenSurface;
	byte *frontSurface;
	byte *textSurface;
	byte *pendulumSurface;

	byte cPal[768];
	byte *pcxBuffer;

	Common::File *ald, *sku;

	int hay_sb;
	int nivel_osc, previousMusic, roomMusic;
	int roomNumber;
	char roomDisk[20];
	char currentData[20];
	int numRoomObjs;
	char menuBackground[20];

	char objName[30][20];
	char iconName[44][13];

	int objectNum[40], visible[40], isDoor[40];
	int sitiobj_x[40], sitiobj_y[40], sentidobj[40];
	int inventoryObjects[43];
	char targetScreen[40][20];
	int x_alakeva[40], y_alakeva[40], sentido_alkeva[40], alapuertakeva[40];
	int x1[40], y1[40], x2[40], y2[40];
	int takeObject, pickedObject;
	int withVoices;
	int menuBar, menuScreen, hasName;
	char textName[20];
	int frame_blind;
	int frame_snore;
	int frame_bat;
	int c_mirar;
	int c_poder;

	int flags[NUM_FLAGS];

	int frame_y;
	int hare_x, hare_y, characterMoved, direccion_hare, sentido_hare, num_frame, hare_se_ve;
	int sitio_x, sitio_y, checkFlags;
	int doBreak;
	int stepX, stepY;
	int alto_hare, ancho_hare, feetHeight;
	int alto_talk, ancho_talk;
	int suelo_x1, suelo_y1, suelo_x2, suelo_y2;
	int near, far;
	int sentido_final, walkToObject;
	int objExit;
	int diff_vez, conta_vez;
	int hasAnswer;
	int conta_blind_vez;
	int changeColor;
	int rompo_y_salgo;
	int vb_x, sentido_vb, vb_se_mueve, frame_vb;
	float newHeight, newWidth;
	int factor_red[202];
	int frame_piano;
	int frame_drunk;
	int frame_velas;
	int color_solo;
	int blinking;
	int x_igor, y_igor, sentido_igor;
	int x_dr, y_dr, sentido_dr;
	int x_bj, y_bj, sentido_bj;
	int cont_sv;
	int term_int;
	int num_ejec;
	int hay_que_load;
	char saveName[13];
	int _color;
	int musicStopped;
	char select[23];
	int hay_seleccion;
	int mouseX;
	int mouseY;
	int mouseY_ant;
	int button_izq;
	int button_dch;

	bool escoba();
	void black();
	void talk_vb(int);
	void talk_vb(const char *, const char *);
	void talk_vbpuerta(int);
	void talk_vbpuerta(const char *said, const char *filename);
	void talk_blind(const char *, const char *, const char *);
	void talk_hacker(const char *, const char *);
	void pickObject(int);
	void anda_parriba();
	void anda_pabajo();
	void pon_vb();
	void lleva_vb(int pointX);
	void hipo_sin_nadie(int counter);
	void openDoor(int nflag, int doorNum);
	void showMap();
	void animation_1_1();
	void animation_2_1();
	void animation_1_2();
	void animation_2_2();
	void animation_3_1();
	void animation_4_1();
	void animation_3_2();
	void animation_4_2();
	void animation_5_2();
	void animation_6_2();
	void animation_7_2();
	void animation_8_2();
	void animation_9_2();
	void animation_10_2();
	void animation_11_2();
	void animation_12_2();
	void animation_13_2();
	void animation_14_2();
	void animation_15_2();
	void animation_16_2();
	void animation_17_2();
	void animation_18_2();
	void animation_19_2();
	void animation_20_2();
	void animation_21_2();
	void animation_22_2();
	void animation_23_2();
	void animation_23_anexo();
	void animation_23_anexo2();
	void animation_24_2();
	void animation_25_2();
	void animation_26_2();
	void animation_27_2();
	void animation_28_2();
	void animation_29_2();
	void animation_30_2();
	void animation_31_2();
	void animation_32_2();
	void animation_33_2();
	void animation_34_2();
	void animation_35_2();
	void animation_36_2();

	void update_1_pre();
	void update_2();
	void update_3();
	void update_3_pre();
	void update_4();
	void update_5();
	void update_5_pre();
	void update_6_pre();
	void update_7_pre();
	void update_9_pre();
	void update_12_pre();
	void update_14_pre();
	void update_15();
	void update_16_pre();
	void update_17_pre();
	void update_17();
	void update_18_pre();
	void update_18();
	void update_21_pre();
	void update_22_pre();
	void update_23_pre();
	void update_24_pre();
	void update_26_pre();
	void update_26();
	void update_27();
	void update_27_pre();
	void update_29();
	void update_29_pre();
	void update_30_pre();
	void update_31_pre();
	void update_34_pre();
	void update_35_pre();
	void update_31();
	void update_34();
	void update_35();
	void hare_oscuro();


	void withoutVerb();
	bool para_cargar(char[]);
	void carga_escoba(const char *);
	void clearRoom();
	void lleva_al_hare(int, int);
	void moveCursor();
	void checkObjects();
	void elige_en_barra();
	bool comprueba1();
	bool comprueba2();
	Common::KeyCode getScan();
	void selectVerb(int);
	void mesa();
	bool saves();
	void print_abc(const char *, int, int);
	void delay(int ms);
	bool confirma_salir();
	void screenSaver();
	void chooseObject(int objeto);
	void addObject(int);
	int removeObject(int osj);
	void fliplay(const char *filefli, int vel);
	void fadeFromBlack(int VelocidadDeFundido);
	char LimitaVGA(char value);
	void color_abc(int cl);
	void centerText(const char *,int,int);
	void playSound(const char *);
	bool animate(const char *animation, int FPS);
	void stopSound_corte();
	void fadeToBlack(int VelocidadDeFundido);
	void pause(int);
	void talk_dr_grande(const char *said, const char *filename);
	void placeIgor();
	void placeBJ();
	void placeDrascula();

	void talkInit(const char *filename);
	void talk_igor_dch(int);
	void talk_igor_dch(const char *said, const char *filename);
	void talk_dr_dch(int);
	void talk_dr_dch(const char *said, const char *filename);
	void talk_dr_izq(int);
	void talk_dr_izq(const char *said, const char *filename);
	void talk_solo(const char *, const char *);
	void talk_igor_front(const char *, const char *);
	void talk_tabernero(int);
	void talk_tabernero(const char *said, const char *filename);
	void talk_igor_door(const char *said, const char *filename);
	void talk_igor_wig(const char *said, const char *filename);
	void talk_pen(const char *, const char *);
	void talk_pen2(const char *, const char *);
	void talk_taber2(int);
	void talk_taber2(const char *, const char *);
	void talk_bj_bed(int);
	void talk_bj_bed(const char *said, const char * filename);
	void talk_htel(const char *said, const char *filename);
	void talk_bj(int);
	void talk_bj(const char *, const char *);
	void talk_baul(const char *said, const char *filename);
	void talk(int);
	void talk(const char *, const char *);
	void talk_sinc(const char *, const char *, const char *);
	void talk_drunk(const char *said, const char *filename);
	void talk_pianista(const char *said, const char *filename);
	void talk_igor_seated(const char *, const char *);
	void talk_wolf(const char *said, const char *filename);
	void talk_mus(const char *said, const char *filename);

	void hiccup(int);
	void stopSound();
	void closeDoor(int nflag, int doorNum);
	void playMusic(int p);
	void stopMusic();
	int musicStatus();
	void updateRoom();
	bool loadGame(const char *);
	void updateDoor(int);
	void animastopSound_corte();
	void color_hare();
	void funde_hare(int oscuridad);
	void paleta_hare_claro();
	void paleta_hare_oscuro();
	void hare_claro();
	void updateData();
	void startWalking();
	void updateRefresh();
	void updateRefresh_pre();
	void pon_hare();
	void showMenu();
	void clearMenu();
	void removeObject();
	bool exitRoom(int);
	bool pickupObject();
	bool checkFlag(int);
	void setCursorTable();
	void enterName();
	void para_grabar(char[]);
	int LookForFree();
	void openSSN(const char *Name, int Pause);
	void WaitFrameSSN();
	void MixVideo(byte *OldScreen, byte *NewScreen);
	void Des_RLE(byte *BufferRLE, byte *MiVideoRLE);
	void Des_OFF(byte *BufferOFF, byte *MiVideoOFF, int Lenght);
	void set_dacSSN(byte *dacSSN);
	byte *TryInMem(Common::File *Session);
	void EndSSN();
	int playFrameSSN();

	byte *AuxBuffOrg;
	byte *AuxBuffLast;
	byte *AuxBuffDes;

	byte *pointer;
	int UsingMem;
	Common::File *_Session;
	byte CHUNK;
	byte CMP, dacSSN[768];
	byte *MiVideoSSN;
	byte *mSession;
	int FrameSSN;
	int globalSpeed;
	uint32 LastFrame;

	int frame_pen;
	int flag_tv;

	byte *loadPCX(byte *NamePcc);
	void set_dac(byte *dac);
	void WaitForNext(int FPS);
	int getTime();
	void reduce_hare_chico(int, int, int, int, int, int, int, byte *, byte *);
	void quadrant_1();
	void quadrant_2();
	void quadrant_3();
	void quadrant_4();
	void update_62();
	void update_62_pre();
	void update_63();
	void saveGame(char[]);
	void increaseFrameNum();
	int whichObject();
	bool checkMenuFlags();
	bool roomParse(RoomTalkAction*, int);
	void room_0();
	void room_1(int);
	void room_2(int);
	void room_3(int);
	void room_4(int);
	void room_5(int);
	void room_6(int);
	void room_7(int);
	void room_8(int);
	void room_9(int);
	void room_12(int);
	void room_14(int);
	void room_15(int);
	void room_16(int);
	void room_17(int);
	void room_18(int);
	void room_19(int);
	bool room_21(int);
	void room_22(int);
	void room_23(int);
	void room_24(int);
	void room_26(int);
	void room_27(int);
	void room_29(int);
	void room_30(int);
	void room_31(int);
	void room_34(int);
	void room_35(int);
	void room_44(int);
	void room_62(int);
	void room_63(int);
	void converse(const char *);
	void print_abc_opc(const char *, int, int, int);
	void response(int);

	void MusicFadeout();
	void ctvd_end();
	void ctvd_stop();
	void ctvd_terminate();
	void ctvd_speaker(int flag);
	void ctvd_output(Common::File *file_handle);
	void ctvd_init(int b);
	void grr();
	bool room_13(int fl);
	void update_13();
	void update_20();
	void updateAnim(int y, int destX, int destY, int width, int height, int count, byte* src, int delay = 3);
	void updateAnim2(int y, int px, int py, int width, int height, int count, byte* src);
	void animation_1_3();
	void animation_2_3();
	void animation_3_3();
	void animation_4_3();
	void animation_5_3();
	void animation_6_3();
	void animation_rayo();
	void animation_1_4();
	void animation_2_4();
	void animation_3_4();
	void animation_4_4();
	void animation_5_4();
	void animation_6_4();
	void animation_7_4();
	void animation_8_4();
	void animation_9_4();
	void animation_1_5();
	void animation_2_5();
	void animation_3_5();
	void animation_4_5();
	void animation_5_5();
	void animation_6_5();
	void animation_7_5();
	void animation_8_5();
	void animation_9_5();
	void animation_10_5();
	void animation_11_5();
	void animation_12_5();
	void animation_13_5();
	void animation_14_5();
	void animation_15_5();
	void animation_16_5();
	void animation_17_5();
	void room_49(int);
	void room_53(int);
	void room_54(int);
	void room_55(int);
	bool room_56(int);
	void update_53_pre();
	void update_54_pre();
	void update_49_pre();
	void update_56_pre();
	void update_50();
	void update_57();
	void room_58(int);
	void room_59(int);
	bool room_60(int);
	void room_61(int);
	void room_pendulum(int);
	void update_pendulum();
	void update_58();
	void update_58_pre();
	void update_59_pre();
	void update_60_pre();
	void update_60();
	void update_61();
	void animation_1_6();
	void animation_2_6();
	void animation_3_6();
	void animation_4_6();
	void animation_5_6();
	void animation_6_6();
	void animation_7_6();
	void animation_9_6();
	void animation_10_6();
	void animation_11_6();
	void animation_12_6();
	void animation_13_6();
	void animation_14_6();
	void animation_15_6();
	void animation_18_6();
	void animation_19_6();
	void activatePendulum();

private:
	int _lang;
};

extern const char *_text[][501];
extern const char *_textd[][84];
extern const char *_textb[][15];
extern const char *_textbj[][29];
extern const char *_texte[][24];
extern const char *_texti[][33];
extern const char *_textl[][32];
extern const char *_textp[][20];
extern const char *_textt[][25];
extern const char *_textvb[][63];
extern const char *_textsys[][4];
extern const char *_texthis[][5];
extern const char *_textverbs[][6];
extern const char *_textmisc[][2];
extern const char *_textd1[][11];

} // End of namespace Drascula

#endif /* DRASCULA_H */
