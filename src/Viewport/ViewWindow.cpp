#include "ViewWindow.hpp"
#include "../Collada/Collada.hpp"
#include "../GameData/Camera.hpp"

#include "../Debug/console.h"

ViewWindow::ViewWindow(const int width, const int height) {
   camera_ = shared_ptr<Camera>();
   setSize(width, height);
}

void ViewWindow::setSize(const int width, const int height) {
   width_ = width;
   height_ = height;
}

int ViewWindow::getWidth() {
   return width_;
}

int ViewWindow::getHeight() {
   return height_;
}

void ViewWindow::setCamera(shared_ptr<Camera> camera) {
   camera_ = camera;
   camera_->setRenderer(getRenderer());
}

shared_ptr<Camera> ViewWindow::getCamera() {
   return camera_;
}

void ViewWindow::setCollada(shared_ptr<Collada> collada) {
   collada_ = collada;
   collada_->setRenderer(getRenderer());
   /*if(getCamera()) {
      shared_ptr<Position> position = static_pointer_cast<Position, Collada>(collada);
      getCamera()->setTarget(position);
   }*/
}

shared_ptr<Collada> ViewWindow::getCollada() {
   return collada_;
}