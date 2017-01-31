/*
    SNOOTLAB
    Application N°1 Shield Mémoire : Stockage des valeurs de temperature dans un fichier texte
    avec fonction d'horodatage
    MàJ: 2 juillet 2012
    Environnement : linux debian based / IDE Arduino 1.0.1 / Arduino UNO rev3 et Duemilanove
*/
#include<stdlib.h>                                        //contient la fonction permettant de passer de FloatToString
#include <LCD4884.h>
#include <Wire.h>
#include <SD.h>
#include <RTClib.h>
#define DELAI1 10                                          //Bref temps d'attente pour la mesure

// Declaration des pin utilisées
const char pin_chipSelect = 10;                            //10 est la pin utilisée par le shield mémoire pour le signal ChipSelect
const char pin_capteur = 1;                                //Numéro de la pin où est relié le capteur en question
const byte LED_Write = 9;
const byte LED_retroeclairage = 7;

// Declaration des variables générales
const unsigned long F_refresh = 5000;                      //Entrez ici la fréquence de rafraichissement du capteur, en millisecondes
const int cycles = 20;                                     //Nombre de mesures a moyenner

RTC_DS1307 RTC;
File fichier;
String datastring;
char temp_a_afficher[12] = {'T', 'e', 'm','p','=',' ','0','0','.','0','C'};
char buffer[10];

void setup()
{
 
  Serial.begin(57600);                                     //Initialisation de la liaison série
  Serial.print("Initialisation de la SD card...");
 
  pinMode(LED_retroeclairage, OUTPUT);  //La pin 7 controle le rétroéclairage de l'écran LCD
  lcd.LCD_init();
  lcd.LCD_clear();
  digitalWrite(LED_retroeclairage, LOW);  
 
  pinMode(pin_chipSelect, OUTPUT);                         // Mettre la pin en output
  pinMode(LED_Write, OUTPUT);                              // Initialisation de la LED indiquant si une écriture est en cours
  digitalWrite(LED_Write, LOW);
  
   Wire.begin();
   RTC.begin();                           
   if (! RTC.isrunning())
 {
 // Si la RTC n'est pas configurée, le faire avec les valeurs de l'ordinateur au moment de la compilation
 RTC.adjust(DateTime(__DATE__, __TIME__));
 }
 
  if (!SD.begin(pin_chipSelect))                           // Vérification de la présence de la SD card
    {                         
      Serial.println("Carte fausse ou absente");
      return;                                              // Si problème, sortir et bloquer l'exécution
    }
 
  Serial.println("Carte OK.");
 
  datastring = "\nReleve et horodatage des valeurs du capteur, selon horloge RTC\n---------\n ";
 
  fichier = SD.open("donnees.txt", FILE_WRITE);               // Ouverture du fichier en mode Ecriture.

  if (fichier)                                             // Si le fichier existe déja, on écrira à la suite des données présentes.
    {

      fichier.println(datastring);

      fichier.close();

      Serial.println(datastring);              // dupliquer la donnée sur port série
    }
   
  else                                                     //Si problème à l'ouverture, alors signaler l'erreur
    {                                                       
        Serial.println("erreur d'ouverture du fichier durant la configuration");
    }
 
}

void loop()
{
  delay(F_refresh);
  digitalWrite(LED_Write, HIGH);        //Allumer la LED pour indiquer qu'une écriture va commencer
 
  DateTime moment = RTC.now();         //Init de l'objet temporel
  datastring=String(moment.day(),DEC);
  datastring+='/';
  datastring+=String(moment.month(),DEC);
  datastring+='/';
  datastring+=String(moment.year(),DEC);
  datastring+=' ';
  datastring+=String(moment.hour(),DEC);
  datastring+=':';
  datastring+=String(moment.minute(),DEC);
  datastring+=':';
  datastring+=String(moment.second(),DEC);
 
  float resultTemp = 0.0;
  for(int i =0;i<cycles;i++){
     int analogValue = analogRead(pin_capteur);
     float temperature = (((5 * 100.0 * analogValue) / 1024)-50.0);  //Température en degrés Celsius avec un TMP36GZ
     resultTemp += temperature;
     delay(DELAI1);
  }
  resultTemp /= cycles;
   
  String temp = dtostrf(resultTemp, 4, 1, buffer);
  
  datastring += ("\t Temperature = ");            // préalablement remplie avec les données temporelles
  datastring += temp;

  fichier = SD.open("donnees.txt", FILE_WRITE);
 
  if (fichier)
    {
     
      fichier.println(datastring);
     
      fichier.close();
     
      Serial.print(datastring);                 // dupliquer la donnée sur port série
      Serial.print((char)176);
      Serial.println("C");

      temp_a_afficher[6] = buffer[0];
      temp_a_afficher[7] = buffer[1];
      temp_a_afficher[8] = buffer[2];
      temp_a_afficher[9] = buffer[3];

      lcd.LCD_clear();                     // blanks the display
      lcd.LCD_write_string(1,1, temp_a_afficher, MENU_NORMAL);
      
    }
  else {
        Serial.println("erreur d'ouverture du fichier");
        }
 
  datastring =0;
  digitalWrite(LED_Write, LOW);        //Eteindre la LED pour indiquer la fin de l'écriture dans la carte
}
