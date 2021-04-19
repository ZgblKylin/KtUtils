#include "KtUtils/Json.hpp"
#include "Settings_p.hpp"

namespace KtUtils {
namespace Json {
QList<QStandardItem*> toStandardItem(QVariant name, const QJsonValue& json) {
  QList<QList<QStandardItem*>> children;

  QStandardItem* col0 = new QStandardItem;
  col0->setData(name, Qt::EditRole);
  col0->setData(name, Qt::DisplayRole);
  col0->setData(name, Qt::AccessibleTextRole);

  switch (json.type()) {
    case QJsonValue::Null:
      return {col0};

    case QJsonValue::Object: {
      QJsonObject obj = json.toObject();
      for (auto it = obj.constBegin(); it != obj.constEnd(); ++it) {
        children << toStandardItem(it.key(), it.value());
      }
      break;
    }

    case QJsonValue::Array: {
      QJsonArray array = json.toArray();
      for (int i = 0; i < array.count(); ++i) {
        children << toStandardItem(i, array.at(i));
      }
      break;
    }

    default: {
      QStandardItem* col1 = new QStandardItem();
      col1->setData(json, Qt::EditRole);
      col1->setData(json.toVariant(), Qt::DisplayRole);
      col1->setData(json.toVariant(), Qt::AccessibleTextRole);
      return {col0, col1};
    }
  }

  // Object or array.
  for (auto&& child : children) {
    col0->appendRow(child);
  }
  return {col0};
}

QList<QStandardItem*> toStandardItem(const QJsonValue& json,
                                     const QString& name) {
  return toStandardItem(name, json);
}

QJsonValue fromStandardItem(QStandardItem* col0, QStandardItem* col1) {
  if (!col0) return QJsonValue();
  if (!col0->hasChildren()) {
    if (col1) {
      return QJsonValue::fromVariant(col1->data(Qt::EditRole));
    } else {
      return QJsonValue();
    }
  }

  QVariant value = col0->child(0, 0)->data(Qt::EditRole);
  bool isArray = value.canConvert<int>();
  if (isArray) {
    int begin = value.toInt();
    isArray = (value == 0) || (value == 1);
    for (int row = 0; isArray && (row < col0->rowCount()); ++row) {
      value = col0->child(0, 0)->data(Qt::EditRole);
      isArray = value.canConvert<int>() && (value.toInt() == (begin + row));
    }
  }

  if (isArray) {
    QJsonArray array;
    for (int row = 0; row < col0->rowCount(); ++row) {
      array << fromStandardItem(col0->child(row, 0), col0->child(row, 1));
    }
    return array;
  } else {
    QJsonObject object;
    for (int rpw = 0; rpw < col0->rowCount(); ++rpw) {
      QStandardItem* first = col0->child(rpw, 0);
      QStandardItem* second = col0->child(rpw, 1);
      object[first->data(Qt::DisplayRole).toString()] =
          fromStandardItem(first, second);
    }
    return object;
  }
}

bool settingsReadFunc(QIODevice& device, QSettings::SettingsMap& map) {
  QJsonParseError error;
  const QJsonDocument doc = QJsonDocument::fromJson(device.readAll(), &error);
  if (error.error != QJsonParseError::NoError) {
    qWarning("Json parse error at pos %d: %s", error.offset,
             error.errorString().toUtf8().constData());
    return false;
  }
  QVariantMap variantMap = doc.object().toVariantMap();
  map = ::KtUtils::SettingsExtra::ToSettingsMap(variantMap);
  return true;
}

bool settingsWriteFunc(QIODevice& device, const QSettings::SettingsMap& map) {
  QVariantMap variantMap = ::KtUtils::SettingsExtra::FromSettingsMap(map);
  const QJsonDocument doc(QJsonObject::fromVariantMap(variantMap));
  return device.write(doc.toJson(QJsonDocument::Indented)) >= 0;
}
}  // namespace Json
}  // namespace KtUtils
