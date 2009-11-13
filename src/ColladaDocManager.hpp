#ifndef COLLADACPP_COLLADADOCMANAGER_HPP_
#define COLLADACPP_COLLADADOCMANAGER_HPP_

#include <map>
#include <string>
using namespace std;

#include "SmartPointers.hpp"

#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/dom/DOM.hpp>
#include <xercesc/sax/HandlerBase.hpp>
#include <xercesc/util/XMLString.hpp>
#include <xercesc/util/PlatformUtils.hpp>
using namespace xercesc;

class VisualScene;
class ColladaDoc;
class Scene;
class ColladaObject;

typedef map<string, shared_ptr<ColladaDoc>> ColladaDocMap;
typedef ColladaDocMap::iterator ColladaDocMapIterator;
typedef pair<string, shared_ptr<ColladaDoc>> ColladaDocMapPair;

/**
 * 
 */
class ColladaDocManager {
   public:
      shared_ptr<ColladaDoc> getColladaDoc(string url);
      void unloadColladaDocs();
      shared_ptr<ColladaObject> getColladaObjectByUrl(string url);

   private:
      ColladaDocMap colladaDocs_;
};

#endif /* COLLADACPP_COLLADADOCMANAGER_HPP_ */
