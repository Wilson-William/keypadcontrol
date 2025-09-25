
#ifndef MYAPP_H
#define MYAPP_H


#ifdef __cplusplus
extern "C" {
#endif
	
void MyApp(void);
void init_the_thing(void);
void to_hex(char buf[], int i);
bool checkIfBadConfig();
bool updateFirmwareOnUSB();
bool loop();
bool loop2();
bool loop3();
void LogToBufferLog(char * stringToLog, int size);
#ifdef __cplusplus
}
#endif


#endif
