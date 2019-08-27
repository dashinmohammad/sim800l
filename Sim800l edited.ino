#include <SPI.h> // SPI communication library

#define TINY_GSM_MODEM_SIM800 // definition of used modem (SIM800L)
#include <TinyGsmClient.h> // library with GSM commands


// SIM800L serial communication object
HardwareSerial SerialGSM (1);

// library object with GSM functions
TinyGsm modemGSM (SerialGSM);

// serial speed of both SIM800L and serial monitor
const int BAUD_RATE = 9600;

// variables used to count time without locking loop function
// reference millis
long int millisRefCon, millisUserResp;
// flag that indicates the time count (used by the timeout function)
bool flagCon = false, flagUserResp = false;

// pins where relays will be connected and RX / TX where SIM800L will be connected
const int RX_PIN = 4, TX_PIN = 2;

// vivo access point name
const char * APN = "airtelgprs.com";
// User, if none exist leave empty
const char * USER = "";
// Password, if none exist leave empty
const char * PASSWORD = "";

// the variables below used by the loop function
// flag that indicates if, after the call made by SIM800L, a user answered with an SMS within 1min
bool userResponseSMS = false;
// flag indicating if the sensor is active
bool sensorActivated = false;
// index of the mobile number vector, used to scroll the vector
int i = 0;

// number of phones that will receive messages and calls and can send SMS commands
const int numbersTL = 2;
// number of phones, the order of call by the program is left to right
const String numbers [numbersTL] = {"+918220807238", "+5518999999999"};

// Send AT command and wait for a response to be obtained
String sendAT (String command)
{
  String response = "";
  SerialGSM.println (command);
  // wait until SIM800L responds
  while (! SerialGSM.available ());

  response = SerialGSM.readString ();

  return response;
}

// initialize GSM
void setupGSM ()
{
  Serial.println ("Setup GSM ...");

  
  // start serial SIM800L
  SerialGSM.begin (BAUD_RATE, SERIAL_8N1, RX_PIN, TX_PIN, false);
  delay (3000);

  // Serial modem info on serial monitor
  Serial.println (modemGSM.getModemInfo ());

  // start the modem
  if (! modemGSM.restart ())
  {
    
    Serial.println ("Restarting GSM \ nModem failed");
    delay (10000);
    ESP.restart ();
    return;
  }
  Serial.println ("Modem restart OK");
  
  // wait for network
  if (! modemGSM.waitForNetwork ())
  {
    
    Serial.println ("Failed to connect \ nto network");
    delay (10000);
    ESP.restart ();
    return;
  }
  Serial.println ("Modem network OK");

  // connect to network (GPRS technology)
  if (! modemGSM.gprsConnect (APN, USER, PASSWORD))
  {
    Serial.println ("GPRS Connection \ nFailed");
    delay (10000);
    ESP.restart ();
    return;
  }
  
  Serial.println ("GPRS Connect OK");

  // Set SMS mode to text (0 = PDU mode, 1 = Text mode)
  if (sendAT ("AT + CMGF = 1"). indexOf ("OK") <0)
  {
    Serial.println ("SMS Txt mode Error");
    delay (10000);
    ESP.restart ();
    return;
  }
  Serial.println ("SMS Txt mode OK");

  // Delete all stored SMS
  sendAT ("AT + CMGD = 1.4");
}

// execute call
void call (String number)
{
  Serial.print ("Calling ...");
  Serial.println (number);
  
  // try to execute call
  bool res = modemGSM.callNumber (number);

  // if successful Serials OK, if not fail
  if (res)
    Serial.println ("OK");
  else
    Serial.println ("fail");

  if (res)
  {
    // as soon as the call is made it ends
    res = modemGSM.callHangup ();

    // Serial if it was possible to finish or not
    Serial.print ("Hang up:");
    if (res)
      Serial.println ("OK");
    else
      Serial.println ("fail");
  }
}

void setup ()
{
  Serial.print("Helaaaaaa");
  Serial.begin (BAUD_RATE);
  Serial.println ("Starting ...");

  // set the time count variables to the current time before entering the loop
  millisRefCon = millisUserResp = millis ();

  // start and configure SIM800L
  setupGSM ();
 
  Serial.println ("GPRS: Connected");
}


// send SMS via GSM
bool sendSMS (String msg, String cel)
{
    Serial.println ("Sending sms '" + msg + "'");
    
    if (modemGSM.sendSMS (cel, msg))
      return true;

    return false;
}

// send a reply SMS to the mobile number that sent us a command
void sendResponse (String number)
{
  String response = "Relay 1:";

  // concatenates string with current relay signal
  // inverse logic, HIGH -> off and LOW -> on
 
  
  Serial.println ("Trying to send sms");
  
  // send SMS and Serial success / failure
  if (! sendSMS (response, number))
  {
    Serial.println ("Fail to send SMS");
    Serial.println ("Fail to send SMS");
  }
  else
  {
    Serial.println ("SMS sent");
    Serial.println ("SMS sent");
  }

  // delete all stored SMS
  sendAT ("AT + CMGD = 1.4");
}

// check if SIM800L has disconnected, if yes try to reconnect
void verifyGPRSConnection ()
{

  Serial.print ("GPRS:");

  if (modemGSM.isGprsConnected ())
    Serial.println ("Connected");
  else
  {
    Serial.println ("Disconnect");
    Serial.println ("Reconnecting ...");
    
    if (! modemGSM.waitForNetwork ())
    {
     
      Serial.println ("GPRS Con. Failed");

      delay (5000);

    }
    else
    {
      if (! modemGSM.gprsConnect (APN, USER, PASSWORD))
      {
    
        Serial.println ("GPRS Con. Failed");
        delay (5000);
      ;
      }
      else
      {
        
        Serial.println ("GPRS Con OK");
      }
    }
  }
}

void loop ()
{
  String msg, number;
  Serial.print("Helaaaaaa");
  // every 5 seconds check if SIM800L is disconnected, if yes try to reconnect
  
    verifyGPRSConnection ();

  // if SIM800L is connected
  if (modemGSM.isGprsConnected ())
  {
    // function that checks whether or not to call
    
      // signal that the sensor has been activated
      sensorActivated = true;
      userResponseSMS = false;
      // assign to the reference variable to count the time, the current value of millis
      millisUserResp = millis ();

      Serial.println ("Sensor activated!");
      Serial.println ("Calling to number" + String (i + 1));
      Serial.println ("Sensor activated!");
      Serial.println ("Calling to number" + String (i + 1));

      // call one of the vector numbers starting with 0
      call (numbers [i ++]);
      // after the call adds 1 to i
  }
   else 
    Serial.println ("Disconnected");
    
  // single delay in 10ms loop (disregarding the reconnect function, which has delay for Serial Serial)
  delay (10);
}
