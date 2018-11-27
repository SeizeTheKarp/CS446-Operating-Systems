#include "OSSim.hpp"

//GLobal Variables
	int COUNT_DOWN_TIME;
	bool quantumFlag;
	bool loadFlag;
//

/**
 * @brief      Constructs Process Control Block object.
 * @details    defines integer values for process states, creates timer 
 * 				object, and intitalizes class variables
 */
PCB::PCB(MetaSettings* meta, ConfigSettings* config)
{
	START = 1;
	READY = 2;
	RUNNING = 3;
	WAITING = 4;
	EXIT = 0;

	program_timer = new Timer();
	hdInCount = -1;
	hdOutCount = -1;
	projOutCount = -1;
	address = 0;

	pcbMetaData = meta;
	pcbConfigurationData = config;

	pcbMetaData->alg = pcbConfigurationData->type;
}


/**
 * @brief      exectutes the simulation
 */
void PCB::runSim() throw(std::logic_error)
{
	//Variable declarations

		quantumFlag = false;
		loadFlag = false;

		pthread_t process, scheduling, load;
		int cycleTime = 0;
		int i;		
		//long  null = 6;		
		MetaSettings::data temp;

	try
	{
		pcbMetaData->loadVec();
		
		if(pcbConfigurationData->type == "STR")
		{
			std::sort(++pcbMetaData->metaDataVec.begin(), pcbMetaData->metaDataVec.end(), [](const std::queue<MetaSettings::data> & b, const std::queue<MetaSettings::data> & c){ return b.size() < c.size(); });
		}
		if(pcbConfigurationData->type == "RR")
		{
			pthread_create(&scheduling, NULL, RRcount, (void*) &pcbConfigurationData->quantum);
		}
		
		processState = READY;
		program_timer->start();
		pthread_create(&load, NULL, loadProcess, (void*) pcbMetaData);
		logProcess(" - Simulator program starting");
		
		while(!pcbMetaData->metaDataVec.empty())
		{
			if(pcbMetaData->metaDataVec.size() == 1)
			{
				if(pcbMetaData->metaDataVec[0].empty())
					{ break; }
			}
			for(i = 0; i < pcbMetaData->metaDataVec.size(); ++i)
			{
				
				
				while(!pcbMetaData->metaDataVec[i].empty())
				{
					
					temp = pcbMetaData->metaDataVec[i].front();
					

					if (quantumFlag == true)
					{
						
						logProcess(" - Process " + std::to_string(temp.processNum) + ": interrupt processing action");
						
						quantumFlag = false;
						
						break;
					}
					
					if(hdInCount == pcbConfigurationData->hdQuan-1)
						{ hdInCount = -1; }
					
					if(hdOutCount == pcbConfigurationData->hdQuan-1)
						{ hdOutCount = -1; }
					
					if(projOutCount == pcbConfigurationData->projQuan-1)
						{ projOutCount = -1; }
					
					processState = RUNNING;
					
					cycleTime = (temp.numCycles * pcbConfigurationData->getData(temp.meta_data_desc));
					
					if(temp.meta_data_code == "S" && temp.meta_data_desc == "begin")
					{
						
					}
					
					else if(temp.meta_data_code == "A")
					{
						
						if (temp.meta_data_desc == "begin")
						{
							logProcess(" - OS: preparing process " + std::to_string(temp.processNum));
							
							checkTimer(cycleTime);
							
							logProcess(" - OS: starting process " + std::to_string(temp.processNum));
							
						}
						
						else
						{
							logProcess(" - OS: process " + std::to_string(temp.processNum) + " completed");
							
							checkTimer(cycleTime);
						}
					}
					
					else if(temp.meta_data_code == "I")
					{
						
						logProcess(getStartLogString(temp.meta_data_desc, temp.meta_data_code, temp.processNum));
						
						processState = WAITING;
						
						pthread_create(&process, NULL, runProcess, (void*) &cycleTime);
						
						pthread_join(process, NULL);
						
						processState = RUNNING;
						
						logProcess(" - Process " + std::to_string(temp.processNum) + ": end " + temp.meta_data_desc + " input");
					}
					else if(temp.meta_data_code == "O")
					{
						
						logProcess(getStartLogString(temp.meta_data_desc, temp.meta_data_code, temp.processNum));
						
						processState = WAITING;
						
						pthread_create(&process, NULL, runProcess, (void*) &cycleTime);
						
						pthread_join(process, NULL);
						
						processState = RUNNING;
						
						logProcess(" - Process " + std::to_string(temp.processNum) + ": end " + temp.meta_data_desc + " output");
					}
					else if(temp.meta_data_code == "P")
					{
						logProcess(" - Process " + std::to_string(temp.processNum) + ": start processing action");
						
						processState = WAITING;
						
						checkTimer(cycleTime);
						
						processState = RUNNING;
						
						logProcess(" - Process " + std::to_string(temp.processNum) + ": end processing action");
					}
					else if(temp.meta_data_code == "M")
					{
						
						if(temp.meta_data_desc == "allocate")
						{	

							logProcess(" - Process " + std::to_string(temp.processNum) + ": allocating memory");
							
							processState = WAITING;
							
							checkTimer(cycleTime);
							
							processState = RUNNING;
							
							std::stringstream addr;

							addr << std::internal << std::hex << std::setw(8) << std::setfill('0')  << address;

							logProcess( " - Process " + std::to_string(temp.processNum) + ": memory allocated at 0x" + addr.str());

							address += 8;
						}

						else if(temp.meta_data_desc == "block")
						{
							logProcess(" - Process " + std::to_string(temp.processNum) + ": start memory blocking");
							
							processState = WAITING;
							
							checkTimer(cycleTime);
							
							processState = RUNNING;
							
							logProcess(" - Process " + std::to_string(temp.processNum) + ": end memory blocking");
						}
					}
					
					--pcbMetaData->numItems;
					
					pcbMetaData->metaDataVec[i].pop();

					if(pcbMetaData->metaDataVec[i].empty())
					{
						if(pcbMetaData->metaDataVec.size() == 1) { break; }
						
						pcbMetaData->metaDataVec.erase(pcbMetaData->metaDataVec.begin() + i);
					}
					
					
				}
			}
		}

		logProcess(" - Simulator Program Ending");
		
		
	}	
	catch(std::logic_error& e){ throw std::logic_error(e.what()); }
	
	if(pcbConfigurationData->outputType == "File" || pcbConfigurationData->outputType == "Both")
	{
		std::ofstream fp(pcbConfigurationData->logFile);
		fp << log.str() << std::endl;
	}
	
	if(pcbConfigurationData->outputType == "Monitor" || pcbConfigurationData->outputType == "Both") { std::cout << log.str() << std::endl; }
		
}

/**
 * @brief      Gets the start log string.
 *
 * @param[in]  desc  The description
 * @param[in]  code  The code
 * @param[in]  i     process number
 *
 * @return     The start log string.
 */
std::string PCB::getStartLogString(const std::string& desc, const std::string& code, int i) throw(std::logic_error)
{
	std::string line;
	
	if(code == "I")
	{
		if(desc == "hard drive")
		{
			++hdInCount;
			
			line = " - Process " + std::to_string(i) + ": start hard drive input on HDD " + std::to_string(hdInCount);
		}
		else
		{
			line = " - Process " + std::to_string(i) + ": start " + desc + " input";
		}
	}
	else if(code == "O")
	{
		if(desc == "projector")
		{
			++projOutCount;
			
			line = " - Process " + std::to_string(i) + ": start projector output on PROJ " + std::to_string(projOutCount);
		}
		else if(desc == "hard drive")
		{
			++hdOutCount;
			
			line = " - Process " + std::to_string(i) + ": start hard drive output on HDD " + std::to_string(hdOutCount);
		}
		else
		{
			line = " - Process " + std::to_string(i) + ": start " + desc + " output";
		}
	}
	
	else { throw std::logic_error("badMeta"); }
	
	return line;
}

/**
 * @brief load thread
 * @details thread to itterativley load the meta data 10 times every 100ms
 * 
 * @param settings Meta Data object
 * @return retruns a void pointer
 */
void* PCB::loadProcess(void* settings)
{
	
	MetaSettings* pcbMetaData = static_cast<MetaSettings*> (settings);	
									//cast settings as MetaSettings pointer
    
    for(int i = 0; i < 10; ++i)
    {
    	readInTimer();

        while(pthread_mutex_trylock(&mutex1) != 0) {}
		
            pcbMetaData->loadVec();
           

            if(pcbMetaData->alg == "STR")
            {
                std::sort(++pcbMetaData->metaDataVec.begin(), pcbMetaData->metaDataVec.end(), [](const std::queue<MetaSettings::data> & b, const std::queue<MetaSettings::data> & c){ return b.size() < c.size(); });

                quantumFlag = true;
            }

        pthread_mutex_unlock(&mutex1);
       
    }
    pthread_exit(NULL);
}

/**
 * @brief      Timer for load process thread
 */
void PCB::readInTimer()
{
    int interval = 100;
    
    clock_t endwait( clock( ) + (interval * CLOCKS_PER_SEC)/1000);

            while( clock( ) < endwait ) {}
}

/**
 * @brief      pthread function for I/O processes
 *
 * @param      p      cycle times
 *
 * @return     void pointer 
 */
void* PCB::runProcess(void* p)
{

 	while(pthread_mutex_trylock(&mutex1) != 0) {}
	
	checkTimer(*(int*) p);
	
	pthread_mutex_unlock(&mutex1);	

	pthread_exit(NULL);
}

/**
 * @brief      timer for RR algorithm
 *
 * @param      q void pointer to quantum for RR algorithm
 *
 * @return     void pointer
 */
void* PCB::RRcount(void* q)
{

	int tempQuantum = *(int*) q;
	
	while(1)
	{
		if(quantumFlag == false)
		{
			
			clock_t endwait( clock( ) + (tempQuantum * CLOCKS_PER_SEC)/1000);

		    while( clock( ) < endwait ){}

			quantumFlag = true;
		}
	}
}

/**
 * @brief      Counts the number of down.
 *
 * @param[in]  cdtime  The cdtime
 */
void PCB::checkTimer(int cdtime)
{
	COUNT_DOWN_TIME = ((cdtime*CLOCKS_PER_SEC)/1000);
	
	int interval = 100;
	

	while(1)
	{
			
			clock_t endwait( clock( ) + (interval * CLOCKS_PER_SEC)/1000);

		    while( clock( ) < endwait ){}
		    
		    
		    
		    if(COUNT_DOWN_TIME <= 0) { return; }	
	}
}

/**
 * @brief      Logs a process.
 *
 * @param[in]  line  The line
 */
void PCB::logProcess(std::string line) throw(std::logic_error)
{
	try{
		
		program_timer->lap();
		
		double stamp = program_timer->getElapsedTime();
		
		log <<std::setprecision(6) << stamp << std::fixed << line << std::endl;
	}
	
	catch(std::logic_error& e) { throw logic_error(e.what()); }
	
}

/**
 * @brief      Constructs the ConfigSettings object
 * @details    defines regular expressions for parising .config file, declares 
 * 				keys for map, initializes class variables
 */
ConfigSettings::ConfigSettings()
{
	matchCriteria = {
			"(Start Simulator Configuration File)",
			"Version/Phase: (\\d+\\.\\d+)",
			"File Path: (Test_\\w+\\.mdf)",
			"Processor Quantum Number \\{msec\\}: (\\d+)",
			"CPU Scheduling Code: (RR|STR)",
			"Processor cycle time \\{msec\\}: (\\d+)",
			"Monitor display time \\{msec\\}: (\\d+)",
			"Hard drive cycle time \\{msec\\}: (\\d+)",
			"Projector cycle time \\{msec\\}: (\\d+)",
			"Keyboard cycle time \\{msec\\}: (\\d+)",
			"Memory cycle time \\{msec\\}: (\\d+)",
			"System memory \\{(kbytes|Mbytes|Gbytes)\\}: (\\d+)",
			"Memory block size \\{(kbytes|Mbytes|Gbytes)\\}: (\\d+)",
			"Projector quantity: (\\d+)",
			"Hard drive quantity: (\\d+)",
			"Log: Log to (File|Monitor|Both)",
			"Log File Path: (logfile_\\d+\\.lgf)",
			"(End Simulator Configuration File)"

			};

			key = {
				"begin",
				"version",
				"path",
				"quantum",
				"type",
				"run",
				"monitor",
				"hard drive",
				"projector",
				"keyboard",
				"memory",
				"system",
				"size",
				"projquan",
				"hdquan",
				"log",
				"log path",
				"finish"
			};
			memSize = 0;
			version = 0;
			blockSize = 0;
			projQuan = 0;
			hdQuan = 0;
}

/**
 * @brief      checks for proper file extension
 *
 * @param[in]  path  The path
 *
 * @return     return bool based on check
 */
bool ConfigSettings::checkFilePath(const std::string& path)
{
    size_t found = path.find(".conf"); //if config, ".conf" is searched for
    
    if (found != std::string::npos) { return true; }	//npos is the a null 													   charater. value is 														  returned if search 													   is unsuccesful
    
    return false;
}

/**
 * @brief      Loads settings.
 *
 * @param[in]  filePath  The file path
 */
void ConfigSettings::loadSettings(const std::string& filePath) throw(std::logic_error)
{
	if (!checkFilePath(filePath)) {	throw std::logic_error("badExt"); }

	//Declarations
    std::ifstream fp(filePath);
   
    int count = 0; //used to verify that the correct ammount of lines are 					 being read
	std::smatch data;
	std::string line;
	std::regex matchKey;
	
	while(!fp.eof())
	{
		std::getline(fp, line);
		
		matchKey = matchCriteria[count];
		
		if (std::regex_match(line, data, matchKey))
		{
			switch(count)
			{
				case 0:
					
					configData.insert(std::pair<std::string, int> ("begin", 0));
					
					++count;
					
					break;
				case 1:
					
					version = std::stod(data[1], nullptr);
					
					++count;
					
					break;
				case 2:
					
					metaFile = data[1];
					
					++count;
					
					break;
				case 3:
					
					quantum = std::stoi(data[1], nullptr);
					
					++count;
					
					break;
				case 4:
					
					type = data[1];
					
					++count;
					
					break;
				case 10:
					
					configData.insert(std::pair<std::string, int> ("block", std::stoi(data[1], nullptr)));
					
					configData.insert(std::pair<std::string, int> ("allocate", std::stoi(data[1], nullptr)));
					
					++count;					
					
					break;
				case 11:
					
					memType = data[1];
					
					memSize = std::stoi(data[2], nullptr);
					
					++count;
					
					break;
				case 12:
					
					blockSize = std::stoi(data[2], nullptr);
					
					++count;
					
					break;
				case 13:
					
					projQuan = std::stoi(data[1], nullptr);
					
					++count;
					
					break;
				case 14:
					
					hdQuan = std::stoi(data[1], nullptr);
					
					++count;
					
					break;
				case 15:
					
					outputType = data[1];
					
					++count;
					
					break;
				case 16:
					
					logFile = data[1];
					
					++count;
					
					break;
				case 17:
					
					configData.insert(std::pair<std::string, int> ("finish", 0));
					
					++count;
					
					break;
				default:
					
					configData.insert(std::pair<std::string, int> (key[count], std::stoi(data[1], nullptr)));
					
					++count;
					
					break;
			}
		}
		else { throw std::logic_error("badConf"); }
	}
	
	if (count != 18) { throw std::logic_error("badConf"); }
}

/**
 * @brief      Gets the data.
 *
 * @param      key   The key
 *
 * @return     The data.
 */
int ConfigSettings::getData(std::string& key) throw(std::logic_error)
{
  	if (configData.find(key) != configData.end()) {	return configData[key]; }
	
	else
	{
		std::cout << key <<std::endl;
		throw std::logic_error("badKey");
	}
}

/**
 * @brief      Constructs the Meta Settings object.
 */
MetaSettings::MetaSettings()
{
    matchKey = "\\s?(([S|A|P|I|O|M])\\{([a-z]+\\s?[a-z]+)\\}(\\d+))";
    numItems = 0;
    processNum = 1;
}

/**
 * @brief      check for proper file extention
 *
 * @param[in]  path  The path
 *
 * @return    bool based on check
 */
bool MetaSettings::checkFilePath(const std::string& path)
{
    size_t found = path.find(".mdf"); //if config, ".conf" is searched for
    
    if (found != std::string::npos) { return true; }	//npos is the a null 													   charater. value is 														  returned if search 													   is unsuccesful
    return false;
}

/**
 * @brief      Loads a data.
 *
 * @param      metaFilePath  The meta file path
 */
void MetaSettings::loadData(std::string& metaFilePath) throw(std::logic_error)
{
	if(!checkFilePath(metaFilePath))
	{
		throw std::logic_error("badMetaExt");
	}
	
	std::ifstream fp(metaFilePath);
    
    std::smatch match;
    
    std::string line;
   
    data mdata;
    
    filePath = metaFilePath;

    std::getline(fp, line);
    
    if (line == "Start Program Meta-Data Code:")
    {
    	
	    while(std::getline(fp, line, ';'))
	    {
	    	
		    
		    //each index of match holds a requested peice of the matched string
		    
		    if(std::regex_match (line, match, matchKey))	//uses regular 													  expression to 												  identify 														  data and 														  verify that it 												   is in the 														  correct format
		    {
		    	mdata.meta_data_code = match[2];	//meta category
		        
		        mdata.meta_data_desc = match[3];	//meta description. should 										 match map keys
		       
		        mdata.numCycles = std::stoi(match[4], nullptr);	//number of 												  cycles for 												   each 														  process
		        mdata.processNum = 0;
		        
		        metaData.push_back(mdata);
		        
		        ++numItems;
		      
		    }
	    } 
	}
	else { throw std::logic_error("badMeta"); }

	return;
}

/**
 * @brief      Loads a vector.
 * @details    cretes a vector of queues and loads processes from the meta 	
 * 				Data into it	
 */
void MetaSettings::loadVec() throw(std::logic_error)
{
	if (!metaData.empty())
    {
    	std::vector<data> tempVec(metaData);
    	
    	std::queue<data> process;

    	for(int i = 1; i < tempVec.size(); i++)
    	{
    		
    		tempVec[i].processNum = processNum;
    		
    		process.push(tempVec[i]);
    		
    		if(tempVec[i].meta_data_desc == "finish")
    		{
    			++processNum;
    			
    			metaDataVec.push_back(process);
    			
    			while(!process.empty()) { process.pop(); }
    		}
    	}
		
    }
    
    else { throw std::logic_error("badKey"); }
}

/**
 * @brief      Determines if empty.
 *
 * @return     True if empty, False otherwise.
 */
bool MetaSettings::isEmpty() const
{
	if (metaData.empty()) { return true; }
	
	return false;
}
