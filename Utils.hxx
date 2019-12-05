#ifndef UTILS_HXX
#define UTILS_HXX

#include <QColor>

class Utils {
public:
  Utils() = delete;
  Utils(Utils const&) = delete;
  Utils& operator=(Utils const&) = delete;

  static QColor GetRedColor();
  static QColor GetFadedRedColor();
  static QColor GetGreenColor();
  static QColor GetFadedGreenColor();
  static QColor GetOrangeColor();

};

#endif
