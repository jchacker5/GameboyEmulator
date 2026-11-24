#include "renderer.h"
#include <QApplication>
#include <QImage>
#include <QPainter>
#include <QTimer>
#include <QWidget>
#include <QStringList>
#include <fstream>
#include <iostream>
class Viewer final: public QWidget {
 gb::Video video;
 QImage image{gb::width,gb::height,QImage::Format_RGB32};
 QTimer timer;
 void refresh() {
  const auto frame=gb::render(video);
  constexpr int shades[]={255,170,85,0};
  for(int y=0;y<gb::height;++y) {
   auto* line=reinterpret_cast<QRgb*>(image.scanLine(y));
   for(int x=0;x<gb::width;++x) {int s=shades[frame[y*gb::width+x]]; line[x]=qRgb(s,s,s);}
  }
  update();
 }
 protected:
 void paintEvent(QPaintEvent*) override {
  QPainter painter(this);
  painter.fillRect(rect(),Qt::black);
  auto size=image.size(); size.scale(this->size(),Qt::KeepAspectRatio);
  painter.drawImage(QRect(QPoint((width()-size.width())/2,(height()-size.height())/2),size),image);
 }
 public:
 Viewer(gb::Video v,bool animate):video(v) {
  setWindowTitle("Game Boy | Tile Renderer"); resize(640,576);
  refresh();
  connect(&timer,&QTimer::timeout,this,[this]{video.scrollX=(static_cast<unsigned>(video.scrollX)+1)&255u;refresh();});
  if(animate) timer.start(16);
 }
};
int main(int argc,char** argv) {
 QApplication app(argc,argv);
 try {
  auto args=app.arguments(); bool smoke=args.contains("--smoke"); args.removeAll("--smoke");
  if(args.size()>2) {std::cerr<<"Usage: gameboy_viewer [snapshot] [--smoke]\n";return 2;}
  auto video=gb::demo();
  if(args.size()==2) {std::ifstream in(args[1].toStdString()); if(!in) throw std::runtime_error("cannot open snapshot"); video=gb::load(in);}
  Viewer viewer(video,args.size()==1); viewer.show();
  if(smoke) QTimer::singleShot(100,&app,&QApplication::quit);
  return app.exec();
 } catch(const std::exception& e) {std::cerr<<e.what()<<'\n';return 1;}
}
