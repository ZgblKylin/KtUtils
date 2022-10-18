#ifndef KTUTILS_ANCHORWIDGET_HPP
#define KTUTILS_ANCHORWIDGET_HPP

#include "Global.hpp"

namespace KtUtils {
/* ============================== Declaration =============================== */
class AnchorLayout;
struct LayoutAnchor;
class KTUTILS_EXPORT AnchorWidget : public QGraphicsView {
  Q_OBJECT
 public:
  Q_PROPERTY(qreal anchorHorizontalSpacing READ anchorHorizontalSpacing WRITE
                 setAnchorHorizontalSpacing)
  Q_PROPERTY(qreal anchorVerticalSpacing READ anchorVerticalSpacing WRITE
                 setAnchorVerticalSpacing)

  explicit AnchorWidget(QWidget* parent = nullptr);
  ~AnchorWidget() override;

  AnchorLayout* layout() const;
  void setLayout(AnchorLayout* layout);

  template <typename FirstItem, typename SecondItem>
  QGraphicsAnchor* addAnchor(FirstItem* firstItem, Qt::AnchorPoint firstEdge,
                             SecondItem* secondItem,
                             Qt::AnchorPoint secondEdge);
  template <typename FirstItem, typename SecondItem>
  QGraphicsAnchor* anchor(FirstItem* firstItem, Qt::AnchorPoint firstEdge,
                          SecondItem* secondItem, Qt::AnchorPoint secondEdge);

  template <typename FirstItem, typename SecondItem>
  void addCornerAnchors(FirstItem* firstItem, Qt::Corner firstCorner,
                        SecondItem* secondItem, Qt::Corner secondCorner);

  template <typename FirstItem, typename SecondItem>
  void addAnchors(FirstItem* firstItem, SecondItem* secondItem,
                  Qt::Orientations orientations);

  qreal anchorHorizontalSpacing() const;
  void setAnchorHorizontalSpacing(qreal spacing);
  qreal anchorVerticalSpacing() const;
  void setAnchorVerticalSpacing(qreal spacing);
  void setAnchorSpacing(qreal spacing);

 protected:
  bool event(QEvent* event) override;

 private:
  friend class AnchorLayout;
  struct Private;
  QScopedPointer<Private> d;
};

class KTUTILS_EXPORT AnchorLayout : public QGraphicsAnchorLayout {
 public:
  explicit AnchorLayout(AnchorWidget* parent = nullptr);
  ~AnchorLayout() override;

  template <typename FirstItem, typename SecondItem>
  QGraphicsAnchor* addAnchor(FirstItem* firstItem, Qt::AnchorPoint firstEdge,
                             SecondItem* secondItem,
                             Qt::AnchorPoint secondEdge);
  template <typename FirstItem, typename SecondItem>
  QGraphicsAnchor* anchor(FirstItem* firstItem, Qt::AnchorPoint firstEdge,
                          SecondItem* secondItem, Qt::AnchorPoint secondEdge);

  template <typename FirstItem, typename SecondItem>
  void addCornerAnchors(FirstItem* firstItem, Qt::Corner firstCorner,
                        SecondItem* secondItem, Qt::Corner secondCorner);

  template <typename FirstItem, typename SecondItem>
  void addAnchors(FirstItem* firstItem, SecondItem* secondItem,
                  Qt::Orientations orientations);

 protected:
  QGraphicsLayoutItem* createItem(QGraphicsLayoutItem* item);
  QGraphicsLayoutItem* createItem(QWidget* widget);
  QGraphicsLayoutItem* createItem(QLayout* layout);
  QGraphicsLayoutItem* layoutItem(QGraphicsLayoutItem* item);
  QGraphicsLayoutItem* layoutItem(QWidget* widget);
  QGraphicsLayoutItem* layoutItem(QLayout* layout);
};
/* ============================== Declaration =============================== */

/* =============================== Definition =============================== */
/* ======================== AnchorWidget ======================== */
template <typename FirstItem, typename SecondItem>
inline QGraphicsAnchor* AnchorWidget::addAnchor(FirstItem* firstItem,
                                                Qt::AnchorPoint firstEdge,
                                                SecondItem* secondItem,
                                                Qt::AnchorPoint secondEdge) {
  return layout()->addAnchor(firstItem, firstEdge, secondItem, secondEdge);
}

template <typename FirstItem, typename SecondItem>
inline QGraphicsAnchor* AnchorWidget::anchor(FirstItem* firstItem,
                                             Qt::AnchorPoint firstEdge,
                                             SecondItem* secondItem,
                                             Qt::AnchorPoint secondEdge) {
  return layout()->anchor(firstItem, firstEdge, secondItem, secondEdge);
}

template <typename FirstItem, typename SecondItem>
inline void AnchorWidget::addCornerAnchors(FirstItem* firstItem,
                                           Qt::Corner firstCorner,
                                           SecondItem* secondItem,
                                           Qt::Corner secondCorner) {
  layout()->addCornerAnchors(firstItem, firstCorner, secondItem, secondCorner);
}

template <typename FirstItem, typename SecondItem>
inline void AnchorWidget::addAnchors(FirstItem* firstItem,
                                     SecondItem* secondItem,
                                     Qt::Orientations orientations) {
  layout()->addAnchors(firstItem, secondItem, orientations);
}
/* ======================== AnchorWidget ======================== */

/* ======================== AnchorLayout ======================== */
template <typename FirstItem, typename SecondItem>
inline QGraphicsAnchor* AnchorLayout::addAnchor(FirstItem* firstItem,
                                                Qt::AnchorPoint firstEdge,
                                                SecondItem* secondItem,
                                                Qt::AnchorPoint secondEdge) {
  return QGraphicsAnchorLayout::addAnchor(createItem(firstItem), firstEdge,
                                          createItem(secondItem), secondEdge);
}

template <typename FirstItem, typename SecondItem>
inline QGraphicsAnchor* AnchorLayout::anchor(FirstItem* firstItem,
                                             Qt::AnchorPoint firstEdge,
                                             SecondItem* secondItem,
                                             Qt::AnchorPoint secondEdge) {
  return QGraphicsAnchorLayout::anchor(layoutItem(firstItem), firstEdge,
                                       layoutItem(secondItem), secondEdge);
}

template <typename FirstItem, typename SecondItem>
inline void AnchorLayout::addCornerAnchors(FirstItem* firstItem,
                                           Qt::Corner firstCorner,
                                           SecondItem* secondItem,
                                           Qt::Corner secondCorner) {
  QGraphicsAnchorLayout::addCornerAnchors(createItem(firstItem), firstCorner,
                                          createItem(secondItem), secondCorner);
}

template <typename FirstItem, typename SecondItem>
inline void AnchorLayout::addAnchors(FirstItem* firstItem,
                                     SecondItem* secondItem,
                                     Qt::Orientations orientations) {
  QGraphicsAnchorLayout::addAnchors(createItem(firstItem),
                                    createItem(secondItem), orientations);
}
/* ======================== AnchorLayout ======================== */
/* =============================== Definition =============================== */
}  // namespace KtUtils

#endif  // KTUTILS_ANCHORWIDGET_HPP
