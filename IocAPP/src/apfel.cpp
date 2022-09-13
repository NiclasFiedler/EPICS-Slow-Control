#include "serialadapter_aux_spi.h"
#include "apfel.h"
#include <string>
#include <memory>
#include <unistd.h>
#include <iostream>
#include <algorithm>
#include <iomanip>
#include <vector>
#include <bitset>
#include <Python.h>
#include <fstream>
#include <sstream>
#include <iterator>
#include <map>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <cstring>
#include <sys/types.h>

//Everything that uses apfel.cpp should be executed with sudo -s

// int sn = 0;

apfel::apfel(std::shared_ptr<serialadapter_aux_spi> spi)
: m_spi(spi)
{

}

void apfel::set_DAC(uint8_t chip_ID, uint8_t dac_channel, uint16_t data, std::size_t i_saa, const bool output)
{
    chip_ID &= 0x0f; //clear everything over 4 bit
    dac_channel &= 0x0f; //clear everything over 4 bit
    data &= 0x03ff; //clear everthing over 10 bit
    
    uint32_t command = dac_channel << 18;
    command |= (data << 8);
    command |= (chip_ID);
    m_spi->write32bit(0x49,command,i_saa,output);
    usleep(1000);
}

auto apfel::read_DAC(uint8_t chip_ID, uint8_t dac_channel, std::size_t i_saa, const bool output) -> uint16_t
{
    chip_ID &= 0x0f; //clear everything over 4 bit
    dac_channel &= 0x0f; //clear everything over 4 bit
    dac_channel += 4; //output mode

    uint32_t command = dac_channel << 18;
    command |= (chip_ID);
    m_spi->write32bit(0x49,command,i_saa,output);
    usleep(1000);
    uint16_t data = m_spi->read16bit(0x49,i_saa,output); 
    usleep(1000);
    data = data >> 1;
    data &= 0x3ff; 
    return data;
}

void apfel::set_amp_mode(uint8_t chip_ID, uint8_t channel, uint8_t mode, std::size_t i_saa, const bool output)
{
    chip_ID &= 0x0f; //clear everything over 4 bit

    uint32_t command;
    if(channel == 1 && mode == 16){command = 0x3c0000;}
    else if(channel == 1 && mode == 32){command = 0x2c0000;}
    else if(channel == 2 && mode == 16){command = 0x380000;}
    else if(channel == 2 && mode == 32){command = 0x340000;}
    command |= (chip_ID);
    m_spi->write32bit(0x49,command,i_saa,output);
}

void apfel::global_reset(uint8_t chip_ID, std::size_t i_saa, const bool output)
{
    chip_ID &= 0x0f; //clear everything over 4 bit

    uint32_t command = 0;
    command |= (chip_ID);
    m_spi->write32bit(0x49,command,i_saa,output);
}

void apfel::testpulse(uint8_t chip_ID, uint8_t channel, uint8_t data, std::size_t i_saa, const bool output)
{
    chip_ID &= 0x0f; //clear everything over 4 bit
    uint8_t op_mode = 0x09;
    data &= 0x0f; //clear everthing over 4 bit
    
    uint32_t command = op_mode << 18;
    if(channel == 1){command |= (data << 9);}
    else if(channel == 2){command |= (data << 14);}
    command |= (chip_ID);
    m_spi->write32bit(0x49,command,i_saa,output);
}

void apfel::testpulse_toggle(uint8_t chip_ID, uint8_t channel, uint8_t data, uint32_t t, std::size_t i_saa, const bool output)
{    
    t *= 1000;
    for(uint8_t i = 1; i <= 10; i++){ //10 is placeholder
        testpulse(chip_ID,channel,data,i_saa,output);
        
        for(uint8_t j = 0; j < (t%85000); j++){
            usleep(85000);
        }
        usleep((t-(t%85000)*85000));

        testpulse(chip_ID,channel,0,i_saa,output);

        for(uint8_t j = 0; j < (t%85000); j++){
            usleep(85000);
        }
        usleep((t-(t%85000)*85000));
    }
}

void apfel::autocalib(uint8_t chip_ID, std::size_t i_saa, const bool output)
{
    chip_ID &= 0x0f; //clear everything over 4 bit
    uint8_t op_mode = 0xc;
    uint16_t data = 0x00;
    
    uint32_t command = (op_mode << 18);
    command |= (data << 8);
    command |= (chip_ID);
    m_spi->write8bit(0x48,0x08,i_saa,output);
    usleep(100);    
    m_spi->write32bit(0x49,command,i_saa,output);
    usleep(30000);
    m_spi->write8bit(0x48,0x00,i_saa,output);
}

void apfel::autocalib_all(std::size_t i_saa, const bool output)
{
    autocalib(1);
    autocalib(2);
    autocalib(3);
    autocalib(4);
}

auto apfel::apfel2hash(apfel_dac v_apfel) -> size_t{
    return v_apfel.chip_id * 100 + v_apfel.ch * 10 + v_apfel.gain;
}

auto apfel::hash2apfel(size_t hash) -> apfel_dac{
    return {hash/100,(hash%100)/10,hash%10};
}

auto apfel::bin2txt(std::string path) -> std::vector<std::vector<std::vector<float>>>
//int main()
{
std::vector<std::vector<std::vector<float>>> traces = {{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{}};
int sn = 1;
std::string inputpath = path;
inputpath = inputpath + "/raw";

char inputfile[200];	//location of inputfile
int event=-1;			//event number for later iteration	

inputpath = inputpath + "/temp.bin";
char inputpath_char[inputpath.length()];
strcpy(inputpath_char, inputpath.c_str());
sprintf(inputfile,inputpath_char);		//Set path to inputfileorooo

std::vector<float> main_list; 	//vector for trace output

std::ifstream original(inputfile);		//Öffnen der Binärdatei zum Auslesen
  while( !original.eof() && event<10000 )
//	while( !original.eof() )
    {
		
	// **********	Read of trace data, do not change	************************************
	char testkopf[1]; 		//zum Auslesen der bin-Datei in korrekten Schritten
    char testkopf2[2];		//zum Auslesen der bin-Datei in korrekten Schritten
    char testkopf3[3];		//zum Auslesen der bin-Datei in korrekten Schritten
    char testkopf4[4];		//zum Auslesen der bin-Datei in korrekten Schritten
	int channel = 0; 
 	int SADC_IP = 0;
	int sampleNr = 0;

 	original.read(testkopf,1); 					//liest ein bit aus und speichert es in testkopf
	//printf("found %s \n", testkopf);
	if ((uint8_t)testkopf[0] == 0x0a){
		original.read(testkopf2,2);				//skip two bytes
		original.read(testkopf,1);				//read SADC IP
		SADC_IP =(int)(testkopf[0]);			// convert SADC IP from byte to integer
		original.read(testkopf,1);				//skip the first 0xCB
	}

	original.read(testkopf3,3); 				//read 3 bytes (cbcbcb) and do nothing
	original.read(testkopf4,4);					//read 4 bytes (burst info) and do nothing


	for(int i = 1; i<5; i++){	original.read(testkopf,1);	}

	original.read(testkopf,1);					//skip daqbufnum
	original.read(testkopf,1);
	channel = (int)testkopf[0];
	original.read(testkopf2,2);					//skip "part" and "sampling rate"
	sampleNr = 0;

    if (channel==0){  // datastream loops over all inputs per event -> new event starts with ch0
		event=event+1; 
	}
//printf("*****************read traces in file***********************************\n");
	main_list.clear();

	while (sampleNr < 512){
	original.read(testkopf2,2);
		uint8_t couple_bytes = testkopf2[0]; //linker Teil der Binärzahl
		uint8_t couple_bytes2 = testkopf2[1];//rechter Teil der Binärzahl
		uint16_t coupli_bytes = ((uint16_t)couple_bytes) << 8;  //um 8 Stellen nach links verschieben
		coupli_bytes |= couple_bytes2; //verodern mit zweiter Binärzahl
		int couple_bytes_deci = (int)coupli_bytes; //ganze Binärzahl in Integer umwandeln
		main_list.push_back((float)couple_bytes_deci);
	// printf("%f read in sample %i of channel %i \n",main_list[sampleNr], sampleNr, channel);
	sampleNr++;	
	}
    traces[channel].push_back({main_list});
}    //end of file reading loop
return traces;
}

auto apfel::getHeight(apfel_dac v_apfel, std::size_t i_saa) -> double {
    const char *homedir = getenv("PWD");
    std::string homedirst = homedir;
    homedirst.erase(homedirst.end()-25,homedirst.end());
    std::string filenamest = homedirst + "SADC/SADC_Programme_BT_11_19/utilities/oneTraceRecordingExample.py";
    double height = 0;
    const char *filename = filenamest.c_str();
    FILE* fp;
    Py_Initialize();
    fp = _Py_fopen(filename, "r"); //only works until Pyhton3.9
    PyRun_SimpleFile(fp, filename); //start SADC measuring file (0.5 seconds) and save it in temp directory
    Py_Finalize();
    
    std::vector<std::vector<std::vector<float>>> data = bin2txt(homedirst + "SADC/temp"); //save file (data[channel][event][datapoint (max 512)])
    std::string removest = "rm -r " + homedirst + "SADC/temp";
    const char *remove = removest.c_str();
    system(remove); //delete temp directory

    //             int i = 0;
    //             while(i < data[10][1].size()) {
    //                 std::ofstream measfile;
    //                 measfile.open(homedirst + "SADC/SN" + std::to_string(sn) + "_ch" + std::to_string(11) + "_ev" + std::to_string(1) + ".txt", std::ios_base::app);
	//                 measfile << data[11][1][i] << std::endl;
    //                 measfile.close();
    //                 measfile.open(homedirst + "SADC/SN" + std::to_string(sn) + "_ch" + std::to_string(15) + "_ev" + std::to_string(1) + ".txt", std::ios_base::app);
	//                 measfile << data[15][1][i] << std::endl;
	//                 measfile.close();
    //                 i++;
    // }
    // std::cout << sn << ": ";
    // sn++;

        double mean_base = 0;
        double mean_peak = 0;
        int good_traces = 0;

        for(int j=0;j<data[10].size();j++){
            double mean_peak_one = 0;
            double mean_base_left = 0;
            double mean_base_right = 0;
            double mean_base_one = 0;
            
            for(int i=260;i<=280;i++){mean_peak_one += data[sadc_map.at(apfel2hash(v_apfel)).sadc_ch][j][i];}
            mean_peak_one /= 21; //height of peak
            
            for(int i=50;i<=100;i++){mean_base_left += data[sadc_map.at(apfel2hash(v_apfel)).sadc_ch][j][i];}
            mean_base_left /= 51; //height of baseline left of peak
            
            for(int i=420;i<=470;i++){mean_base_right += data[sadc_map.at(apfel2hash(v_apfel)).sadc_ch][j][i];}
            mean_base_right /= 51; //height of baseline right of peak
            
            mean_base_one = (mean_base_left + mean_base_right) / 2; //height of baseline

            if(abs(mean_peak_one-mean_base_one) >= 1000){ //test for peak
                mean_base += mean_base_one;
                mean_peak += mean_peak_one;
                good_traces++;
            }
        }
        if(good_traces > 0){ //if peaks were detected, calculation of mean heights for peak and baseline
            mean_base /= good_traces;
            mean_peak /= good_traces;
            height = abs(mean_peak-mean_base); //calculation of height
        }
    return height;
}

auto apfel::sw_autocalib_one_ch(apfel_dac v_apfel, std::size_t i_saa, bool dac_value_output) -> std::vector<int>{
    autocalib(v_apfel.chip_id); //hardware autocalib
    // if(v_apfel.ch == 1){
    // std::cout << "Bilder machen! 1 Minute Zeit!" << std::endl;
    // sleep(30);
    // std::cout << "30 Sekunden!" << std::endl;
    // sleep(30);
    // }
    int old_dac = read_DAC(v_apfel.chip_id,sadc_map.at(apfel2hash(v_apfel)).dac); //get current DAC value
    set_DAC(v_apfel.chip_id,sadc_map.at(apfel2hash(v_apfel)).dac,old_dac-20); //set DAC 30 lower to cover whole area around hw-autocalib
    if(dac_value_output) std::cout << "APFEL " << v_apfel.chip_id << "  DAC " << sadc_map.at(apfel2hash(v_apfel)).dac << ": " << std::dec << old_dac << " -> " << old_dac-20 << std::endl;
    
    double height = getHeight(v_apfel); //get new peak height
    double autocalib_height = height; //save peak height of hw-autocalib for later use
    double height_prev = 0;
    int repeatno = 0;
    bool repeat = 1;
    while(repeat && repeatno <= 5){ //repeat until sw-autocalib is better than hw-autocalib
        while(height > height_prev){ //repeat until height not gets higher  
            height_prev = height; //save current height
            old_dac = read_DAC(v_apfel.chip_id,sadc_map.at(apfel2hash(v_apfel)).dac); //get current DAC value
            set_DAC(v_apfel.chip_id,sadc_map.at(apfel2hash(v_apfel)).dac,old_dac+10); //set DAC 10 higher for rough adjustments
            if(dac_value_output) std::cout << "APFEL " << v_apfel.chip_id << "  DAC " << sadc_map.at(apfel2hash(v_apfel)).dac << ": " << std::dec << old_dac << " -> " << old_dac+10 << std::endl;
            height = getHeight(v_apfel); //get new peak height
        }
        
        height_prev = height; //save current height
        old_dac = read_DAC(v_apfel.chip_id,sadc_map.at(apfel2hash(v_apfel)).dac); //get current DAC value
        set_DAC(v_apfel.chip_id,sadc_map.at(apfel2hash(v_apfel)).dac,old_dac-10); //set DAC 10 lower to prepare for fine adjustments
        if(dac_value_output) std::cout << "APFEL " << v_apfel.chip_id << "  DAC " << sadc_map.at(apfel2hash(v_apfel)).dac << ": " << std::dec << old_dac << " -> " << old_dac-10 << std::endl;
        height = getHeight(v_apfel); //get new peak height
        
        while(height > height_prev){ //repeat until height not gets higher      
            height_prev = height; //save current height
            old_dac = read_DAC(v_apfel.chip_id,sadc_map.at(apfel2hash(v_apfel)).dac); //get current DAC value
            set_DAC(v_apfel.chip_id,sadc_map.at(apfel2hash(v_apfel)).dac,old_dac+1); //set DAC 1 higher for fine adjustments
            if(dac_value_output) std::cout << "APFEL " << v_apfel.chip_id << "  DAC " << sadc_map.at(apfel2hash(v_apfel)).dac << ": " << std::dec << old_dac << " -> " << old_dac+1 << std::endl;
            height = getHeight(v_apfel); //get new peak height
        }
        
        old_dac = read_DAC(v_apfel.chip_id,sadc_map.at(apfel2hash(v_apfel)).dac); //get current DAC value
        set_DAC(v_apfel.chip_id,sadc_map.at(apfel2hash(v_apfel)).dac,old_dac-1); //set DAC 1 lower to be safe
        if(dac_value_output) std::cout << "APFEL " << v_apfel.chip_id << "  DAC " << sadc_map.at(apfel2hash(v_apfel)).dac << ": " << std::dec << old_dac << " -> " << old_dac-1 << std::endl;
        height = getHeight(v_apfel); //get new peak height
        if(dac_value_output) std::cout << "APFEL " << v_apfel.chip_id << "  DAC " << sadc_map.at(apfel2hash(v_apfel)).dac << ": " << "Autocalib Height: " << autocalib_height << "  Height: " << height << std::endl;
        
        if(autocalib_height > height){ //if sw-autocalib is worse than hw-autocalib, repeat and reset
            autocalib(v_apfel.chip_id);
            if(dac_value_output) std::cout << "APFEL " << v_apfel.chip_id << "  DAC " << sadc_map.at(apfel2hash(v_apfel)).dac << ": " << "Reset" << std::endl;
            old_dac = read_DAC(v_apfel.chip_id,sadc_map.at(apfel2hash(v_apfel)).dac); //get current DAC value
            set_DAC(v_apfel.chip_id,sadc_map.at(apfel2hash(v_apfel)).dac,old_dac-20);
            if(dac_value_output) std::cout << "APFEL " << v_apfel.chip_id << "  DAC " << sadc_map.at(apfel2hash(v_apfel)).dac << ": " << std::dec << old_dac << " -> " << old_dac-20 << std::endl;
            height = getHeight(v_apfel); //get new peak height
            autocalib_height = height; //save peak height of hw-autocalib for later use
            height_prev = 0;
            repeatno++;
        }
        else{
            repeat = 0;
        }
    }
    
    if(repeat == 1){
        std::cout << "Something didn't work. Please try again later.";
        return {0,0};
    }

    //DAC 4
    old_dac = read_DAC(v_apfel.chip_id,4); //get current DAC value
    set_DAC(v_apfel.chip_id,4,old_dac-20); //set DAC 30 lower to cover whole area around hw-autocalib
    if(dac_value_output) std::cout << "APFEL " << v_apfel.chip_id << "  DAC 4: " << std::dec << old_dac << " -> " << old_dac-20 << std::endl;
    
    height = getHeight(v_apfel); //get new peak height
    autocalib_height = height; //save peak height of hw-autocalib for later use
    height_prev = 0;
    repeatno = 0;
    repeat = 1;
    while(repeat && repeatno <= 5){ //repeat until sw-autocalib is better than hw-autocalib
        while(height > height_prev){ //repeat until height not gets higher   
            height_prev = height; //save current height
            old_dac = read_DAC(v_apfel.chip_id,4); //get current DAC value
            set_DAC(v_apfel.chip_id,4,old_dac+10); //set DAC 10 higher for rough adjustments
            if(dac_value_output) std::cout << "APFEL " << v_apfel.chip_id << "  DAC 4: " << std::dec << old_dac << " -> " << old_dac+10 << std::endl;
            height = getHeight(v_apfel); //get new peak height
        }
        
        height_prev = height; //save current height
        old_dac = read_DAC(v_apfel.chip_id,4); //get current DAC value
        set_DAC(v_apfel.chip_id,4,old_dac-10); //set DAC 10 lower to prepare for fine adjustments
        if(dac_value_output) std::cout << "APFEL " << v_apfel.chip_id << "  DAC 4: " << std::dec << old_dac << " -> " << old_dac-10 << std::endl;
        height = getHeight(v_apfel); //get new peak height
        
        while(height > height_prev){ //repeat until height not gets higher
            height_prev = height; //save current height
            old_dac = read_DAC(v_apfel.chip_id,4); //get current DAC value
            set_DAC(v_apfel.chip_id,4,old_dac+1); //set DAC 1 higher for fine adjustments
            if(dac_value_output) std::cout << "APFEL " << v_apfel.chip_id << "  DAC 4: " << std::dec << old_dac << " -> " << old_dac+1 << std::endl;
            height = getHeight(v_apfel); //get new peak height
        }
        
        old_dac = read_DAC(v_apfel.chip_id,4); //get current DAC value
        set_DAC(v_apfel.chip_id,4,old_dac-1); //set DAC 1 lower to be safe
        if(dac_value_output) std::cout << "APFEL " << v_apfel.chip_id << "  DAC 4: " << std::dec << old_dac << " -> " << old_dac-1 << std::endl;
        height = getHeight(v_apfel); //get new peak height
        if(dac_value_output) std::cout << "APFEL " << v_apfel.chip_id << "  DAC 4: " << "Autocalib Height: " << autocalib_height << "  Height: " << height << std::endl;
        
        if(autocalib_height > height){ //if sw-autocalib is worse than hw-autocalib, repeat and reset
            autocalib(v_apfel.chip_id);
            if(dac_value_output) std::cout << "APFEL " << v_apfel.chip_id << "  DAC 4: " << "Reset" << std::endl;
            old_dac = read_DAC(v_apfel.chip_id,4); //get current DAC value
            set_DAC(v_apfel.chip_id,4,old_dac-20);
            if(dac_value_output) std::cout << "APFEL " << v_apfel.chip_id << "  DAC 4: " << std::dec << old_dac << " -> " << old_dac-20 << std::endl;
            height = getHeight(v_apfel); //get new peak height
            autocalib_height = height; //save peak height of hw-autocalib for later use
            height_prev = 0;
            repeatno++;
        }
        else{
            repeat = 0;
        }
    }

    if(repeat == 1){
        std::cout << "Something didn't work. Please try again later.";
        return {0,0};
    }

    return {read_DAC(v_apfel.chip_id,sadc_map.at(apfel2hash(v_apfel)).dac),read_DAC(v_apfel.chip_id,4)}; //return both DAC values
}

auto apfel::sw_autocalib_all_ch(uint8_t chip_id, std::size_t i_saa, bool dac_value_output) -> std::vector<int>{
    std::vector<int> ch1lg = sw_autocalib_one_ch({chip_id,1,0},i_saa,dac_value_output); //sw-autocalib all channels
    std::vector<int> ch1hg = sw_autocalib_one_ch({chip_id,1,1},i_saa,dac_value_output);
    std::vector<int> ch2lg = sw_autocalib_one_ch({chip_id,2,0},i_saa,dac_value_output);
    std::vector<int> ch2hg = sw_autocalib_one_ch({chip_id,2,1},i_saa,dac_value_output);

    set_DAC(chip_id,1,ch1hg[0]); //set DACs according to sw-autocalib 
    set_DAC(chip_id,2,ch2hg[0]);
    set_DAC(chip_id,3,(ch1lg[0]+ch2lg[0])/2); //mean for DACs that influence more than one channel
    set_DAC(chip_id,4,(ch1hg[1]+ch2hg[1]+ch1lg[1]+ch2lg[1])/4);

    return {ch1hg[0],ch2hg[0],(ch1lg[0]+ch2lg[0])/2,(ch1hg[1]+ch2hg[1]+ch1lg[1]+ch2lg[1])/4}; //return all DAC values
}

auto apfel::sw_autocalib_lg(uint8_t chip_id, std::size_t i_saa, bool dac_value_output) -> std::vector<int>{
    std::vector<int> ch1lg = sw_autocalib_one_ch({chip_id,1,0},i_saa,dac_value_output); //sw-autocalib both low gain channels
    std::vector<int> ch2lg = sw_autocalib_one_ch({chip_id,2,0},i_saa,dac_value_output);

    set_DAC(chip_id,3,(ch1lg[0]+ch2lg[0])/2); //set DACs according to sw-autocalib
    set_DAC(chip_id,4,(ch1lg[1]+ch2lg[1])/2);
    getHeight({chip_id,1,0});
    return {(ch1lg[0]+ch2lg[0])/2,(ch1lg[1]+ch2lg[1])/2}; //return both DAC values
}

auto apfel::sw_autocalib_all(std::size_t i_saa, bool dac_value_output) -> std::vector<std::vector<int>>{
    std::vector<std::vector<int>> dac_values;
    for(uint8_t chip_id = 1; chip_id <= 4; chip_id++){
        dac_values.push_back(sw_autocalib_all_ch(chip_id,i_saa,dac_value_output)); //sw-autocalib all channels
    }

    return dac_values; //return all DAC values
}



