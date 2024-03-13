//#include <iostream>
//#include <QtGui>
//#include <QtWidgets>
#include <QApplication>
#include "screen.h"

using namespace std;

//some global vars:
//squares wide
const int GRIDWIDTH=160;
//squares tall
const int GRIDHEIGHT=144;
//the window is this many pixels wide
const int SCREENWIDTH=160*3;
//and pixels tall
const int SCREENHEIGHT=144*3;
//an array of clickable grid squares
Square* square;
int pixels[GRIDWIDTH][GRIDHEIGHT];
//the window background
QGraphicsScene* thescene;
ScreenView* view;
QApplication* app;

void updateSquare(int x, int y, int c)
{
    pixels[x][y]=c;
    //	square[x][y]->shade=c;
    //	square[x][y]->update();
    //app->processEvents();
}

int fc=0;
void onFrame()
{
    thescene->update();
    fc++;
    if(fc==5)
    {
        app->processEvents();
        fc=0;
    }
}

//constructor for a grid square
Square::Square()
{
}

//location of the square on the screen, used for rendering
QRectF Square::boundingRect() const
{
    //	return QRectF(xcoor1,ycoor1,xcoor2-xcoor1,ycoor2-ycoor1);
    return QRectF(0,0,SCREENWIDTH,SCREENHEIGHT);
}

//render the square on the screen
void Square::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    int dw=SCREENWIDTH/GRIDWIDTH;
    int dh=SCREENHEIGHT/GRIDHEIGHT;
    QColor color[4];
    color[0].setBlue(255); color[0].setRed(255); color[0].setGreen(255);
    color[1].setBlue(160); color[1].setRed(160); color[1].setGreen(160);
    color[2].setBlue(80); color[2].setRed(80); color[2].setGreen(80);
    color[3].setBlue(0); color[3].setRed(0); color[3].setGreen(0);
    for (int x=0; x<GRIDWIDTH; x++)
    {
        for(int y=0; y<GRIDHEIGHT; y++)
        {
            painter->setBrush(color[pixels[x][y]]);
            painter->drawRect(dw*x,dh*y,dw,dh);
        }
    }
}

ScreenView::ScreenView(QGraphicsScene *scene, QWidget* parent):QGraphicsView(scene, parent)
{
}

void ScreenView::keyPressEvent(QKeyEvent* event)
{
    //	keydown(event->nativeScanCode());
}
void ScreenView::keyReleaseEvent(QKeyEvent* event)
{
    //	keyup(event->nativeScanCode());
}

int setup(int argc, char** argv)
{
    app=new QApplication(argc,argv);

    thescene=new QGraphicsScene();
    thescene->setSceneRect(0,0,SCREENWIDTH+100,SCREENHEIGHT+100);

    //add in all the squares
    square = new Square;
    thescene->addItem(square);

    view=new ScreenView(thescene);
    view->setWindowTitle("Gameboy");
    view->resize(SCREENWIDTH+100,SCREENHEIGHT+100);
    view->show();

    //make the window visible.  we're done with setup.
    //	app->exec();
    app->processEvents();

    return 0;

}
