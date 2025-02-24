#include <esp32_can.h>  // https://github.com/collin80/esp32_can and https://github.com/collin80/can_common CAN libraries
#include <esp32_obd2.h>
#define BAUD_RATE 19200
#define RXD2 16
#define TXD2 17
#define CAN_RX_PIN 13
#define CAN_TX_PIN 14
int PFCSTARTED = 0;  // begin obd only once
///////////// QATIF SPEED FACTORY
//////////// SALEH.M
///////////// APEXI POWER FC ARDUINO SERIAL ADAPTER

// Define command and response arrays
const byte startcmd[] = {0xF9, 0x02, 0x04};
const byte startrspnd[] = {0xF9, 0x03, 0x07, 0xFC};

const byte enginename[] = {0xF3, 0x02, 0x0A};
// const byte enginenamer[] = {0xF3, 0x0A, 0x51, 0x41, 0x54, 0x49, 0x46, 0xEE,
// 0xEE, 0xEE, 0xD5}; //QATIF F3 0a 32 4A 5A 2D 47 54 45 31 EE 2JZGTE const byte
// enginenamer[] =   {0xF3, 0x0A, 0x46, 0x36, 0x41, 0x54, 0x20,0x20,
// 0x20,0x20,0xD5}; //F6A const byte enginenamer[] = {0xF3, 0x0A, 0xFF, 0xFF,
// 0xFF, 0xFF, 0xFF,0xFF, 0xFF, 0xFF, 0xD5}; //FFFF

const byte enginenamer[] = {0xF3, 0x0A, 0x52, 0x42, 0x32, 0x36,
                            0x44, 0x45, 0x54, 0x54, 0xD5};  // RB26 WORKING
// const byte enginenamer[] = {0xF3, 0x0A, 0x20, 0x33, 0x53, 0x2D, 0x47,0x45,
// 0x20, 0x20, 0x63}; //3S-GE WORKING
//  static byte enginenamer[] = {0xF3, 0x0A,0x31, 0x4A, 0x5A, 0x2D, 0x47,0x54,
//  0x45, 0xE2 } ; //1JZGTE

// const byte enginenamer[] = {0xF3, 0x0A, 0x53, 0x52, 0x32, 0x30, 0x54,0x31,
// 0x2D, 0x44,lastbyte }; //SR20
/////////////////////////// ENGINE CMD    R     B     2     6     D    E     T
///T     D5
///////////////////////////////////////// S     R     2     0     T    1    - D
//////  //S 0x53      D  0x44
// const byte enginenamer[] =    {0xF3, 0x0A, 0x20, 0x52, 0x42, 0x32, 0x35,
// 0x44,0x45,0x54,0x01}; //RB25 const byte enginenamer[] = {0xF3, 0x0A, 0x32,
// 0x4A, 0x5A, 0x2D, 0x47, 0x54,0x45,0x31,0xEE}; //2JZGTE WORKING
// const byte enginenamer[] = {0xF3, 0x0A, 0x31, 0x4A, 0x5A, 0x47,0x54,
// 0x45,0x2D, 0x44 ,0xD5} ; //1JZGTE const byte enginenamer[] = {0xF3, 0x0A,
// 0x31, 0x33, 0x42, 0x2D, 0x52,0x45, 0x57, 0x20, 0x21}; //13B       WORKING 52
// 42 32 36 44 45 54 54 //0x51,0x41,0x54,0x49,0x46
const byte ver[] = {0xF5, 0x02, 0x08};
const byte verunknown[] = {0x6B, 0x02, 0x92};

// const byte verr[] = {0xF5, 0x07,  0x46, 0x36, 0x41, 0x54, 0x41, 0xF8};
// const byte verr[] = {0xF5, 0x07, 0x52, 0x00, 0x00, 0x00, 0x00, 0xF8};

// const byte verr[] = {0xF6, 0x07, 0x34, 0x2E, 0x31, 0x31, 0x20, 0x1F};
// const byte verr[] = {0xF5, 0x07, 0x34, 0x2E, 0x31, 0x30, 0x20, 0x20};// 4.10
const byte verr[] = {0xF5, 0x07, 0x34, 0x2E, 0x30, 0x39, 0x41, 0xF7};  // 4.09A
// const byte verr[] = {0xF5, 0x07, 0x36, 0x2E, 0x30, 0x33, 0x20, 0x20};// 6.03
// const byte verr[] = {0xF5, 0x07, 0x37, 0x2E, 0x30, 0x30, 0x20,
// 0x20};// 7.00..

// f6 07 34 2e 31 31 20 1f           F5 07 34 2E 31 30 20 20
////// FUNCTION SELECT
const byte FUNCTIONCMD[] = {0xD6, 0x02, 0x27};

const byte FUNCTIONR[] = {
    0xD6, 0x57, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x31, 0x2E, 0x42, 0x6F,
    0x6F, 0x73, 0x74, 0x20, 0x43, 0x6E, 0x74, 0x6C, 0x20, 0x6B, 0x69,
    0x74, 0x32, 0x2E, 0x49, 0x6E, 0x6A, 0x2F, 0x41, 0x69, 0x72, 0x46,
    0x20, 0x57, 0x61, 0x72, 0x6E, 0x2E, 0x33, 0x2E, 0x4B, 0x6E, 0x6F,
    0x63, 0x6B, 0x20, 0x57, 0x61, 0x72, 0x6E, 0x69, 0x6E, 0x67, 0x20,
    0x34, 0x2E, 0x4F, 0x32, 0x20, 0x46, 0x2F, 0x42, 0x20, 0x43, 0x6F,
    0x6E, 0x74, 0x72, 0x6F, 0x6C, 0x35, 0x2E, 0x4F, 0x32, 0x20, 0x53,
    0x65, 0x6E, 0x73, 0x6F, 0x72, 0x20, 0x52, 0x65, 0x67, 0x2E, 0x4A};

const byte INJECTORADJUST[] = {0xDC, 0x02, 0x21};
const byte INJECTORREPLY[] = {0xDC, 0x08, 0x00, 0x00, 0x00,
                              0x00, 0x00, 0x00, 0x1B};
///////////BOOST
const byte BOOSTSETTING[] = {0xD2, 0x02, 0x2B};
const byte BOOSTREPLY[] = {0xD5, 0x0F, 0x00, 0x13, 0x12, 0x14, 0x18, 0xFF,
                           0xFF, 0xFF, 0xFF, 0x24, 0x2A, 0x2E, 0x38, 0x1A};
///////////CRANKING
const byte CRANKING[] = {0xDC, 0x02, 0x21};
const byte CRANKINGR[] = {0xD2, 0x0E, 0xE2, 0x04, 0xD0, 0x07, 0xB8, 0x0B,
                          0x6A, 0x18, 0xD4, 0x30, 0xB4, 0x5F, 0x06};
///////////WATERTEMP
const byte WATER[] = {0xD3, 0x02, 0x2A};
const byte WATERR[] = {0xD3, 0x0E, 0x40, 0x43, 0x49, 0x50, 0x60, 0x66,
                       0x40, 0x46, 0x53, 0x60, 0x70, 0x80, 0x13};
///////////REVLIMIT
const byte REVLIMIT[] = {0xD4, 0x02, 0x29};
const byte REVLIMITR[] = {0xD4, 0x0C, 0xA4, 0x1F, 0xB0, 0x04, 0x14,
                          0x05, 0x84, 0x03, 0xB6, 0x03, 0x4F};
///////////ACCELER
const byte ACCEL[] = {0xD1, 0x02, 0x2C};
const byte ACCELR[] = {0xD1, 0x16, 0xE8, 0x03, 0x65, 0x04, 0xE2, 0x04,
                       0x65, 0x04, 0x6B, 0x03, 0xFA, 0x00, 0x2C, 0x01,
                       0x77, 0x01, 0x2C, 0x01, 0xFA, 0x00, 0x41};

const byte AIRFLOWMENU[] = {0xCE, 0x02, 0x2F};  /// 0x51,0x41,0x49,0x46,0x0A
const byte AIRFLOWREPLY[] = {
    0xCE, 0x6D, 0x03, 0x20, 0x31, 0x2E, 0x42, 0x43, 0x4E, 0x52, 0x33,
    0x33, 0x20, 0x4E, 0x6F, 0x72, 0x6D, 0x61, 0x6C, 0x20, 0x00, 0x20,
    0x32, 0x2E, 0x42, 0x4E, 0x52, 0x33, 0x32, 0x20, 0x20, 0x4E, 0x6F,
    0x72, 0x6D, 0x61, 0x6C, 0x20, 0x00, 0x20, 0x33, 0x2E, 0x50, 0x6F,
    0x77, 0x65, 0x72, 0x20, 0x49, 0x6E, 0x74, 0x61, 0x6B, 0x65, 0x20,
    0x20, 0x00, 0x20, 0x34, 0x2E, 0x38, 0x30, 0x81, 0x82, 0x52, 0x42,
    0x32, 0x35, 0x20, 0x41, 0x69, 0x72, 0x2D, 0x46, 0x00, 0x20, 0x35,
    0x2E, 0x38, 0x30, 0x81, 0x82, 0x56, 0x47, 0x33, 0x30, 0x20, 0x41,
    0x69, 0x72, 0x2D, 0x46, 0x00, 0xE8, 0x03, 0xE8, 0x03, 0xE8, 0x03,
    0xE8, 0x03, 0xE8, 0x03, 0xE8, 0x03, 0xE8, 0x03, 0xE8, 0x03, 0xA8};

const byte menuboot[] = {0xD7, 0x02, 0x26};
const byte menubootr[] = {0xD7, 0x14, 0x00, 0x00, 0x01, 0x00, 0x01,
                          0x02, 0x03, 0x00, 0x01, 0x02, 0x03, 0x04,
                          0x05, 0x06, 0x07, 0x04, 0x00, 0x01, 0xEC};

const byte menudata[] = {0xCA, 0x02, 0x33};

const byte menudata_full[] = {
    0xCA, 0x52, 0x00, 0x00, 0xCD, 0x05, 0xB3, 0x08, 0x99, 0x0B, 0x7F, 0x0E,
    0x66, 0x11, 0x4C, 0x14, 0x32, 0x17, 0x19, 0x1A, 0xFF, 0x1C, 0xCC, 0x22,
    0x98, 0x28, 0x65, 0x2E, 0xFE, 0x39, 0x97, 0x45, 0x30, 0x51, 0xCA, 0x5C,
    0x63, 0x68, 0xFC, 0x73, 0x2E, 0x8B, 0x20, 0x03, 0xB0, 0x04, 0x40, 0x06,
    0xD0, 0x07, 0x60, 0x09, 0xF0, 0x0A, 0x80, 0x0C, 0x10, 0x0E, 0xA0, 0x0F,
    0x30, 0x11, 0xC0, 0x12, 0x50, 0x14, 0xE0, 0x15, 0x00, 0x19, 0x90, 0x1A,
    0x20, 0x1C, 0x40, 0x1F, 0xD0, 0x20, 0x60, 0x22, 0xF0, 0x23, 0xCA};

const byte monitorreqr[] = {0xF2, 0x02, 0x0B};

const byte tracer[] = {0xDB, 0x02, 0x22};

const byte SENSORCHK[] = {0xDD, 0x02, 0x20};
const byte
    SENSORCHKr[] = {0xDD, 0x52, 0x20, 0x20, 0x20, 0x20,  // AF1
                    0x52, 0x55, 0x53, 0x20,  ////////// R U S (SPACE)
                    0x20, 0x45, 0x46, 0x49,  // E F I
                    0x51, 0x53, 0x20, 0x20, 0x41, 0x50, 0x20, 0x20, 0x54,
                    0x45, 0x20, 0x20, 0x49, 0x45, 0x20, 0x20, 0x46, 0x44,
                    0x20, 0x20,  // 0x51,0x41,0x54,0x49,0x46

                    0x32, 0x49, 0x47, 0x4E, 0x53, 0x52, 0x49, 0x44, 0x4C,
                    0x41, 0x2F, 0x43, 0x50, 0x57, 0x53, 0x4E, 0x54, 0x52,
                    0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x45, 0x00, 0x43,
                    0x57, 0x52, 0x4E, 0x46, 0x2F, 0x50, 0x46, 0x50, 0x31,
                    0x46, 0x50, 0x32, 0x4F, 0x32, 0x48, 0x41, 0x43, 0x52,
                    0x46, 0x41, 0x4E, 0x79};  /// 0x51,0x41,0x49,0x46,0x0A

//---------------------------------  Y      X  --------
// const byte rtracer[] = {0xDB, 0x04, 0x00, 0x00, 0x20};
// const byte rtracer[] = {0xDB, 0x04, 0x16, 0x13, 0x20};
const byte FUNREQ[] = {0xD6, 0x07};
const byte MONREQ[] = {0xD7, 0x14};
const byte SELECTREQ[] = {0xCE, 0x13};

const byte DATAREQ[] = {
    0xDA, 0x02,
    0x23};  // 1     2   3     4   5     1    2    3   4    5     6    7    8 9

////////////////////////// 1    2    3     4    5    6   7     8   9    10   11
///12   13  14   15   16    17  18   19   20    21  22   23

// byte DATAREPLYE[] =    {0xDA,0x16 ,0x00 , 0x00   ,0x00,0x00   ,0x00,0x00
// ,0x00,0x00    ,0x00,0x00,    0x00,0x00  ,  0x00,0x00  ,0x00,0x00  ,0x00,0x00,
// 0x00,0x00 ,0x00};// DONT TOUCH SECOND DEG

const byte SENSREQ[] = {0xDE, 0x02, 0x1F};
const byte
    SENSREPLY[] =
        {
            0xDE, 0x14, 0x00, 0x80, 0x09, 0x00, 0x99, 0x10,
            0x39, 0x01, 0x39, 0x01, 0x70, 0x80, 0x10, 0x10,
            0x90, 0x00, 0x90, 0x00, 0x80, 0x00, 0xFC};  // DONT TOUCH SECOND DEG

const byte MAPINJ_DATA_REQ1[] = {0xC6, 0x02, 0x37};
const byte MAPINJ_DATA_REQ2[] = {0xC7, 0x02, 0x36};
const byte MAPINJ_DATA_REQ3[] = {0xC8, 0x02, 0x35};
const byte MAPINJ_DATA_REQ4[] = {0xC9, 0x02, 0x34};

const byte MAP_DATA_REQ1[] = {0xC1, 0x02, 0x3C};
const byte MAP_DATA_REQ2[] = {0xC2, 0x02, 0x3B};
const byte MAP_DATA_REQ3[] = {0xC3, 0x02, 0x3A};
const byte MAP_DATA_REQ4[] = {0xC4, 0x02, 0x39};

const byte MAP_DATA_RSP[] = {0xC1,
                             0x66,
                             0x00,
                             0x00,
                             0x00,
                             0x00,
                             0x00,
                             0x00,
                             0x00,
                             0x00,
                             0x00,
                             0x00,  // 10
                             0x00,
                             0x00,
                             0x00,
                             0x00,
                             0x00,
                             0x00,
                             0x00,
                             0x00,
                             0x00,
                             0x00,  // 20
                             0x00,
                             0x00,
                             0x00,
                             0x00,
                             0x00,
                             0x00,
                             0x00,
                             0x00,
                             0x00,
                             0x00,  // 30
                             0x00,
                             0x00,
                             0x00,
                             0x00,
                             0x00,
                             0x00,
                             0x00,
                             0x00,
                             0x00,
                             0x00,  // 40
                             0x00,
                             0x00,
                             0x00,
                             0x00,
                             0x00,
                             0x00,
                             0x00,
                             0x00,
                             0x00,
                             0x00,  // 50
                             0x00,
                             0x00,
                             0x00,
                             0x00,
                             0x00,
                             0x00,
                             0x00,
                             0x00,
                             0x00,
                             0x00,  // 60
                             0x00,
                             0x00,
                             0x00,
                             0x00,
                             0x00,
                             0x00,
                             0x00,
                             0x00,
                             0x00,
                             0x00,  // 70
                             0x00,
                             0x00,
                             0x00,
                             0x00,
                             0x00,
                             0x00,
                             0x00,
                             0x00,
                             0x00,
                             0x00,  // 80
                             0x00,
                             0x00,
                             0x00,
                             0x00,
                             0x00,
                             0x00,
                             0x00,
                             0x00,
                             0x00,
                             0x00,  // 90
                             0x00,
                             0x00,
                             0x00,
                             0x00,
                             0x00,
                             0x00,
                             0x00,
                             0x00,
                             0x00,
                             0x00  // 100
                             ,
                             0xD8

};
const byte MAP_DATA2_RSP[] = {
    0xC2,
    0x66,  //////////// sector command
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,  // 10 MAP DATA
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,  // 20
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,  // 30
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,  // 40
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,  // 50
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,  // 60
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,  // 70
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,  // 80
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,  // 90
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00  // 100
    ,
    0xD7  //////// sector end

};
const byte MAP_DATA3_RSP[] = {
    0xC3,
    0x66,  //////////// sector command
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,  // 10  MAP DATA
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,  // 20  MAP DATA
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,  // 30  MAP DATA
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,  // 40  MAP DATA
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,  // 50
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,  // 60
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,  // 70
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,  // 80
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,  // 90
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00  // 100
    ,
    0xD6  //////// sector end
};
const byte MAP_DATA4_RSP[] = {
    0xC4,
    0x66,  //////////// sector command
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,  // 10  MAP DATA
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,  // 20
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,  // 30
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,  // 40
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,  // 50
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,  // 60
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,  // 70
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,  // 80
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,  // 90
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00  // 100
    ,
    0xD5  //////// sector end
};

////////////////// INJECTOR DATA
const byte MAPINJ_DATA_RSP[] = {0xC6,
                                0x66,
                                0x00,
                                0x00,
                                0x00,
                                0x00,
                                0x00,
                                0x00,
                                0x00,
                                0x00,
                                0x00,
                                0x00,  // 10
                                0x00,
                                0x00,
                                0x00,
                                0x00,
                                0x00,
                                0x00,
                                0x00,
                                0x00,
                                0x00,
                                0x00,  // 20
                                0x00,
                                0x00,
                                0x00,
                                0x00,
                                0x00,
                                0x00,
                                0x00,
                                0x00,
                                0x00,
                                0x00,  // 30
                                0x00,
                                0x00,
                                0x00,
                                0x00,
                                0x00,
                                0x00,
                                0x00,
                                0x00,
                                0x00,
                                0x00,  // 40
                                0x00,
                                0x00,
                                0x00,
                                0x00,
                                0x00,
                                0x00,
                                0x00,
                                0x00,
                                0x00,
                                0x00,  // 50
                                0x00,
                                0x00,
                                0x00,
                                0x00,
                                0x00,
                                0x00,
                                0x00,
                                0x00,
                                0x00,
                                0x00,  // 60
                                0x00,
                                0x00,
                                0x00,
                                0x00,
                                0x00,
                                0x00,
                                0x00,
                                0x00,
                                0x00,
                                0x00,  // 70
                                0x00,
                                0x00,
                                0x00,
                                0x00,
                                0x00,
                                0x00,
                                0x00,
                                0x00,
                                0x00,
                                0x00,  // 80
                                0x00,
                                0x00,
                                0x00,
                                0x00,
                                0x00,
                                0x00,
                                0x00,
                                0x00,
                                0x00,
                                0x00,  // 90
                                0x00,
                                0x00,
                                0x00,
                                0x00,
                                0x00,
                                0x00,
                                0x00,
                                0x00,
                                0x00,
                                0x00  // 100
                                ,
                                0xEE

};
const byte MAPINJ_DATA2_RSP[] = {
    0xC7,
    0x66,  //////////// sector command
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,  // 10 MAP DATA
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,  // 20
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,  // 30
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,  // 40
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,  // 50
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,  // 60
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,  // 70
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,  // 80
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,  // 90
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00  // 100
    ,
    0xDB  //////// sector end

};
const byte MAPINJ_DATA3_RSP[] = {
    0xC8,
    0x66,  //////////// sector command
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,  // 10  MAP DATA
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,  // 20  MAP DATA
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,  // 30  MAP DATA
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,  // 40  MAP DATA
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,  // 50
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,  // 60
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,  // 70
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,  // 80
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,  // 90
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00  // 100
    ,
    0xC0  //////// sector end
};
const byte MAPINJ_DATA4_RSP[] = {
    0xC9,
    0x66,  //////////// sector command
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,  // 10  MAP DATA
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,  // 20
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,  // 30
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,  // 40
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,  // 50
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,  // 60
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,  // 70
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,  // 80
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,  // 90
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00  // 100
    ,
    0x8E  //////// sector end
};

bool start = false;
void sendResponse(const byte *response, size_t responseSize);
void setup() {
  CAN0.setCANPins((gpio_num_t)CAN_RX_PIN, (gpio_num_t)CAN_TX_PIN);
  Serial2.begin(BAUD_RATE, SERIAL_8E1, RXD2, TXD2);  // serial 2
}
static byte commandBuffer[3];  // BFR
static int commandIndex = 0;   // BFFR

void loop() {
  while (PFCSTARTED == 1) {
    OBD2.begin();
    // Serial.print(OBD2.pidName(ENGINE_COOLANT_TEMPERATURE));
    // Serial.print(" = ");
    // Serial.print(OBD2.pidRead(ENGINE_COOLANT_TEMPERATURE));
    // Serial.print(OBD2.pidUnits(ENGINE_COOLANT_TEMPERATURE));

    int RPM_OBDDATA = OBD2.pidRead(ENGINE_RPM);
    int SPEED_OBD = OBD2.pidRead(VEHICLE_SPEED);
    int MAP_OBD = OBD2.pidRead(INTAKE_MANIFOLD_ABSOLUTE_PRESSURE);
    int IAT_OBD = OBD2.pidRead(AIR_INTAKE_TEMPERATURE);
    int CLT_OBD = (OBD2.pidRead(ENGINE_COOLANT_TEMPERATURE));
    int LOAD_OBD = OBD2.pidRead(CALCULATED_ENGINE_LOAD);
    int BATT_OBD = OBD2.pidRead(CONTROL_MODULE_VOLTAGE);
    int TIMING_OBD = OBD2.pidRead(TIMING_ADVANCE);
    int DisplayBatt = 0;

    if (RPM_OBDDATA < 1) {
      RPM_OBDDATA = 0;
    } else {
      RPM_OBDDATA = OBD2.pidRead(ENGINE_RPM);
    }

    int correctedtemp;
    int correctedtempIAT;
    const float offset = 80.0;        // Offset to map -80 degrees to 0x00,0x00
    const float scalingFactor = 1.0;  // Scaling factor to be determined based
                                      // on your fixed-point format
    float adjustedValue = CLT_OBD + offset;
    float adjustedValueIAT = IAT_OBD + offset;

    correctedtemp = adjustedValue * scalingFactor;
    correctedtempIAT = adjustedValueIAT * scalingFactor;

    // Serial.print(correctedtemp);

    ////// BATT
    byte LASTHEXBATT = (DisplayBatt >> 8) & 0xFF;  //  (0x01)
    byte FIRSTHEXBATT = DisplayBatt & 0xFF;        //  (0x39)
                                                   /////////// RPM
    byte LASTHEXRPM = (RPM_OBDDATA >> 8) & 0xFF;   //  (0x01)
    byte FIRSTHEXRPM = RPM_OBDDATA & 0xFF;         //  (0x39)
                                                   /////////// TEMPCLT
    byte LCLT = (correctedtemp >> 8) & 0xFF;       //  (0x01)
    byte FCLT = correctedtemp & 0xFF;              //  (0x39)
                                                   /////////// TEMPIAT
    byte LIAT = (correctedtempIAT >> 8) & 0xFF;    //  (0x01)
    byte FIAT = correctedtempIAT & 0xFF;           //  (0x39)
    /////////// MAP
    byte LMAP = (MAP_OBD >> 8) & 0xFF;  //  (0x01)
    byte FMAP = MAP_OBD & 0xFF;         //  (0x39)
                                 /////////// SPEED
    byte LSPD = (SPEED_OBD >> 8) & 0xFF;    //  (0x01)
    byte FSPD = SPEED_OBD & 0xFF;           //  (0x39)
                                            /////////// LOAD
    byte LLOAD = (LOAD_OBD >> 8) & 0xFF;    //  (0x01)
    byte FLOAD = LOAD_OBD & 0xFF;           //  (0x39)
                                            /////////// TIMING
    byte LTIME = (TIMING_OBD >> 8) & 0xFF;  //  (0x01)
    byte FTIME = TIMING_OBD & 0xFF;         //  (0x39)

    byte DATAREPLY[] = {
        0xDA, 0x16, FLOAD,        LLOAD,       FTIME, LTIME,
        FMAP, LMAP, FIRSTHEXRPM,  LASTHEXRPM,  FSPD,  LSPD,
        0x00, 0x00, 0x00,         0x00,        FCLT,  LCLT,
        FIAT, LIAT, FIRSTHEXBATT, LASTHEXBATT, 0x00};  // DONT TOUCH SECOND DEG
    sendResponse(DATAREPLY, sizeof(DATAREPLY));
  }

  if (Serial2.available() > 0) {
    if (commandBuffer[0] == DATAREQ[0] && commandBuffer[1] == DATAREQ[1] &&
        commandBuffer[2] == DATAREQ[2]) {
      if (PFCSTARTED == 0) {
        PFCSTARTED = 1;
      }

      // Serial.println(); ///// BOOST 1.12 BAR = (0x70,0x80)
      /// PID NOTES VEHICLE_SPEED (SPEED)  TIMING_ADVANCE
      /// INTAKE_MANIFOLD_ABSOLUTE_PRESSURE  AIR_INTAKE_TEMPERATURE
      /// ENGINE_COOLANT_TEMPERATURE ABSOLUTE_LOAD_VALUE CONTROL_MODULE_VOLTAGE
      //------------------------CMD  CMD     INJDTY      IGN TIMING    MAP/MAF
      //RPM SPEED                 SPEED 255KM    BOOSTPRES 1BAR    KNOCK   WATER
      //TEMP AIR TEMP     BATT VOLT

      commandIndex = 0;
    }

    byte currentByte = Serial2.read();
    // Check for command sequences
    if (currentByte == startcmd[0] || currentByte == enginename[0] ||
        currentByte == ver[0] || currentByte == menuboot[0] ||
        currentByte == menudata[0] || currentByte == tracer[0] ||
        currentByte == MAP_DATA_REQ1[0] || currentByte == MAP_DATA_REQ2[0] ||
        currentByte == MAP_DATA_REQ3[0] || currentByte == MAP_DATA_REQ4[0] ||
        currentByte == MAPINJ_DATA_REQ1[0] ||
        currentByte == MAPINJ_DATA_REQ2[0] ||
        currentByte == MAPINJ_DATA_REQ3[0] ||
        currentByte == MAPINJ_DATA_REQ4[0] || currentByte == DATAREQ[0] ||
        currentByte == MONREQ[0] || currentByte == FUNREQ[0] ||
        currentByte == SELECTREQ[0] || currentByte == AIRFLOWMENU[0] ||
        currentByte == INJECTORADJUST[0] || currentByte == SENSORCHK[0] ||
        currentByte == FUNCTIONCMD[0] || currentByte == BOOSTSETTING[0] ||
        currentByte == WATER[0] || currentByte == ACCEL[0] ||
        currentByte == REVLIMIT[0] || currentByte == CRANKING[0] ||
        currentByte == verunknown[0]) {
      // Start of a potential command sequence

      commandBuffer[0] = currentByte;
      commandIndex = 1;
      PFCSTARTED = 0;

    } else if (commandIndex > 0) {
      commandBuffer[commandIndex] = currentByte;
      commandIndex++;

      if (commandIndex == 3) {
        // Check for START REQ

        if (commandBuffer[0] == ver[0] && commandBuffer[1] == ver[1] &&
            commandBuffer[2] == ver[2]) {
          sendResponse(verr, sizeof(verr));
          commandIndex = 0;
        }
        if (commandBuffer[0] == verunknown[0] &&
            commandBuffer[1] == verunknown[1] &&
            commandBuffer[2] == verunknown[2]) {
          sendResponse(verunknown, sizeof(verunknown));
          commandIndex = 0;
        }

        // ENGINE NAME
        if (commandBuffer[0] == enginename[0] &&
            commandBuffer[1] == enginename[1] &&
            commandBuffer[2] == enginename[2]) {
          sendResponse(enginenamer, sizeof(enginenamer));
          commandIndex = 0;
        }
        if (commandBuffer[0] == startcmd[0] &&
            commandBuffer[1] == startcmd[1] &&
            commandBuffer[2] == startcmd[2]) {
          sendResponse(startrspnd, sizeof(startrspnd));
          commandIndex = 0;
        }
        if (commandBuffer[0] == FUNCTIONCMD[0] &&
            commandBuffer[1] == FUNCTIONCMD[1] &&
            commandBuffer[2] == FUNCTIONCMD[2]) {
          sendResponse(FUNCTIONR, sizeof(FUNCTIONR));
          commandIndex = 0;
        }

        if (commandIndex == 3) {
          if (commandBuffer[0] == menuboot[0] &&
              commandBuffer[1] == menuboot[1] &&
              commandBuffer[2] == menuboot[2]) {
            sendResponse(menubootr, sizeof(menubootr));
            commandIndex = 0;
          }

          if (commandBuffer[0] == SENSREQ[0] &&
              commandBuffer[1] == SENSREQ[1] &&
              commandBuffer[2] == SENSREQ[2]) {
            sendResponse(SENSREPLY, sizeof(SENSREPLY));
            commandIndex = 0;
          } else if (commandBuffer[0] == SENSORCHK[0] &&
                     commandBuffer[1] == SENSORCHK[1] &&
                     commandBuffer[2] == SENSORCHK[2]) {
            sendResponse(SENSORCHKr, sizeof(SENSORCHKr));
            commandIndex = 0;
          }
          // MENU DATA
          else if (commandBuffer[0] == menudata[0] &&
                   commandBuffer[1] == menudata[1] &&
                   commandBuffer[2] == menudata[2]) {
            sendResponse(menudata, sizeof(menudata));
            commandIndex = 0;
          }
          // TRACER
          else if (commandBuffer[0] == tracer[0] &&
                   commandBuffer[1] == tracer[1] &&
                   commandBuffer[2] == tracer[2]) {
            int RPM_OBDDATA = OBD2.pidRead(ENGINE_RPM);
            int MAP_OBD = OBD2.pidRead(INTAKE_MANIFOLD_ABSOLUTE_PRESSURE);
            byte LASTHEXRPM = (RPM_OBDDATA >> 8) & 0xFF;  //  (0x01)
            byte FIRSTHEXRPM = RPM_OBDDATA & 0xFF;        //  (0x39)

            /////////// MAP
            byte LMAP = (MAP_OBD >> 8) & 0xFF;  //  (0x01)
            byte FMAP = MAP_OBD & 0xFF;         //  (0x39)

            byte rtracer[] = {0xDB, 0x04, FMAP, FIRSTHEXRPM, 0x20};

            sendResponse(rtracer, sizeof(rtracer));
            commandIndex = 0;
          }
          // AIRFLOW
          else if (commandBuffer[0] == AIRFLOWMENU[0] &&
                   commandBuffer[1] == AIRFLOWMENU[1] &&
                   commandBuffer[2] == AIRFLOWMENU[2]) {
            sendResponse(AIRFLOWREPLY, sizeof(AIRFLOWREPLY));
            commandIndex = 0;
          } else if (commandBuffer[0] == BOOSTSETTING[0] &&
                     commandBuffer[1] == BOOSTSETTING[1] &&
                     commandBuffer[2] == BOOSTSETTING[2]) {
            sendResponse(BOOSTREPLY, sizeof(BOOSTREPLY));
            commandIndex = 0;
          } else if (commandBuffer[0] == CRANKING[0] &&
                     commandBuffer[1] == CRANKING[1] &&
                     commandBuffer[2] == CRANKING[2]) {
            sendResponse(CRANKINGR, sizeof(CRANKINGR));
            commandIndex = 0;
          } else if (commandBuffer[0] == REVLIMIT[0] &&
                     commandBuffer[1] == REVLIMIT[1] &&
                     commandBuffer[2] == REVLIMIT[2]) {
            sendResponse(REVLIMITR, sizeof(REVLIMITR));
            commandIndex = 0;
          } else if (commandBuffer[0] == WATER[0] &&
                     commandBuffer[1] == WATER[1] &&
                     commandBuffer[2] == WATER[2]) {
            sendResponse(WATERR, sizeof(WATERR));
            commandIndex = 0;
          }

          // INJ ADJUST
          else if (commandBuffer[0] == INJECTORADJUST[0] &&
                   commandBuffer[1] == INJECTORADJUST[1] &&
                   commandBuffer[2] == INJECTORADJUST[2]) {
            sendResponse(INJECTORREPLY, sizeof(INJECTORREPLY));
            commandIndex = 0;
          }

          ////////////// IGNITION DATA
          else if (commandBuffer[0] == MAP_DATA_REQ1[0] &&
                   commandBuffer[1] == MAP_DATA_REQ1[1] &&
                   commandBuffer[2] == MAP_DATA_REQ1[2]) {
            sendResponse(MAP_DATA_RSP, sizeof(MAP_DATA_RSP));
            commandIndex = 0;
          } else if (commandBuffer[0] == MAP_DATA_REQ2[0] &&
                     commandBuffer[1] == MAP_DATA_REQ2[1] &&
                     commandBuffer[2] == MAP_DATA_REQ2[2]) {
            sendResponse(MAP_DATA2_RSP, sizeof(MAP_DATA2_RSP));
            commandIndex = 0;
          } else if (commandBuffer[0] == MAP_DATA_REQ3[0] &&
                     commandBuffer[1] == MAP_DATA_REQ3[1] &&
                     commandBuffer[2] == MAP_DATA_REQ3[2]) {
            sendResponse(MAP_DATA3_RSP, sizeof(MAP_DATA3_RSP));
            commandIndex = 0;
          } else if (commandBuffer[0] == MAP_DATA_REQ4[0] &&
                     commandBuffer[1] == MAP_DATA_REQ4[1] &&
                     commandBuffer[2] == MAP_DATA_REQ4[2]) {
            sendResponse(MAP_DATA4_RSP, sizeof(MAP_DATA4_RSP));
            commandIndex = 0;
          }
          ///////////////// INJECTOR DATA

          else if (commandBuffer[0] == MAPINJ_DATA_REQ1[0] &&
                   commandBuffer[1] == MAPINJ_DATA_REQ1[1] &&
                   commandBuffer[2] == MAPINJ_DATA_REQ1[2]) {
            sendResponse(MAPINJ_DATA_RSP, sizeof(MAPINJ_DATA_RSP));
            commandIndex = 0;
          } else if (commandBuffer[0] == MAPINJ_DATA_REQ2[0] &&
                     commandBuffer[1] == MAPINJ_DATA_REQ2[1] &&
                     commandBuffer[2] == MAPINJ_DATA_REQ2[2]) {
            sendResponse(MAPINJ_DATA2_RSP, sizeof(MAPINJ_DATA2_RSP));
            commandIndex = 0;
          } else if (commandBuffer[0] == MAPINJ_DATA_REQ3[0] &&
                     commandBuffer[1] == MAPINJ_DATA_REQ3[1] &&
                     commandBuffer[2] == MAPINJ_DATA_REQ3[2]) {
            sendResponse(MAPINJ_DATA3_RSP, sizeof(MAPINJ_DATA3_RSP));
            commandIndex = 0;
          } else if (commandBuffer[0] == MAPINJ_DATA_REQ4[0] &&
                     commandBuffer[1] == MAPINJ_DATA_REQ4[1] &&
                     commandBuffer[2] == MAPINJ_DATA_REQ4[2]) {
            sendResponse(MAPINJ_DATA4_RSP, sizeof(MAPINJ_DATA4_RSP));
            commandIndex = 0;
          }

          if (commandBuffer[0] == FUNREQ[0] && commandBuffer[1] == FUNREQ[1] &&
                  commandIndex == 21 ||
              42 || 63 || 84 || 105) {
            sendResponse(monitorreqr, sizeof(monitorreqr));
            commandIndex = 0;
          }
          if (commandBuffer[0] == MONREQ[0] && commandBuffer[1] == MONREQ[1] &&
                  commandIndex == 21 ||
              42 || 63 || 84 || 105) {
            start = true;
            sendResponse(monitorreqr, sizeof(monitorreqr));

            commandIndex = 0;
          }
          if (commandBuffer[0] == SELECTREQ[0] &&
                  commandBuffer[1] == SELECTREQ[1] && commandIndex == 21 ||
              42 || 63 || 84 || 105) {
            sendResponse(monitorreqr, sizeof(monitorreqr));
            commandIndex = 0;
          }
        }
      }
    }
  }
}
void sendResponse(const byte *response, size_t responseSize) {
  for (size_t i = 0; i < responseSize; i++) {
    Serial2.write(response[i]);
  }
}
