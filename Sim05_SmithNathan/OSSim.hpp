//
//  OSSim.hpp
//  Sim05
//
//  Created by Nathan Smith on 5/9/18.
//  Copyright © 2018 Nathan Smith. All rights reserved.
//

#ifndef OSSIM_HPP
#define OSSIM_HPP

#include <fstream>
#include <stdexcept>
#include <cstring>
#include <iostream>
#include <queue>
#include <string>
#include <iomanip>
#include <map>
#include <regex>
#include <sstream>
#include <vector>
#include <pthread.h>
#include "Timer.hpp"

//Global Variables
	
	extern int COUNT_DOWN_TIME;
	static pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;
	extern bool quantumFlag;
	extern bool loadFlag;
//

/**
 * @brief      class for Configuration Data
 * @details    Abstract data type for collection and storing Configuartion	
 * 				Data information	
 */
class ConfigSettings
{
	public:

		//Constructor

			ConfigSettings();	
		
		//Functions

			bool checkFilePath(const std::string& path); 
			
			void loadSettings(const std::string& filePath) throw(std::logic_error); 
			
			int getData(std::string& key) throw(std::logic_error); 

		//Variables for public use

			std::string metaFile, 
						logFile, 
						outputType, 
						memType,
						type;
			
			
			int memSize, 
				blockSize, 
				projQuan, 
				hdQuan, 
				quantum;
			
			double version;
	private:

		std::map<std::string, int> configData;
		std::vector<std::string> key, matchCriteria;
};

/**
 * @brief      class for meta data
 * @details    Abstract data type for collection and storing Meta Data 
 * 				information	
 */
class MetaSettings //set up before output
{
	public:

		//Constructor
			
			MetaSettings(); //default constructor
		
		//Functions

		  	bool checkFilePath(const std::string& path);
			
			void loadData(std::string& line) throw(std::logic_error);

			void loadVec() throw(std::logic_error);

			bool isEmpty() const;

		//Structure for each Meta Data task
			
			struct data
	   		{
	   			data& operator=(const data& other)
	   			{
	   				if(this != &other)
	   				{
		   				meta_data_code = other.meta_data_code;
		   				meta_data_desc = other.meta_data_desc;
		   				numCycles = other.numCycles;
		   				processNum = other.processNum;
		   			}
		   			return *this;

	   			}

	   			std::string meta_data_code, 
	   						meta_data_desc;
	   			
	   			int numCycles, 
	   				processNum;
	   		};

	   	//Variables for public use

	   		int processNum, numItems;
	   		
	   		std::string filePath, 
	   					alg;
	   		
	   		std::vector<std::queue<data>> metaDataVec;
			
			std::vector<data> metaData;	
   	private:
   		
   		//Regular Expression match key

        	std::regex matchKey; //key used to match meta data

};

/**
 * @brief      Process Control block
 * @details    controls exectuition od simulation and manipulation of data
 */
class PCB
{
	public:
		
		//Constructors

		PCB();
		PCB(MetaSettings* meta, ConfigSettings* config);	

		
		//Functions

			void runSim() throw(std::logic_error);	
			std::string getStartLogString(const std::string& desc, const
			std::string& code, int i) throw(std::logic_error);	
			void logProcess(std::string line) throw(std::logic_error);	

			//pThread Functions

			static void* runProcess(void* p);		
			static void* RRcount(void* q);	
			static void* loadProcess(void* settings);
		
			//Static Functions

			static void readInTimer();
			static void checkTimer(int cdtime);		

		//Variables for public access


			int hdInCount, 		//Keeps track of the current hard drive being 							written to
				
				hdOutCount,		//Keeps track of the current hard drive being 						read from
				
				projOutCount,		//Keeps track of current projector being 						used
				
				processState,		//state of simulator

				address;		//address for allocationg memory

			Timer* program_timer;		//timer for logging
			
			std::ostringstream log;		//stream for output
			
			ConfigSettings* pcbConfigurationData;		//Pointer to 														previously created 													ConfigSettings 														object
			
			MetaSettings* pcbMetaData;		//Pointer to previously created 									MetaSettings object
	private:	
		
		//Process States
			
			int START,
				READY,
				RUNNING,
				WAITING,
				EXIT;
};

/**
 * @brief      ouput for different possible errors
 */
struct Errors
{
	std::string& err(std::string& str)
	{
		if(str == "badConfExt")
			return badConfExt;
		else if(str == "badMetExt")
			return badMetExt;
		else if(str == "badLogExt")
			return badLogExt;
		else if(str == "badConf")
			return badConf;
		else if(str == "badMeta")
			return badMeta;
		else if(str == "badKey")
			return badKey;
		else
			return badOut;
	}
    std::string

        badConfExt = "ERROR: The supplied configuration file does not use the required file extention. Please use '.conf' for all configuration files. The simulation will end now.",
        badMetExt = "ERROR: The supplied meta-data file does not use the required file extention. Please use '.mdf' for all meta-data files. The simulation will end now.",
        badLogExt = "ERROR: The supplied log file path does not use the required file extention. Please use '.lgf' for all log files. The simulation will end now.",
        badConf = "ERROR: The configuration file is missing or contains unexpected data. Please review the file content for errors and try again. The simulation will end now.",
        badMeta = "ERROR: The metadata file is missing or contains unexpected data. Please review the file content for errors and try again. The simulation will end now.",
        badKey = "ERROR: Requested data not found. This could be due to errors in the meta-data file. The simulation will end now.",
        badOut = "ERROR: the requested output configuration is not supported. The simulation will end now.";
};

void* countDown(void* unused);


#endif /* Sim01_h */
