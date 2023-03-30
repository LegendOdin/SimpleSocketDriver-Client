#pragma once
#include <Windows.h>
#include <stdint.h>
#include <time.h>
#include <string>
#include <Psapi.h>
#include <TlHelp32.h>
#include <functional>
#include "Server.h"
#include <vector>
#include <iostream>
class ControlMem {
public:
	ControlMem();
	~ControlMem();
	void GetProcessList();
	void GetProcessList(std::function<void(DWORD, const std::wstring&)> callback);
	void initialize();
	void unloadsystem();

	SOCKET ConnectSocket();
	void Disconnect(SOCKET connection);
	void SendPacket(PacketType type, const void* data, size_t size);
	uint64_t g_proc_base_addy(SOCKET connection, uint32_t proc_id);
	uint64_t g_proc_peb(SOCKET connection, uint32_t proc_id);
	void cpy_mem(SOCKET connection, uint32_t dest_proc_id, uint64_t dest_address, uint32_t src_proc_id, uint64_t src_address, uint32_t size);
	void write_mem(SOCKET connection, uint32_t dest_proc_id, uint64_t dest_address, uint32_t src_proc_id, uint64_t src_address, uint32_t size);
	void read_mem(SOCKET connection, uint32_t dest_proc_id, uint64_t dest_address, uint32_t src_proc_id, uint64_t src_address, uint32_t size);

};

class MemController;
class MemController
{
	MemController(ControlMem& mem, SOCKET connection);
	uint32_t readbuffer(uint64_t address, PVOID buffer, size_t size);
	uint64_t ReadChainMem(uint64_t base, const std::vector<uint64_t>& offsets);
	std::string GetUniString(uint64_t addr, int stringLength);
	template <typename T>
	T Read(uint64_t address);
	template <typename T>
	T Write(uint64_t address, T value);
	uint64_t get_proc_base() const { return cached_proc_base_address; };
	uint64_t get_proc_peb() const { return cached_proc_peb; };
	uint64_t find_proc_by_id(const std::wstring& name);

private:
	uint64_t cached_proc_base_address = 0, cached_PID = 0, cached_proc_peb = 0;
	SOCKET stored_connection;
	ControlMem& mem;
};
template<typename T>
inline T MemController::Read(uint64_t address)
{
	T buffer;
	readbuffer(address, &buffer, sizeof(T));
	return buffer;
}
extern MemController* g_mem_controller;