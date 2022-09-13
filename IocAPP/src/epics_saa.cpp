#include <iostream>
#include <unistd.h>
#include "hvboard.h"
#include <vector>
/* ANSI C includes  */
extern "C" {
#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
}
/* EPICS includes */

#include "aiRecord.h"
#include "aoRecord.h"
#include "boRecord.h"
#include "devSup.h"
#include "epicsExport.h"
#include "devLib.h"
#include "recGbl.h"
#include "dbAccessDefs.h"
#include <string>

auto hv=std::make_shared<hvboard>(1,1e6,0);
int *reg; //default register

// Typdef struct initialization

//      SAA VERSION
typedef struct {
  int n_saa;
  int boardID ;
} saa_info;

//	LM75
typedef struct {
  uint8_t bit_rate;
  uint8_t boardID ;
} lm75_info;

//	SHT21
typedef struct {
  uint8_t meas_typ;
  uint8_t bit_rate;
  uint8_t boardID ;
} sht21_info;

//	AI/GET DAC
typedef struct {
  uint8_t channel;
  uint8_t boardID;
} dac_ai_info;

//	AO/SET DAC
typedef struct {
  uint8_t channel;
  uint8_t boardID;
} dac_ao_info;

//	ADS1115
typedef struct {
  uint8_t meas_typ;
  uint8_t channel;
  uint8_t bit_rate;
  uint8_t boardID ;
} ads1115_info;

//  EEPROM GET
typedef struct {
  uint8_t mode;
  uint8_t bitrate;
  uint8_t boardID;
} eeprom_get_info;

//  EEPROM REG
typedef struct {
  uint8_t bitrate;
  uint8_t boardID;
} eeprom_reg_info;

//  EEPROM SET
typedef struct {
  uint8_t bitrate;
  uint8_t boardID;
} eeprom_set_info;

//  PCA9554 AI
typedef struct {
  uint8_t mode;
  uint8_t bitrate;
  uint8_t boardID;
} pca9554_ai_info;

//  PCA9554 A0
typedef struct {
  uint8_t mode;
  uint8_t bitrate;
  uint8_t boardID;
} pca9554_ao_info;

//  APFEL DAC AI
typedef struct {
  uint8_t chipid;
  uint8_t channel;
  uint8_t boardID;
} apfel_ai_info;

//  APFEL DAC AO
typedef struct {
  uint8_t chipid;
  uint8_t channel;
  uint8_t boardID;
} apfel_ao_info;

//  APFEL DAC BO
typedef struct {
  uint8_t mode;
  uint8_t chipid;
  uint8_t channel;
  uint8_t boardID;
} apfel_bo_info;


//	END Typdef initialisation

//-----------------------------------------------------//
//                                                     //
//                +------------------+                 //
//                |  Device Support  |                 //
//                +------------------+                 //
//                                                     //
//-----------------Version-Register--------------------//

//static long version_init(int pass){
//  if(pass==0) return 0;
//  else{
//    epicsUInt8 anzahl = atoi(ai->inp.value.instio.string);
//    reg = malloc(sizeof(*reg) * )
//    return 0;
//  }
//}

static long version_init_record(aiRecord *ai) {
  saa_info *saainfo;
  saainfo = (saa_info *) calloc( 1, sizeof( saa_info ) );

  std::string input = ai->inp.value.instio.string;
  size_t pos = 0;

  if(ai->inp.type != INST_IO){
    recGblRecordError(S_db_badField, ai, "DevAiSAA (init_record) Illegal INP field.");
    return S_db_badField; // error output
  }

  pos = input.find("."); //find delimiter
  const char *c = input.substr(0,pos).c_str();
  saainfo->n_saa = std::atoi(c);
  input.erase(0, pos+1); //erase to delimiter

  c = input.c_str();
  saainfo->boardID = std::atoi(c);

  reg = (int*)malloc(sizeof(*reg) * saainfo->n_saa);
  ai->dpvt=saainfo;
  return 0;
}

static long version_read_ai( aiRecord *ai ) {
  saa_info *saainfo = (saa_info *)ai->dpvt;
  epicsUInt32 version = hv->m_spi->read32bit(0xff, saainfo->boardID);
  ai->val = version;
  return 2;
}

//-----------------------------------------------------//

//-----------------------LM75--------------------------//

static long lm75_init_record(aiRecord *ai) {
  lm75_info *lm75info;
  lm75info = (lm75_info *) calloc( 1, sizeof( lm75_info ) );

  std::string input = ai->inp.value.instio.string;
  size_t pos = 0;

  if(ai->inp.type != INST_IO){
    recGblRecordError(S_db_badField, ai, "DevAiLM75 (init_record) Illegal INP field.");
    return S_db_badField; // error output
  }

  pos = input.find("."); //find delimiter
  const char *c = input.substr(0,pos).c_str();
  lm75info->bit_rate = std::atoi(c);
  input.erase(0, pos+1); //erase to delimiter

  c = input.c_str();
  lm75info->boardID = std::atoi(c);

  ai->dpvt=lm75info;
  return 0;
}

static long lm75_read_ai( aiRecord *ai ) {
  lm75_info *lm75info = (lm75_info *)ai->dpvt;
  float temp = hv->m_lm75->read(lm75info->bit_rate, lm75info->boardID);
  ai->val=temp;
  return 2;
}

//-----------------------------------------------------//

//-----------------------SHT21-------------------------//

static long sht21_init_record(aiRecord *ai) {
  sht21_info *sht21info;
  sht21info = (sht21_info *) calloc( 1, sizeof( sht21_info ) );

  std::string input = ai->inp.value.instio.string;
  size_t pos = 0;

  if(ai->inp.type != INST_IO){
    recGblRecordError(S_db_badField, ai, "DevAiSHT21 (init_record) Illegal INP field.");
    return S_db_badField; // error output
  }

  const char *c = input.substr(0,1).c_str();
  sht21info->meas_typ = std::atoi(c);
  input.erase(0, 2); //erase to delimiter

  pos = input.find("."); //find delimiter
  c = input.substr(0,pos).c_str();
  sht21info->bit_rate = std::atoi(c);
  input.erase(0, pos+1); //erase to delimiter

  c = input.c_str();
  sht21info->boardID = std::atoi(c);

  ai->dpvt=sht21info;
  return 0;
}

static long sht21_read_ai( aiRecord *ai ) {
  sht21_info *sht21info = (sht21_info *)ai->dpvt;
  if(sht21info->meas_typ == 0){
	ai->val = hv->m_sht21->read_temp(sht21info->bit_rate, sht21info->boardID);
	return 2;
  }
  else{
	ai->val = hv->m_sht21->read_hum(sht21info->bit_rate, sht21info->boardID);
	return 2;
  }
}

//-----------------------------------------------------//

//-----------------------GETDAC------------------------//

static long dac_ai_init_record(aiRecord *ai) {
  dac_ai_info *dacaiinfo;
  dacaiinfo = (dac_ai_info *) calloc( 1, sizeof( dac_ai_info ) );

  std::string input = ai->inp.value.instio.string;

  if(ai->inp.type != INST_IO){
    recGblRecordError(S_db_badField, ai, "DevAiDAC (init_record) Illegal INP field.");
    return S_db_badField; // error output
  }

  const char *c = input.substr(0,1).c_str();
  dacaiinfo->channel = std::atoi(c);
  input.erase(0, 2); //erase to delimiter

  c = input.c_str();
  dacaiinfo->boardID = std::atoi(c);

  ai->dpvt=dacaiinfo;
  return 0;
}

static long dac_ai_read_ai( aiRecord *ai ) {
  dac_ai_info *dacaiinfo = (dac_ai_info *)ai->dpvt;
  ai->val=hv->getDAC(dacaiinfo->channel, dacaiinfo->boardID);
  return 2;
}

//-----------------------------------------------------//

//-----------------------SETDAC------------------------//

static long dac_ao_init_record(aoRecord *ao) {
  dac_ao_info *dacaoinfo;
  dacaoinfo = (dac_ao_info *) calloc( 1, sizeof( dac_ao_info ) );

  std::string input = ao->out.value.instio.string;

  if(ao->out.type != INST_IO){
    recGblRecordError(S_db_badField, ao, "DevAoDAC (init_record) Illegal INP field.");
    return S_db_badField; // error output
  }

  const char *c = input.substr(0,1).c_str();
  dacaoinfo->channel = std::atoi(c);
  input.erase(0, 2); //erase to delimiter

  c = input.c_str();
  dacaoinfo->boardID = std::atoi(c);

  ao->dpvt=dacaoinfo;
  return 0;
}

static long dac_ao_write_ao( aoRecord *ao ) {
  dac_ao_info *dacaoinfo = (dac_ao_info *)ao->dpvt;

  hv->setVoltage(dacaoinfo->channel,ao->oval,dacaoinfo->boardID);
  return 0;
}

//-----------------------------------------------------//

//-----------------------ADS1115-----------------------//

static long ads1115_init_record(aiRecord *ai) {
  ads1115_info *ads1115info;
  ads1115info = (ads1115_info *) calloc( 1, sizeof( ads1115_info ) );

  std::string input = ai->inp.value.instio.string;
  size_t pos = 0;

  if(ai->inp.type != INST_IO){
    recGblRecordError(S_db_badField, ai, "DevAiAds1115 (init_record) Illegal INP field.");
    return S_db_badField; // error output
  }

  const char *c = input.substr(0,1).c_str();
  ads1115info->meas_typ = std::atoi(c);
  input.erase(0, 2); //erase to delimiter

  pos = input.find("."); //find delimiter
  c = input.substr(0,pos).c_str();
  ads1115info->channel = std::atoi(c);
  input.erase(0, pos+1); //erase to delimiter

  pos = input.find("."); //find delimiter
  c = input.substr(0,pos).c_str();
  ads1115info->bit_rate = std::atoi(c);
  input.erase(0, pos+1); //erase to delimiter

  c = input.c_str();
  ads1115info->boardID = std::atoi(c);

  ai->dpvt=ads1115info;
  return 0;
}

static long ads1115_read_ai( aiRecord *ai ) {
  ads1115_info *ads1115info = (ads1115_info *)ai->dpvt;
  if(ads1115info->meas_typ==0){
	ai->val=hv->readVoltage(ads1115info->channel, ads1115info->boardID, ads1115info->bit_rate);
	return 2;
  }
  else{
	ai->val=hv->readCurrent(ads1115info->channel, ads1115info->boardID, ads1115info->bit_rate);
	return 2;
  }
}

//-----------------------------------------------------//

//-------------------GET-EEPROM------------------------//

static long eeprom_get_init_record(aiRecord *ai) {
  eeprom_get_info *eepromgetinfo;
  eepromgetinfo = (eeprom_get_info *) calloc( 1, sizeof( eeprom_get_info ) );

  std::string input = ai->inp.value.instio.string;
  size_t pos = 0;

  if(ai->inp.type != INST_IO){
    recGblRecordError(S_db_badField, ai, "DevAiEEPROM (init_record) Illegal INP field.");
    return S_db_badField; // error output
  }

  pos = input.find("."); //find delimiter
  const char *c = input.substr(0,pos).c_str();
  eepromgetinfo->mode = std::atoi(c);
  input.erase(0, pos+1); //erase to delimiter

  pos = input.find("."); //find delimiter
  c = input.substr(0,pos).c_str();
  eepromgetinfo->bitrate = std::atoi(c);
  input.erase(0, pos+1); //erase to delimiter

  c = input.c_str();
  eepromgetinfo->boardID = std::atoi(c);

  ai->dpvt=eepromgetinfo;
  return 0;
}

static long eeprom_get_read_ai( aiRecord *ai ) {
  eeprom_get_info *eepromgetinfo = (eeprom_get_info *)ai->dpvt;
  if(eepromgetinfo->mode == 0){
  ai->rval=hv->m_eeprom->read(reg[eepromgetinfo->boardID], eepromgetinfo->bitrate, eepromgetinfo->boardID) ;
  return 0;
  }
  else{
  std::vector<uint8_t> dataraw = hv->m_eeprom->read_nbytes(0xfa,6);
  double version = 0;
  for(int i = 4; i >=0; i--) {
        version += dataraw[i]/16 *pow(16,(4-i)*2+1) + dataraw[i]%16*pow(16,(4-i)*2);
  }

  ai->val = version;
  return 2;
  }
}

//-----------------------------------------------------//

//-------------------REG-EEPROM------------------------//

static long eeprom_reg_init_record(aoRecord *ao) {
  eeprom_reg_info *eepromreginfo;
  eepromreginfo = (eeprom_reg_info *) calloc( 1, sizeof( eeprom_reg_info ) );

  std::string input = ao->out.value.instio.string;
  size_t pos = 0;

  if(ao->out.type != INST_IO){
    recGblRecordError(S_db_badField, ao, "DevAoEEPROMREG (init_record) Illegal INP field.");
    return S_db_badField; // error output
  }

  pos = input.find("."); //find delimiter
  const char *c = input.substr(0,pos).c_str();
  eepromreginfo->bitrate = std::atoi(c);
  input.erase(0, pos+1); //erase to delimiter

  c = input.c_str();
  eepromreginfo->boardID = std::atoi(c);

  ao->dpvt=eepromreginfo;
  return 0;
}

static long eeprom_reg_read_ao( aoRecord *ao ) {
  eeprom_reg_info *eepromreginfo = (eeprom_reg_info *)ao->dpvt;
  reg[eepromreginfo->boardID] = ao->rval;
  return 0;
}

//-----------------------------------------------------//

//-------------------SET-EEPROM------------------------//

static long eeprom_set_init_record(aoRecord *ao) {
  eeprom_set_info *eepromsetinfo;
  eepromsetinfo = (eeprom_set_info *) calloc( 1, sizeof( eeprom_set_info ) );

  std::string input = ao->out.value.instio.string;
  size_t pos = 0;

  if(ao->out.type != INST_IO){
    recGblRecordError(S_db_badField, ao, "DevAiEEPROMREG (init_record) Illegal INP field.");
    return S_db_badField; // error output
  }

  pos = input.find("."); //find delimiter
  const char *c = input.substr(0,pos).c_str();
  eepromsetinfo->bitrate = std::atoi(c);
  input.erase(0, pos+1); //erase to delimiter

  c = input.c_str();
  eepromsetinfo->boardID = std::atoi(c);

  ao->dpvt=eepromsetinfo;
  return 0;
}

static long eeprom_set_read_ao( aoRecord *ao ) {
  eeprom_set_info *eepromsetinfo = (eeprom_set_info *)ao->dpvt;
  hv->m_eeprom->write(reg[eepromsetinfo->boardID],ao->oval, eepromsetinfo->bitrate, eepromsetinfo->boardID);
  return 0;
}

//-----------------------------------------------------//

//-------------------GET-PCA9554-----------------------//
// mode = 0 -> status led; mode = 1 -> status apfel pin 1 ...
static long pca9554_ai_init_record(aiRecord *ai) {
  pca9554_ai_info *pca9554aiinfo;
  pca9554aiinfo = (pca9554_ai_info *) calloc( 1, sizeof( pca9554_ai_info ) );

  std::string input = ai->inp.value.instio.string;
  size_t pos = 0;

  if(ai->inp.type != INST_IO){
    recGblRecordError(S_db_badField, ai, "DevAiPCA9554 (init_record) Illegal INP field.");
    return S_db_badField; // error output
  }

  const char *c = input.substr(0,1).c_str();
  pca9554aiinfo->mode = std::atoi(c);
  input.erase(0, 2); //erase to delimiter

  pos = input.find("."); //find delimiter
  c = input.substr(0,pos).c_str();
  pca9554aiinfo->bitrate = std::atoi(c);
  input.erase(0, pos+1); //erase to delimiter

  c = input.c_str();
  pca9554aiinfo->boardID = std::atoi(c);

  ai->dpvt=pca9554aiinfo;
  return 0;
}

static long pca9554_ai_read_ai( aiRecord *ai ) {
  pca9554_ai_info *pca9554aiinfo = (pca9554_ai_info *)ai->dpvt;
  if(pca9554aiinfo->mode == 0){
    ai->rval=hv->m_pca->status_led(pca9554aiinfo->bitrate, pca9554aiinfo->boardID);
    return 0;
  }
  else if(pca9554aiinfo->mode == 1){
    ai->rval=hv->m_pca->status_apfel_pin((pca9554aiinfo->mode-1),pca9554aiinfo->bitrate, pca9554aiinfo->boardID);
  return 0;
  }
  else return 0;
}

//-----------------------------------------------------//

//-------------------SET-PCA9554-----------------------//
// off/on:  0/1->led  2/3->single  4/5-> apfel
static long pca9554_ao_init_record(aoRecord *ao) {
  pca9554_ao_info *pca9554aoinfo;
  pca9554aoinfo = (pca9554_ao_info *) calloc( 1, sizeof( pca9554_ao_info ) );

  std::string input = ao->out.value.instio.string;
  size_t pos = 0;

  if(ao->out.type != INST_IO){
    recGblRecordError(S_db_badField, ao, "DevAoPCA9554 (init_record) Illegal INP field.");
    return S_db_badField; // error output
  }

  const char *c = input.substr(0,1).c_str();
  pca9554aoinfo->mode = std::atoi(c);
  input.erase(0, 2); //erase to delimiter

  pos = input.find("."); //find delimiter
  c = input.substr(0,pos).c_str();
  pca9554aoinfo->bitrate = std::atoi(c);
  input.erase(0, pos+1); //erase to delimiter

  c = input.c_str();
  pca9554aoinfo->boardID = std::atoi(c);

  ao->dpvt=pca9554aoinfo;
  return 0;
}

static long pca9554_ao_read_ao( aoRecord *ao ) {
  pca9554_ao_info *pca9554aoinfo = (pca9554_ao_info *)ao->dpvt;
  switch(pca9554aoinfo->mode)
    {
      case 0: // turn off led
          hv->m_pca->toggle_led_off(pca9554aoinfo->bitrate,pca9554aoinfo->boardID);
          break;
      case 1: // turn on led
          hv->m_pca->toggle_led_on(pca9554aoinfo->bitrate,pca9554aoinfo->boardID);
          break;
      case 2: // turn off single pin
          hv->m_pca->toggle_off(ao->oval,pca9554aoinfo->bitrate,pca9554aoinfo->boardID);
          break;
      case 3: // turn on single pin
          hv->m_pca->toggle_on(ao->oval,pca9554aoinfo->bitrate,pca9554aoinfo->boardID);
          break;
      case 4: // turn off apfel pins
          hv->m_pca->toggle_apfel_off(pca9554aoinfo->bitrate,pca9554aoinfo->boardID);
          break;
      case 5: // turn on apfel pins
          hv->m_pca->toggle_apfel_on(pca9554aoinfo->bitrate,pca9554aoinfo->boardID);
          break;
    }
  return 0;
}

//-----------------------------------------------------//

//---------------------APFEL-Ai------------------------//

static long apfel_ai_init_record(aiRecord *ai) {
  apfel_ai_info *apfelaiinfo;
  apfelaiinfo = (apfel_ai_info *) calloc( 1, sizeof( apfel_ai_info ) );

  std::string input = ai->inp.value.instio.string;
  size_t pos = 0;

  if(ai->inp.type != INST_IO){
    recGblRecordError(S_db_badField, ai, "DevAiAPFEL (init_record) Illegal INP field.");
    return S_db_badField; // error output
  }

  pos = input.find("."); //find delimiter
  const char *c = input.substr(0,pos).c_str();
  apfelaiinfo->chipid = std::atoi(c);
  input.erase(0, pos+1); //erase to delimiter

  pos = input.find("."); //find delimiter
  c = input.substr(0,pos).c_str();
  apfelaiinfo->channel = std::atoi(c);
  input.erase(0, pos+1); //erase to delimiter

  c = input.c_str();
  apfelaiinfo->boardID = std::atoi(c);

  ai->dpvt=apfelaiinfo;
  return 0;
}

static long apfel_ai_read_ai( aiRecord *ai ) {
  apfel_ai_info *apfelaiinfo = (apfel_ai_info *)ai->dpvt;
  ai->val=hv->m_apfel->read_DAC(apfelaiinfo->chipid, apfelaiinfo->channel, apfelaiinfo->boardID);
  return 2;
}

//-----------------------------------------------------//

//-----------------------APFEL-AO----------------------//

static long apfel_ao_init_record(aoRecord *ao) {
  apfel_ao_info *apfelaoinfo;
  apfelaoinfo = (apfel_ao_info *) calloc( 1, sizeof( apfel_ao_info ) );

  std::string input = ao->out.value.instio.string;
  size_t pos = 0;

  if(ao->out.type != INST_IO){
    recGblRecordError(S_db_badField, ao, "DevAoAPFEL (init_record) Illegal INP field.");
    return S_db_badField; // error output
  }

  pos = input.find("."); //find delimiter
  const char *c = input.substr(0,pos).c_str();
  apfelaoinfo->chipid = std::atoi(c);
  input.erase(0, pos+1); //erase to delimiter

  pos = input.find("."); //find delimiter
  c = input.substr(0,pos).c_str();
  apfelaoinfo->channel = std::atoi(c);
  input.erase(0, pos+1); //erase to delimiter

  c = input.c_str();
  apfelaoinfo->boardID = std::atoi(c);

  ao->dpvt=apfelaoinfo;
  return 0;
}

static long apfel_ao_write_ao( aoRecord *ao ) {
  apfel_ao_info *apfelaoinfo = (apfel_ao_info *)ao->dpvt;
  hv->m_apfel->set_DAC(apfelaoinfo->chipid, apfelaoinfo->channel, ao->oval, apfelaoinfo->boardID);
  return 0;
}

//-----------------------------------------------------//

//-----------------------APFEL-BO----------------------//

static long apfel_bo_init_record(boRecord *bo) {
  apfel_bo_info *apfelboinfo;
  apfelboinfo = (apfel_bo_info *) calloc( 1, sizeof( apfel_bo_info ) );

  std::string input = bo->out.value.instio.string;
  size_t pos = 0;

  if(bo->out.type != INST_IO){
    recGblRecordError(S_db_badField, bo, "DevBoAPFEL (init_record) Illegal INP field.");
    return S_db_badField; // error output
  }

  const char *c = input.substr(0,1).c_str();
  apfelboinfo->mode = std::atoi(c);
  input.erase(0, 2); //erase to delimiter

  pos = input.find("."); //find delimiter
  c = input.substr(0,pos).c_str();
  apfelboinfo->chipid = std::atoi(c);
  input.erase(0, pos+1); //erase to delimiter


  pos = input.find("."); //find delimiter
  c = input.substr(0,pos).c_str();
  apfelboinfo->channel = std::atoi(c);
  input.erase(0, pos+1); //erase to delimiter

  c = input.c_str();
  apfelboinfo->boardID = std::atoi(c);

  bo->dpvt=apfelboinfo;
  return 0;
}

static long apfel_bo_write_bo( boRecord *bo ) {
  apfel_bo_info *apfelboinfo = (apfel_bo_info *)bo->dpvt;
  switch(apfelboinfo->mode)
  {
    case 0: // set DAC value
  	if(bo->val ==1){
  	hv->m_apfel->set_amp_mode(apfelboinfo->chipid, apfelboinfo->channel, 32, apfelboinfo->boardID);
  	}
  	else hv->m_apfel->set_amp_mode(apfelboinfo->chipid, apfelboinfo->channel, 16, apfelboinfo->boardID);
  	break;
   case 1:
	if(bo->val == 1){
	hv->m_apfel->global_reset(apfelboinfo->chipid, apfelboinfo->boardID, 0);
	}
	break;
   case 3:
	if(bo->val == 1){
	hv->m_apfel->autocalib(apfelboinfo->chipid, apfelboinfo->boardID);
	}
	break;
   case 2:
	if(bo->val == 1){
	hv->m_apfel->autocalib_all(apfelboinfo->boardID);
	}
	break;
   case 4:
	if(bo->val == 1){
	hv->m_apfel->sw_autocalib_all_ch(apfelboinfo->chipid, apfelboinfo->boardID,0);
	}
	break;
   case 5:
	if(bo->val == 1){
	hv->m_apfel->sw_autocalib_all(apfelboinfo->boardID,0);
	}
	break;
  }
  return 0;
}


//-----------------------------------------------------//


//-----------------------DSET-STRUCTS------------------//
static struct {
  long   number;                		/* number of device support routines */
  long   (*report)(int);           		/* print reports */
  long   (*init)(int);           		/* initialization of device support module */
  long   (*initRecord)(aiRecord *);      	/* initialization of single record instances */
  long   (*getIoIntInfo)();  			/* get I/O interrupt info */
  long   (*read)(aiRecord *);          		/* read/write value */
  long   (*special_linconv)(aiRecord *, int);  	/* Calculate ESLO from EGUL and EGUF for ai/ao records */
} dev_ai_SAA =
{
  6, NULL, NULL, version_init_record, NULL, version_read_ai, NULL
};

static struct {
  long   number;		                /* number of device support routines */
  long   (*report)(int);        		/* print reports */
  long   (*init)(int);             		/* initialization of device support module */
  long   (*initRecord)(aiRecord *);    		/* initialization of single record instances */
  long   (*getIoIntInfo)();  			/* get I/O interrupt info */
  long   (*read)(aiRecord *);          		/* read/write value */
  long   (*special_linconv)(aiRecord *, int);  	/* Calculate ESLO from EGUL and EGUF for ai/ao records */
} dev_ai_lm75 =
{
  6, NULL, NULL, lm75_init_record, NULL, lm75_read_ai, NULL
};

static struct {
  long   number;                		/* number of device support routines */
  long   (*report)(int);           		/* print reports */
  long   (*init)(int);            		/* initialization of device support module */
  long   (*initRecord)(aiRecord *);     	/* initialization of single record instances */
  long   (*getIoIntInfo)();   			/* get I/O interrupt info */
  long   (*read)(aiRecord *);   	        /* read/write value */
  long   (*special_linconv)(aiRecord *, int);   /* Calculate ESLO from EGUL and EGUF for ai/ao records */
} dev_ai_sht21 =
{
  6, NULL, NULL, sht21_init_record, NULL, sht21_read_ai, NULL
};

static struct {
  long   number;     			         /* number of device support routines */
  long   (*report)(int);         		 /* print reports */
  long   (*init)(int);            		 /* initialization of device support module */
  long   (*initRecord)(aiRecord *);    		 /* initialization of single record instances */
  long   (*getIoIntInfo)();  			 /* get I/O interrupt info */
  long   (*read)(aiRecord *);         		 /* read/write value */
  long   (*special_linconv)(aiRecord *, int);    /* Calculate ESLO from EGUL and EGUF for ai/ao records */
} dev_ai_dac =
{
  6, NULL, NULL, dac_ai_init_record, NULL, dac_ai_read_ai, NULL
};

static struct {
  long   number;		                /* number of device support routines */
  long   (*report)(int); 		        /* print reports */
  long   (*init)(int);         			/* initialization of device support module */
  long   (*initRecord)(aoRecord *);   		/* initialization of single record instances */
  long   (*getIoIntInfo)();  			/* get I/O interrupt info */
  long   (*write)(aoRecord *);         		/* read/write value */
  long   (*special_linconv)(aoRecord *, int);   /* Calculate ESLO from EGUL and EGUF for ao/ao records */
} dev_ao_dac =
{
  6, NULL, NULL, dac_ao_init_record, NULL, dac_ao_write_ao, NULL
};

static struct {
  long   number;                		/* number of device support routines */
  long   (*report)(int);           		/* print reports */
  long   (*init)(int);	 	         	/* initialization of device support module */
  long   (*initRecord)(aiRecord *);	        /* initialization of single record instances */
  long   (*getIoIntInfo)();  	 		/* get I/O interrupt info */
  long   (*read)(aiRecord *);    	        /* read/write value */
  long   (*special_linconv)(aiRecord *, int);   /* Calculate ESLO from EGUL and EGUF for ai/ao records */
} dev_ai_ads1115 =
{
  6, NULL, NULL, ads1115_init_record, NULL, ads1115_read_ai, NULL
};

static struct {
  long   number;                		/* number of device support routines */
  long   (*report)(int);           		/* print reports */
  long   (*init)(int);	 	         	/* initialization of device support module */
  long   (*initRecord)(aiRecord *);	        /* initialization of single record instances */
  long   (*getIoIntInfo)();  	 		/* get I/O interrupt info */
  long   (*read)(aiRecord *);    	        /* read/write value */
  long   (*special_linconv)(aiRecord *, int);   /* Calculate ESLO from EGUL and EGUF for ai/ao records */
} dev_ai_eeprom =
{
  6, NULL, NULL, eeprom_get_init_record, NULL, eeprom_get_read_ai, NULL
};

static struct {
  long   number;                		/* number of device support routines */
  long   (*report)(int);           		/* print reports */
  long   (*init)(int);	 	         	/* initialization of device support module */
  long   (*initRecord)(aoRecord *);	        /* initialization of single record instances */
  long   (*getIoIntInfo)();  	 		/* get I/O interrupt info */
  long   (*read)(aoRecord *);    	        /* read/write value */
  long   (*special_linconv)(aoRecord *, int);   /* Calculate ESLO from EGUL and EGUF for ai/ao records */
} dev_ao_eeprom_reg =
{
  6, NULL, NULL, eeprom_reg_init_record, NULL, eeprom_reg_read_ao, NULL
};

static struct {
  long   number;                		/* number of device support routines */
  long   (*report)(int);           		/* print reports */
  long   (*init)(int);	 	         	/* initialization of device support module */
  long   (*initRecord)(aoRecord *);	        /* initialization of single record instances */
  long   (*getIoIntInfo)();  	 		/* get I/O interrupt info */
  long   (*write)(aoRecord *);    	        /* read/write value */
  long   (*special_linconv)(aoRecord *, int);   /* Calculate ESLO from EGUL and EGUF for ai/ao records */
} dev_ao_eeprom_set =
{
  6, NULL, NULL, eeprom_set_init_record, NULL, eeprom_set_read_ao, NULL
};

static struct {
  long   number;                		/* number of device support routines */
  long   (*report)(int);           		/* print reports */
  long   (*init)(int);	 	         	/* initialization of device support module */
  long   (*initRecord)(aiRecord *);	        /* initialization of single record instances */
  long   (*getIoIntInfo)();  	 		/* get I/O interrupt info */
  long   (*read)(aiRecord *);    	        /* read/write value */
  long   (*special_linconv)(aiRecord *, int);   /* Calculate ESLO from EGUL and EGUF for ai/ao records */
} dev_ai_pca9554 =
{
  6, NULL, NULL, pca9554_ai_init_record, NULL, pca9554_ai_read_ai, NULL
};

static struct {
  long   number;                		/* number of device support routines */
  long   (*report)(int);           		/* print reports */
  long   (*init)(int);	 	         	/* initialization of device support module */
  long   (*initRecord)(aoRecord *);	        /* initialization of single record instances */
  long   (*getIoIntInfo)();  	 		/* get I/O interrupt info */
  long   (*write)(aoRecord *);    	        /* read/write value */
  long   (*special_linconv)(aoRecord *, int);   /* Calculate ESLO from EGUL and EGUF for ai/ao records */
} dev_ao_pca9554 =
{
  6, NULL, NULL, pca9554_ao_init_record, NULL, pca9554_ao_read_ao, NULL
};

static struct {
  long   number;                                 /* number of device support routines */
  long   (*report)(int);                         /* print reports */
  long   (*init)(int);                           /* initialization of device support module */
  long   (*initRecord)(aiRecord *);              /* initialization of single record instances */
  long   (*getIoIntInfo)();                      /* get I/O interrupt info */
  long   (*read)(aiRecord *);                    /* read/write value */
  long   (*special_linconv)(aiRecord *, int);    /* Calculate ESLO from EGUL and EGUF for ai/ao records */
} dev_ai_apfel =
{
  6, NULL, NULL, apfel_ai_init_record, NULL, apfel_ai_read_ai, NULL
};

static struct {
  long   number;                                /* number of device support routines */
  long   (*report)(int);                        /* print reports */
  long   (*init)(int);                          /* initialization of device support module */
  long   (*initRecord)(aoRecord *);             /* initialization of single record instances */
  long   (*getIoIntInfo)();                     /* get I/O interrupt info */
  long   (*write)(aoRecord *);                  /* read/write value */
  long   (*special_linconv)(aoRecord *, int);   /* Calculate ESLO from EGUL and EGUF for ao/ao records */
} dev_ao_apfel =
{
  6, NULL, NULL, apfel_ao_init_record, NULL, apfel_ao_write_ao, NULL
};

static struct {
  long   number;                                /* number of device support routines */
  long   (*report)(int);                        /* print reports */
  long   (*init)(int);                          /* initialization of device support module */
  long   (*initRecord)(boRecord *);             /* initialization of single record instances */
  long   (*getIoIntInfo)();                     /* get I/O interrupt info */
  long   (*write)(boRecord *);                  /* read/write value */
  long   (*special_linconv)(boRecord *, int);   /* Calculate ESLO from EGUL and EGUF for ao/ao records */
} dev_bo_apfel =
{
  6, NULL, NULL, apfel_bo_init_record, NULL, apfel_bo_write_bo, NULL
};


//----------------Device-Exports------------------//
epicsExportAddress(dset, dev_ai_SAA);//		SAA version register
epicsExportAddress(dset, dev_ai_lm75);//	LM75
epicsExportAddress(dset, dev_ai_sht21);// 	SHT21
epicsExportAddress(dset, dev_ai_dac);//		DAC AI/GET
epicsExportAddress(dset, dev_ao_dac);//		DAC AO/SET
epicsExportAddress(dset, dev_ai_ads1115);//	ADS1115
epicsExportAddress(dset, dev_ai_eeprom);//	EEPROM GET
epicsExportAddress(dset, dev_ao_eeprom_reg);//	EEPROM SET REG
epicsExportAddress(dset, dev_ao_eeprom_set);//	EEPROM SET VALUE
epicsExportAddress(dset, dev_ai_pca9554);//	PCA9554
epicsExportAddress(dset, dev_ao_pca9554);//	PCA9554
epicsExportAddress(dset, dev_ai_apfel);//   	APFEL DAC AI/GET
epicsExportAddress(dset, dev_ao_apfel);//   	APFEL DAC AO/SET
epicsExportAddress(dset, dev_bo_apfel);//   	APFEL DAC BO/SET


