#include "common/scummsys.h"

#include "common/config-manager.h"
#include "common/debug.h"
#include "common/debug-channels.h"
#include "common/error.h"
#include "common/EventRecorder.h"
#include "common/file.h"
#include "common/fs.h"

#include "engines/util.h"

#include "expert-system/expert-system.h"

extern "C" {
   #include "clips.h"
}

namespace ExpertSystem {
   ExpertSystemEngine::ExpertSystemEngine(OSystem* syst)
      : Engine(syst) {
         //setup the CLIPS environment here
         env = CreateEnvironment(); 
         //give the environment this engine as context
         debug("ExpertSystemEngine::ExpertSystemEngine");
      }
   ExpertSystemEngine::~ExpertSystemEngine() {
      debug("ExpertSystemEngine::~ExpertSystemEngine");
      //stop the clips environment
      EnvHalt(env);
      //destroy the clips environment
      DestroyEnvironment(env);
   }
   Console* ExpertSystemEngine::getConsole() {
      return _console;
   }
   Common::Error ExpertSystemEngine::run() {
      _console = new Console(this);
      debug("ExpertSystemEngine::init");
      //Anything else necessary 
      //The rest of the initialization is completely controlled by the expert
      //system and corresponding functions
      debug("ExpertSystemEngine::go: EnvReset");
      EnvReset(env);
      debug("ExpertSystemEngine::go: EnvRun");
      EnvRun(env);
      //Have some way to extract finished state context so it can be described
      //to the return value
      return Common::kNoError;
   }
}

