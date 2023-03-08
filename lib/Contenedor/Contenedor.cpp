/*
*   LIBRERIA EN USO
*/

#include "Contenedor.h"

#define		NETWORK_DELAY	  	      1000
#define     	GPRS_APN          	"wap.gprs.unifon.com.ar" 
#define     	GPRS_USER         	NULL       
#define     	GPRS_PASS         	NULL
#define     	SAMPLE_TIME       	2

#define   ESPERA                  100
#define   TIME_OUT                300000                          // Time out en ms para salir del bucle SIM Initialize

Contenedor::Contenedor(SIM808 *sim808, SoftwareSerial *SIM, const String ID_servidor, Print &print)
{
  this->SIM = SIM;
  this->sim808 = sim808;
  this->ID_servidor = ID_servidor;
  info.cantBotellas = 0;
  inSerial = &print;    //operate on the adress of print - Puntero a objeto 'Serial' para mostrar info por monitor serie.
}

Contenedor::~Contenedor(){}

bool Contenedor::enviarInfo()
{
      String URL_string = ID_servidor + timeStamp + "/" + String(info.nivel) + "/" + String(info.cantBotellas) + "/" + info.GPS_loc;  // AAAA-MM-DDTHH:MM:SS/Nivel(%)/CantBotellas
      char URLstring[512];
      URL_string.toCharArray(URLstring, 512);
      char BUFFER[BUFFER_SIZE];
      uint16_t respuesta = sim808->httpGet(URLstring, BUFFER, BUFFER_SIZE);

      inSerial->println(respuesta);

      //nextSampleTime();       // REVISAR!. aparentemente sin uso.

      if (respuesta != 200){
        return 0;
      }
      else{
        return 1;
      }
}


bool Contenedor::SIM_Initialize()
{
  SIM808NetworkRegistrationState stat;
  SIM808SignalQualityReport report;

  SIM->begin(9600);
  sim808->begin(*SIM);

  inSerial->println("Encendiendo...");

  bool OnOff_status;

  OnOff_status = sim808->powerOnOff(true);
  //inSerial->println("OnOffStatus: "); inSerial->println(OnOff_status);

  if(OnOff_status == 0){
    inSerial->println("ERROR");
    return 0;
  }

  sim808->init();
  delay(10000);
 
  inSerial->println("Registrando red...");
  bool isAvailable = false;
  while(!isAvailable)
  {  
    stat = sim808->getNetworkRegistrationStatus();
    report = sim808->getSignalQuality();

    isAvailable = static_cast<int8_t>(stat) &
                  (static_cast<int8_t>(SIM808NetworkRegistrationState::Registered) | static_cast<int8_t>(SIM808NetworkRegistrationState::Roaming))
                  != 0 ;

     delay(NETWORK_DELAY);
     inSerial->print(".");
     break;
  }

  inSerial->println("");
  inSerial->println("Enable Gprs...");
  inSerial->print("Signal Quality: "); inSerial->println(report.rssi);

  bool enabled = false;
  unsigned long checkTime = millis();

  do {
    enabled = sim808->enableGprs(GPRS_APN, GPRS_USER, GPRS_PASS);

    delay(ESPERA);
    inSerial->print(".");
    if(millis()-checkTime >= TIME_OUT){
      inSerial->println("Time Out");
      break;
    }

  } while(!enabled);

  // Chequear si la sincronizacion con la red esta activa. AT+CLTS? debe ser 1.
  //char comResp[20];
  //size_t comRespSize;
  //sim808->sendCommand("AT+CLTS?", comResp, comRespSize);
  //Serial.print("AT+CLTS?: "); Serial.println(comResp);

  inSerial->println("Update Date...");  
  updateDate();

  //REVISAR! aparentemente sin uso:
  //sampleHour = info.hour;         
  //sampleMinute = info.minute;
  //nextSampleTime();

  inSerial->println(isAvailable);
  inSerial->println(enabled);
  
  return enabled;
}


void Contenedor::updateDate()
{
  SIM->println("AT+CCLK?");
  delay(100);
  while(SIM->available())
  {
    SIM->readStringUntil('\"');
    String year = SIM->readStringUntil('/');
    String month = SIM->readStringUntil('/');
    String day = SIM->readStringUntil(',');
    String Time = SIM->readStringUntil('-');
    info.hour = (Time.substring(0,2)).toInt();
    info.minute = (Time.substring(3,5)).toInt();
    info.second = (Time.substring(6,8)).toInt();
    timeStamp = "20" + year + "-" + month + "-" + day + "T" + Time;
    SIM->readString();
    delay(100);
  }

}

// REVISAR!. aparentemente sin uso:
/*
void Contenedor::nextSampleTime()
{
  sampleMinute += SAMPLE_TIME;
  if (sampleMinute >= 60)
  {
    sampleMinute -= 60;
    sampleHour++;
    if (sampleHour >= 24)
    {
      sampleHour -= 24;
    }
  }
}
*/
