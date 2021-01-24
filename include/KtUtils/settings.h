#pragma once
#ifndef KTUTILS_SETTINGS_H
#define KTUTILS_SETTINGS_H

#include "global.h"

namespace KtUtils {
namespace SettingsExtra {
KT_UTILS_EXPORT bool jsonReadFunc(QIODevice& device,
                                  QSettings::SettingsMap& map);
KT_UTILS_EXPORT bool jsonWriteFunc(QIODevice& device,
                                   const QSettings::SettingsMap& map);

KT_UTILS_EXPORT bool xmlReadFunc(QIODevice& device,
                                 QSettings::SettingsMap& map);
KT_UTILS_EXPORT bool xmlWriteFunc(QIODevice& device,
                                  const QSettings::SettingsMap& map);
}  // namespace SettingsExtra
}  // namespace KtUtils

#endif  // KTUTILS_SETTINGS_H
