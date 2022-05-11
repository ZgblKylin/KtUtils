#ifndef KT_UTILS_JSON_HPP
#define KT_UTILS_JSON_HPP

#include "Global.hpp"

namespace KtUtils {
namespace Json {
/** \brief Generate QStandardItem tree from give json.
 *  \param json json to generate the tree from.
 *  \param name name for the return item.
 *  \return List of items for a tree row.
 *          First: Item with text `name`, contains children(if has).
 *          Second(if exist): value when json is neither object nor array. */
QList<QStandardItem*> KTUTILS_EXPORT toStandardItem(const QJsonValue& json,
                                                    const QString& name = {});

/** \brief Generate json value from QStandardItem tree.
 *  \param col0 Item at column 0, contains item name and children.
 *  \param col1 Item at column 1, contains item value.
 *  \return
 *  QJsonObject if roow is a tree.
 *  QJsonArray if all direct children are named 0,1,...,n.
 *  QJsonValue for elsewise. */
QJsonValue KTUTILS_EXPORT fromStandardItem(QStandardItem* col0,
                                           QStandardItem* col1 = nullptr);

// QSettings::registerFormat
KTUTILS_EXPORT bool settingsReadFunc(QIODevice& device,
                                     QSettings::SettingsMap& map);
// QSettings::registerFormat
KTUTILS_EXPORT bool settingsWriteFunc(QIODevice& device,
                                      const QSettings::SettingsMap& map);
}  // namespace Json
}  // namespace KtUtils

#endif  // KT_UTILS_JSON_HPP
