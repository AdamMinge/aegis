#ifndef AEGIS_EXPORT_H
#define AEGIS_EXPORT_H

/* ------------------------------------ Qt ---------------------------------- */
#include "aegis/config.h"
/* -------------------------------------------------------------------------- */

#if defined(AEGIS_EXPORTS)
#define LIB_AEGIS_API AEGIS_API_EXPORT
#else
#define LIB_AEGIS_API AEGIS_API_IMPORT
#endif

#endif  // AEGIS_EXPORT_H
