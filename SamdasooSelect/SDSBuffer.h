#pragma once

class SDSBuffer
{
public:
	typedef std::byte _ValueType;
	typedef std::vector<_ValueType> _ContainerType;
	typedef _ContainerType::size_type _SizeType;

private:
	_ContainerType buf;
	_SizeType readerPos;
	_SizeType writerPos;

public:
	inline SDSBuffer()
		: buf(), readerPos(), writerPos()
	{

	}

	inline _SizeType GetReaderPosition() const
	{
		return this->readerPos;
	}

	inline void SetReaderPosition(_SizeType readerPos)
	{
		this->readerPos = readerPos;
	}

	inline _SizeType GetWriterPosition() const
	{
		return this->writerPos;
	}

	inline void SetWriterPosition(_SizeType writerPos)
	{
		this->writerPos = writerPos;
	}

	template<typename T>
	const T Get()
	{
		std::array<_ValueType, sizeof(T)> tmp;
		std::memcpy(tmp.data(), &this->buf[this->readerPos], sizeof(T));
		this->readerPos += sizeof(T);
		return std::bit_cast<T>(tmp);
	}

	inline _SizeType Raw(char*& out)
	{
		out = reinterpret_cast<char*>(&this->buf[this->readerPos]);
		return this->writerPos - this->readerPos;
	}

	template<typename T>
	void Put(const T& t, _SizeType n = sizeof(T))
	{
		std::array<_ValueType, sizeof(T)> tmp = std::bit_cast<decltype(tmp)>(t);
		this->buf.insert(std::next(this->buf.begin(), this->writerPos), tmp.begin(), std::next(tmp.begin(), n));
		this->writerPos += n;
	}

	inline void Delete(_SizeType len)
	{
		this->buf.erase(this->buf.begin(), std::next(this->buf.begin(), len));
		this->readerPos -= len;
		this->writerPos -= len;
	}

	inline std::string Dump()
	{
		bool first = true;
		std::ostringstream result;
		result << std::format("RPos={0}; WPos={1}; Data=[", this->readerPos, this->writerPos);
		for (auto x : this->buf)
		{
			if (!first)
			{
				result << " ";
			}
			result << std::format("{:02X}", static_cast<unsigned long>(x));
			first = false;
		}
		result << "]";
		return result.str();
	}
};
