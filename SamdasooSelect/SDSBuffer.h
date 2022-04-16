#pragma once

class SDSBuffer
{
public:
	typedef std::byte _ValueType;

private:
	std::vector<_ValueType> buf;

public:
	inline SDSBuffer() = default;

	inline int GetCount() const
	{
		return static_cast<int>(this->buf.size());
	}

	template<typename T>
	const T Get()
	{
		std::array<_ValueType, sizeof(T)> tmp;
		std::memcpy(tmp.data(), this->buf.data(), sizeof(T));
		return std::bit_cast<T>(tmp);
	}

	inline char* RawGet()
	{
		_ValueType* tmp = this->buf.data();
		return reinterpret_cast<char*>(tmp);
	}

	template<typename T>
	void Put(const T& t)
	{
		std::array<_ValueType, sizeof(T)> tmp = std::bit_cast<decltype(tmp)>(t);
		this->buf.insert(buf.end(), tmp.begin(), tmp.end());
	}

	inline void RawPut(char* in, int len)
	{
		_ValueType* tmp = reinterpret_cast<_ValueType*>(in);
		this->buf.insert(buf.end(), &tmp[0], &tmp[len]);
	}

	inline void Delete(int len)
	{
		this->buf.erase(this->buf.begin(), std::next(this->buf.begin(), len));
	}

	inline std::string Dump()
	{
		bool first = true;
		std::ostringstream result;
		result << "Data=[";
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
