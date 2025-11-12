#include "pch.h"
#include "FileUtils.h"

#if 1
FileUtils::FileUtils()
{
}

FileUtils::~FileUtils()
{
	if (_handle != INVALID_HANDLE_VALUE)
	{
		::CloseHandle(_handle);
		_handle = INVALID_HANDLE_VALUE;
	}
}


void FileUtils::Open(wstring filePath, FileMode mode)
{
	if (mode == FileMode::Write)
	{
		_handle = ::CreateFile(
			filePath.c_str(),
			GENERIC_WRITE,
			0,
			nullptr,
			CREATE_ALWAYS,
			FILE_ATTRIBUTE_NORMAL,
			nullptr
		);
	}
	else
	{
		_handle = ::CreateFile
		(
			filePath.c_str(),
			GENERIC_READ,
			FILE_SHARE_READ,
			nullptr,
			OPEN_EXISTING,
			FILE_ATTRIBUTE_NORMAL,
			nullptr
		);
	}

	assert(_handle != INVALID_HANDLE_VALUE);



	/*	○ ::CreateFile
		 - (1) : 파일 경로
		 - (2) : GENERIC_READ , GENERIC_WRITE, GENERIC_READ | GENERIC_WRITE -> 읽기, 쓰기, 읽기 | 쓰기 모드 설정
		 - (3) : 파일에 어떤 권한으로 접근할지를 지정. 0일시, 공유 안함(다른 프로세스가 접근 불가), FILE_SHARE_READ 일시,
				 다른 프로스세에서 읽기 권한으로 접근할 수 있음
		 - (4) : 고정 nullptr 로 외우자
		 - (5) : 파일 생성/열기 방식. 플래그들 이름 그대로
		 - (6) : FILE_ATTIRBUTE_NORMAL 고정이라 외우자
		 - (7) : 고정 nullptr 로 외우자		*/
}


void FileUtils::Write(void* data, uint32 dataSize)
{
	uint32 numOfBytes = 0;
	assert(::WriteFile(_handle, data, dataSize, reinterpret_cast<LPDWORD>(&numOfBytes), nullptr));
}

void FileUtils::Write(const string& data)
{
	uint32 size = (uint32)data.size();
	Write(size);

	if (data.size() == 0)
		return;

	Write((void*)data.data(), size);
}

//void FileUtils::Read(void** data, uint32 dataSize)
//{
//	uint32 numOfBytes = 0;
//	assert(::ReadFile(_handle, *data, dataSize, reinterpret_cast<LPDWORD>(&numOfBytes), nullptr));
//}

void FileUtils::Read(void* data, uint32 dataSize)
{
	uint32 numOfBytes = 0;
	assert(::ReadFile(_handle, data, dataSize, reinterpret_cast<LPDWORD>(&numOfBytes), nullptr));
}

void FileUtils::Read(OUT string& data)
{
	uint32 size = Read<uint32>();

	if (size == 0)
		return;

	char* temp = new char[size + 1];
	temp[size] = 0;
	//Read((void**)&temp, size); // ※ 기존 강의 코드 함수 호출. 실수 추정
	Read((void*)temp, size);
	data = temp;

	delete[] temp;
}
#endif 



















































































#if 0 // Second
FileUtils::FileUtils() {}
FileUtils::~FileUtils()
{
	if (_handle != INVALID_HANDLE_VALUE)
	{
		::CloseHandle(_handle);
		_handle = INVALID_HANDLE_VALUE;
	}
}

void FileUtils::Open(wstring filePath, FileMode mode)
{
	if (mode == FileMode::Write)
	{
		_handle = ::CreateFile(
			filePath.c_str(),
			GENERIC_WRITE,
			0,
			nullptr,
			CREATE_ALWAYS,
			FILE_ATTRIBUTE_NORMAL,
			nullptr
		);
	}
	else
	{
		_handle = ::CreateFile(
			filePath.c_str(),
			GENERIC_READ,
			FILE_SHARE_READ,
			nullptr,
			OPEN_EXISTING,
			FILE_ATTRIBUTE_NORMAL,
			nullptr
		);
	}

	assert(_handle != INVALID_HANDLE_VALUE);
}


void FileUtils::Write(void* data, uint32 dataSize)
{
	uint32 numOfBytes = 0;
	assert(::WriteFile(_handle, data, dataSize, reinterpret_cast<LPDWORD>(&numOfBytes), nullptr));
}

void FileUtils::Write(const string& data)
{
	uint32 size = (uint32)data.size();
	Write(size);

	if (data.size() == 0) return;

	Write((void*)data.data(), size);
}


void FileUtils::Read(void* data, uint32 dataSize)
{
	uint32 numOfBytes = 0;
	assert(::ReadFile(_handle, data, dataSize, reinterpret_cast<LPDWORD>(&numOfBytes), nullptr));
}


void FileUtils::Read(OUT string& data)
{
	uint32 size = Read<uint32>();

	if (size == 0) return;

	char* temp = new char[size + 1];
	temp[size] = 0;

	Read((void*)temp, size);
	data = temp;
	
	delete[] temp;
}
#endif // Second




