#pragma once

class SDSException : public std::exception
{
public:
    SDSException() = delete;
    inline explicit SDSException(char const* const message) noexcept
        : std::exception(message)
    {

    }
};

class SDSWSException : public SDSException
{
private:
    const int error;

public:
    SDSWSException() = delete;
    inline explicit SDSWSException(int error) noexcept
        : SDSException("Windows Socket Error"), error(error)
    {

    }

    inline int GetError() const
    {
        return error;
    }
};
