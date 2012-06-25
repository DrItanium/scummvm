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
 */

/*
 * This file is based on WME Lite.
 * http://dead-code.org/redir.php?target=wmelite
 * Copyright (c) 2011 Jan Nedoma
 */

#ifndef WINTERMUTE_BDYNBUFFER_H
#define WINTERMUTE_BDYNBUFFER_H


#include "engines/wintermute/Base/BBase.h"

namespace WinterMute {

class CBDynBuffer : public CBBase {
public:
	bool _initialized;
	void PutText(LPCSTR fmt, ...);
	void PutTextIndent(int Indent, LPCSTR fmt, ...);
	uint32 GetDWORD();
	void PutDWORD(uint32 Val);
	char *GetString();
	void PutString(const char *Val);
	HRESULT GetBytes(byte  *Buffer, uint32 Size);
	HRESULT PutBytes(byte  *Buffer, uint32 Size);
	uint32 GetSize();
	HRESULT Init(uint32 InitSize = 0);
	void cleanup();
	uint32 _size;
	byte *_buffer;
	CBDynBuffer(CBGame *inGame, uint32 InitSize = 1000, uint32 GrowBy = 1000);
	virtual ~CBDynBuffer();

private:
	uint32 _realSize;
	uint32 _growBy;
	uint32 _initSize;
	uint32 _offset;
	void PutTextForm(const char *format, va_list argptr);
};

} // end of namespace WinterMute

#endif
