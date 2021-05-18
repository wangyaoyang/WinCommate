#include <stdafx.h>
#include <stdio.h>
#include <conio.h>
#include "kbd.h"


/* KBDUS means US Keyboard Layout. This is a scancode table
*  used to layout a standard US keyboard. I have left some
*  comments in to give you an idea of what key is what, even
*  though I set it's array index to 0. You can change that to
*  whatever you want using a macro, if you wish! */
//unsigned char kbdus[128] =
//{
//    0,  27, '1', '2', '3', '4', '5', '6', '7', '8',	/* 9 */
//  '9', '0', '-', '=', '\b',	/* Backspace */
//  '\t',			/* Tab */
//  'q', 'w', 'e', 'r',	/* 19 */
//  't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',	/* Enter key */
//    0,			/* 29   - Control */
//  'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';',	/* 39 */
// '\'', '`',   0,		/* Left shift */
// '\\', 'z', 'x', 'c', 'v', 'b', 'n',			/* 49 */
//  'm', ',', '.', '/',   0,				/* Right shift */
//  '*',
//    0,	/* Alt */
//  ' ',	/* Space bar */
//    0,	/* Caps lock */
//    0,	/* 59 - F1 key ... > */
//    0,   0,   0,   0,   0,   0,   0,   0,
//    0,	/* < ... F10 */
//    0,	/* 69 - Num lock*/
//    0,	/* Scroll Lock */
//    0,	/* Home key */
//    0,	/* Up Arrow */
//    0,	/* Page Up */
//  '-',
//    0,	/* Left Arrow */
//    0,
//    0,	/* Right Arrow */
//  '+',
//    0,	/* 79 - End key*/
//    0,	/* Down Arrow */
//    0,	/* Page Down */
//    0,	/* Insert Key */
//    0,	/* Delete Key */
//    0,   0,   0,
//    0,	/* F11 Key */
//    0,	/* F12 Key */
//    0,	/* All other keys are undefined */
//};
//

CKeyboard::CKeyboard() {
    m_keyboard = -1;
    m_oldmode = 0;
    m_bEcho = false;
}

CKeyboard::~CKeyboard() {
}

bool CKeyboard::m_Open() {
    HANDLE	hStdin = GetStdHandle(STD_INPUT_HANDLE); 
    HANDLE	hStdout = GetStdHandle(STD_OUTPUT_HANDLE); 
    if (hStdin == INVALID_HANDLE_VALUE || 
        hStdout == INVALID_HANDLE_VALUE) 
    {
        MessageBox(NULL, _T("GetStdHandle"), _T("Console Error"), MB_OK);
        return false;
    }

    return true;
}

bool CKeyboard::m_IsEchoOn() {
    return m_bEcho;
}

bool CKeyboard::m_SetEcho(bool bEcho) {
	HANDLE	hStdin = GetStdHandle(STD_INPUT_HANDLE);
	DWORD	dwMode = 0;
	GetConsoleMode(hStdin,&dwMode);
    if (m_bEcho) {    //default console mode
		dwMode |= ENABLE_ECHO_INPUT;
	} else {    //graphic(echo off) console mode
		dwMode &= (~ENABLE_ECHO_INPUT);
    }
	SetConsoleMode( hStdin,dwMode );
    m_bEcho = bEcho;
    return true;
}

void CKeyboard::m_PressKey(char cKey) {
}

int CKeyboard::m_IsKeyPressed() {
    char	c = _getch();
	return c;
}

