#ifndef COLLADACPP_RENDERERGL_HPP_
#define COLLADACPP_RENDERERGL_HPP_

#include <GL/glew.h>

#include "../Render/Renderer.hpp"
#include "../Collada/Phong.hpp"
#include "../Render/RendererGL_DevIL.hpp"
#include "../Render/GLMatrixStack.hpp"
#include "../Render/GLSLShaderManager.hpp"
#include "../Render/GLSLShader.hpp"

class RendererGL : public Renderer {
   public:
      virtual void init();
      virtual void preFrame();
      virtual void postFrame();
      virtual void setCameraMatrix(Camera* camera);
      virtual void setSize(const int width, const int height);

      virtual void render(Renderable* renderable);
      virtual void render(GameObject* gameObject);
      virtual void render(ColladaMesh* colladaMesh);
      virtual void render(Area* area);
      virtual void render(Collada* collada);
      virtual void render(ColladaObject* colladaObject);
      virtual void render(Scene* scene);
      virtual void render(VisualScene* vs);
      virtual void render(ColladaNode* node);
      virtual void render(Position* position);
      virtual void render(RotationGL* rotation);
      virtual void render(Scale* scale);
      virtual void render(Camera* camera);
      virtual void render(Grid* grid);
      virtual void render(Geometry* geometry);
      virtual void render(InstanceGeometry* ig);
      virtual void render(GeometricPrimitive* geometry);
      virtual void render(Triangles* triangles);
      virtual void render(Material* material);
      virtual void render(ColladaLitShader* lit);
      virtual void render(Phong* phong);
      virtual void render(Lambert* lambert);
      virtual void render(TestRenderable* tr);
      virtual void render(Octree* octree);
      virtual void render(BlockChunk* blockchunk);

      int debugPrimDraw;

   private:
      void renderDefaultMaterial_();
      void fixAxis_(const Collada* collada);
      void renderAxis_();
      void renderCube_(const float& size);
      void setPerspective_();
      void setPolygonMode_();
      void setWireframeMode_();
      void setRenderMode_();
      void setUnlitMode_();
      void setLights_();
      void renderOctreeNode_(Octree* octree);
      void bindShader_(const GLSLShaderPtr& shader);

      Phong defaultMaterial_;
      int width_;
      int height_;
      bool glewInit_;
      RendererGL_DevIL imageLoader_;
      GLMatrixStack stack_;
      glm::mat4 projection_matrix_;
      GLSLShaderManager shader_manager_;
      light lights_[8];
};

#endif /* COLLADACPP_RENDERERGL_HPP_ */
