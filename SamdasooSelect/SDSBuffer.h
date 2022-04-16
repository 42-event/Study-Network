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
	const T Get() const
	{
		std::array<_ValueType, sizeof(T)> tmp;
		std::memcpy(tmp.data(), this->buf.data(), sizeof(T));
		return std::bit_cast<T>(tmp);
	}

	inline const char* RawGet() const
	{
		const _ValueType* tmp = this->buf.data();
		return reinterpret_cast<const char*>(tmp);
	}

	template<typename T>
	void Put(const T& t)
	{
		std::array<_ValueType, sizeof(T)> tmp = std::bit_cast<decltype(tmp)>(t);
		this->buf.insert(buf.end(), tmp.begin(), tmp.end());
	}

	inline void RawPut(const char* in, int len)
	{
		const _ValueType* tmp = reinterpret_cast<const _ValueType*>(in);
		this->buf.insert(buf.end(), &tmp[0], &tmp[len]);
	}

	inline void Delete(int len)
	{
		this->buf.erase(this->buf.begin(), std::next(this->buf.begin(), len));
	}

	inline std::string Dump() const
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
