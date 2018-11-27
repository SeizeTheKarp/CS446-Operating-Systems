// Timer.h

#ifndef TIMER_HPP
#define TIMER_HPP

#include <ctime>
#include <sys/time.h>
#include <stdexcept>
#include <iostream>

using namespace std;


class Timer {
  public:
    Timer();
    void start() throw (runtime_error);
    void stop() throw (logic_error);
    double getElapsedTime() const throw (logic_error);
    void lap() throw (logic_error);


  	struct timeval beginTime, duration;

  private:
    bool timerWasStarted;
};


#endif	// ifndef TIMER_H
