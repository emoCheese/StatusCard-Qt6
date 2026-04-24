#pragma once

#include <QtCore/qglobal.h>

#if defined(STATUSCARDKIT_LIBRARY)
#  define STATUSCARDKIT_EXPORT Q_DECL_EXPORT
#else
#  define STATUSCARDKIT_EXPORT Q_DECL_IMPORT
#endif
