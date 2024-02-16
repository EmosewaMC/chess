#ifndef __GAMEGLOBAL__H__
#define __GAMEGLOBAL__H__


#include <memory>

class Logger;

namespace GameGlobal {
	// Defined as extern to avoid multiple definitions.
	extern std::unique_ptr<Logger> logger;
};

#endif  //!__GAMEGLOBAL__H__
