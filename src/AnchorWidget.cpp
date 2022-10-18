#include <KtUtils/AnchorWidget>

namespace KtUtils {
/* ============================== AnchorWidget ============================== */
struct AnchorWidget::Private {
  Q_DISABLE_COPY(Private)

  AnchorWidget* q;
  explicit Private(AnchorWidget* q);

  AnchorLayout* layout = nullptr;
  QGraphicsWidget* layoutWidget = nullptr;
};

AnchorWidget::Private::Private(AnchorWidget* q_ptr) : q(q_ptr) {}

AnchorWidget::AnchorWidget(QWidget* parent)
    : QGraphicsView(parent), d(new Private(this)) {
  setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  setViewportMargins(0, 0, 0, 0);
  setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);

  QGraphicsScene* scene = new QGraphicsScene(this);
  setScene(scene);

  setLayout(new AnchorLayout);
}

AnchorWidget::~AnchorWidget() {
  if (d->layout) {
    delete d->layout;
    d->layout = nullptr;
  }
}

AnchorLayout* AnchorWidget::layout() const { return d->layout; }

void AnchorWidget::setLayout(AnchorLayout* layout) {
  if (!layout) {
    qWarning() << "AnchorWidget::setLayout: layout is nullptr";
    return;
  }

  if (!d->layoutWidget) {
    d->layoutWidget = new QGraphicsWidget;
  }
  if (d->layout) {
    delete d->layout;
    d->layout = nullptr;
  }
  d->layout = layout;

  d->layoutWidget->setLayout(d->layout);
  d->layoutWidget->resize(size());
  scene()->addItem(d->layoutWidget);
}

qreal AnchorWidget::anchorHorizontalSpacing() const {
  return d->layout->horizontalSpacing();
}

void AnchorWidget::setAnchorHorizontalSpacing(qreal spacing) {
  d->layout->setHorizontalSpacing(spacing);
}

qreal AnchorWidget::anchorVerticalSpacing() const {
  return d->layout->verticalSpacing();
}

void AnchorWidget::setAnchorVerticalSpacing(qreal spacing) {
  d->layout->setVerticalSpacing(spacing);
}

void AnchorWidget::setAnchorSpacing(qreal spacing) {
  d->layout->setSpacing(spacing);
}

bool AnchorWidget::event(QEvent* event) {
  bool ret = QGraphicsView::event(event);

  switch (event->type()) {
    case QEvent::Resize: {
      QResizeEvent* e = static_cast<QResizeEvent*>(event);
      d->layoutWidget->resize(e->size());
      // fitInView(d->layoutWidget);
    } break;

    default:
      break;
  }

  return ret;
}
/* ============================== AnchorWidget ============================== */

/* ============================== AnchorLayout ============================== */
Q_DECLARE_METATYPE(QGraphicsLayoutItem*)
static constexpr const char* kItemPropertyName = "GraphicsLayoutItem";

AnchorLayout::AnchorLayout(AnchorWidget* parent) {
  if (parent) {
    parent->setLayout(this);
  }
}

AnchorLayout::~AnchorLayout() {}

QGraphicsLayoutItem* AnchorLayout::createItem(QGraphicsLayoutItem* item) {
  return item;
}

QGraphicsLayoutItem* AnchorLayout::createItem(QWidget* widget) {
  if (Q_UNLIKELY(!widget)) {
    qWarning() << "AnchorLayout::createItem: widget is nullptr";
    return nullptr;
  }

  if (AnchorWidget* view = qobject_cast<AnchorWidget*>(widget)) {
    return createItem(view->layout());
  }

  if (QGraphicsLayoutItem* item = layoutItem(widget)) {
    return item;
  } else {
    QGraphicsProxyWidget* proxy = new QGraphicsProxyWidget;
    proxy->setWidget(widget);
    widget->setProperty(kItemPropertyName,
                        QVariant::fromValue<QGraphicsLayoutItem*>(
                            static_cast<QGraphicsLayoutItem*>(proxy)));
    return proxy;
  }
}

QGraphicsLayoutItem* AnchorLayout::createItem(QLayout* layout) {
  if (Q_UNLIKELY(!layout)) {
    qWarning() << "AnchorLayout::createItem: layout is nullptr";
    return nullptr;
  }

  if (QGraphicsLayoutItem* item = layoutItem(layout)) {
    return item;
  } else {
    QWidget* widget = new QWidget;
    widget->setLayout(layout);
    QGraphicsLayoutItem* ret = createItem(widget);
    layout->setProperty(kItemPropertyName,
                        QVariant::fromValue<QGraphicsLayoutItem*>(ret));
    return ret;
  }
}

QGraphicsLayoutItem* AnchorLayout::layoutItem(QGraphicsLayoutItem* item) {
  return item;
}

QGraphicsLayoutItem* AnchorLayout::layoutItem(QWidget* widget) {
  if (Q_UNLIKELY(!widget)) {
    qWarning() << "AnchorLayout::createItem: widget is nullptr";
    return nullptr;
  }

  if (AnchorWidget* view = qobject_cast<AnchorWidget*>(widget)) {
    return layoutItem(view->d->layoutWidget);
  }

  return widget->property(kItemPropertyName).value<QGraphicsLayoutItem*>();
}

QGraphicsLayoutItem* AnchorLayout::layoutItem(QLayout* layout) {
  if (Q_UNLIKELY(!layout)) {
    qWarning() << "AnchorLayout::createItem: layout is nullptr";
    return nullptr;
  }

  return layout->property(kItemPropertyName).value<QGraphicsLayoutItem*>();
}
/* ============================== AnchorLayout ============================== */
}  // namespace KtUtils
