#pragma once

#if 1
enum FileMode : uint8
{
	Write,
	Read,
};

class FileUtils
{
public:
	FileUtils();
	~FileUtils();

	void Open(wstring filePath, FileMode mode);

	template<typename T>
	void Write(const T& data)
	{
		DWORD numOfBytes = 0;
		assert(::WriteFile(_handle, &data, sizeof(T), (LPDWORD)&numOfBytes, nullptr));
	}

	template<> // ※ template<> : template<typename T> (위)에서 제외되는 특수 타입임을 지정 (여기서는 string)
	void Write<string>(const string& data)
	{
		return Write(data);
	}

	void Write(void* data, uint32 dataSize);
	void Write(const string& data);

	template<typename T>
	void Read(OUT T& data)
	{
		DWORD numOfBytes = 0;
		assert(::ReadFile(_handle, &data, sizeof(T), (LPDWORD)&numOfBytes, nullptr));
	}

	template<typename T>
	T Read()
	{
		T data;
		Read(data);
		return data;
	}

	void Read(void* data, uint32 dataSize);

	void Read(OUT string& data);
	/* ※ Write 에서는 템플릿 특수화 template<> 를 사용한 반면, 여기서는 사용하지 않았다. Read에서는 오버로드 
		  사용. 우선순위는 오버로드 > 특수화 (template<>) > 일반템플릿 template<typename T>  */

private:
	HANDLE _handle = INVALID_HANDLE_VALUE;
};
#endif















































































#if 0 // Second
enum FileMode : uint8
{
	Write,
	Read,
};

class FileUtils
{
public:
	FileUtils();
	~FileUtils();

	void Open(wstring filePath, FileMode mode);

	template<typename T>
	void Write(const T& data)
	{
		DWORD numOfBytes = 0;
		assert(::WriteFile(_handle, &data, sizeof(T), (LPDWORD)&numOfBytes, nullptr));
	}

	template<>
	void Write<string>(const string& data)
	{
		return Write(data);
	}

	void Write(void* data, uint32 dataSize);
	void Write(const string& data);

	template<typename T>
	void Read(OUT T& data)
	{
		DWORD numOfBytes = 0;
		assert(::ReadFile(_handle, &data, sizeof(T), (LPDWORD)&numOfBytes, nullptr));
	}

	template<typename T>
	T Read()
	{
		T data;
		Read(data);
		return data;
	}

	void Read(void* data, uint32 dataSize);

	void Read(OUT string& data);


private:
	HANDLE _handle = INVALID_HANDLE_VALUE;
};
#endif // Second