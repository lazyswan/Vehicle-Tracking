#include <SoftwareSerial.h> 
#include <SD.h>
#include <TinyGPS.h>
char inchar; // Will hold the incoming character from the GSM shield
SoftwareSerial SIM900(2,3);
//SoftwareSerial SDGPS(4,5);
int numring=0;
int comring=1;
float latitude, longitude ,lat ,lon;
String textForSMS;
// Create an instance of the TinyGPS object
TinyGPS gps;
void getgps(TinyGPS &gps);
void setup()
{
//pinMode(10, OUTPUT);

Serial.begin(9600);
// check that the microSD card exists and can be used
if (!SD.begin(8)) {
Serial.println("Card failed, or not present");
// stop the sketch
return;
}
Serial.println("microSD card is ready");


Serial.begin(9600);
SIM900.begin(9600);
//delay(2000);  // give time to log on to network.
  SIM900.print("AT+CMGF=1\r");  // set SMS mode to text
  delay(1500);
  SIM900.print("AT+CNMI=2,2,0,0,0\r"); 
  // blurt out contents of new SMS upon receipt to the GSM shield's serial out
  delay(1500);
  Serial.println("GSM is Ready...");
}
void getgps(TinyGPS &gps)
{
int year;
byte month, day, hour, minute, second, hundredths;
//decode and display position data
gps.f_get_position(&latitude, &longitude);
File dataFile = SD.open("INDIA.TXT", FILE_WRITE);
// if the file is ready, write to it
 if (dataFile)
{
 dataFile.print("Lat: ");
dataFile.print(latitude,5);
dataFile.print(" ");
dataFile.print("Long: ");
dataFile.print(longitude,5);
dataFile.print(" ");
// decode and display time data
gps.crack_datetime(&year,&month,&day,&hour,&minute,&second,&hundredths);
// correct for your time zone as in Project 45
hour=hour+5;
minute=minute+30;
if (hour>23)
{
hour=hour-24;
}

if (hour<10)
{
dataFile.print("0");
}
dataFile.print(hour, DEC);
dataFile.print(":");
if (minute<10)
{
dataFile.print("0");
}
dataFile.print(minute, DEC);
dataFile.print(":");
if (second<10)
{
dataFile.print("0");
}
dataFile.print(second, DEC);
dataFile.print(" ");
dataFile.print(day,DEC);
dataFile.print("/");
dataFile.print(month, DEC);
dataFile.print("/");
dataFile.print(year,DEC);
dataFile.print("  ");

dataFile.print(gps.f_speed_kmph());
 dataFile.println("km/h");
dataFile.close();
 delay(1500); // record a measurement every 30 seconds
}
}
void sendSMS(String message )
{
  SIM900.println("AT+CMGF=1");                                                        // AT command to send SMS message
  delay(1500); //VERY IMP
  SIM900.println("AT + CMGS = \"+919987706452\"");                                     // recipient's mobile number, in international format
  delay(2000);
  SIM900.println(message);        // message to send
  delay(100);
  SIM900.println((char)26);                       // End AT command with a ^Z, ASCII code 26
  delay(100); 
  SIM900.println();
  delay(1500);                                     // give module time to send SMS
  //SIM900power();                                   // turn off module
}   


void loop()
{

if(SIM900.available() >0)
  {
    inchar=SIM900.read(); 
    if (inchar=='R')
    {
      delay(10);
      inchar=SIM900.read(); 
      if (inchar=='I')
      {
        delay(10);
        inchar=SIM900.read();
        if (inchar=='N')
        {
          delay(10);
          inchar=SIM900.read(); 
          if (inchar=='G')
          {
            delay(10);
            // So the phone (our GSM shield) has 'rung' once, i.e. if it were a real phone
            // it would have sounded 'ring-ring' or 'blurrrrr' or whatever one cycle of your ring tone is
            numring++;
           // Serial.println("ring!");
            if (numring==comring)
            {
              numring=0; // reset ring counter
                      
              SIM900.println("ATH");          
              delay(1000);
              gps.f_get_position(&lat, &lon);

               textForSMS = "Your Cordinates are Latitude: ";
                textForSMS.concat(lat);
                 textForSMS = textForSMS + " Longitude: ";
                   textForSMS.concat(lon);
                  textForSMS = textForSMS + "Enjoy!";  
                    sendSMS(textForSMS);
                   // do {} while (1);

                        //do {} while (1);
              
            }
          }
        }
      }
    }
  }


if ( Serial.available() > 0 ) // if there is data coming into the serial line
{
  
byte a = Serial.read(); // get the byte of data
if(gps.encode(a)) // if there is valid GPS data...
{
 Serial.println("coordinates calculating...");
  getgps(gps); // then grab the data and display it on the LCD
delay(1000);

  
}
}
}

