// Timer.cpp
/**
	@file Timer.cpp

	@brief this file is the implementation of the Timer class

	@version Original Code 1.0.0 (09/24/2017) - Nathan Smith
 */

// Program Description/Support ////////////////////////////////////////////////
/**
	@mainpage
	   This file is the implementation of the Timer class
*/
// Header Files ///////////////////////////////////////////////////////////////
//
	#include "Timer.hpp"
//

// Class/Data Structure Member Implementation /////////////////////////////////
//
// Timer Constructor //////////////////////////////////////////////////////////
//	
	/**
		This is a constructor for the Timer class.
		
		@post
		   -# duration is initialized to 0
		   -# timerWasStarted is initialized to false
	*/
		Timer::Timer()
		{
			timerWasStarted = false;
		}
//
// start() ////////////////////////////////////////////////////////////////////
// 
	/**
		start() records the current time. will throw an exception if recorded 
		time is negative
	
		@pre
		   -# timeWasStarted is false
		   -# gettimerofday() works correctly
		   -# durration is 0
		@post
		   -# beginTime struct is initialized to current time
		   -# timWasStarted set to true
		@exception runtime_error if time returned is negative, "gettimeofday()
		   returned -1. Serious problem. Can't run timer."
	 */
		void Timer::start() throw (runtime_error) 
		{
			if (timerWasStarted == true)
			{
				throw runtime_error("gettimeofday() was already started");
			} else {
		    	gettimeofday(&beginTime, NULL);
		    	timerWasStarted = true;
		    }
		}
//
// stop() /////////////////////////////////////////////////////////////////////
//
	/**
		stop() records the current time then calculates duation. Will throw an exception if timer was never started.

		@pre
		   -# timerWasStarted set to true
		@post
		   -# duration struct initialized to current time
		@detail @balgorithm
		   -# value stored in beginTime is multiplied by 1000000 then the 
		      product is added to the origial value ad sotred in t2
		   -# value stored in endTime is nultiplied by 1000000 then the 
		   	  product is added to the original value and stored in t1
		   -# t2 is subracted from t1 and stored in duration
		@exception logic_error is thrown if timer was never started
	*/
		void Timer::stop() throw (logic_error)
		{
		    if(timerWasStarted == false)
		    {
				throw logic_error("Timer was never started.");
		    } else {
		    	gettimeofday(&duration, NULL);
		    	timerWasStarted = false;
		    }
		    
		}
//
// getElapsedTime() ///////////////////////////////////////////////////////////
//
	/**
		getElapsedTime() returns the value of duration. exception is thrown if duration is 0 or timer was never started

		@pre
		   -# duration has been calculated
		   -# timerWasStarted is set to false
		@post 
		   -# duration is returned		
		@exception logic_error is thrown if timer was not stopped
		@exception logic_error is thrown if duration was not calculated
	*/
		double Timer::getElapsedTime() const throw (logic_error)
		{
			
				double time = 0;
		    	long long t1, t2;
		    	t2 = beginTime.tv_sec * 1000000.0 + beginTime.tv_usec;
		    	t1 = duration.tv_sec * 1000000.0 + duration.tv_usec;
		    	time = t1 - t2;
		    	return time/1000000;

			
		}

		void Timer::lap() throw (logic_error)
		{
		    if(timerWasStarted == false)
		    {
				throw logic_error("Timer was never started.");
		    } else {
		    	gettimeofday(&duration, NULL);
		    }
		    
		}

//







