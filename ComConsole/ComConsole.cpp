// ComConsole.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"


/* 
 * File:   main.cc
 * Author: liveuser
 *
 * Created on 2009骞?鏈?0鏃? 涓嬪崍9:35
 */

#include <stdio.h>
#include <conio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <string>
#include "vt100.h"
#include "kbd.h"
#include "E:\Development\TmnDev\TmnLibs\SerialCom\SerialCom.h"

using namespace std;

#define BYTE        unsigned char
#define KEY_LF          ((short)10)
#define KEY_ENTER       ((short)13)
#define KEY_ESC         ((short)27)
#define KEY_CTRL_A      ((short)1)
#define KEY_CTRL_Q      ((short)17)
#define KEY_BACKSPACE   ((short)127)
/*
 * 
 */
#ifdef  RELEASE_ON_ARM
#define IS_ENTER_KEY(nKey)  true
#else   //RELEASE_ON_ARM
#define IS_ENTER_KEY(nKey)  (KEY_ENTER == nKey || KEY_LF == nKey)
#endif  //RELEASE_ON_ARM

int PrintHelp() {
    printf("\nUssage:");
    printf("\ncommate [o=COM Port] [r=Baud Rate] [b=Byte Size] [s=Stop Bits] [p=Parity Bit] ");
    printf("\n\to = 1..8					(default=1)");
    printf("\n\tr = 1200/2400/4800/9600/1920038400/57600/115200 (default=9600)");
    printf("\n\tb = 7..8					(default=8)");
    printf("\n\ts = 1..2					(default=1)");
    printf("\n\tp = N|O|E					(default=N)\n");
    return 0;
}

static bool bEchoOn = false;
static bool bDisplayHex = false;
static bool bInvertData = false;
static bool bCommandMode = false;
//void* ThreadPrintData(void* pParam) {
UINT ThreadPrintData(LPVOID pParam) {
    if (pParam) {
        CSerialCom* pComm = (CSerialCom*) pParam;
        char pData[1024];
        char sFormat[8];
        bool bReceived = false;
        DWORD nData = 0;
        int nIdel = 0;

        memset(pData, 0x00, 1024);
        memset(sFormat,0x00,8);
        for(;true;Sleep(20)) {
            if ( pComm->m_PortReading(pData, nData = 1024) && nData > 0 ) {
                bReceived = true;
                if (bDisplayHex) strcpy(sFormat,"%02X ");
                    else strcpy(sFormat,"%c");
                if (bInvertData) printf(VT100_STYLE2);  //invert text color
                for (int r = 0; r < nData; r++) printf(sFormat, 0x00ff & pData[r]);
                if (bInvertData) printf(VT100_RESET);   //restore text color
                fflush(stdout);
                memset(pData, 0x00, nData);
            }
        }
    }
	return 0;
}

static char nHalfByte = 0;      //  0---left half byte(high 4 bits) 1---right half byte(low 4 bits)
static char sHexValue[3] = {0,0,0};

bool CharToHex(char cKey,BYTE& cHex) {
    switch (cKey) {
        case 'a':   case 'A':   case 'b':   case 'B':
        case 'c':   case 'C':   case 'd':   case 'D':
        case 'e':   case 'E':   case 'f':   case 'F':
        case '1':   case '2':   case '3':   case '4':   case '5':
        case '6':   case '7':   case '8':   case '9':   case '0':
            sHexValue[nHalfByte] = cKey;
            nHalfByte ++;
            break;
        case '\n':  case ' ':   if (nHalfByte > 0) nHalfByte = 2;  break;
        default:;
    }
    if (2 == nHalfByte) {
        int nHex = 0;
        nHalfByte = 0;
        sscanf(sHexValue,"%x",&nHex);
        memset(sHexValue,0x00,3);
        cHex = (BYTE)nHex;
        return true;
    }
    return false;
}

int StrToHex(char* sText,BYTE* sHex) {
    if (NULL == sText || NULL == sHex) return 0;
    int nHex = 0;
    for (int i=0; sText[i]; i ++) {
        BYTE    cHex = 0;
        if (CharToHex(sText[i],cHex)) {
            sHex[nHex] = cHex;
            nHex ++;
        }
    }
    return nHex;
}

void ShowCommandLine() {
    printf(VT100_GOTO("1","1")
        VT100_STYLE_MENU "[Asci"    "%s" "i"    VT100_STYLE_MENU "/"        "%s" "H" VT100_STYLE_MENU "ex] "
        VT100_STYLE_MENU "[Echo o"  "%s" "n"    VT100_STYLE_MENU "/"        "%s" "o" VT100_STYLE_MENU "ff] "
        VT100_STYLE_MENU "[Inver"   "%s" "t"    VT100_STYLE_MENU "/norma"   "%s" "l" VT100_STYLE_MENU "] "
        VT100_STYLE_MENU "[Clea"VT100_STYLE_KEY"r"VT100_STYLE_MENU "] "
        VT100_STYLE_MENU "["VT100_STYLE_KEY"Q"VT100_STYLE_MENU "uit]>" VT100_RESET,
        (bDisplayHex ? VT100_STYLE_KEY : VT100_STYLE_HOT),(bDisplayHex ? VT100_STYLE_HOT : VT100_STYLE_KEY),
        (bEchoOn     ? VT100_STYLE_HOT : VT100_STYLE_KEY),(bEchoOn     ? VT100_STYLE_KEY : VT100_STYLE_HOT),
        (bInvertData ? VT100_STYLE_HOT : VT100_STYLE_KEY),(bInvertData ? VT100_STYLE_KEY : VT100_STYLE_HOT));
    fflush(stdout);
//    printf(VT100_GOTO("1","1")
//        VT100_STYLE_MENU "[asci"    VT100_STYLE_HKEY "%c"   VT100_STYLE_MENU "/"        VT100_STYLE_HKEY "%c" VT100_STYLE_MENU "ex] "
//        VT100_STYLE_MENU "[echo o"  VT100_STYLE_HKEY "%c"   VT100_STYLE_MENU "/"        VT100_STYLE_HKEY "%c" VT100_STYLE_MENU "ff] "
//        VT100_STYLE_MENU "[inver"   VT100_STYLE_HKEY "%c"   VT100_STYLE_MENU "/norma"   VT100_STYLE_HKEY "%c" VT100_STYLE_MENU "] "
//        VT100_STYLE_MENU "[clea"    VT100_STYLE_HKEY "r"    VT100_STYLE_MENU "] "
//        VT100_STYLE_MENU "["        VT100_STYLE_HKEY "Q"    VT100_STYLE_MENU "uit]>"
//            VT100_RESET,
//            bDisplayHex ? 'i' : 'I',bDisplayHex ? 'H' : 'h',
//            bEchoOn ? 'N' : 'n',bEchoOn ? 'o' : 'O',
//            bInvertData ? 'T' : 't',bInvertData ? 'l' : 'L');
//    fflush(stdout);
}

void ShowStatusBar(char* sText) {
    printf(VT100_SAVE_CURSOR VT100_GOTO("1","1") VT100_STYLE2);
    printf(sText);
    printf(VT100_LOAD_CURSOR VT100_RESET);
    fflush(stdout);
}

void SwitchMode(CKeyboard& keyb) {
    bCommandMode = !bCommandMode;
    if (bCommandMode) { //switch to Command mode
        keyb.m_SetEcho(false);
        printf(VT100_SAVE_CURSOR);
        ShowCommandLine();
    } else {            //switch to Transparent mode
        printf(VT100_LOAD_CURSOR);
        fflush(stdout);
    }
}

int main(int argc, char* argv[]) {
    CKeyboard keyb;
    CSerialCom comm;
//    pthread_t printer = 0;
    HANDLE	printer = 0;
    int nPort = 1;
    int nBaud = 9600;
    int nBits = 8;
    int nStop = 1;
	DWORD	dwComPara = 0;
    char cParity = 'N';

    if (argc < 2)
        return PrintHelp();
    for (int i = 1; i < argc; i++) {
        string szArgv(argv[i]);
        string::size_type nIndex = string::npos;
        if ((nIndex = szArgv.find("o=")) != string::npos) {
            sscanf(szArgv.substr(nIndex).data() + 2, "%d", &nPort);
            if (nPort < 1 || nPort > 8) return PrintHelp();
        } else if ((nIndex = szArgv.find("r=")) != string::npos) {
            sscanf(szArgv.substr(nIndex).data() + 2, "%d", &nBaud);
            if (nBaud != 1200 && nBaud != 2400 && nBaud != 4800 &&
                    nBaud != 9600 && nBaud != 19200 && nBaud != 38400 &&
                    nBaud != 57600 && nBaud != 115200) return PrintHelp();
        } else if ((nIndex = szArgv.find("b=")) != string::npos) {
            sscanf(szArgv.substr(nIndex).data() + 2, "%d", &nBits);
            if (nBits != 7 && nBits != 8) return PrintHelp();
        } else if ((nIndex = szArgv.find("s=")) != string::npos) {
            sscanf(szArgv.substr(nIndex).data() + 2, "%d", &nStop);
            if (nStop != 1 && nStop != 2) return PrintHelp();
        } else if ((nIndex = szArgv.find("p=")) != string::npos) {
            sscanf(szArgv.substr(nIndex).data() + 2, "%c", &cParity);
            if (cParity != 'N' && cParity != 'n' && cParity != 'O' &&
                    cParity != 'o' && cParity != 'E' && cParity != 'e') return PrintHelp();
        } else return PrintHelp();
    }
	dwComPara |= 0xf0000000 & ( nPort << 28 );		//端口号
	dwComPara |= 0x000fffff & nBaud;				//波特率
	dwComPara |= 0x0f000000 & ( nBits << 24 );		//数据位
	dwComPara |= 0x00c00000 & ( nStop << 22 );		//停止位
	dwComPara |= 0x00300000 & ( cParity << 20 );	//校验位
	if (comm.m_PortSetting(dwComPara) && comm.m_PortOpen(USE_CURR_PORT) && keyb.m_Open()) {
//        pthread_create(&printer, NULL, ThreadPrintData, &comm);
		DWORD	dwTId = 0;
		printer = CreateThread( NULL, 0, (LPTHREAD_START_ROUTINE)ThreadPrintData, &comm, 0, &dwTId );

        BYTE    pData[1024];
        char    sData[1024];
        char    sMessage[256];
        DWORD	nData = 0;
		DWORD	nSend = 0;
        int     nKey = 0;
        bool    bRun = true;

        memset(pData,0x00,1024);
        memset(sData,0x00,1024);
        memset(sMessage,0x00,256);
        printf(VT100_CLEAR);
        sprintf(sMessage,"Open COM port: %d,baud rate=% 6d,byte size=%d,stop bits=%d,parity bit=%c",
                nPort, nBaud, nBits, nStop, cParity);
        ShowStatusBar(sMessage);
        while (bRun) {
            if (IS_ENTER_KEY(nKey) && keyb.m_IsEchoOn()) {
                sprintf(sMessage,"Press Esc and Enter to switch into Command mode.");
                ShowStatusBar(sMessage);
                memset(sData, 0x00, 1024);
                fgets(sData,1023,stdin);
                if ((nData = strlen(sData)) > 0) {
                    if (KEY_ESC == sData[0] ) {
                        SwitchMode(keyb);
                    } else if (bDisplayHex) {  //in Hex mode
                        if ((nData = StrToHex(sData,pData)) > 0) {
							comm.m_PortWritting((char*)pData, nData);
                            memset(pData, 0x00, 1024);
                            nData = 0;
                        }
                    } else {            //in Ascii mode
                        comm.m_PortWritting(sData, nData);
                        memset(sData, 0x00, 1024);
                        nData = 0;
                    }
                }
            }
            if (nKey = keyb.m_IsKeyPressed()) {
                if (KEY_CTRL_A == nKey) {   //mode switching
                    SwitchMode(keyb);
                } else if (bCommandMode) { //in Command mode
                    Sleep(100);
                    switch((char)nKey) {
                        case 'I':   case 'i':   bDisplayHex = false;    break;  //switch to ASCII mode
                        case 'N':   case 'n':   bEchoOn = true;         break;  //switch to ECHO ON mode
                        //if switch to HEX mode then must set to ECHO ON mode
                        case 'H':   case 'h':   bEchoOn = bDisplayHex = true;   break;
                        //if switch to ECHO OFF mode then must set to ASCII mode
                        case 'O':   case 'o':   bDisplayHex = bEchoOn = false;  break;
                        case 'T':   case 't':   bInvertData = true;     break;
                        case 'L':   case 'l':   bInvertData = false;    break;
                        case 'R':   case 'r':   printf(VT100_GOTO("2","1") VT100_SAVE_CURSOR VT100_CLEAR);    break;
                        case 'Q':   case 'q':   bRun = false;           break;
                        default:;
                    }
                    ShowCommandLine();
                    keyb.m_SetEcho(bEchoOn);
                    bCommandMode = false;   //switch to Transparent mode
                    printf(VT100_LOAD_CURSOR);
                    fflush(stdout);
                } else if (false == bDisplayHex && false == keyb.m_IsEchoOn()) {
                    //Current in ASCII & ECHO OFF & Transparent mode
                    comm.m_PortWritting((char*)&nKey, nSend = 1);
                    sprintf(sMessage,"Press Ctrl+A to switch into Command mode. %s %c",
                            VT100_STYLE_KEY,(char)nKey,(char)nKey);
                    ShowStatusBar(sMessage);
                } else {
                    sprintf(sMessage,"Press Ctrl+A to switch into Command mode. %s<%02x> %c",
                            VT100_STYLE_KEY,(char)nKey,(char)nKey);
                    ShowStatusBar(sMessage);
                }
//                printf("<%02x>",(char)nKey);
            }
        }
		TerminateThread(printer,0);
        //pthread_cancel(printer);
        //pthread_join(printer, NULL);
    } else {
		comm.m_PortClose();
        return 0;
    }
COMMATE_EXIT:
	comm.m_PortClose();
    printf(VT100_CLEAR VT100_GOTO("0","0"));
    fflush(stdout);
    return 0;
}
