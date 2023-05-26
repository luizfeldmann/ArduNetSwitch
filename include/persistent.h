#ifndef _PERSISTENT_H_
#define _PERSISTENT_H_

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

//! Compares the given username to the value stored on EEPROM
//! @return True if match
bool Persist_CheckUserName(const char* pData, unsigned char uLen);

//! Compares the given password to the value stored on EEPROM
//! @return True if match
bool Persist_CheckPassword(const char* pData, unsigned char uLen);

//! Updates the stored user name and password
//! @param szUser Null-terminated string of new user name
//! @param szPass Null-terminated string of new pass word
bool Persist_SetUserNameAndPassword(const char* szUser, const char* szPass);

//! Resets all values to default
void Persist_ResetDefaults();

#ifdef __cplusplus
}
#endif

#endif