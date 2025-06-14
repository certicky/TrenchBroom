/*
 Copyright (C) 2010 Kristian Duske

 This file is part of TrenchBroom.

 TrenchBroom is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 TrenchBroom is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with TrenchBroom. If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#include "vm/vec.h"
#undef CursorShape

#include <QBoxLayout>
#include <QLocale>
#include <QObject>
#include <QPointer>
#include <QSettings>
#include <QString>
#include <QStringList>
#include <QWidget>

#include "Ensure.h"
#include "ui/ViewConstants.h"

#include <filesystem>
#include <string>

class QButtonGroup;
class QColor;
class QCompleter;
class QDialog;
class QDialogButtonBox;
class QEvent;
class QFont;
class QLayout;
class QLineEdit;
class QMainWindow;
class QPalette;
class QSlider;
class QSplitter;
class QString;
class QTableView;
class QToolButton;
class QVBoxLayout;
class QWidget;

namespace tb
{
class Color;
}

namespace tb::ui
{
enum class MapTextEncoding;

class SyncHeightEventFilter : public QObject
{
private:
  QPointer<QWidget> m_primary;
  QPointer<QWidget> m_secondary;

public:
  SyncHeightEventFilter(QWidget* primary, QWidget* secondary, QObject* parent = nullptr);
  ~SyncHeightEventFilter() override;

  bool eventFilter(QObject* target, QEvent* event) override;
};

enum class FileDialogDir
{
  Map,
  MaterialCollection,
  CompileTool,
  Engine,
  EntityDefinition,
  GamePath
};

/**
 * Gets the default directory from QSettings to use for the given type of file chooser.
 */
QString fileDialogDefaultDirectory(FileDialogDir type);

void updateFileDialogDefaultDirectoryWithFilename(
  FileDialogDir type, const QString& filename);
void updateFileDialogDefaultDirectoryWithDirectory(
  FileDialogDir type, const QString& newDefaultDirectory);

QString windowSettingsPath(const QWidget* window, const QString& suffix = "");

void saveWindowGeometry(QWidget* window);
void restoreWindowGeometry(QWidget* window);

template <typename T>
void saveWindowState(const T* window)
{
  ensure(window != nullptr, "window must not be null");

  const auto path = windowSettingsPath(window, "State");
  auto settings = QSettings{};
  settings.setValue(path, window->saveState());
}

template <typename T>
void restoreWindowState(T* window)
{
  ensure(window != nullptr, "window must not be null");

  const auto path = windowSettingsPath(window, "State");
  auto settings = QSettings{};
  window->restoreState(settings.value(path).toByteArray());
}

/**
 * Return true if the given widget or any of its children currently has focus.
 */
bool widgetOrChildHasFocus(const QWidget* widget);

class MapFrame;
MapFrame* findMapFrame(QWidget* widget);

QToolButton* createBitmapButton(
  const std::filesystem::path& imagePath,
  const QString& tooltip,
  QWidget* parent = nullptr);
QToolButton* createBitmapButton(
  const QIcon& icon, const QString& tooltip, QWidget* parent = nullptr);
QToolButton* createBitmapToggleButton(
  const std::filesystem::path& imagePath,
  const QString& tooltip,
  QWidget* parent = nullptr);

QWidget* createDefaultPage(const QString& message, QWidget* parent = nullptr);
QSlider* createSlider(int min, int max);

float getSliderRatio(const QSlider* slider);
void setSliderRatio(QSlider* slider, float ratio);

QLayout* wrapDialogButtonBox(QWidget* buttonBox);
QLayout* wrapDialogButtonBox(QLayout* buttonBox);

void addToMiniToolBarLayout(QBoxLayout* layout);

template <typename... Rest>
void addToMiniToolBarLayout(QBoxLayout* layout, int first, Rest... rest);

template <typename... Rest>
void addToMiniToolBarLayout(QBoxLayout* layout, QWidget* first, Rest... rest)
{
  layout->addWidget(first);
  addToMiniToolBarLayout(layout, rest...);
}

template <typename... Rest>
void addToMiniToolBarLayout(QBoxLayout* layout, int first, Rest... rest)
{
  layout->addSpacing(first - LayoutConstants::NarrowHMargin);
  addToMiniToolBarLayout(layout, rest...);
}

template <typename... Rest>
QLayout* createMiniToolBarLayout(QWidget* first, Rest... rest)
{
  auto* layout = new QHBoxLayout{};
  layout->setContentsMargins(
    LayoutConstants::NarrowHMargin, 0, LayoutConstants::NarrowHMargin, 0);
  layout->setSpacing(LayoutConstants::NarrowHMargin);
  addToMiniToolBarLayout(layout, first, rest...);
  layout->addStretch(1);
  return layout;
}

template <typename... Rest>
QLayout* createMiniToolBarLayoutRightAligned(QWidget* first, Rest... rest)
{
  auto* layout = new QHBoxLayout{};
  layout->setContentsMargins(
    LayoutConstants::NarrowHMargin, 0, LayoutConstants::NarrowHMargin, 0);
  layout->setSpacing(LayoutConstants::NarrowHMargin);
  layout->addStretch(1);
  addToMiniToolBarLayout(layout, first, rest...);
  return layout;
}

void setHint(QLineEdit* ctrl, const char* hint);
void centerOnScreen(QWidget* window);

QWidget* makeDefault(QWidget* widget);
QWidget* makeEmphasized(QWidget* widget);
QWidget* makeUnemphasized(QWidget* widget);
QWidget* makeInfo(QWidget* widget);
QWidget* makeSmall(QWidget* widget);
QWidget* makeHeader(QWidget* widget);
QWidget* makeError(QWidget* widget);

QWidget* makeSelected(QWidget* widget, const QPalette& defaultPalette);
QWidget* makeUnselected(QWidget* widget, const QPalette& defaultPalette);

Color fromQColor(const QColor& color);
QColor toQColor(const Color& color);
void setWindowIconTB(QWidget* window);
void setDebugBackgroundColor(QWidget* widget, const QColor& color);

void setDefaultWindowColor(QWidget* widget);
void setBaseWindowColor(QWidget* widget);
void setHighlightWindowColor(QWidget* widget);

QLineEdit* createSearchBox();

void checkButtonInGroup(QButtonGroup* group, int id, bool checked);
void checkButtonInGroup(QButtonGroup* group, const QString& objectName, bool checked);

/**
 * Insert a separating line as the first item in the given layout on platforms where
 * this is necessary.
 */
void insertTitleBarSeparator(QVBoxLayout* layout);

template <typename I>
QStringList toQStringList(I cur, I end)
{
  auto result = QStringList{};
  std::transform(cur, end, std::back_inserter(result), [](const auto& str) {
    return QString::fromStdString(str);
  });
  return result;
}

class AutoResizeRowsEventFilter : public QObject
{
  Q_OBJECT
private:
  QTableView* m_tableView;

public:
  explicit AutoResizeRowsEventFilter(QTableView* tableView);

  bool eventFilter(QObject* watched, QEvent* event) override;
};

void autoResizeRows(QTableView* tableView);
void deleteChildWidgetsLaterAndDeleteLayout(QWidget* widget);

void showModelessDialog(QDialog* dialog);

QString mapStringToUnicode(MapTextEncoding encoding, const std::string& string);
std::string mapStringFromUnicode(MapTextEncoding encoding, const QString& string);

/**
 * Maps one of Qt::META, Qt::SHIFT, Qt::CTRL, Qt::ALT to the
 * label for it on the current OS.
 *
 * @param modifier one of Qt::META, Qt::SHIFT, Qt::CTRL, Qt::ALT
 * @return the native label for this modifier on the current OS
 *         (e.g. "Ctrl" on Windows or the Command symbol on macOS)
 */
QString nativeModifierLabel(int modifier);

template <typename T, size_t S>
QString toString(const vm::vec<T, S>& vec)
{
  return QString{"%L1 %L2 %L3"}.arg(vec.x()).arg(vec.y()).arg(vec.z());
}

namespace detail
{

template <std::floating_point T, size_t S>
std::optional<vm::vec<T, S>> parse(const QStringList& parts, const QLocale& locale)
{
  auto result = vm::vec<T, S>{};
  for (size_t i = 0; i < S; ++i)
  {
    auto ok = false;
    result[i] = static_cast<T>(locale.toDouble(parts[qsizetype(i)], &ok));
    if (!ok)
    {
      return std::nullopt;
    }
  }

  return result;
}

} // namespace detail

template <std::floating_point T, size_t S>
std::optional<vm::vec<T, S>> parse(const QString& str)
{
  const auto parts = str.split(' ', Qt::SkipEmptyParts);
  if (parts.size() != S)
  {
    return std::nullopt;
  }

  if (const auto result = detail::parse<T, S>(parts, QLocale{}))
  {
    return result;
  }

  // try to parse as english format to allow pasting from compiler output and such:
  return detail::parse<T, S>(parts, QLocale::c());
}

template <std::integral T, size_t S>
std::optional<vm::vec<T, S>> parse(const QString& str)
{
  const auto parts = str.split(' ', Qt::SkipEmptyParts);
  if (parts.size() != S)
  {
    return std::nullopt;
  }

  const auto locale = QLocale{};
  auto result = vm::vec<T, S>{};
  for (size_t i = 0; i < S; ++i)
  {
    auto ok = false;
    result[i] = static_cast<T>(locale.toLong(parts[qsizetype(i)], &ok));
    if (!ok)
    {
      return std::nullopt;
    }
  }
  return result;
}

} // namespace tb::ui
