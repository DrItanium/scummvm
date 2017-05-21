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

#include "common/config-manager.h"


#include "hanekawa/hanekawa.h"
#include "hanekawa/script.h"

extern "C" {
#include "maya/clips.h"
}

namespace Hanekawa {

HanekawaEngine::HanekawaEngine(OSystem *syst, const HanekawaGameDescription *gameDesc) 
	: Engine(syst),
	_gameDescription(gameDesc),
	_rnd("hanekawa") {

}

HanekawaEngine::~HanekawaEngine() {
	DestroyEnvironment(_environment);
	_environment = nullptr;
	delete _console;
}
#define HANEKAWA_ENGINE_REFERENCE USER_ENVIRONMENT_DATA
void HanekawaEngine::initialize() {
	_environment = EnvCreateEnvironment();
	// install a back reference to this class
	AllocateEnvironmentData(_environment, HANEKAWA_ENGINE_REFERENCE, sizeof(HanekawaEngineWrapper), NULL);
	auto wrap = extractReference(_environment);
	wrap->_reference = this;
}

HanekawaEngineWrapper* extractReference(void* _env) noexcept {
	return static_cast<HanekawaEngineWrapper*>(GetEnvironmentData(_environment, HANEKAWA_ENGINE_REFERENCE));
}


} // End of namespace Hanekawa
