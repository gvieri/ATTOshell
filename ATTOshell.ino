/* 
 *  ATTOshell. (C) 2020 Vieri Giovambattista 
 *  All Rights Reserved.
 *  License GPL V2. 
 *  
 */


#include "SPIFFS.h"
#include "esp_spiffs.h"



#define WAIT_TIME 50
#define HEADERLINE "ATTOshell is an esp32 really simple cli"
#define CURRENT_DIR_LEN 500
#define COMMAND_LEN 500

struct command_str {
//  int    tok;
  char * cmd_str;
  bool   mand_arg; // flag to identify if the command has a mandatory argument.
  char* (* func)(char *); 
  char * desc_str;
  char * help_str;  
};



char current_dir[CURRENT_DIR_LEN]; 
char incoming=0;
char command[COMMAND_LEN];
int commandindex=0; 
bool commandready=false; 

char *eraseFile(char *); 
char *myhelp(char *); 
char *dummy(char *); 
char *lsfile(char *); 
char *mymkdir(char *); 
char *myrmdir(char *); 
char *cd  (char *); 
char *pwd  (char *);
/* 
char *  (char *); 
char *  (char *); 
char *  (char *); 
char *  (char *); 
*/
struct command_str commands[]= {
  {"help"  , false, myhelp     ,"help command" , "help [command to be explained] ."},
  {"ls"    , false, lsfile   ,"ls command", "ls [dir] . Example: ls , ls /, ls mydirectory." },  
  {"format", false,dummy    ,"format", "format file system. Now you can only format the SPIFF volume (and lose all the stored data)" },
  {"edit"  , false,dummy    ,"edit a file","edit unimplemented" },
  {"cat"   , false,dummy    ,"cat a file", "cat unimplemented" }, 
  {"rm"    , true ,eraseFile,"rm command","rm <file>" }
};
////////////////////////////////////////////////////////
// utility function 
////////////////////////////////////////////////////////

int n_commands=sizeof(commands)/sizeof(commands[0]);

char * dummy(char * arg) {
  Serial.println("called dummy...");
}

char * format(char * arg) { 
  bool formatted = SPIFFS.format();
  if(formatted){
    Serial.println("\n\nSuccess formatting");
  }else{
    Serial.println("\n\nError formatting");
  }  
}

  
char * myhelp(char * arg) {
  Serial.println("ATTOshell is a really simple shell for IoT development board.\nThe command list is the following:"); 
  
  for (int i=0; i< n_commands; i++) { 
    Serial.println(commands[i].help_str);
  }

  Serial.println("----------------------------------------"); 
  return ("" ) ; 
}

char * eraseFile(char * filename ) { 
  // this function will close than reopen the file. So file content will be wiped. 
  
  SPIFFS.remove(filename);
  return("file erased");
}

char *lsfile(char * point) {
  if (point == NULL) point = "/"; 
  
  File dir = SPIFFS.open(point);  
  int numtabs=1; 
    
  if (!dir) {
     Serial.printf("Error opening %s\n",point);
     return ("not a dir" ) ;
  }
    
  if(dir.isDirectory()) { 
     while (true) {
        File entry =  dir.openNextFile();
        if (! entry) {
           // no more files in the folder
           break;
          }
        for (uint8_t i = 0; i < numtabs; i++) {
           Serial.print('\t');
        }
        Serial.print(entry.name());
        if (entry.isDirectory()) {
           Serial.println("/");

        } else {
      // display size for file, nothing for directory
           Serial.print("\t\t");
           Serial.println(entry.size(), DEC);
        }
        entry.close();
     }
  } else { 
       Serial.print(dir.name());
       Serial.print("\t\t");
       Serial.println(dir.size(), DEC);
  }
  return (""); 
}

/////////////////////////////////////////////////

void setup() {
  int i; 

  
  Serial.begin(115200);
  for (i=0; i < WAIT_TIME; ++i) {
    Serial.print(".");
    Serial.flush(); 
  }
  Serial.println("\nScanning completed :-) ...");

  bool success = SPIFFS.begin(true);
 
  if(success){
    Serial.println("SPIFFS mounted with success");  
  } else {
    Serial.println("SPIFFS: Error mounting the file system");  
  }

  Serial.println(HEADERLINE); 

  Serial.printf("Total Bytes %d\nUsed Bytes %d\n",SPIFFS.totalBytes(), SPIFFS.usedBytes()); 

  char *pt=myhelp(""); 

}

//////////////////////////////////////////////////////////////////

void loop() {

  delay(10);
  if (Serial.available() > 0) {
    // read the incoming byte:
    incoming = Serial.read();
    command[commandindex++]=incoming; 
    Serial.print(incoming); 
    if ( (incoming=='\n') || (commandindex==COMMAND_LEN-1)) {
      command[commandindex-1]='\0';  
      commandready=true;
      commandindex=0; 
    }
  }
  if (commandready) { 
    commandready=false;
    char *cmd; 
    char *argument;

    cmd=strtok(command," "); 
    argument = strtok(NULL," "); 
   
    for (int i=0; i< n_commands; i++) {
      if(!strcmp(cmd,commands[i].cmd_str)) { 
        *commands[i].func(argument); 
      }
    }
  }
   
}
