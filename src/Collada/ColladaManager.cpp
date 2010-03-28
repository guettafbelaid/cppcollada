#include "../Collada/ColladaManager.hpp"

#include "../Collada/Collada.hpp"
#include "../Collada/ColladaDoc.hpp"
#include "../Collada/Scene.hpp"
#include "../Collada/VisualScene.hpp"

/**
 * Gets a Collada object from a URL.
 * @param url No remote urls, only 'file://somedir/file.dae' or '/somedir/file.dae'.
 * @return A Collada object.
 */
shared_ptr<Collada> ColladaManager::getCollada(const string& url) {
   // See if the collada is already loaded...
   ColladaMapIterator iter = colladas_.find(url);
   if(iter != colladas_.end()) {
      return iter->second;
   }

   shared_ptr<ColladaDoc> colladaDoc(colladaDocManager_.getColladaDoc(url));
   shared_ptr<Collada> collada(colladaDoc->getCollada());
   
   colladas_.insert(ColladaMapPair(url, collada));
   return collada;
}

/**
 * Unloads raw COLLARA XML documents from memory. Doesn't effect loaded
 * Collada objects. Usefull to call after batch loading a bunch of .dae.
 */
void ColladaManager::scrub() {
   colladaDocManager_.unloadColladaDocs();
}