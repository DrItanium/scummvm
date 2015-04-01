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

#include "sherlock/scripts.h"
#include "sherlock/sherlock.h"

namespace Sherlock {

Scripts::Scripts(SherlockEngine *vm): _vm(vm) {
	_scriptMoreFlag = 0;
	_scriptSaveIndex = 0;
	_scriptSelect = 0;
}

void Scripts::doScript(const Common::String &str) {
	// TODO
}

void Scripts::popStack() {
	ScriptEntry script = _scriptStack.pop();
	_scriptName = script._name;
	_scriptSaveIndex = script._index;
	_scriptSelect = script._select;
	_scriptMoreFlag = true;
}


} // End of namespace Sherlock
