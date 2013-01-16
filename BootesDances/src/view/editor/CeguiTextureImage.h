#ifndef BOOTESDANCES_VIEW_CEGUI_TEXTUREIMAGE_H
#define BOOTESDANCES_VIEW_CEGUI_TEXTUREIMAGE_H

#include "../../include.h"
#include <bootes/dances/IStageManager.h>
#include <map>
#include <string>

#include <CEGUI.h>
#include <RendererModules/Direct3D9/CEGUIDirect3D9Renderer.h>
#include <RendererModules/Direct3D9/CEGUIDirect3D9Texture.h>

namespace bootes { namespace dances { namespace editor {

/*
 D3DTexture と CEGUI::Imageset, CEGUI::DefaultWindow を結びつける。
 1. あらかじめ使用するイメージを登録しておく。
    obj.addLook(name);
 2. テクスチャが更新されたら update してもらう。
    obj.onRender(int, int);
 3. 与えられたウィンドウに指定されたルックを描画する
    obj.renderWindow(name, win);
*/
class CeguiTextureImage
{
public:
   explicit CeguiTextureImage();
   ~CeguiTextureImage();

   bool addLook(std::string* id);
   const CEGUI::Size& getSize() const;
   __int64 getTime() const;
   bool renderImage(const std::string& id, CEGUI::DefaultWindow* pWindow);

public:
   void onRender(double currentTime, int elapsedTime);
   void onLostDevice();
   void onInitDevice();
   void onResetDevice();

private:
   bool createImageset();

   ISceneSequencer* _pSceneSeq;
   std::string _is_name;
   CEGUI::Direct3D9Texture* _pCeguiTex;
   CEGUI::Size _size;
   __int64 _time;

   struct Item {
      std::string im_name;
      std::string setter_string;
   };
   typedef std::map< std::string, Item* > t_items;
   t_items _items;
};

} } }

#endif

/**
 * Local Variables:
 * mode: c++
 * coding: utf-8-dos
 * indent-tabs-mode: nil
 * tab-width: 8
 * c-basic-offset: 3
 * End:
 */
