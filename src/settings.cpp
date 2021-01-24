#include <KtUtils/Settings>

namespace KtUtils {
namespace SettingsExtra {
// Check if a QSettings array
Q_DECL_HIDDEN bool CheckIfArray(const QVariantMap& map) {
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
Q_DECL_HIDDEN QVariant FromSettingsArray(const QVariant& value) {
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
Q_DECL_HIDDEN QVariantMap FromSettingsMap(const QSettings::SettingsMap& map) {
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
Q_DECL_HIDDEN QSettings::SettingsMap ToSettingsMap(const QVariantMap& map) {
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

bool jsonReadFunc(QIODevice& device, QSettings::SettingsMap& map) {
  QJsonParseError error;
  const QJsonDocument doc = QJsonDocument::fromJson(device.readAll(), &error);
  if (error.error != QJsonParseError::NoError) {
    qWarning("Json parse error at pos %d: %s", error.offset,
             error.errorString().toUtf8().constData());
    return false;
  }
  map = ToSettingsMap(doc.object().toVariantMap());
  return true;
}

bool jsonWriteFunc(QIODevice& device, const QSettings::SettingsMap& map) {
  const QJsonDocument doc(QJsonObject::fromVariantMap(FromSettingsMap(map)));
  return device.write(doc.toJson(QJsonDocument::Indented)) >= 0;
}

bool xmlReadFunc(QIODevice& device, QSettings::SettingsMap& map) {
  QXmlStreamReader xml(&device);
  std::function<QVariant()> readNextValue = [&readNextValue,
                                             &xml]() -> QVariant {
    switch (xml.readNext()) {
      case QXmlStreamReader::StartElement: {
        if (xml.attributes().value("isList") == QStringLiteral("true")) {
          // List
          QVariantList list;
          for (QVariant value = readNextValue(); value.isValid();
               value = readNextValue()) {
            list << value;
          }  // finish with invalid value at EndElement of this list
          return list;
        } else {
          if (xml.readNext() == QXmlStreamReader::StartElement) {
            // Map
            QVariantMap map;
            for (QVariant value = readNextValue(); value.isValid();
                 value = readNextValue()) {
              map[xml.name().toString()] = readNextValue();
            }  // finish with invalid value at EndElement of this map
            return map;
          } else {
            // Value
            QString text = xml.text().toString();
            xml.readNext();  // finish at EndElement of this value
            return text;
          }
        }
      }

      case QXmlStreamReader::StartDocument:
        return readNextValue();

      case QXmlStreamReader::Invalid:
        qWarning("Xml parse error at %lld:%lld, pos %lld: %s",
                 xml.columnNumber(), xml.lineNumber(), xml.characterOffset(),
                 xml.errorString().toUtf8().constData());
        throw false;

      default:
        return {};
    }
  };

  try {
    map = ToSettingsMap(readNextValue().toMap());
  } catch (...) {
    return false;
  }
  return true;
}

bool xmlWriteFunc(QIODevice& device, const QSettings::SettingsMap& map) {
  QXmlStreamWriter xml(&device);
  xml.setAutoFormatting(true);
  xml.setAutoFormattingIndent(2);
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
  xml.setCodec(QTextCodec::codecForName("UTF-8"));
#endif

  std::function<void(const QString&, const QVariant&)> writeValue =
      [&xml, &writeValue](const QString& key, const QVariant& value) {
        xml.writeStartElement(key);

        switch (int(value.type())) {
          case QMetaType::QVariantMap: {
            QVariantMap map = value.toMap();
            for (auto it = map.cbegin(); it != map.cend(); ++it) {
              writeValue(it.key(), it.value());
            }
            break;
          }

          case QMetaType::QVariantList: {
            QVariantList list = value.toList();
            xml.writeAttribute("isList", "true");
            for (const QVariant& var : list) {
              writeValue("Value", var.toString());
            }
            break;
          }

          default:
            xml.writeCharacters(value.toString());
            break;
        }

        xml.writeEndElement();
      };

  writeValue("Settings", FromSettingsMap(map));

  xml.writeEndDocument();
  return true;
}
}  // namespace SettingsExtra
}  // namespace KtUtils
