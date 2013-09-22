#include "CeguiTextureImage.h"
#include <iostream>

namespace bootes { namespace dances { namespace editor {

CeguiTextureImage::CeguiTextureImage()
{
   {
      uintptr_t a = reinterpret_cast< uintptr_t >(this);
      std::ostringstream o;
      o << "CeguiTextureImage-" << a;
      _is_name = o.str();
   }
   _pCeguiTex = NULL;
   _pSceneSeq = NULL;
}

CeguiTextureImage::~CeguiTextureImage()
{
   for (t_items::iterator i = _items.begin(); i != _items.end(); ++i) {
      Item* pi = i->second;
      delete pi;
   }
}

void CeguiTextureImage::onLostDevice()
{
}

void CeguiTextureImage::onResetDevice()
{
}

void CeguiTextureImage::onInitDevice()
{
   try {
      CEGUI::Direct3D9Renderer* pr = static_cast< CEGUI::Direct3D9Renderer* >
         (CEGUI::System::getSingleton().getRenderer());

      // これの delete は renderer がやってくれる
      _pCeguiTex = static_cast< CEGUI::Direct3D9Texture* >(&pr->createTexture());

      CEGUI::ImagesetManager::getSingleton().create(_is_name.c_str(), *_pCeguiTex);
      _items.clear();

   } catch (CEGUI::Exception& e) {
      const char* msg = e.getMessage().c_str();
      const char* filename = e.getFileName().c_str();
      const char* exname = e.getName().c_str();
      int line = e.getLine();
      DebugBreak();
   }
}

bool CeguiTextureImage::addLook(std::string* id_)
{
   Item* pi = new Item();
   std::string id = g_pFnd->toString((void*)pi);

   {
      t_items::iterator i = _items.find(id);
      if (i != _items.end()) {
         DebugBreak();
         delete pi;
         return false;
      }

      i = _items.insert( i, t_items::value_type(id, pi) );
      pi->im_name = id;
   }

   {
      std::ostringstream o;
      o << "set:" << _is_name << " image:" << pi->im_name;
      pi->setter_string = o.str();
   }
   *id_ = id;
   return true;
}

const CEGUI::Size& CeguiTextureImage::getSize() const { return _size; }
__int64            CeguiTextureImage::getTime() const { return _time; }

void CeguiTextureImage::onRender(const ::bootes::lib::framework::GameTime* gt)
{
   if (_pSceneSeq == NULL) {
      IStageManager* p = g_pGame->getStageManager();
      if (p) { _pSceneSeq = p->getSceneSequencer(); }
   }

   if (_pSceneSeq == NULL) { return; }
   {
      Scene scene = _pSceneSeq->getScene(true);
      if (! scene.isValid()) { return; }

      IDirect3DTexture9* pTex = scene.refTexture();
      _size.d_width  = (float)scene.videoinfo().width;
      _size.d_height = (float)scene.videoinfo().height;
      _time = scene.clock().clock;
      _pCeguiTex->setDirect3D9Texture(pTex);
      _pCeguiTex->setOriginalDataSize(_size);
      if (pTex) {
         pTex->Release();
      }
   }

   CEGUI::Point zero(0,0);
   CEGUI::Imageset& is = CEGUI::ImagesetManager::getSingleton().get(_is_name.c_str());
   t_items::iterator i;
   for (i = _items.begin(); i != _items.end(); ++i) {
      Item* pi = i->second;
      is.undefineImage(pi->im_name.c_str());
      is.defineImage(pi->im_name.c_str(), zero, _size, zero);
   }
}

bool CeguiTextureImage::renderImage(const std::string& im_name, CEGUI::DefaultWindow* pWindow)
{
   t_items::iterator i = _items.find(im_name);
   if (i == _items.end()) { return false; }
   Item* pi = i->second;

   pWindow->setProperty("Image", pi->setter_string.c_str());
   //pWindow->invalidate(); //setProperty("Image", ...) で呼ばれるので不要
   return true;
}

} } }

/**
 * Local Variables:
 * mode: c++
 * coding: utf-8-dos
 * indent-tabs-mode: nil
 * tab-width: 8
 * c-basic-offset: 3
 * End:
 */
