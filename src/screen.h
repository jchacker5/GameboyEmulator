#pragma once

// Qt display derived from the course screen helper (Margaret Black, 10/2014).
// The window and PNG export share one framebuffer and automatic ownership.
#include <QImage>
#include <QWidget>
#include "video.h"

class Screen final : public QWidget {
public:
    explicit Screen(QWidget* parent = nullptr);
    void setFrame(const Frame& frame);
    bool saveFrame(const char* path) const;
    const QImage& image() const { return image_; }

protected:
    void paintEvent(QPaintEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;
    void keyReleaseEvent(QKeyEvent* event) override;

private:
    QImage image_;
};

void keydown(int);
void keyup(int);
