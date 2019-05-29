/* *********************************************************************** */
/* *** Fragger is a concept to hide data files from forensic analysis  *** */
/* *** like border control in usa or china when they are searching for *** */
/* *** strong crypted files.                                           *** */
/* *** This little piece of code is only a concept but it works        *** */
/* ***                                                                 *** */
/* *** How it works:                                                   *** */
/* *** You call the programm with an argument and the value of how     *** */
/* *** many files will be created. Eg. fragger -c 3 and 3 files will   *** */
/* *** be created                                                      *** */
/* *** The file what is to fragment is scrambled into the 3 files.     *** */
/* *** Eg. we give a Text into fragger                                 *** */
/* *** fragger -c 3 and enter the text "ABCdefGHI" so three files will *** */
/* *** be created and have following text inserted:                    *** */
/* *** 1 = AdG, 2 = BeH and 3 = CfI                                    *** */
/* *** This three files you can rename and hide into several places    *** */
/* *** and no forensic tool can find the whole file what you scrambled *** */
/* *********************************************************************** */

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "fragger.h"

int FragDefrag = FRAGMENT;
int CountFiles = 2;
char *DateiNameZuLesen = "";
char *MyName = "";

/* *********************************************************************** */
/* *** Programmeinsprungspunkt. Wir erwarten hier mindestens ein       *** */
/* *** Argument, naemlich die Datei, die aufgesplittet werden soll     *** */
/* *********************************************************************** */
int main (int argc, char *argv [])
{
    MyName = argv[0];
        
// Wenn kein Argument uebergeben wurde, online Hilfe anzeigen und dann beenden
    if (argc <= 1) {
      ShowHelp();
      return 1;         
    }
// Dann Kommandozeilenargumente abarbeiten
   HandleSwitches(argc, argv);

   if (FragDefrag ==   FRAGMENT) { Fragmentieren(); }
   if (FragDefrag == DEFRAGMENT) { DeFragmentieren(); }
   return;
}

/* *********************************************************************** */
/* *** Defragmentieren der Daten.                                      *** */
/* *********************************************************************** */
int DeFragmentieren()
{
    int zaehler;          // Countervariable
    int istZaehler = 1;   // Countervariable
    char buffer[20];      // Puffer um Zaehlervariable nach String zu konvertieren
    int zeichen;          // Ein Zeichen aus STDIN
    FILE *fp[128];        // 128 moegliche Dateipointer
    FILE *FTest;          // Dateipointer

    istZaehler = DoCountDefrags(); // Anzahl gefundener Fragmentierungsdateien

/* *********************************************************************** */
/* *** Wurden keine fragmentierten Dateien gefunden, also Dateiname 1, *** */
/* *** Programm beenden                                                *** */
/* *********************************************************************** */
    if (istZaehler == 1) 
    {
      printf("%s\n","No frag files found, nothing to do");
      return 1;               
    }
/* *********************************************************************** */
/* *** Alle fragmentierten Dateien oeffnen                             *** */
/* *********************************************************************** */
    for (zaehler = 1; zaehler <= istZaehler; zaehler ++)
    {
      snprintf (buffer, (size_t)20, "%d", zaehler); // Integer nach String casten
      fp[zaehler] = fopen(buffer,"r");              // Datei zum lesen oeffnen
    }
/* *********************************************************************** */
/* *** Alle fragmientierten Dateien in der richtigen Reihenfolge       *** */
/* *** einlesen und nach STDOUT schreiben                              *** */
/* *********************************************************************** */
    while (1) 
    {
      for (zaehler = 1; zaehler < istZaehler; zaehler ++)
      {
        zeichen = fgetc(fp[zaehler]);   // Zeichen aus Fragmentdatei einlesen
        putc(zeichen,stdout);           // Zeichen nach STDOUT ausgeben
      }
      if (zeichen == EOF) { return 1; } // Wenn EOF erreicht ist, beenden
    }
}

/* *********************************************************************** */
/* *** Dateien fragmentieren                                           *** */
/* *********************************************************************** */
int Fragmentieren()
{
    char zeichen;         // Ein Zeichen
    char buffer[20];      // 20 Zeichen Puffer fuer Dateinamen
    int  zaehler;         // Countervariable
    FILE *fp[128];        // 128 Filepointer
    
   // Diese Schleife liest vom STDIN oder, in Verbindung mit < eine Datei ein
   // Abbruch bei erreichen von EOF
   for (zaehler = 1; zaehler <= CountFiles; zaehler ++)
   {
     snprintf (buffer, (size_t)20, "%d", zaehler); // Integer nach String casten
     fp[zaehler] = fopen(buffer,"w");              // Datei oeffnen
   }
   
   zaehler = 1; // Zaehler zuruecksetzen
/* *********************************************************************** */
/* *** Eingabedatei komplett abarbeiten und in alle Ausgabedateien     *** */
/* *** schreiben                                                       *** */
/* *********************************************************************** */
   while (( zeichen = getc(stdin)) != EOF ) 
   {
     fputc (zeichen, fp[zaehler]);
     zaehler ++;
     if (zaehler > CountFiles) { zaehler = 1; }
   }
   // Alle Zieldateien schliessen
   for (zaehler = 1; zaehler <= CountFiles; zaehler ++)
   {
     fflush(fp[zaehler]);
     fclose(fp[zaehler]);
   }
   return 1;
}

/* *********************************************************************** */
/* *** Zaehler alle moeglichen Fragmentierungsdateien, also von 1      *** */
/* *** hoch, bis keine Datei mehr gefunden wird                        *** */
/* *********************************************************************** */
int DoCountDefrags()
{
    int zaehler;        // Countervariable
    int istZaehler = 1; // Countervariable
    char buffer[20];    // 20 Zeichen Puffer fuer Dateinamen
    FILE *FTest;        // Dateizeiger
        
    for (zaehler = 1; zaehler <= 128; zaehler ++)
    {
      snprintf (buffer, (size_t)20, "%d", zaehler); // Integer nach String casten  
      FTest = fopen(buffer,"r");                    // Laesst sich die Datei oeffnen?
      if (FTest != NULL)                            // Wenn ja, dann
      {
        istZaehler ++;                              // Zaehler hochsetzen
        fclose (FTest);                             // und Datei wieder schliessen
      }
    }
    return istZaehler;                              // Anzahl gefundene Dateien zurueckgeben
}

/* *********************************************************************** */
/* *** Kommandozeilenargumente abarbeiten                              *** */
/* *********************************************************************** */
void HandleSwitches(int argc, char *argv [])
{
  char *cvalue = NULL;   // Stringzeiger
  int zaehler;           // Countervariable
  int ch;                // Das Argument
  opterr = 0;            // Fehlerspeicher zuruecksetzen

  // Argumentliste abarbeiten
  // -d ==> defragmentieren
  // -c ==> Anzahl Dateien, die generiert werden sollen
  // Mit : sind Argumente, die Optionen erfordern
 while ((ch = getopt (argc, argv, "dhc:")) != -1)
  {                                         
          switch (ch) {
            case 'd':
              FragDefrag = DEFRAGMENT;
              break;
            case 'c':
              CountFiles = atoi(optarg); // optarg ist ein Pointer auf einen String. Konvertieren mit atoi
              if (CountFiles > 128) 
              {
                fprintf (stderr, "%s\n", "Too many destination files! -c 128 is maximum");
                exit(1);
              }
              break;
            case 'h':
              ShowHelp ();   
              break;
            case '?':
              if (optopt == 'c')
                fprintf (stderr, "Option -%c requires an argument.\n", optopt);
              else if (isprint (optopt))
              {
                fprintf (stderr, "Unknown option `-%c'.\n", optopt);
                exit(1);
                }
              else
                fprintf (stderr, "Unknown option character `\\x%x'.\n", optopt);
              return;
            default:
               ShowHelp ();
           }
  }
  if (FragDefrag == NICHTS) { FragDefrag = FRAGMENT; }
  return;  
}

void ShowHelp()
{  

 printf ("Usage: \n\n");
 printf ("%s [-d] [-c] [-h]\n\n",MyName);
 printf ("-d => defrag from fragmented files 1,2,3,4.....\n");
 printf ("-c => count of files to build, minimum 2, maximum 128\n");
 printf ("-h => this help screen\n");

 exit(1);
}


