
#include "main.h"
#include "pcb_config.h"
#include "MyApp.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

extern UART_HandleTypeDef huart2;
char search(char[] ,char* );
extern I2C_HandleTypeDef hi2c2;
char bufferLog[1020];
int indexBufferLog = 0;

#ifdef __cplusplus
extern "C" {
#endif

//#ifdef __GNUC__
///* With GCC, small printf (option LD Linker->Libraries->Small printf
//   set to 'Yes') calls __io_putchar() */
//#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
//#else
//#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
//#endif /* __GNUC__ */
//PUTCHAR_PROTOTYPE
//{
//  /* Place your implementation of fputc here */
//  /* e.g. write a character to the USART2 and Loop until the end of transmission */
//  HAL_UART_Transmit(&huart2, (uint8_t *)&ch, 1, 0xFFFF);
//
//  return ch;
//}

#ifdef __cplusplus
}
#endif



#include "Ch376msc.h"
extern struct PCB_Config pcb_config;
bool problem = false;

#define printInfo printf

//..............................................................................................................................
// Connect to USART port
Ch376msc flashDrive(9600); // chipSelect, busy pins (use this if no other device are attached to SPI port(MISO pin used as interrupt))

//If the SPI port shared with other devices e.g SD card, display, etc. remove from comment the code below and put the code above in a comment
//Ch376msc flashDrive(10, 9, 8); // chipSelect, busy, interrupt pins
//..............................................................................................................................
 // buffer for reading
char adatBuffer[255];// max length 255 = 254 char + 1 NULL character
char firmwareBuffer[249];
bool readMore;

//
//struct PCB_Config
//{
//	char MagicNumber[4];
//	char sSetIR[15];
//	char sSetR2S9C[15];
//	char sSetR2S9COFF[15];
//	char sSetTempTreshMin[10];
//	char sSetTempTreshMax[10];
//	char sSetTempON[10];
//	char sSetPIRON[10];
//	char sPIRDelayON[10];
//	char sPIRDelayOFF[10];
//	char sSetIRON[10];
//	char sSetRS232ON[10];
//
//};
uint32_t _crc32(const uint8_t *data, size_t length) {
    static uint32_t crcTable[256];
    static int tableGenerated = 0;

    if (!tableGenerated) {
        // Generate the CRC table
        uint32_t crc;
        for (int i = 0; i < 256; i++) {
            crc = i;
            for (int j = 8; j > 0; j--) {
                if (crc & 1) {
                    crc = (crc >> 1) ^ 0xEDB88320;
                } else {
                    crc >>= 1;
                }
            }
            crcTable[i] = crc;
        }
        tableGenerated = 1;
    }

    // Calculate the CRC
    uint32_t crc = 0xFFFFFFFF;
    while (length--) {
        crc = (crc >> 8) ^ crcTable[(crc ^ *data++) & 0xFF];
    }

    return crc ^ 0xFFFFFFFF;
}

bool updateFirmwareOnUSB()
{
	bool ret = 0;
	uint64_t data;
	int index = 0;
	uint64_t totalSize = 0;
	uint8_t d_num = 0;
	uint32_t PageError = 0;
	FLASH_EraseInitTypeDef EraseInitStruct = {0};
	EraseInitStruct.Banks = FLASH_BANK_2;
	EraseInitStruct.TypeErase   = FLASH_TYPEERASE_MASS;

	if(flashDrive.checkDrive())
	{
		HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, GPIO_PIN_RESET);
		flashDrive.setFileName("FIRMWARE.BIN");        //set the file name
		flashDrive.openFile();  //open the file
		readMore = true;
		HAL_FLASH_Unlock();


		while(readMore)
		{
			readMore = flashDrive.readFile(firmwareBuffer, sizeof(firmwareBuffer), &d_num);
			if(d_num > 0)
			{
				if(index == 0)	HAL_FLASHEx_Erase(&EraseInitStruct, &PageError);
				for(int i = 0 ; i < d_num ; i += 8)
				{
					memcpy(&data, firmwareBuffer + i, 8);
					HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, APPLICATION_ADDRESS + index*248 + i, data);
				}
				totalSize += d_num;
				index++;
			}
			d_num = 0;
		}
		if(index > 0)
		{
			data = ((uint64_t)totalSize<<32)|0x1234ABCD;
			HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, META_ADDRESS, data);
			data = _crc32((uint8_t*)APPLICATION_ADDRESS, totalSize);
			HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, META_ADDRESS + 8, data);
			firmware_update = 1;
			ret = 1;
		}
		HAL_FLASH_Lock();
		flashDrive.closeFile();
		HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, GPIO_PIN_SET);
	}
	return ret;
}

bool loop()
{
	uint8_t d_num;
	if(flashDrive.checkDrive()){ //if any flash drive attach
		//printInfo("Read File: programFile.txt");          // Read the contents of this file on the USB disk, and display contents in the Serial Monitor
		flashDrive.setFileName("FILE.TXT");        //set the file name
		flashDrive.openFile();  //open the file
		//while(flashDrive.openFile() != ANSW_USB_INT_SUCCESS);
		readMore = true;
		                //read data from flash drive until we reach EOF
		while(readMore){ // our temporary buffer where we read data from flash drive and the size of that buffer
			readMore = flashDrive.readFile(adatBuffer, sizeof(adatBuffer), &d_num);
		    //printf("%s\r\n",adatBuffer);          //print the contents of the temporary buffer
		}
		flashDrive.closeFile();               //at the end, close the file
		//printInfo("Done!");

		//printf("Flash drive content:%s\r\n",adatBuffer);          //print the contents of the temporary buffer
		return 1;
	}else{
		return 0;
	}




}//end loop
bool loop2()
{
	char bufferchar1[sizeof("IR Command ON : ")] = "IR Command ON : ";
	char bufferchar2[sizeof("\n\rIR Command OFF : ")] = "\n\rIR Command OFF : ";

	flashDrive.setFileName("LOG.TXT");        //set the file name
	//flashDrive.deleteFile();
	flashDrive.openFile();  //open the file

	flashDrive.writeFile(bufferchar1, sizeof(bufferchar1));  //open the file

	for (int i = 0; i< 8; i++)
		flashDrive.writeFile(&pcb_config.sSetIR[i],1);

	flashDrive.writeFile(bufferchar2, sizeof(bufferchar2));  //open the file

	for (int j = 0; j< 8; j++)
			flashDrive.writeFile(&pcb_config.sSetIROFF[j],1);


	flashDrive.closeFile();               //at the end, close the file
	return 1;
}
bool loop3()
{


	flashDrive.setFileName("LOGRS232.TXT");        //set the file name
	//flashDrive.deleteFile();
	flashDrive.openFile();  //open the file

	flashDrive.writeFile(bufferLog, (uint8_t)sizeof(bufferLog));  //open the file

	flashDrive.closeFile();               //at the end, close the file
	return 1;
}

void LogToBufferLog(char * stringToLog, int size)
{
	if (indexBufferLog + size > 1020)
		indexBufferLog = 0;

	for (int i = 0 ; i < size ; i++)
	{
		bufferLog[i+indexBufferLog] = stringToLog[i];

	}
	indexBufferLog+=size;
}
struct PCB_Config file;
char a, b;
int i, m, j;

void MyApp(void) {


	if (loop()) {

		// Write the actual config first
		memcpy(&file, &pcb_config, sizeof(pcb_config));

		file.MagicNumber[0] = 0xDD;
		file.MagicNumber[1] = 0x55;
		file.MagicNumber[2] = MAGIC;
		file.MagicNumber[3] = 0x11;

		///1///
		a = search(adatBuffer, (char*) "SetIR:");
		b = search(adatBuffer, (char*) ";");
		if (a) {
			for (i = 0, j = a; j < b - 1; j++, i++) {
				file.sSetIR[i] = adatBuffer[j];
			}
			m = 0;
			while (m < 256 - b) {
				adatBuffer[m] = adatBuffer[m + b];
				m++;
			}
		} else
			problem = true;

		///1///
		a = search(adatBuffer, (char*) "SetIROFF:");
		b = search(adatBuffer, (char*) ";");
		if (a) {
			for (i = 0, j = a; j < b - 1; j++, i++) {
				file.sSetIROFF[i] = adatBuffer[j];
			}
			m = 0;
			while (m < 256 - b) {
				adatBuffer[m] = adatBuffer[m + b];
				m++;
			}
		} else
			problem = true;
		///2///
		a = search(adatBuffer, (char*) "R2S9C:");
		b = search(adatBuffer, (char*) ";");

		file.rs232nbcharsON[0] = (b - a - 1) / 2;
		if (a) {
			for (i = 0, j = a; j < b - 1; j++, i++) {
				file.sSetR2S9C[i] = adatBuffer[j];
			}
			m = 0;
			while (m < 256 - b) {
				adatBuffer[m] = adatBuffer[m + b];
				m++;
			}
		} else
			problem = true;
		///11///
		a = search(adatBuffer, (char*) "R2S9COFF:");
		b = search(adatBuffer, (char*) ";");

		file.rs232nbcharsOFF[0] = (b - a - 1) / 2;
		if (a) {
			for (i = 0, j = a; j < b - 1; j++, i++) {
				file.sSetR2S9COFF[i] = adatBuffer[j];
			}
			m = 0;
			while (m < 256 - b) {
				adatBuffer[m] = adatBuffer[m + b];
				m++;
			}
		} else
			problem = true;
		///3///
		a = search(adatBuffer, (char*) "TMin:");
		b = search(adatBuffer, (char*) ";");
		if (a) {
			for (i = 0, j = a; j < b - 1; j++, i++) {
				file.sSetTempTreshMin[i] = adatBuffer[j];
			}
			m = 0;
			while (m < 256 - b) {
				adatBuffer[m] = adatBuffer[m + b];
				m++;
			}
		} else
			problem = true;
		///4///
		a = search(adatBuffer, (char*) "TMax:");
		b = search(adatBuffer, (char*) ";");
		if (a) {
			for (i = 0, j = a; j < b - 1; j++, i++) {
				file.sSetTempTreshMax[i] = adatBuffer[j];
			}
			m = 0;
			while (m < 256 - b) {
				adatBuffer[m] = adatBuffer[m + b];
				m++;
			}
		} else
			problem = true;
		///5///
		a = search(adatBuffer, (char*) "TON:");
		b = search(adatBuffer, (char*) ";");
		if (a) {
			for (i = 0, j = a; j < b - 1; j++, i++) {
				file.sSetTempON[i] = adatBuffer[j];
			}
			m = 0;
			while (m < 256 - b) {
				adatBuffer[m] = adatBuffer[m + b];
				m++;
			}
		} else
			problem = true;
		///6///
		a = search(adatBuffer, (char*) "PIRON:");
		b = search(adatBuffer, (char*) ";");
		if (a) {
			for (i = 0, j = a; j < b - 1; j++, i++) {
				file.sSetPIRON[i] = adatBuffer[j];
			}
			m = 0;
			while (m < 256 - b) {
				adatBuffer[m] = adatBuffer[m + b];
				m++;
			}
		} else
			problem = true;
		///7///
		a = search(adatBuffer, (char*) "PIRDelON:");
		b = search(adatBuffer, (char*) ";");
		if (a) {
			for (i = 0, j = a; j < b - 1; j++, i++) {
				file.sPIRDelayON[i] = adatBuffer[j];
			}
			m = 0;
			while (m < 256 - b) {
				adatBuffer[m] = adatBuffer[m + b];
				m++;
			}
		} else
			problem = true;
		///8///
		a = search(adatBuffer, (char*) "PIRDelOFF:");
		b = search(adatBuffer, (char*) ";");
		if (a) {
			for (i = 0, j = a; j < b - 1; j++, i++) {
				file.sPIRDelayOFF[i] = adatBuffer[j];
			}
			m = 0;
			while (m < 256 - b) {
				adatBuffer[m] = adatBuffer[m + b];
				m++;
			}
		} else
			problem = true;
		///9///
		a = search(adatBuffer, (char*) "IRON:");
		b = search(adatBuffer, (char*) ";");
		if (a) {
			for (i = 0, j = a; j < b - 1; j++, i++) {
				file.sSetIRON[i] = adatBuffer[j];
			}
			m = 0;
			while (m < 256 - b) {
				adatBuffer[m] = adatBuffer[m + b];
				m++;
			}
		} else
			problem = true;
		///10///
		a = search(adatBuffer, (char*) "RS232ON:");
		b = search(adatBuffer, (char*) ";");
		if (a) {
			for (i = 0, j = a; j < b - 1; j++, i++) {
				file.sSetRS232ON[i] = adatBuffer[j];
			}
			m = 0;
			while (m < 256 - b) {
				adatBuffer[m] = adatBuffer[m + b];
				m++;
			}
		} else
			problem = true;
		///11///
		a = search(adatBuffer, (char*) "tvt:");
		b = search(adatBuffer, (char*) ";");
		if (a) {
			for (i = 0, j = a; j < b - 1; j++, i++) {
				file.tvtype[i] = adatBuffer[j];
			}
			m = 0;
			while (m < 256 - b) {
				adatBuffer[m] = adatBuffer[m + b];
				m++;
			}
		} else
			problem = true;
		///11///
		a = search(adatBuffer, (char*) "rs1:");
		b = search(adatBuffer, (char*) ";");
		if (a) {
			for (i = 0, j = a; j < b - 1; j++, i++) {
				file.rs1[i] = adatBuffer[j];
			}
			m = 0;
			while (m < 256 - b) {
				adatBuffer[m] = adatBuffer[m + b];
				m++;
			}
		} else
			problem = true;
		///11///
		a = search(adatBuffer, (char*) "rs2:");
		b = search(adatBuffer, (char*) ";");
		if (a) {
			for (i = 0, j = a; j < b - 1; j++, i++) {
				file.rs2[i] = adatBuffer[j];
			}
			m = 0;
			while (m < 256 - b) {
				adatBuffer[m] = adatBuffer[m + b];
				m++;
			}
		} else
			problem = true;
		///11///
		a = search(adatBuffer, (char*) "rs3:");
		b = search(adatBuffer, (char*) ";");
		if (a) {
			for (i = 0, j = a; j < b - 1; j++, i++) {
				file.rs3[i] = adatBuffer[j];
			}
			m = 0;
			while (m < 256 - b) {
				adatBuffer[m] = adatBuffer[m + b];
				m++;
			}
		} else
			problem = true;

		///12///
		char loopontemp[2];
		a = search(adatBuffer, (char*) "lpon:");
		b = search(adatBuffer, (char*) ";");
		if (a) {
			for (i = 0, j = a; j < b - 1; j++, i++) {
				loopontemp[i] = adatBuffer[j];
			}
			m = 0;
			while (m < 256 - b) {
				adatBuffer[m] = adatBuffer[m + b];
				m++;
			}
		} else
			problem = true;

		file.loopon = loopontemp[0] - 48;

		///12///
		char loopofftemp[2];
		a = search(adatBuffer, (char*) "lpoff:");
		b = search(adatBuffer, (char*) ";");
		if (a) {
			for (i = 0, j = a; j < b - 1; j++, i++) {
				loopofftemp[i] = adatBuffer[j];
			}
			m = 0;
			while (m < 256 - b) {
				adatBuffer[m] = adatBuffer[m + b];
				m++;
			}
		} else
			problem = true;

		file.loopoff = loopofftemp[0] - 48;

		//14///
		char looptimetemp[3];
		a = search(adatBuffer, (char*) "lpt:");
		b = search(adatBuffer, (char*) ";");
		if (a) {
			for (i = 0, j = a; j < b - 1; j++, i++) {
				looptimetemp[i] = adatBuffer[j];
			}
			m = 0;
			while (m < 256 - b) {
				adatBuffer[m] = adatBuffer[m + b];
				m++;
			}
		} else
			problem = true;

		file.looptime = (looptimetemp[0] - 48) * 10 + (looptimetemp[1] - 48);

		if (file.sPIRDelayON[0] == '0' && file.sPIRDelayON[1] == '0'
				&& file.sPIRDelayON[2] == '0') {
			file.sPIRDelayON[2] = '1';
			file.sPIRDelayON[3] = '0';
		}
		if (file.sPIRDelayOFF[0] == '0' && file.sPIRDelayOFF[1] == '0'
				&& file.sPIRDelayOFF[2] == '0') {
			file.sPIRDelayOFF[2] = '1';
			file.sPIRDelayOFF[3] = '0';
		}
		memcpy(&pcb_config, &file, sizeof(file));

		if (problem) {
			Send_String((char*) "BAD USB Config, verify file\n\r",
					sizeof("BAD USB Config, verify file\n\r"));

		} else if (!checkIfBadConfig()) {
			Send_String((char*) "BAD USB param, verify file\n\r",
					sizeof("BAD USB param, verify file\n\r"));
		} else {
			Send_String((char*) "GOOD USB Config/n/r",
					sizeof("GOOD USB Config/n/r"));

			WriteToEeprom();

		}
	}
}

bool checkIfBadConfig()
{
	if (pcb_config.sSetIRON[0] == '1')
	{
		for (int i = 0; i< 5 ; i++)
		{
			if (pcb_config.sSetIR[i] == 0)
			{
				Send_String((char*)"BAD IR set, verify your IR cmd\n\r", sizeof("BAD IR set, verify your IR command\n\r"));
				return false;
			}
		}
	}
	if (pcb_config.sSetIRON[0] == '1')
	{
		for (int i = 0; i< 5 ; i++)
		{
			if (pcb_config.sSetIROFF[i] == 0)
			{
				Send_String((char*)"BAD IROFF set, verify your IR cmd\n\r", sizeof("BAD IROFF set, verify your IR command\n\r"));
				return false;
			}
		}
	}
	if (pcb_config.sSetRS232ON[0] == '1')
	{
		for (int i = 0; i< pcb_config.rs232nbcharsON[0] ; i++)
		{
			if (pcb_config.sSetR2S9C[i] == 0)
			{
				Send_String((char*)"BAD RS232ON set, verify your IR cmd\n\r", sizeof("BAD RS232ON set, verify your RS232 command\n\r"));
				return false;
			}
		}
	}
	if (pcb_config.sSetRS232ON[0] == '1')
	{
		for (int i = 0; i< pcb_config.rs232nbcharsOFF[0] ; i++)
		{
			if (pcb_config.sSetR2S9COFF[i] == 0)
			{
				Send_String((char*)"BAD RS232OFF set, verify your IR cmd\n\r", sizeof("BAD RS232OFF set, verify your RS232 command\n\r"));
				return false;
			}
		}
	}
	//Send_IR_Command_Debug();
//	char data[2]; //size of the number
//	data[0] = '\n';
//	data[1] = '\r';
//
//	HAL_UART_Transmit(&huart1, (uint8_t *)pcb_config.sSetIR[0], 8, 1000); //Send dataFrame through UART
//	HAL_UART_Transmit(&huart1, (uint8_t *)data[0], 2, 1000); //Send dataFrame through UART

	return true;

}
void to_hex(char buf[], int i)
{
  if (*buf == '\0')
    return;

  to_hex(buf + 2, i+1);
  buf[1] = strtol(buf, NULL, 16);
  *buf = '\0';
}

char search(char str[],char* s){
	char *pch;
	int n;
	pch = strstr (str,s);
	if (pch != NULL){
		n = pch - str;
		n += int((strlen(s)));
		return n;
	}
	return 0;
}
void init_the_thing(void)
{
	flashDrive.init();
}
