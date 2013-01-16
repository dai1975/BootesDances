#include "Motion.h"
#include "WiimoteMotionSimple.h"

Motion* Motion::Realize(const pb::Motion* idea)
{
   Motion* rea = NULL;
   if (idea->has_wiimote_simple()) {
      rea = new WiimoteMotionSimple();
      if (!rea->realize(idea)) {
         delete rea;
         rea = NULL;
      }
   } else {
      return NULL;
   }
   if (rea) {
      rea->setDuration( idea->duration() );
   }
   return rea;
}

pb::Motion* Motion::Idealize(const Motion* rea)
{
   pb::Motion* idea = new pb::Motion();
   switch (rea->getDetectorType()) {
   case Motion::WIIMOTE_SIMPLE:
      if (! rea->idealize(idea)) { goto fail; }
      break;
   }
   if (idea) {
      idea->set_duration(rea->getDuration());
   }
   return idea;

fail:
   delete idea;
   return NULL;
}

Motion::Motion(DEVICE dev, DETECTOR det)
   : _device_type(dev), _detector_type(det)
{
   _duration = 0;
}

Motion::~Motion()
{
}

void Motion::setDuration(int dur)
{
   _duration = dur;
}

int Motion::getDuration() const
{
   return _duration;
}

/**
 * Local Variables:
 * mode: c++
 * coding: utf-8-dos
 * indent-tabs-mode: nil
 * c-basic-offset: 3
 * tab-width: 8
 * End:
 */
