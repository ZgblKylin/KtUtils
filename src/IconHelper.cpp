#include <KtUtils/IconHelper>

void InitializeResources() {
#ifndef KTUTILS_SHARED_LIBRARY
  static const bool kResourceInitialized = [] {
    Q_INIT_RESOURCE(KtUtils);
    return true;
  }();
  Q_UNUSED(kResourceInitialized)
#endif
}

namespace KtUtils {
//  Add path/fill attribute to svg
void SetColor(QXmlStreamReader& reader, QXmlStreamWriter& writer,
              const QString& value) {
  while (!reader.atEnd()) {
    switch (reader.readNext()) {
      case QXmlStreamReader::StartDocument:
        writer.writeStartDocument();
        break;

      case QXmlStreamReader::EndDocument:
        writer.writeEndDocument();
        break;

      case QXmlStreamReader::StartElement: {
        writer.writeStartElement(reader.namespaceUri().toString(),
                                 reader.name().toString());
        bool isPath = reader.name() == QStringLiteral("path");
        auto attributes = reader.attributes();
        for (auto&& ns : reader.namespaceDeclarations()) {
          writer.writeNamespace(ns.namespaceUri().toString(),
                                ns.prefix().toString());
        }
        for (auto&& attribute : attributes) {
          if (!isPath || (attribute.name() != QStringLiteral("fill"))) {
            writer.writeAttribute(attribute);
          }
        }
        if (isPath) {
          writer.writeAttribute(QStringLiteral("fill"), value);
        }
      } break;

      case QXmlStreamReader::EndElement:
        writer.writeEndElement();
        break;

      case QXmlStreamReader::Characters:
        writer.writeCharacters(reader.text().toString());
        break;

      case QXmlStreamReader::Comment:
        writer.writeComment(reader.text().toString());
        break;

      case QXmlStreamReader::DTD:
        writer.writeDTD(reader.text().toString());
        break;

      case QXmlStreamReader::EntityReference:
        writer.writeEntityReference(reader.text().toString());
        break;

      case QXmlStreamReader::ProcessingInstruction:
        writer.writeProcessingInstruction(
            reader.processingInstructionTarget().toString(),
            reader.processingInstructionData().toString());
        break;

      default:
        break;
    }
  }
}

QSharedPointer<QSvgRenderer> GetRenderer(IconHelper::Icon iconType,
                                         QColor color) {
  InitializeResources();

  static QMetaObject mo = IconHelper::staticMetaObject;
  static QMetaEnum me = mo.enumerator(0);

  QString name = me.valueToKey(iconType);
  name[name.indexOf('_')] = '/';
  name.replace('_', '-');

  QString svg = QStringLiteral(":/Fonts/svgs/%1.svg").arg(name);
  QFile file(svg);
  file.open(QIODevice::ReadOnly | QIODevice::Text);
  QXmlStreamReader reader(&file);

  QString xml;
  QXmlStreamWriter writer(&xml);

  SetColor(reader, writer, color.name(QColor::HexRgb));

  return QSharedPointer<QSvgRenderer>::create(xml.toUtf8());
}

QPixmap RenderPixmap(QSharedPointer<QSvgRenderer> renderer, int size) {
  QPixmap pixmap = QPixmap(size, size);
  pixmap.fill(Qt::transparent);

  QSizeF svgSize =
      renderer->defaultSize().scaled(size, size, Qt::KeepAspectRatio);
  QRectF rect(0, 0, svgSize.width(), svgSize.height());
  rect.moveCenter(QPointF(size / 2.0, size / 2.0));

  QPainter painter;
  painter.begin(&pixmap);
  renderer->render(&painter, rect);
  painter.end();

  return pixmap;
}

QPixmap IconHelper::pixmap(Icon iconType, int size, const QColor& color,
                           bool cached) {
  static QMutex mutex;
  using Key = std::tuple<Icon, int, QColor, bool>;
  static std::vector<std::pair<Key, QPixmap>> cache;
  if (cached) {
    QMutexLocker locker(&mutex);
    auto it = std::find_if(cache.begin(), cache.end(),
                           [&](const std::pair<Key, QPixmap>& p) {
                             return std::get<0>(p.first) == iconType &&
                                    std::get<1>(p.first) == size &&
                                    std::get<2>(p.first) == color &&
                                    std::get<3>(p.first) == cached;
                           });
    if (it != cache.end()) {
      return it->second;
    }
  }

  QSharedPointer<QSvgRenderer> renderer = GetRenderer(iconType, color);
  QPixmap pixmap = RenderPixmap(renderer, size);
  if (cached) {
    QMutexLocker locker(&mutex);
    cache.emplace_back(Key{iconType, size, color, cached}, pixmap);
  }
  return pixmap;
}

QIcon IconHelper::icon(Icon iconType, const QColor& color, bool cached) {
  static const QVector<int> sizes = {16, 24, 32,  36,  48,  64,
                                     72, 96, 128, 144, 192, 256};

  static QMutex mutex;
  using Key = std::tuple<Icon, QColor, bool>;
  static std::vector<std::pair<Key, QIcon>> cache;
  if (cached) {
    QMutexLocker locker(&mutex);
    auto it = std::find_if(cache.begin(), cache.end(),
                           [&](const std::pair<Key, QIcon>& p) {
                             return std::get<0>(p.first) == iconType &&
                                    std::get<1>(p.first) == color &&
                                    std::get<2>(p.first) == cached;
                           });
    if (it != cache.end()) {
      return it->second;
    }
  }

  QIcon icon;
  QSharedPointer<QSvgRenderer> renderer = GetRenderer(iconType, color);
  QSharedPointer<QSvgRenderer> renderer_gray = GetRenderer(iconType, Qt::gray);
  for (const int size : sizes) {
    QPixmap px = RenderPixmap(renderer, size);
    icon.addPixmap(px, QIcon::Normal, QIcon::On);
    icon.addPixmap(px, QIcon::Active, QIcon::On);
    icon.addPixmap(px, QIcon::Selected, QIcon::On);
    icon.addPixmap(px, QIcon::Normal, QIcon::Off);
    icon.addPixmap(px, QIcon::Active, QIcon::Off);
    icon.addPixmap(px, QIcon::Selected, QIcon::Off);

    px = RenderPixmap(renderer_gray, size);
    icon.addPixmap(px, QIcon::Disabled, QIcon::On);
    icon.addPixmap(px, QIcon::Disabled, QIcon::Off);
  }
  if (cached) {
    QMutexLocker locker(&mutex);
    cache.emplace_back(Key{iconType, color, cached}, icon);
  }
  return icon;
}

QFont IconHelper::font(Font fontType) {
  InitializeResources();

  static QMap<int, int> kFontIds = {
      {Brand, QFontDatabase::addApplicationFont(QStringLiteral(
                  ":/Fonts/otfs/Font Awesome 6 Brands-Regular-400.otf"))},
      {Regular, QFontDatabase::addApplicationFont(QStringLiteral(
                    ":/Fonts/otfs/Font Awesome 6 Free-Regular-400.otf"))},
      {Solid, QFontDatabase::addApplicationFont(QStringLiteral(
                  ":/Fonts/otfs/Font Awesome 6 Free-Solid-900.otf"))},
  };
  return QFont(
      QFontDatabase::applicationFontFamilies(kFontIds.value(fontType)).front());
}

QPixmap IconHelper::pixmap(Font fontType, QChar ch, int size,
                           const QColor& color, QFont::Weight weight,
                           bool cached) {
  static QMutex mutex;
  using Key = std::tuple<Font, QChar, int, QColor, QFont::Weight, bool>;
  static std::vector<std::pair<Key, QPixmap>> cache;

  if (cached) {
    QMutexLocker locker(&mutex);
    auto it = std::find_if(
        cache.begin(), cache.end(), [&](const std::pair<Key, QPixmap>& p) {
          return std::get<0>(p.first) == fontType &&
                 std::get<1>(p.first) == ch && std::get<2>(p.first) == size &&
                 std::get<3>(p.first) == color &&
                 std::get<4>(p.first) == weight &&
                 std::get<5>(p.first) == cached;
        });
    if (it != cache.end()) {
      return it->second;
    }
  }

  QFont font = IconHelper::font(fontType);
  font.setPixelSize(size);
  font.setWeight(weight);

  QPixmap pixmap(size, size);
  pixmap.fill(Qt::transparent);
  QPainter painter;
  painter.begin(&pixmap);
  painter.setRenderHint(QPainter::TextAntialiasing);
  painter.setFont(font);
  painter.drawText(QRect(0, 0, size, size), Qt::AlignCenter, QString(ch));
  painter.end();
  if (cached) {
    QMutexLocker locker(&mutex);
    cache.emplace_back(Key{fontType, ch, size, color, weight, cached}, pixmap);
  }
  return pixmap;
}

QIcon IconHelper::icon(Font fontType, QChar ch, const QColor& color,
                       QFont::Weight weight, bool cached) {
  static const QVector<int> sizes = {16, 24, 32,  36,  48,  64,
                                     72, 96, 128, 144, 192, 256};

  static QMutex mutex;
  using Key = std::tuple<Font, QChar, QColor, QFont::Weight, bool>;
  static std::vector<std::pair<Key, QIcon>> cache;

  if (cached) {
    QMutexLocker locker(&mutex);
    auto it = std::find_if(
        cache.begin(), cache.end(), [&](const std::pair<Key, QIcon>& p) {
          return std::get<0>(p.first) == fontType &&
                 std::get<1>(p.first) == ch && std::get<2>(p.first) == color &&
                 std::get<3>(p.first) == weight &&
                 std::get<4>(p.first) == cached;
        });
    if (it != cache.end()) {
      return it->second;
    }
  }

  QIcon icon;
  for (const int size : sizes) {
    QPixmap px = pixmap(fontType, ch, size, color, weight);
    icon.addPixmap(px, QIcon::Normal, QIcon::On);
    icon.addPixmap(px, QIcon::Active, QIcon::On);
    icon.addPixmap(px, QIcon::Selected, QIcon::On);
    icon.addPixmap(px, QIcon::Normal, QIcon::Off);
    icon.addPixmap(px, QIcon::Active, QIcon::Off);
    icon.addPixmap(px, QIcon::Selected, QIcon::Off);

    px = pixmap(fontType, ch, size, Qt::gray, weight);
    icon.addPixmap(px, QIcon::Disabled, QIcon::On);
    icon.addPixmap(px, QIcon::Disabled, QIcon::Off);
  }
  if (cached) {
    QMutexLocker locker(&mutex);
    cache.emplace_back(Key{fontType, ch, color, weight, cached}, icon);
  }
  return icon;
}
}  // namespace KtUtils
