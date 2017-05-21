/*
 * hanekawa
 * Copyright (c) 2017, Joshua Scoggins and Contributors
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

// the hanekawa architecture is a game engine / simulator
#ifndef HANEKAWA_HANEKAWA_H
#define HANEKAWA_HANEKAWA_H

#if __cplusplus < 201103L
#error "C++11 support required for hanekawa architecture!"
#endif

#include "common/scummsys.h"
#include "common/file.h"
#include "common/util.h"
#include "common/str.h"
#include "common/hashmap.h"
#include "common/hash-str.h"
#include "common/random.h"

#include "engines/engine.h"
#include "gui/debugger.h"

namespace Hanekawa {
class Console;

enum DebugChannels {
	kHanekawaDebugExample = 1 << 0,
	kHanekawaDebugExample2 = 1 << 1,
};

class HanekawaEngine : public Engine {
public:
	HanekawaEngine(OSystem *syst);
	virtual ~HanekawaEngine();

	Common::Language getLanguage() const;
	Common::Platform getPlatform() const;

	Common::RandomSource _rng;

protected:
	virtual Common::Error run() override;

	void *_environment;
	Console *_console;
};


struct HanekawaEngineWrapper {
	// we don't want anyone to mess with this in memory once it has been placed
	HanekawaEngine * _reference; 
};

HanekawaEngineWrapper* extractReference(void* _env) noexcept;

class Console : public GUI::Debugger {
public:
	Console(HanekawaEngine *vm) { };
	virtual ~Console() { }

};

} // End of namespace Hanekawa

#endif // end HANEKAWA_HANEKAWA_H
