#define RM3100Address 0x20 // Hexadecimal slave address for RM3100 with Pin 2 and Pin 4 set to LOW

//pin definitions
#define PIN_DRDY 9 //Set pin D9 to be the Data Ready Pin

//internal register values without the R/W bit
#define RM3100_REVID_REG 0x36 // Hexadecimal address for the Revid internal register
#define RM3100_POLL_REG 0x00 // Hexadecimal address for the Poll internal register
#define RM3100_CMM_REG 0x01 // Hexadecimal address for the CMM internal register
#define RM3100_STATUS_REG 0x34 // Hexadecimal address for the Status internal register
#define RM3100_CCX1_REG 0x04 // Hexadecimal address for Cycle Count X1 internal register
#define RM3100_CCX0_REG 0x05 // Hexadecimal address for the Cycle Count X0 internal register

//var declarations
extern float gain;


// Functions Declarations
uint8_t readReg(uint8_t addr);//addr is the 7 bit value of the register's address (without the R/W bit)
void writeReg(uint8_t addr, uint8_t data);//addr is the 7 bit (No r/w bit) value of the internal register's address, data is 8 bit data being written
void changeCycleCount(uint16_t newCC);//newCC is the new cycle count value (16 bits) to change the data acquisition
bool rm3100_init(uint8_t measmode);//0 = use continuous measurement mode; 1 = use single measurement mode
