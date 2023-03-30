#include "Server.h"
#include "MemHelp.h"
#include <codecvt>

#pragma comment(lib, "Ws2_32")
#pragma warning(disable :  4996)
//Send Packet to Socket Driver
static bool send_packet(const SOCKET connection, const Packet& packet, uint64_t& out_results)
{
	Packet completion_packet{};
	if (send(connection, (const char*)&packet, sizeof(packet), 0) == SOCKET_ERROR)
	{
		printf("send failed with error: %d\n", WSAGetLastError());
		return false;
	}
	const auto result = recv(connection, (char*)&completion_packet, sizeof(Packet), 0);
	if (result < sizeof(PacketHeader) ||
		completion_packet.header.magic != packet_magic ||
		completion_packet.header.type != PacketType::packet_completed)
		return false;
	out_results = completion_packet.data.completed.result;
	return true;
}
//Copy Memory From Socket Driver
static uint32_t cpymem(
	const SOCKET connection,
	const uint32_t src_process_id,
	const uintptr_t src_address,
	const uint32_t dest_process_id,
	const uintptr_t dest_address,
	const size_t size
)
{
	Packet packet{};
	packet.header.magic = packet_magic;
	packet.header.type = PacketType::packet_copy_memory;
	auto& data = packet.data.copy_memory;
	data.src_process_id = src_process_id;
	data.src_address = uint64_t(src_address);
	data.dest_process_id = dest_process_id;
	data.dest_address = uint64_t(dest_address);
	data.size = uint64_t(size);
	uint64_t result = 0;
	if (send_packet(connection, packet, result))
		return uint32_t(result);
	return 0;
}
//Driver Connection Init
ControlMem::ControlMem() {
	this->initialize();
}
//Unload System
ControlMem::~ControlMem() {
	this->unloadsystem();
}
//Init
void ControlMem::initialize() {
	WSADATA was_data;
	WSAStartup(MAKEWORD(2, 2), &was_data);
}
//Unload System
void ControlMem::unloadsystem() {
	WSACleanup();
}
//Connect To SocketDriver
SOCKET ControlMem::ConnectSocket() {
	SOCKADDR_IN addr{};
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(server_ip);
	addr.sin_port = htons(server_port);
	const auto connection = socket(AF_INET, SOCK_STREAM, 0);
	if (connection == INVALID_SOCKET)
		return INVALID_SOCKET;
	if (connect(connection, (SOCKADDR*)&addr, sizeof(addr)) == SOCKET_ERROR) {
		closesocket(connection);
		return INVALID_SOCKET;
	}
	return connection;
}

void ControlMem::Disconnect(const SOCKET connection) {
	closesocket(connection);
}

//Get Proccess Base Address
uint64_t ControlMem::g_proc_base_addy(const SOCKET connection, const uint32_t proc_id) {
	Packet packet{};
	packet.header.magic = packet_magic;
	packet.header.type = PacketType::packet_get_base_address;
	auto& data = packet.data.get_base_address;
	data.process_id = proc_id;
	uint64_t result = 0;
	if (send_packet(connection, packet, result))
		return result;
	return 0;
}

//Get PEB Address
uint64_t ControlMem::g_proc_peb(SOCKET connection, uint32_t proc_id) {
	Packet packet{};
	packet.header.magic = packet_magic;
	packet.header.type = PacketType::packet_get_peb;
	auto& data = packet.data.get_base_peb;
	data.process_id = proc_id;
	uint64_t result = 0;
	if (send_packet(connection, packet, result))
		return result;
	return 0;
}

//Mem Start
MemController::MemController(ControlMem& mem, SOCKET connection) :mem(mem) {
	stored_connection = connection;
	if (connection == INVALID_SOCKET) {
		printf("Invalid Socket Connection");
		return;
	}
	cached_PID = find_proc_by_id(L"r5apex.exe");
	cached_proc_peb = mem.g_proc_peb(stored_connection, cached_PID);
	cached_proc_base_address = mem.g_proc_base_addy(stored_connection, cached_PID);
	
	//Start Memory Read for Game Offsets
	//ie
	//uint64_t GWorld = this->read<uint64_t>(cached_proc_base_address + 0x1F4F0A8);
	//uint64_t GNames = this->read<uint64_t>(cached_proc_base_address + 0x1F4F0A8);
	//uint64_t GObjects = this->read<uint64_t>(cached_proc_base_address + 0x1F4F0A8);
	//Print Out in console if needed
	//printf("GWorld: %p", GWorld);
	//printf("GNames: %p", GNames);
	//printf("GObjects: %p", GObjects);
}

//Read Memory
uint32_t MemController::readbuffer(uint64_t address, PVOID buffer, size_t size) {
	if (address == 0)
		return false;
	return cpymem(stored_connection, cached_PID, address, GetCurrentProcessId(), uint64_t(buffer), size);
}
//Read Chain
uint64_t MemController::ReadChainMem(uint64_t base, const std::vector<uint64_t>& offsets) {
	uint64_t result = Read<uint64_t>(base + offsets.at(0));
	for (auto i = 1; i < offsets.size(); i++)
		result = Read<uint64_t>(result + offsets.at(i));
	return result;
}

//Get Unicode String
std::string MemController::GetUniString(uint64_t addr, int stringlength) {
	char16_t wcharTem[64] = { '\0' };
	readbuffer(addr, wcharTem, stringlength * 2);
	std::string uni_conversion = std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t>().to_bytes(wcharTem);
	return uni_conversion;{}
}

//Find Proc by ID
uint64_t MemController::find_proc_by_id(const std::wstring& name) {
	const auto snap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (snap == INVALID_HANDLE_VALUE)
		return 0;
	PROCESSENTRY32W entry{};
	entry.dwSize = sizeof(entry);
	auto found = false;
	if (!!Process32First(snap, &entry)) {
		do {
			if (name == entry.szExeFile) {
				found = true;
				break;
			}
		} while (!!Process32Next(snap, &entry));
	}
	CloseHandle(snap);
	return found
	? entry.th32ProcessID
		: 0;

}
//TODO WriteBuffer&&Write