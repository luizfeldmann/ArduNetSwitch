#include "persistent.h"
#include <string.h>
#include <avr/eeprom.h>

//! Contains the pair of user/pass require to manage this device
struct SCredentials
{
    //! The stored user ID
    char szUserName[32];
    //! The storeed password
    char szPassword[32];
};

//! Pointer in EEPROM of the persistent user credentials
struct SCredentials EEMEM eep_stCredentials;

//! Configuration regarding the initial state of the switch
bool EEMEM eep_bInitialState;

static bool eep_strcmp(const char* szEEP, const char* pData, unsigned char uDataLen)
{
    const char* const pEnd = pData + uDataLen;

    for (;; ++szEEP, ++pData)
    {
        char cheep = eeprom_read_byte(szEEP);

        if (cheep == '\0')
        {
            // The strings are equal size
            if (pData == pEnd)
                return true;

            // The EEP string is shorter
            return false; 
        }

        // The EEP string is longer
        if (pData == pEnd)
            return false;
        
        // The contents are different
        if (*pData != cheep)
            return false;
    }
}

bool Persist_CheckUserName(const char* pData, unsigned char uLen)
{
    return eep_strcmp(eep_stCredentials.szUserName, pData, uLen);
}

bool Persist_CheckPassword(const char* pData, unsigned char uLen)
{
    return eep_strcmp(eep_stCredentials.szPassword, pData, uLen);
}

bool Persist_SetUserNameAndPassword(const char* szUser, const char* szPass)
{
    // Validate user
    unsigned char uLenUser = strlen(szUser);

    if (!uLenUser)
        return false; // Empty

    if (uLenUser + 1 >= sizeof(eep_stCredentials.szUserName))
        return false; // Overflow

    // Validate pass
    unsigned char uLenPass = strlen(szPass);

    if (!uLenPass)
        return false; // Empty

    if (uLenPass + 1 >= sizeof(eep_stCredentials.szPassword))
        return false; // Overflow

    // Save to EEP (include the '/0')
    eeprom_update_block(szUser, eep_stCredentials.szUserName, uLenUser + 1);
    eeprom_update_block(szPass, eep_stCredentials.szPassword, uLenPass + 1);

    return true;
}

bool Persist_GetSwitchInitialState()
{
    return eeprom_read_byte(&eep_bInitialState);
}

void Persist_SetSwitchInitialState(bool b)
{
    eeprom_update_byte(&eep_bInitialState, b);
}

void Persist_ResetDefaults()
{
    static const char c_szDefCredentials[] = "admin";
    eeprom_update_block(c_szDefCredentials, eep_stCredentials.szUserName, sizeof(c_szDefCredentials));
    eeprom_update_block(c_szDefCredentials, eep_stCredentials.szPassword, sizeof(c_szDefCredentials));
}