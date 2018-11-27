//
//  main.cpp
//  Sim03 version 1.0.0
//
//  Created by Nathan Smith on 3/12/18.
//  Copyright © 2018 Nathan Smith. All rights reserved.
//


#include "OSSim.hpp"



/**
 * @brief      main function
 *
 * @param[in]  argc  The number of command line arguments
 * @param      argv  array holding each argument provided
 *
 * @return     returns in to signify end of program
 */
int main(int argc, char* argv[])
{
    // object declaration
    COUNT_DOWN_TIME = 0;    //initialize COUNT_DOWN_TIME to 0
    std::string filePath, str;   //string object for cmd line parameters
    ConfigSettings* configurationData = new ConfigSettings();
    MetaSettings* metaData = new MetaSettings();
    Errors error;
    pthread_t timer;
    long  null = 6;

   
        filePath = argv[1];

        try { 
            
            //std::cout << filePath << std::endl;
            configurationData->loadSettings(filePath);
            //printf("%s\n", "conf loaded");
            metaData->loadData(configurationData->metaFile);
            //printf("%s\n", "meta loaded");
            PCB* pcb = new PCB(metaData, configurationData);
            //printf("%s\n", "pcb created");
            pthread_create(&timer, NULL, countDown, (void*) null);
            //printf("%s\n", "countdown created");
            pcb->runSim();
        }
        catch(std::logic_error& e)
        {
            str = e.what();
            std::cout << error.err(str) << std::endl;
            //std::cout << str << std::endl;
        }
    
    delete configurationData;
    configurationData = NULL;
    delete metaData;
    metaData = NULL;
    
    
    return 0;
}

void* countDown(void* unused)
{
    //clock_t temp = 0;
    while(1)
    {
        if(COUNT_DOWN_TIME != 0)
        {
            //std::cout << COUNT_DOWN_TIME << std::endl;
            clock_t next;
            clock_t prev = clock();
            while(COUNT_DOWN_TIME > 0) 
            {
                next = clock();
                COUNT_DOWN_TIME -= (next - prev);
                prev = next; 
            }
        }
    }
}



