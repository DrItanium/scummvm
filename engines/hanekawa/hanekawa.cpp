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

// I hate doing this but I need to make sure that this compiles :(
#define FORBIDDEN_SYMBOL_ALLOW_ALL
#include "common/config-manager.h"
#include "common/error.h"
#include "common/system.h"
#include "graphics/palette.h"
#include "common/debug.h"
#include "common/debug-channels.h"

#include "engines/util.h"
#include "hanekawa/hanekawa.h"

#include "engines/advancedDetector.h"

extern "C" {
#include "maya/clips.h"
}

namespace Hanekawa {

HanekawaEngine::HanekawaEngine(OSystem *syst) 
	: Engine(syst),
	_rng("hanekawa") {
	debug("HanekawaEngine::HanekawaEngine");
}

HanekawaEngine::~HanekawaEngine() {
	debug("HanekawaEngine::~HanekawaEngine");
	DestroyEnvironment(_environment);
	_environment = nullptr;
	delete _console;
}
#define HANEKAWA_ENGINE_REFERENCE USER_ENVIRONMENT_DATA

Common::Error HanekawaEngine::run() {
	debug("HanekawaEngine::run()");
	// initialize the screen
	initGraphics(320, 200, false);
	_environment = CreateEnvironment();
	_console = new Console(this);
	// install a back reference to this class
	AllocateEnvironmentData(_environment, HANEKAWA_ENGINE_REFERENCE, sizeof(HanekawaEngineWrapper), NULL);
	auto wrap = extractReference(_environment);
	wrap->_reference = this;


	return Common::kNoError;
}

HanekawaEngineWrapper* extractReference(void* _env) noexcept {
	return static_cast<HanekawaEngineWrapper*>(GetEnvironmentData(_env, HANEKAWA_ENGINE_REFERENCE));
}

static const PlainGameDescriptor HanekawaEngine_setting[] = {
	{ "HanekawaEngine", "Hanekawa Engine developer mode" },
	{ "HanekawaeEngineRelease", "Hanekawa Engine release mode" },
	{ 0, 0 },
};

} // End of namespace Hanekawa


class HanekawaMetaEngine : public MetaEngine {
public:
	virtual const char* getName() const {
		return "Hanekawa Engine";
	}

	virtual const char* getOriginalCopyright() const {
		return "Copyright (C) Joshua Scoggins";
	}

	virtual GameList getSupportedGames() const override {
		GameList games;
		const PlainGameDescriptor *g = Hanekawa::HanekawaEngine_setting;
		while (g->gameId) {
			games.push_back(*g);
			++g;
		}
		return games;
	}

	virtual GameDescriptor findGame(const char* gameId) const {
		const PlainGameDescriptor *g = Hanekawa::HanekawaEngine_setting;
		while (g->gameId) {
			if (0 == scumm_stricmp(gameId, g->gameId)) {
				break;
			}
			++g;
		}
		return GameDescriptor(g->gameId, g->description);
	}

	virtual GameList detectGames(const Common::FSList &fslist) const override {
		GameList detectedGames;

		return detectedGames;
	}


	virtual Common::Error createInstance(OSystem *syst, Engine **engine) const override {
		assert(syst);
		assert(engine);
      *engine = new Hanekawa::HanekawaEngine(syst); 
      return Common::kNoError;
      /*
		// Scan the target directory for files (error out if it does not exist)
		Common::FSList fslist;
		Common::FSNode dir(ConfMan.get("path"));
		if (!dir.getChildren(fslist, Common::FSNode::kListAll)) {
			return Common::kNoGameDataFoundError;
		}
 
		// Invoke the detector
		Common::String gameid = ConfMan.get("gameid");
		GameList detectedGames = detectGames(fslist);
 
		for (uint i = 0; i < detectedGames.size(); i++) {
			if (detectedGames[i].gameid() == gameid) {
				// At this point you may want to perform additional sanity checks.
				*engine = new AdventureEngine::AdventureEngineEngine(syst);
				return Common::kNoError;
			}
		}
 
		// Failed to find any game data
		return Common::kNoGameDataFoundError;
      */
	}


};


#if PLUGIN_ENABLED_DYNAMIC(HANEKAWA)
	REGISTER_PLUGIN_DYNAMIC(HANEKAWA, PLUGIN_TYPE_ENGINE, HanekawaMetaEngine);
#else
	REGISTER_PLUGIN_STATIC(HANEKAWA, PLUGIN_TYPE_ENGINE, HanekawaMetaEngine);
#endif
