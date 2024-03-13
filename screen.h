/*Defines some qt graphical objects
Margaret Black, 10/2014*/

#include <QGraphicsItem>
#include <QGraphicsView>
#include <QTimer>
#include <QWidget>
#include <QKeyEvent>
#include <QApplication>

class QGraphicsScene;


//A window background that is responsive to mouse clicks
class ScreenView : public QGraphicsView
{
	Q_OBJECT
public:
	ScreenView(QGraphicsScene*, QWidget*x=  NULL);
protected:
	void keyPressEvent(QKeyEvent* event);
	void keyReleaseEvent(QKeyEvent* event);
};

//a single grid square
class Square : public QGraphicsItem
{
public:
	//new Square(3,5) defines a square 3 over and 5 down
	Square();
	QRectF boundingRect() const;
	void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
	int xpos,ypos,xcoor1,ycoor1,xcoor2,ycoor2;
	int shade;
};

int setup(int argc,char** argv);
void updateSquare(int,int,int);
void keydown(int);
void keyup(int);
void onFrame();