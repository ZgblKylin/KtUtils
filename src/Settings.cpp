#include <KtUtils/Settings>
#include "Settings_p.hpp"

namespace KtUtils {
namespace SettingsExtra {
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
