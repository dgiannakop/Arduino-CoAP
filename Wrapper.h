#ifndef WRAPPER_H
#define WRAPPER_H

class Coap;

class Wrapper
{
   public:
      static void setObj(Coap & obj) { fObj = &obj; }
      static void timerInterrupt( void );
      static void observeTimerInterrupt( void );
   private:
      static Coap *fObj;
};
#endif
