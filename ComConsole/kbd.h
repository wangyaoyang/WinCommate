/* 
 * File:   kbd.h
 * Author: root
 *
 * Created on 2009��12��23��, ����9:13
 */

#ifndef _KBD_H
#define	_KBD_H

#ifdef	__cplusplus
extern "C" {
#endif

#ifdef  __ARM_ARCH_3__
#define RELEASE_ON_ARM
#endif  //__ARM_ARCH_3__

class CKeyboard
{
private:
    int             m_keyboard;
    int             m_oldmode;
    bool            m_bEcho;
public:
	CKeyboard();
	~CKeyboard();
	bool	m_Open();
	int	m_IsKeyPressed();
        void    m_PressKey(char cKey);
        bool    m_SetEcho(bool bEcho);
        bool    m_IsEchoOn();
};



#ifdef	__cplusplus
}
#endif

#endif	/* _KBD_H */

