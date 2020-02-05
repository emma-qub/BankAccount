#include "Utils.hxx"

QColor Utils::GetRedColor() {
  return QColor("#dc3545");
}

QColor Utils::GetFadedRedColor() {
  auto color = GetRedColor();
  color.setAlpha(127);
  return color;
}

QColor Utils::GetGreenColor() {
  return QColor("#28a745");
}

QColor Utils::GetFadedGreenColor() {
  auto color = GetGreenColor();
  color.setAlpha(127);
  return color;
}

QColor Utils::GetOrangeColor() {
  return QColor("#fd7e14");
}

QColor Utils::GetAutoFilledColor() {
  return QColor("#1b628e");
}
