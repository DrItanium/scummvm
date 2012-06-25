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

#include "engines/wintermute/dcgf.h"
#include "engines/wintermute/Ad/AdGame.h"
#include "engines/wintermute/Base/BScriptHolder.h"
#include "engines/wintermute/Base/BParser.h"
#include "engines/wintermute/Base/scriptables/ScValue.h"
#include "engines/wintermute/Base/scriptables/ScEngine.h"
#include "engines/wintermute/Base/scriptables/ScScript.h"
#include "engines/wintermute/Base/scriptables/ScStack.h"

namespace WinterMute {

IMPLEMENT_PERSISTENT(CBScriptHolder, false)

//////////////////////////////////////////////////////////////////////
CBScriptHolder::CBScriptHolder(CBGame *inGame): CBScriptable(inGame) {
	setName("<unnamed>");

	_freezable = true;
	_filename = NULL;
}


//////////////////////////////////////////////////////////////////////
CBScriptHolder::~CBScriptHolder() {
	cleanup();
}


//////////////////////////////////////////////////////////////////////////
HRESULT CBScriptHolder::cleanup() {
	delete[] _filename;
	_filename = NULL;

	int i;

	for (i = 0; i < _scripts.GetSize(); i++) {
		_scripts[i]->Finish(true);
		_scripts[i]->_owner = NULL;
	}
	_scripts.RemoveAll();

	return S_OK;
}

//////////////////////////////////////////////////////////////////////
void CBScriptHolder::setFilename(const char *Filename) {
	if (_filename != NULL) delete [] _filename;

	_filename = new char [strlen(Filename) + 1];
	if (_filename != NULL) strcpy(_filename, Filename);
}


//////////////////////////////////////////////////////////////////////////
HRESULT CBScriptHolder::applyEvent(const char *EventName, bool Unbreakable) {
	int NumHandlers = 0;

	HRESULT ret = E_FAIL;
	for (int i = 0; i < _scripts.GetSize(); i++) {
		if (!_scripts[i]->_thread) {
			CScScript *handler = _scripts[i]->InvokeEventHandler(EventName, Unbreakable);
			if (handler) {
				//_scripts.Add(handler);
				NumHandlers++;
				ret = S_OK;
			}
		}
	}
	if (NumHandlers > 0 && Unbreakable) Game->_scEngine->TickUnbreakable();

	return ret;
}


//////////////////////////////////////////////////////////////////////////
HRESULT CBScriptHolder::listen(CBScriptHolder *param1, uint32 param2) {
	return E_FAIL;
}


//////////////////////////////////////////////////////////////////////////
// high level scripting interface
//////////////////////////////////////////////////////////////////////////
HRESULT CBScriptHolder::scCallMethod(CScScript *Script, CScStack *Stack, CScStack *ThisStack, const char *Name) {
	//////////////////////////////////////////////////////////////////////////
	// DEBUG_CrashMe
	//////////////////////////////////////////////////////////////////////////
	if (strcmp(Name, "DEBUG_CrashMe") == 0) {
		Stack->CorrectParams(0);
		byte *p = 0;
		*p = 10;
		Stack->PushNULL();

		return S_OK;
	}

	//////////////////////////////////////////////////////////////////////////
	// ApplyEvent
	//////////////////////////////////////////////////////////////////////////
	else if (strcmp(Name, "ApplyEvent") == 0) {
		Stack->CorrectParams(1);
		CScValue *val = Stack->Pop();
		HRESULT ret;
		ret = applyEvent(val->GetString());

		if (SUCCEEDED(ret)) Stack->PushBool(true);
		else Stack->PushBool(false);

		return S_OK;
	}

	//////////////////////////////////////////////////////////////////////////
	// CanHandleEvent
	//////////////////////////////////////////////////////////////////////////
	else if (strcmp(Name, "CanHandleEvent") == 0) {
		Stack->CorrectParams(1);
		Stack->PushBool(canHandleEvent(Stack->Pop()->GetString()));

		return S_OK;
	}

	//////////////////////////////////////////////////////////////////////////
	// CanHandleMethod
	//////////////////////////////////////////////////////////////////////////
	else if (strcmp(Name, "CanHandleMethod") == 0) {
		Stack->CorrectParams(1);
		Stack->PushBool(canHandleMethod(Stack->Pop()->GetString()));

		return S_OK;
	}

	//////////////////////////////////////////////////////////////////////////
	// AttachScript
	//////////////////////////////////////////////////////////////////////////
	else if (strcmp(Name, "AttachScript") == 0) {
		Stack->CorrectParams(1);
		Stack->PushBool(SUCCEEDED(addScript(Stack->Pop()->GetString())));

		return S_OK;
	}

	//////////////////////////////////////////////////////////////////////////
	// DetachScript
	//////////////////////////////////////////////////////////////////////////
	else if (strcmp(Name, "DetachScript") == 0) {
		Stack->CorrectParams(2);
		const char *Filename = Stack->Pop()->GetString();
		bool KillThreads = Stack->Pop()->GetBool(false);
		bool ret = false;
		for (int i = 0; i < _scripts.GetSize(); i++) {
			if (scumm_stricmp(_scripts[i]->_filename, Filename) == 0) {
				_scripts[i]->Finish(KillThreads);
				ret = true;
				break;
			}
		}
		Stack->PushBool(ret);

		return S_OK;
	}

	//////////////////////////////////////////////////////////////////////////
	// IsScriptRunning
	//////////////////////////////////////////////////////////////////////////
	else if (strcmp(Name, "IsScriptRunning") == 0) {
		Stack->CorrectParams(1);
		const char *Filename = Stack->Pop()->GetString();
		bool ret = false;
		for (int i = 0; i < _scripts.GetSize(); i++) {
			if (scumm_stricmp(_scripts[i]->_filename, Filename) == 0 && _scripts[i]->_state != SCRIPT_FINISHED && _scripts[i]->_state != SCRIPT_ERROR) {
				ret = true;
				break;
			}
		}
		Stack->PushBool(ret);

		return S_OK;
	} else return CBScriptable::scCallMethod(Script, Stack, ThisStack, Name);
}


//////////////////////////////////////////////////////////////////////////
CScValue *CBScriptHolder::scGetProperty(const char *Name) {
	_scValue->SetNULL();

	//////////////////////////////////////////////////////////////////////////
	// Type
	//////////////////////////////////////////////////////////////////////////
	if (strcmp(Name, "Type") == 0) {
		_scValue->SetString("script_holder");
		return _scValue;
	}

	//////////////////////////////////////////////////////////////////////////
	// Name
	//////////////////////////////////////////////////////////////////////////
	else if (strcmp(Name, "Name") == 0) {
		_scValue->SetString(_name);
		return _scValue;
	}

	//////////////////////////////////////////////////////////////////////////
	// Filename (RO)
	//////////////////////////////////////////////////////////////////////////
	else if (strcmp(Name, "Filename") == 0) {
		_scValue->SetString(_filename);
		return _scValue;
	}

	else return CBScriptable::scGetProperty(Name);
}


//////////////////////////////////////////////////////////////////////////
HRESULT CBScriptHolder::scSetProperty(const char *Name, CScValue *Value) {
	//////////////////////////////////////////////////////////////////////////
	// Name
	//////////////////////////////////////////////////////////////////////////
	if (strcmp(Name, "Name") == 0) {
		setName(Value->GetString());
		return S_OK;
	} else return CBScriptable::scSetProperty(Name, Value);
}


//////////////////////////////////////////////////////////////////////////
const char *CBScriptHolder::scToString() {
	return "[script_holder]";
}

//////////////////////////////////////////////////////////////////////////
HRESULT CBScriptHolder::saveAsText(CBDynBuffer *Buffer, int Indent) {
	return CBBase::saveAsText(Buffer, Indent);
}


//////////////////////////////////////////////////////////////////////////
HRESULT CBScriptHolder::persist(CBPersistMgr *persistMgr) {
	CBScriptable::persist(persistMgr);

	persistMgr->transfer(TMEMBER(_filename));
	persistMgr->transfer(TMEMBER(_freezable));
	persistMgr->transfer(TMEMBER(_name));
	_scripts.persist(persistMgr);

	return S_OK;
}


//////////////////////////////////////////////////////////////////////////
HRESULT CBScriptHolder::addScript(const char *Filename) {
	for (int i = 0; i < _scripts.GetSize(); i++) {
		if (scumm_stricmp(_scripts[i]->_filename, Filename) == 0) {
			if (_scripts[i]->_state != SCRIPT_FINISHED) {
				Game->LOG(0, "CBScriptHolder::AddScript - trying to add script '%s' mutiple times (obj: '%s')", Filename, _name);
				return S_OK;
			}
		}
	}

	CScScript *scr =  Game->_scEngine->RunScript(Filename, this);
	if (!scr) {
		if (Game->_editorForceScripts) {
			// editor hack
			scr = new CScScript(Game, Game->_scEngine);
			scr->_filename = new char[strlen(Filename) + 1];
			strcpy(scr->_filename, Filename);
			scr->_state = SCRIPT_ERROR;
			scr->_owner = this;
			_scripts.Add(scr);
			Game->_scEngine->_scripts.Add(scr);
			Game->GetDebugMgr()->OnScriptInit(scr);

			return S_OK;
		}
		return E_FAIL;
	} else {
		scr->_freezable = _freezable;
		_scripts.Add(scr);
		return S_OK;
	}
}


//////////////////////////////////////////////////////////////////////////
HRESULT CBScriptHolder::removeScript(CScScript *Script) {
	for (int i = 0; i < _scripts.GetSize(); i++) {
		if (_scripts[i] == Script) {
			_scripts.RemoveAt(i);
			break;
		}
	}
	return S_OK;
}

//////////////////////////////////////////////////////////////////////////
bool CBScriptHolder::canHandleEvent(const char *EventName) {
	for (int i = 0; i < _scripts.GetSize(); i++) {
		if (!_scripts[i]->_thread && _scripts[i]->canHandleEvent(EventName)) return true;
	}
	return false;
}


//////////////////////////////////////////////////////////////////////////
bool CBScriptHolder::canHandleMethod(const char *MethodName) {
	for (int i = 0; i < _scripts.GetSize(); i++) {
		if (!_scripts[i]->_thread && _scripts[i]->canHandleMethod(MethodName)) return true;
	}
	return false;
}


TOKEN_DEF_START
TOKEN_DEF(PROPERTY)
TOKEN_DEF(NAME)
TOKEN_DEF(VALUE)
TOKEN_DEF_END
//////////////////////////////////////////////////////////////////////////
HRESULT CBScriptHolder::parseProperty(byte  *Buffer, bool Complete) {
	TOKEN_TABLE_START(commands)
	TOKEN_TABLE(PROPERTY)
	TOKEN_TABLE(NAME)
	TOKEN_TABLE(VALUE)
	TOKEN_TABLE_END

	byte *params;
	int cmd;
	CBParser parser(Game);

	if (Complete) {
		if (parser.GetCommand((char **)&Buffer, commands, (char **)&params) != TOKEN_PROPERTY) {
			Game->LOG(0, "'PROPERTY' keyword expected.");
			return E_FAIL;
		}
		Buffer = params;
	}

	char *PropName = NULL;
	char *PropValue = NULL;

	while ((cmd = parser.GetCommand((char **)&Buffer, commands, (char **)&params)) > 0) {
		switch (cmd) {
		case TOKEN_NAME:
			delete[] PropName;
			PropName = new char[strlen((char *)params) + 1];
			if (PropName) strcpy(PropName, (char *)params);
			else cmd = PARSERR_GENERIC;
			break;

		case TOKEN_VALUE:
			delete[] PropValue;
			PropValue = new char[strlen((char *)params) + 1];
			if (PropValue) strcpy(PropValue, (char *)params);
			else cmd = PARSERR_GENERIC;
			break;
		}

	}
	if (cmd == PARSERR_TOKENNOTFOUND) {
		delete[] PropName;
		delete[] PropValue;
		PropName = NULL;
		PropValue = NULL;
		Game->LOG(0, "Syntax error in PROPERTY definition");
		return E_FAIL;
	}
	if (cmd == PARSERR_GENERIC || PropName == NULL || PropValue == NULL) {
		delete[] PropName;
		delete[] PropValue;
		PropName = NULL;
		PropValue = NULL;
		Game->LOG(0, "Error loading PROPERTY definition");
		return E_FAIL;
	}


	CScValue *val = new CScValue(Game);
	val->SetString(PropValue);
	scSetProperty(PropName, val);

	delete val;
	delete[] PropName;
	delete[] PropValue;
	PropName = NULL;
	PropValue = NULL;

	return S_OK;
}


//////////////////////////////////////////////////////////////////////////
void CBScriptHolder::makeFreezable(bool Freezable) {
	_freezable = Freezable;
	for (int i = 0; i < _scripts.GetSize(); i++)
		_scripts[i]->_freezable = Freezable;

}


//////////////////////////////////////////////////////////////////////////
CScScript *CBScriptHolder::invokeMethodThread(const char *methodName) {
	for (int i = _scripts.GetSize() - 1; i >= 0; i--) {
		if (_scripts[i]->canHandleMethod(methodName)) {

			CScScript *thread = new CScScript(Game, _scripts[i]->_engine);
			if (thread) {
				HRESULT ret = thread->CreateMethodThread(_scripts[i], methodName);
				if (SUCCEEDED(ret)) {
					_scripts[i]->_engine->_scripts.Add(thread);
					Game->GetDebugMgr()->OnScriptMethodThreadInit(thread, _scripts[i], methodName);

					return thread;
				} else {
					delete thread;
				}
			}
		}
	}
	return NULL;
}


//////////////////////////////////////////////////////////////////////////
void CBScriptHolder::scDebuggerDesc(char *Buf, int BufSize) {
	strcpy(Buf, scToString());
	if (_name && strcmp(_name, "<unnamed>") != 0) {
		strcat(Buf, "  Name: ");
		strcat(Buf, _name);
	}
	if (_filename) {
		strcat(Buf, "  File: ");
		strcat(Buf, _filename);
	}
}


//////////////////////////////////////////////////////////////////////////
// IWmeObject
//////////////////////////////////////////////////////////////////////////
bool CBScriptHolder::sendEvent(const char *EventName) {
	return SUCCEEDED(applyEvent(EventName));
}

} // end of namespace WinterMute
