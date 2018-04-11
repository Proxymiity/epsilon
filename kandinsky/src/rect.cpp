#include <kandinsky/rect.h>

KDRect::KDRect(KDPoint p, KDSize s) :
  m_x(p.x()), m_y(p.y()),
  m_width(s.width()), m_height(s.height())
{
}

KDRect::KDRect(KDCoordinate x, KDCoordinate y, KDSize s) :
  m_x(x), m_y(y),
  m_width(s.width()), m_height(s.height())
{
}

KDRect::KDRect(KDPoint p, KDCoordinate width, KDCoordinate height) :
  m_x(p.x()), m_y(p.y()),
  m_width(width), m_height(height)
{
}

void KDRect::setOrigin(KDPoint p) { m_x = p.x(); m_y = p.y(); }
void KDRect::setSize(KDSize s) { m_width = s.width(); m_height = s.height(); }

bool KDRect::intersects(const KDRect & other) const {
  return (
      other.right() >= left() &&
      other.left() <= right() &&
      other.top() <= bottom() &&
      other.bottom() >= top()
      );
}

KDRect KDRect::intersectedWith(const KDRect & other) const {
  if (!intersects(other)) {
    return KDRectZero;
  }

  KDCoordinate intersectionLeft = max(left(), other.left());
  KDCoordinate intersectionRight = min(right(), other.right());
  KDCoordinate intersectionTop = max(top(), other.top());
  KDCoordinate intersectionBottom = min(bottom(), other.bottom());

  return KDRect(
      intersectionLeft,
      intersectionTop,
      intersectionRight-intersectionLeft+1,
      intersectionBottom-intersectionTop+1);
}

void computeUnionBound(KDCoordinate size1, KDCoordinate size2,
    KDCoordinate * outputMin, KDCoordinate * outputMax,
    KDCoordinate min1, KDCoordinate min2,
    KDCoordinate max1, KDCoordinate max2)
{
  if (size1 != 0) {
    if (size2 != 0) {
      *outputMin = min(min1, min2);
      *outputMax = max(max1, max2);
    } else {
      *outputMin = min1;
      *outputMax = max1;
    }
  } else {
    if (size2 != 0) {
      *outputMin = min2;
      *outputMax = max2;
    }
  }
}

KDRect KDRect::unionedWith(const KDRect & other) const {
  if (this->isEmpty()) {
    return other;
  }
  if (other.isEmpty()) {
    return *this;
  }
  /* We should ignore coordinate whose size is zero
   * For example, if r1.height is zero, just ignore r1.y and r1.height. */

  KDCoordinate resultLeft = 0;
  KDCoordinate resultTop = 0;
  KDCoordinate resultRight = 0;
  KDCoordinate resultBottom = 0;

  computeUnionBound(width(), other.width(),
      &resultLeft, &resultRight,
      left(), other.left(),
      right(), other.right());

  computeUnionBound(height(), other.height(),
      &resultTop, &resultBottom,
      top(), other.top(),
      bottom(), other.bottom());

  return KDRect(
    resultLeft,
    resultTop,
    resultRight-resultLeft+1,
    resultBottom-resultTop+1
    );
}

bool KDRect::contains(KDPoint p) const {
  return (p.x() >= x() && p.x() <= right() && p.y() >= y() && p.y() <= bottom());
}

bool KDRect::isAbove(KDPoint p) const {
  return (p.y() >= y());
}

bool KDRect::isUnder(KDPoint p) const {
  return (p.y() <= bottom());
}

KDRect KDRect::translatedBy(KDPoint p) const {
  return KDRect(x() + p.x(), y() + p.y(), width(), height());
}

KDRect KDRect::movedTo(KDPoint p) const {
  return KDRect(p.x(), p.y(), width(), height());
}

bool KDRect::isEmpty() const {
  return (width() == 0 || height() == 0);
}
