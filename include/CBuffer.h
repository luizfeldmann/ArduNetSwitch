#ifndef _CBUFFER_H_
#define _CBUFFER_H_

#include <Client.h>

//! Manages a static memory buffer
class CBuffer
{
private:
    //! Pointer to the underlying buffer
    char* const m_pData;

    //! Pointer to after the end of the data
    char* m_pEnd;

    //! Total capacity of the buffer
    const size_t m_uCapacity;

public:
    CBuffer(char* pData, size_t uCapacity);

    //! Iterator to the beginning of the collection
    char* begin();

    //! Const-Iterator to the beginning
    const char* cbegin() const;

    //! Iterator to the end of the collection
    char* end();

    //! Const-Iterator to the end of the collection
    const char* cend() const;

    //! Reads the current size of the collection
    size_t size() const;

    //! Reads the total capacity of the collection
    size_t capacity() const;

    //! Resets the buffer
    void clear();

    //! Checks if the buffer is empty
    bool empty() const;

    //! Appends to the end
    //! @return True if success; False if overflow
    bool push_back(char);

    //! Copies into the buffer
    //! @return True if success
    bool push_back(const char*, size_t);

    //! Copies available data into the buffer
    //! True if success
    bool push_back(Client& Src);
};

#endif