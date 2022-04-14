#pragma once

class ISDSWorkUnit
{
public:
	virtual SOCKET GetSocket() const = 0;
	virtual bool IsReadable() const = 0;
	virtual bool IsWritable() const = 0;
	virtual void OnRead() = 0;
	virtual void OnWrite() = 0;
};
