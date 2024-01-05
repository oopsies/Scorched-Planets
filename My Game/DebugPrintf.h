/// \file DebugPrintf.h
/// \brief Defines to support DebugPrintf.

#pragma once

#include "debug.h"

#define USE_DEBUGPRINTF ///< Define this to use the DEBUGPRINTF macro.

/// \brief The DEBUGPRINTF macro, which has a printf style syntax.

/**** BEGIN <DO NOT MESS WITH THIS CODE ZONE> ****/
#ifdef USE_DEBUGPRINTF
  #define DEBUGPRINTF (g_cDebugManager.setsource(__FILE__,__LINE__), realDebugPrintf)
#else
  #define DEBUGPRINTF ;
#endif //USE_DEBUGPRINTF
/**** END <DO NOT MESS WITH THIS CODE ZONE> ****/
