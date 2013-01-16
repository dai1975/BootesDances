#ifndef BOOTESDANCES_GAMEIMPL_H
#define BOOTESDANCES_GAMEIMPL_H

#include "../include.h"
#include <bootes/dances/IBootesGame.h>
#include "../view/RootView.h"

class BootesGame: public IBootesGame
{
public:
   explicit BootesGame(const TCHAR* dir, bool editable);
   virtual ~BootesGame();

   virtual IStageManager* getStageManager();
   inline virtual const TCHAR* getUserDir() const { return _dir.c_str(); }

public:
   virtual void onUpdate(double currentTime, int elapsedTime);

private:
   const ::bootes::lib::framework::WiimoteEvent* filter(const ::bootes::lib::framework::WiimoteEvent*);
   ::bootes::lib::framework::WiimoteEvent _filter_lowpass[1];
   
private:
   enum STATE {
      S_0, S_RUN,
   };
   STATE m_state;
   std::basic_string< TCHAR > _dir;
   bool _editable;
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
