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

  QString svg = QStringLiteral(":/fonts/") + name;
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

QPixmap IconHelper::pixmap(Icon iconType, int size, const QColor& color) {
  QSharedPointer<QSvgRenderer> renderer = GetRenderer(iconType, color);
  return RenderPixmap(renderer, size);
}

QIcon IconHelper::icon(Icon iconType, const QColor& color) {
  static const QVector<int> sizes = {16, 24, 32,  36,  48,  64,
                                     72, 96, 128, 144, 192, 256};

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
  return icon;
}

QFont IconHelper::font(Font fontType) {
  InitializeResources();

  static QMap<int, int> kFontIds = {
      {Brand, QFontDatabase::addApplicationFont(QStringLiteral(
                  ":/Fonts/otfs/Font Awesome 5 Brands-Regular-400.otf"))},
      {Regular, QFontDatabase::addApplicationFont(QStringLiteral(
                    ":/Fonts/otfs/Font Awesome 5 Free-Regular-400.otf"))},
      {Solid, QFontDatabase::addApplicationFont(QStringLiteral(
                  ":/Fonts/otfs/Font Awesome 5 Free-Solid-900.otf"))},
  };
  return QFont(
      QFontDatabase::applicationFontFamilies(kFontIds.value(fontType)).front());
}
}  // namespace KtUtils
