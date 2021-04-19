#pragma once
#ifndef KTUTILS_SETTINGS_HPP
#define KTUTILS_SETTINGS_HPP

#include "Global.hpp"
#include "Json.hpp"

namespace KtUtils {
// Helper functions for QSettings::registerFormat
namespace SettingsExtra {
inline bool jsonReadFunc(QIODevice& device, QSettings::SettingsMap& map) {
  return ::KtUtils::Json::settingsReadFunc(device, map);
}
inline bool jsonWriteFunc(QIODevice& device,
                          const QSettings::SettingsMap& map) {
  return ::KtUtils::Json::settingsWriteFunc(device, map);
}

KTUTILS_EXPORT bool xmlReadFunc(QIODevice& device, QSettings::SettingsMap& map);
KTUTILS_EXPORT bool xmlWriteFunc(QIODevice& device,
                                 const QSettings::SettingsMap& map);
}  // namespace SettingsExtra
}  // namespace KtUtils

#endif  // KTUTILS_SETTINGS_HPP
