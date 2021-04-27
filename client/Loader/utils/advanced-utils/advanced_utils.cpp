/*
			ADVANCED UTILITIES LIBRARY BY FLOWXRC
			MADE FOR SWIFTWARE
*/
#include "advanced_utils.hpp"

namespace utilities
{
	BOOL IsProcessRunning(DWORD pid)
	{
		HANDLE process = OpenProcess(SYNCHRONIZE, FALSE, pid);
		DWORD ret = WaitForSingleObject(process, 0);
		CloseHandle(process);
		return ret == WAIT_TIMEOUT;
	}
	_forceinline std::string get_random_string(size_t length)
	{
		const static std::string chrs = xorstr_("0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ");

		thread_local static std::mt19937 rg{ std::random_device{}() };
		thread_local static std::uniform_int_distribution<std::string::size_type> pick(0, sizeof(chrs) - 2);

		std::string s;

		s.reserve(length);

		while (length--)
			s += chrs[pick(rg)];

		return s;
	}

	__forceinline std::string get_hwid()
	{
		std::string m_sResult;

		HANDLE m_hFile = CreateFileW(L"\\\\.\\PhysicalDrive0", 0, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
		if (m_hFile == INVALID_HANDLE_VALUE)
			return { };

		std::unique_ptr< std::remove_pointer <HANDLE >::type, void(*)(HANDLE) > m_hDevice
		{
			m_hFile, [](HANDLE handle)
			{
				CloseHandle(handle);
			}
		};

		STORAGE_PROPERTY_QUERY m_PropertyQuery;
		m_PropertyQuery.PropertyId = StorageDeviceProperty;
		m_PropertyQuery.QueryType = PropertyStandardQuery;

		STORAGE_DESCRIPTOR_HEADER m_DescHeader;
		DWORD m_dwBytesReturned = 0;
		if (!DeviceIoControl(m_hDevice.get(), IOCTL_STORAGE_QUERY_PROPERTY, &m_PropertyQuery, sizeof(STORAGE_PROPERTY_QUERY),
			&m_DescHeader, sizeof(STORAGE_DESCRIPTOR_HEADER), &m_dwBytesReturned, NULL))
			return { };

		const DWORD m_dwOutBufferSize = m_DescHeader.Size;
		std::unique_ptr< BYTE[] > m_pOutBuffer{ new BYTE[m_dwOutBufferSize] { } };

		if (!DeviceIoControl(m_hDevice.get(), IOCTL_STORAGE_QUERY_PROPERTY, &m_PropertyQuery, sizeof(STORAGE_PROPERTY_QUERY),
			m_pOutBuffer.get(), m_dwOutBufferSize, &m_dwBytesReturned, NULL))
			return { };

		STORAGE_DEVICE_DESCRIPTOR* m_pDeviceDescriptor = reinterpret_cast<STORAGE_DEVICE_DESCRIPTOR*>(m_pOutBuffer.get());
		const DWORD m_dwSerialNumberOffset = m_pDeviceDescriptor->SerialNumberOffset;
		if (m_dwSerialNumberOffset == 0)
			return { };

		m_sResult = reinterpret_cast<const char*>(m_pOutBuffer.get() + m_dwSerialNumberOffset);
		m_sResult.erase(std::remove_if(m_sResult.begin(), m_sResult.end(), std::isspace), m_sResult.end());

		return m_sResult;
	}

	HANDLE GetProcessByName(const char* name)
	{
		HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);

		PROCESSENTRY32 entry;
		entry.dwSize = sizeof(entry);
		HANDLE process;
		ZeroMemory(&process, sizeof(HANDLE));

		do {
			if (!strcmp(name, _bstr_t(entry.szExeFile)))
			{
				process = OpenProcess(PROCESS_ALL_ACCESS, false, entry.th32ProcessID);
				CloseHandle(hSnapshot);
			}
		} while (Process32Next(hSnapshot, &entry));

		return process;
	}
}