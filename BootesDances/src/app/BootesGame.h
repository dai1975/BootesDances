#ifndef BOOTESDANCES_GAMEIMPL_H
#define BOOTESDANCES_GAMEIMPL_H

#include "../include.h"
#include <bootes/dances/IBootesGame.h>
#include "../view/RootView.h"

struct BootesGameOption
{
   std::basic_string< TCHAR > datadir;
   bool editable;

   inline BootesGameOption() {
      datadir = _T("");
      editable = false;
   }
   inline BootesGameOption(const BootesGameOption& r) { operator=(r); }
   inline BootesGameOption& operator=(const BootesGameOption& r) {
      datadir = r.datadir;
      editable = r.editable;
      return *this;
   }
};

class BootesGame: public IBootesGame
{
public:
   explicit BootesGame(const BootesGameOption& opt);
   virtual ~BootesGame();

   virtual IStageManager* getStageManager();
   inline virtual const TCHAR* getUserDir() const { return _opt.datadir.c_str(); }

public:
   virtual void onUpdate(const ::bootes::lib::framework::GameTime*);

private:
   const ::bootes::lib::framework::WiimoteEvent* filter(const ::bootes::lib::framework::WiimoteEvent*);
   ::bootes::lib::framework::WiimoteEvent _filter_lowpass[1];
   
private:
   enum STATE {
      S_0, S_RUN,
   };
   STATE m_state;
   BootesGameOption _opt;
   IStageManager* _pStageManager;
};

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
