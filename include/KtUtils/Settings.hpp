﻿#ifndef KTUTILS_SETTINGS_HPP
#define KTUTILS_SETTINGS_HPP

#include "Global.hpp"

namespace KtUtils {
// Helper functions for QSettings::registerFormat
namespace SettingsExtra {
KTUTILS_EXPORT bool jsonReadFunc(QIODevice& device,
                                 QSettings::SettingsMap& map);
KTUTILS_EXPORT bool jsonWriteFunc(QIODevice& device,
                                  const QSettings::SettingsMap& map);

KTUTILS_EXPORT bool xmlReadFunc(QIODevice& device, QSettings::SettingsMap& map);
KTUTILS_EXPORT bool xmlWriteFunc(QIODevice& device,
                                 const QSettings::SettingsMap& map);
}  // namespace SettingsExtra
}  // namespace KtUtils

#endif  // KTUTILS_SETTINGS_HPP
