#pragma once
#ifndef KTUTILS_SETTINGS_P_HPP
#define KTUTILS_SETTINGS_P_HPP

#include <KtUtils/Settings.hpp>

namespace KtUtils {
namespace SettingsExtra {
// Check if a QSettings array
bool CheckIfArray(const QVariantMap& map) {
  const auto it = map.find(QStringLiteral("size"));
  if (it == map.cend()) return false;

  const int size = it->toInt();
  if (map.count() != (size + 1)) return false;

  for (int i = 1; i <= size; ++i) {
    if (!map.contains(QString::number(i))) return false;
  }
  return true;
}

// Convert QSettings array to QVariantList
QVariant FromSettingsArray(const QVariant& value) {
  if (int(value.type()) != QMetaType::QVariantMap) return value;

  const QVariantMap map = value.toMap();
  if (CheckIfArray(map)) {  // Convert map into list
    QVariantList ret;
    for (int i = 0; i < (map.count() - 1); ++i) {
      ret << FromSettingsArray(map.value(QString::number(i + 1)));
    }
    return ret;
  } else {
    QVariantMap ret;
    for (auto it = map.cbegin(); it != map.cend(); ++it) {
      ret.insert(it.key(), FromSettingsArray(it.value()));
    }
    return ret;
  }
}

// Convert QSettings::SettingsMap to QVariantMap tree
QVariantMap FromSettingsMap(const QSettings::SettingsMap& map) {
  std::function<void(QVariantMap*, QStringList, const QVariant&)> InsertKeys =
      [&InsertKeys](QVariantMap* map, QStringList keys, const QVariant& value) {
        const QString key = keys.takeFirst();
        if (keys.isEmpty()) {
          map->insert(key, value);
        } else {
          QVariant& v = (*map)[key];
          QVariantMap subMap = v.toMap();
          InsertKeys(&subMap, std::move(keys), value);
          v = subMap;
        }
      };

  QVariantMap ret;
  for (auto it = map.begin(); it != map.end(); ++it) {
    InsertKeys(&ret, it.key().split('/'), it.value());
  }
  return FromSettingsArray(ret).toMap();
}

// Convert QVariantMap tree to QSettings::SettingsMap
QSettings::SettingsMap ToSettingsMap(const QVariantMap& map) {
  QSettings::SettingsMap ret;
  std::function<void(QString, const QVariant&)> InsertValue =
      [&InsertValue, &ret](QString keys, const QVariant& value) {
        switch (int(value.type())) {
          case QMetaType::QVariantMap: {
            const QVariantMap map = value.toMap();
            for (auto it = map.cbegin(); it != map.cend(); ++it) {
              InsertValue(keys.isEmpty() ? it.key() : (keys + '/' + it.key()),
                          it.value());
            }
            break;
          }

          case QMetaType::QVariantList: {
            const QVariantList list = value.toList();
            ret.insert(keys + "/size", list.size());
            for (int i = 0; i < list.count(); ++i) {
              InsertValue(keys + '/' + QString::number(i + 1), list[i]);
            }
            break;
          }

          default:
            ret.insert(keys, value);
            break;
        }
      };
  InsertValue("", map);
  return ret;
}
}  // namespace SettingsExtra
}  // namespace KtUtils

#endif  // KTUTILS_SETTINGS_P_HPP
