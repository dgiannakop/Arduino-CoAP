#include "Wrapper.h"
#include "coap.h"

Coap* Wrapper::fObj=NULL;

void Wrapper::timerInterrupt()
{
   fObj->coap_retransmit_loop();
}
void Wrapper::observeTimerInterrupt()
{
   #ifdef OBSERVING
      fObj->coap_notify_from_timer();
   #endif
}
