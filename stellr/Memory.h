#pragma once
#include "MemScan.h"
#include <string>

#define readloc(addr) *(uintptr_t*)(addr)

bool is_char_valid(char x) {
	return 0x20 <= x && x <= 0x7E;
}

bool is_string_valid(std::string str) {
	for (int i = 0; i < str.length(); i++)
		if (!is_char_valid(str[i]))
			return false;

	return true;
}

std::string read_string(int addr, bool loc = true) {
	std::string ret = *(std::string*)addr;

	return is_string_valid(ret) ? ret : read_string(readloc(addr));
}

#define mem_add(name, type, offset) \
auto name() { \
	return (type)(this->ptr() + offset); \
} 

#define addptr(name, type, offset) \
auto name() { \
	return type(readloc(this->ptr() + offset)); \
} 

#define addstr(name, offset) \
auto name(bool ptr = false) { \
	return ptr ? read_string((this->ptr() + offset)) : read_string(readloc(this->ptr() + offset)); \
}