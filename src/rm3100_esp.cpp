#include <Arduino.h>
#include <Wire.h>
#include "rm3100_esp.h"

//options
#define initialCC 200 // Set the cycle count
#define singleMode 0 //0 = use continuous measurement mode; 1 = use single measurement mode
#define useDRDYPin 1 //0 = not using DRDYPin ; 1 = using DRDYPin to wait for data

uint8_t revid;
uint16_t cycleCount;
float gain;


//addr is the 7 bit value of the register's address (without the R/W bit)
uint8_t readReg(uint8_t addr){
  uint8_t data = 0;
  
  // Enable transmission to specific which register to read from
  Wire.beginTransmission(RM3100Address);
  Wire.write(addr); //request from the REVID register
  Wire.endTransmission();

  delay(100);

  // Request 1 byte from the register specified earlier
  Wire.requestFrom(RM3100Address, 1);
  if(Wire.available() == 1) {
    data = Wire.read();
  }
  return data;
}

//addr is the 7 bit (No r/w bit) value of the internal register's address, data is 8 bit data being written
void writeReg(uint8_t addr, uint8_t data){
  Wire.beginTransmission(RM3100Address);
  Wire.write(addr);
  Wire.write(data);
  Wire.endTransmission();
}

//newCC is the new cycle count value (16 bits) to change the data acquisition
void changeCycleCount(uint16_t newCC){
  uint8_t CCMSB = (newCC & 0xFF00) >> 8; //get the most significant byte
  uint8_t CCLSB = newCC & 0xFF; //get the least significant byte
  
  Wire.beginTransmission(RM3100Address);
  Wire.write(RM3100_CCX1_REG);
  Wire.write(CCMSB);  //write new cycle count to ccx1
  Wire.write(CCLSB);  //write new cycle count to ccx0
  Wire.write(CCMSB);  //write new cycle count to ccy1
  Wire.write(CCLSB);  //write new cycle count to ccy0
  Wire.write(CCMSB);  //write new cycle count to ccz1
  Wire.write(CCLSB);  //write new cycle count to ccz0     
  Wire.endTransmission();  
}

//0 = use continuous measurement mode; 1 = use single measurement mode
bool rm3100_init(uint8_t measmode)
{
    //revision identification of the MagI2C ASIC
  revid = readReg(RM3100_REVID_REG);
  
  Serial.print("REVID ID = 0x"); //REVID ID should be 0x22
  Serial.println(revid, HEX);

  changeCycleCount(initialCC); //change the cycle count; default = 200 (lower cycle count = higher data rates but lower resolution)

  cycleCount = readReg(RM3100_CCX1_REG);
  cycleCount = (cycleCount << 8) | readReg(RM3100_CCX0_REG);

  Serial.print("Cycle Counts = "); //display cycle count
  Serial.println(cycleCount);

  gain = (0.3671 * (float)cycleCount) + 1.5; //linear equation to calculate the gain from cycle count

  Serial.print("Gain = "); //display gain; default gain should be around 75 for the default cycle count of 200
  Serial.println(gain);

if (measmode){
    //set up single measurement mode
    writeReg(RM3100_CMM_REG, 0);
    writeReg(RM3100_POLL_REG, 0x70);
  }
  else{
    // Enable transmission to take continuous measurement with Alarm functions off
    writeReg(RM3100_CMM_REG, 0x79);
  }
  return 1;
}