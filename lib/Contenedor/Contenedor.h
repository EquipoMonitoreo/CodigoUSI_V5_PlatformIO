/*
*   LIBRERIA EN USO
*/

#ifndef CONTENEDOR_H
#define CONTENEDOR_H

#include <Arduino.h>
#include <SoftwareSerial.h>
#include <SIM808.h>

struct Info
{
  int nivel;
  uint32_t cantBotellas;     // Total de botellas. Variable de 4 bytes.
  String GPS_loc;
  int hour;
  int minute;
  int second;
};

class Contenedor
{
  public:
  
    Info info;
    char datosListos;
    
    
    Contenedor(SIM808 *sim808, SoftwareSerial *SIM, String ID_servidor, Print &print);
    ~Contenedor();
    bool SIM_Initialize();
    bool enviarInfo();
    void updateDate();
    void nextSampleTime();
    int sampleHour;
    int sampleMinute;
    String timeStamp;
    

   private:
    
    String ID_servidor;
    SoftwareSerial *SIM;
    SIM808 *sim808;
    Print* inSerial;
    
};

#endif