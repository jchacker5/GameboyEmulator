#include "screen.h"
#include <QApplication>
#include <QTemporaryDir>
#include <cstdlib>
#include <iostream>
#include <stdexcept>

#define CHECK(condition) do { if (!(condition)) { \
    std::cerr << __FILE__ << ':' << __LINE__ << ": " << #condition << '\n'; std::exit(1); \
} } while (0)

void keydown(int) {}
void keyup(int) {}

int main(int argc, char** argv) {
    QApplication app(argc, argv);
    Screen screen;
    Frame frame{};
    for (int y = 0; y < kScreenHeight; ++y)
        for (int x = 0; x < kScreenWidth; ++x) frame[y * kScreenWidth + x] = (x + y) % 4;
    screen.setFrame(frame);
    const int greys[] = {255, 160, 80, 0};
    for (int y = 0; y < kScreenHeight; ++y)
        for (int x = 0; x < kScreenWidth; ++x)
            CHECK(screen.image().pixel(x, y) == qRgb(greys[(x + y) % 4], greys[(x + y) % 4], greys[(x + y) % 4]));

    const auto expected = screen.image().scaled(480, 432, Qt::IgnoreAspectRatio, Qt::FastTransformation);
    QImage painted(screen.size(), QImage::Format_RGB32);
    painted.fill(Qt::red);
    screen.render(&painted);
    CHECK(painted == expected); // catches per-pixel rectangle outlines and paint/export drift
    QTemporaryDir directory;
    CHECK(directory.isValid());
    const auto path = directory.filePath("frame.png");
    CHECK(screen.saveFrame(path.toLocal8Bit().constData()));
    CHECK(QImage(path).convertToFormat(QImage::Format_RGB32) == expected);
    CHECK(!screen.saveFrame(directory.filePath("missing/frame.png").toLocal8Bit().constData()));
    frame[42] = 4;
    bool rejected = false;
    try { screen.setFrame(frame); } catch (const std::out_of_range&) { rejected = true; }
    CHECK(rejected);
    CHECK(screen.image().scaled(480, 432) == expected);
    std::cout << "Screen: native pixels, widget paint and PNG match exactly\n";
}
