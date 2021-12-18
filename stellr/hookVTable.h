#pragma once
#include "globals.h"
#include <vector>

namespace Hooks {
	class HookVTable : public Singleton::Singleton<HookVTable> {
	public:
		class hook_data {
		public:
			int size;
			uintptr_t* original;
			uintptr_t base;

			void add_hook(uintptr_t func, int index, uintptr_t* orig) {
				uintptr_t vtable = *(uintptr_t*)base;

				DWORD old;
				VirtualProtect((LPVOID)(vtable + index * sizeof uintptr_t), 4, PAGE_READWRITE, &old);
				*(uintptr_t*)(vtable + index * sizeof uintptr_t) = func;
				VirtualProtect((LPVOID)(vtable + index * sizeof uintptr_t), 4, old, &old);

				*orig = get_original_func(index);
			}

			uintptr_t get_original_func(int index) {
				return original[index];
			}
		};

		static hook_data hook(uintptr_t base, int size) {
			hook_data info;
			info.base = base;
			info.size = size;

			uintptr_t* orig = new uintptr_t[size];
			memcpy(orig, (void*)(*(uintptr_t*)base), size * sizeof uintptr_t);
			info.original = orig;

			return info;
		}
	};
}