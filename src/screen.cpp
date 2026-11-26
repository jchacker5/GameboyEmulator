#include "screen.h"

#include <QKeyEvent>
#include <QPainter>
#include <stdexcept>

namespace {
constexpr int kScale = 3;
const QRgb shades[] = {qRgb(255, 255, 255), qRgb(160, 160, 160), qRgb(80, 80, 80), qRgb(0, 0, 0)};
}

Screen::Screen(QWidget* parent)
    : QWidget(parent), image_(kScreenWidth, kScreenHeight, QImage::Format_RGB32) {
    image_.fill(shades[0]);
    setWindowTitle("Gameboy");
    setFixedSize(kScreenWidth * kScale, kScreenHeight * kScale);
}

void Screen::setFrame(const Frame& frame) {
    // Validate first so an invalid frame cannot leave a partially updated display.
    for (auto shade : frame) if (shade > 3) throw std::out_of_range("invalid framebuffer shade");
    for (int y = 0; y < kScreenHeight; ++y) {
        auto* row = reinterpret_cast<QRgb*>(image_.scanLine(y));
        for (int x = 0; x < kScreenWidth; ++x) row[x] = shades[frame[y * kScreenWidth + x]];
    }
    update();
}

void Screen::paintEvent(QPaintEvent*) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::SmoothPixmapTransform, false);
    painter.drawImage(rect(), image_);
}

bool Screen::saveFrame(const char* path) const {
    return image_.scaled(width(), height(), Qt::IgnoreAspectRatio, Qt::FastTransformation).save(path);
}

void Screen::keyPressEvent(QKeyEvent* event) { keydown(event->nativeScanCode()); }
void Screen::keyReleaseEvent(QKeyEvent* event) { keyup(event->nativeScanCode()); }
