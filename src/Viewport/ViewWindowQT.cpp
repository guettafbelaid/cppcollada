#include "ViewWindowQT.hpp"
#include <QtGui>
#include <QtOpenGL>


#include "../Debug/console.h"

#include "../GameData/Grid.hpp"
#include "../GameObjects/Camera.hpp"
#include "../Collada/Collada.hpp"
#include "../Render/ColladaRendererGL.hpp"

#include "../QTGui/QTEditScene.hpp"
#include "../QTGui/QTEditGeneric.hpp"
#include "../GameObjects/Area.hpp"

OpenGLScene::OpenGLScene(ViewWindowQT* vwqt) {
   grid_ = shared_ptr<Grid>(new Grid);
   vwqt_ = vwqt;
   renderer_.init();
   renderer_.setSize(vwqt_->getWidth(), vwqt_->getHeight());

   QWidget *editCollada = new QDialog(0, Qt::CustomizeWindowHint | Qt::WindowTitleHint);
   editCollada->setWindowTitle("Edit Collada");
   editCollada->setLayout(new QVBoxLayout);
   
   htmlOverlay = new QWebView();
   htmlOverlay->load(QUrl("Data/html/overlay.html"));
   QPalette transp = htmlOverlay->palette();
   transp.setBrush(QPalette::Base, Qt::transparent);
   htmlOverlay->setPalette(transp);
   htmlOverlay->page()->setPalette(transp);
   htmlOverlay->show();
   
   
   QPushButton* editColladaButton = new QPushButton(tr("Edit Collada"));
   editCollada->layout()->addWidget(editColladaButton);
   
   connect(editColladaButton, SIGNAL(clicked()), this, SLOT(editCollada()));
   addOverlayedWidget(editCollada, false, false);
   addOverlayedWidget(htmlOverlay, true, true);

   setSize(vwqt_->getWidth(), vwqt_->getHeight());

   time_.start();
}

void OpenGLScene::setSize(const int width, const int height) {
   // TODO: This doesn't work (maybe set proxy size?)
   htmlOverlay->setContentsMargins(0, 0, width, height);
   htmlOverlay->setMinimumSize(QSize(width, height));
   htmlOverlay->setMaximumSize(QSize(width, height));
}

void OpenGLScene::editCollada() {
   DEBUG_A("Oh boy!, PANCAKES!");
   #warning ['TODO']: Update me for new game object based render system...
   /*newEditColladas = new QTEditColladas(vwqt_->getColladaManager());
   addOverlayedWidget((QWidget*)newEditColladas);*/
}

void OpenGLScene::addOverlayedWidget(QWidget* widget, bool disableFrame, bool transparentBackground) {
   DEBUG_M("Entering function... %p", widget);
   if(!widget) {
      return;
   }
   //widget->setWindowOpacity(0.8);

   QGraphicsProxyWidget *proxy = new QGraphicsProxyWidget(0, Qt::Dialog);

   if(disableFrame) {
      proxy->setWindowFlags(Qt::WindowFlags(Qt::FramelessWindowHint));
   }

   if(transparentBackground) {
      QPalette transp(palette()); 
      transp.setColor (QPalette::Window, Qt::transparent);  
      proxy->setPalette(transp);
      proxy->setAttribute(Qt::WA_OpaquePaintEvent, false);
      proxy->setAutoFillBackground(false);
   }

   proxy->setWidget(widget);
   addItem(proxy);

   //TODO: For some reason setting this to 0 makes the html overlay move to the right side of the screen.
   QPointF pos(1, 1);
   foreach (QGraphicsItem *item, items()) {
      item->setFlag(QGraphicsItem::ItemIsMovable);
      item->setCacheMode(QGraphicsItem::DeviceCoordinateCache);

      const QRectF rect = item->boundingRect();
      item->setPos(pos.x() - rect.x(), pos.y() - rect.y());
      pos += QPointF(0, 10 + rect.height());
   }

   QTEditGeneric* qtedit = dynamic_cast<QTEditGeneric*>(widget);
   if(qtedit) {
      qtedit->setOpenGLScene(this);
   }
}

void OpenGLScene::drawBackground(QPainter *painter, const QRectF &) {
   if (painter->paintEngine()->type() != QPaintEngine::OpenGL
      && painter->paintEngine()->type() != QPaintEngine::OpenGL2)
   {
      static bool nospam = false;
      if(!nospam) {
         WARNING("No OpenGL viewport widget set!");
      }
      nospam = true;
      return;
   }

   static int frame = 0;
   static int last_render_time = 0;
   static int last_fps_time = 0;
   static int fps_ = 0;
   static int mpf_ = 0;
   
   int current_time = time_.elapsed();

   // Limit framerate
   /*if(limit_fps_ && !( (current_time - last_render_time) >= 1000/60) ) {
      return;
   }*/

   // Calculate FPS
   frame++;
   if(current_time - last_fps_time > 1000) {
      fps_ = frame*1000.0f/(current_time-last_fps_time);
      last_fps_time = current_time;
      frame = 0;
      LOG("FPS: %d,\tMPF: %d",fps_, mpf_);
   }
   last_render_time = current_time;

   painter->beginNativePainting();

   renderer_.preFrame();

   CameraPtr camera = vwqt_->getCamera();
   shared_ptr<Area> area;

   if(camera != CameraPtr()) {
      camera->setCamera();
      area = camera->getArea();
   }

   if(area != shared_ptr<Area>()) {
      area->render();
   }

   grid_->render();

   renderer_.postFrame();

   painter->endNativePainting();

   QTimer::singleShot(20, this, SLOT(update()));
   int end_time = time_.elapsed();
   mpf_ = end_time - current_time;
}

ViewWindowQT::ViewWindowQT(const int width, const int height): ViewWindow(width, height) {
   DEBUG_M("ViewWindowQT spawning...");

   int argc = 1;
   char* argv[] = {"No Window Title"};
   //QApplication::setStyle(QStyleFactory::create("motif"));
   app_ = new QApplication(argc, argv);
   scene_ = new OpenGLScene(this);
   vw_ = new ViewWidget(this);
   vw_->setViewport(new QGLWidget(QGLFormat(QGL::SampleBuffers)));
   vw_->setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
   vw_->setScene(scene_);
   vw_->resize(getWidth(), getHeight());
   vw_->show();
}

ViewWindowQT::~ViewWindowQT() {
   DEBUG_M("Cleaning up ViewWindowQT...");

   delete vw_;
   delete scene_;
   delete app_;
}

void ViewWindowQT::setTitle(const string title) {
   if(vw_) {
      vw_->setWindowTitle(title.c_str());
   }
}

OpenGLScene* ViewWindowQT::getOpenGLScene() {
   return scene_;
}

/**
 * The main rendering and input loop.
 */
void ViewWindowQT::mainLoop() {
   DEBUG_L("Interface::mainLoop()");
   if(app_) {
      app_->exec();
   }
   DEBUG_M("Finished...");
}

ColladaRenderer* ViewWindowQT::getRenderer() {
   return &scene_->renderer_;
}

void ViewWindowQT::quit() {
   app_->quit();
}

ViewWidget::ViewWidget(ViewWindowQT* vwqt) {
   vwqt_ = vwqt;
   mouseDown_ = false;
}

void ViewWidget::resizeEvent(QResizeEvent *event) {
   if (scene())
      scene()->setSceneRect(QRect(QPoint(0, 0), event->size()));
   QGraphicsView::resizeEvent(event);

   vwqt_->getRenderer()->setSize(event->size().width(), event->size().height());
   vwqt_->getOpenGLScene()->setSize(vwqt_->getWidth(), vwqt_->getHeight());
}

void ViewWidget::mouseMoveEvent(QMouseEvent *event) {
   DEBUG_H("Fired...");
   // Check if the event is handled by the overdrawn QT widgets
   if(!mouseDown_) {
      QGraphicsView::mouseMoveEvent(event);
      if(event->isAccepted()) {
         return;
      }
   }

   int x = event->x();
   int y = event->y();
   float xrel = x-oldx_;
   float yrel = y-oldy_;

   event->accept();

   CameraPtr camera = vwqt_->getCamera();  
   if(camera == CameraPtr()) {
      WARNING("NO CAMERA!");
      return;
   }
   camera->setRotX(camera->getRotX() + xrel / vwqt_->getWidth() * 100);
   camera->setRotY(camera->getRotY() - yrel / vwqt_->getHeight() * 100);
   update();

   oldx_ = x;
   oldy_ = y;
}

void ViewWidget::mousePressEvent(QMouseEvent *event) {
   DEBUG_H("Fired...");
   // Check if the event is handled by the overdrawn QT widgets
   QGraphicsView::mousePressEvent(event);
   if(event->isAccepted()) {
      return;
   }

   mouseDown_ = true;
   oldx_ = event->x();
   oldy_ = event->y();
   event->accept();
}

void ViewWidget::mouseReleaseEvent(QMouseEvent *event) {
   DEBUG_H("Fired...");
   mouseDown_ = false;
   // Check if the event is handled by the overdrawn QT widgets
   QGraphicsView::mouseReleaseEvent(event);
   if(event->isAccepted()) {
      return;
   }
   
}

void ViewWidget::wheelEvent(QWheelEvent * event) {
   // Check if the event is handled by the overdrawn QT widgets
   /*QGraphicsView::wheelEvent(event);
   if(event->isAccepted()) {
      return;
   }*/

   int numDegrees = event->delta() / 8;
   int numSteps = numDegrees / 15;

   if(event->orientation() == Qt::Horizontal) {
      // left/right scrolling...
   } else {
      CameraPtr camera = vwqt_->getCamera();
      camera->setZoom(camera->getZoomTarget() + ZOOM_STEP * -numSteps);
   }
   event->accept();
   update();
}

void ViewWidget::keyPressEvent(QKeyEvent * event) {
   // Check if the event is handled by the overdrawn QT widgets
   QGraphicsView::keyPressEvent(event);
   if(event->isAccepted()) {
      return;
   }
   if(event->matches(QKeySequence::Quit) ||
      event->key() == Qt::Key_Q ||
      event->key() == Qt::Key_Escape) {
      DEBUG_M("Quit key pressed...");
      vwqt_->quit();
      return;
   }
}

void ViewWidget::keyReleaseEvent(QKeyEvent * event) {
   // Check if the event is handled by the overdrawn QT widgets
   QGraphicsView::keyReleaseEvent(event);
   if(event->isAccepted()) {
      return;
   }
}
