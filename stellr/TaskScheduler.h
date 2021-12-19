#pragma once
#include "rblx.h"

namespace RBX {
	class TaskScheduler : public ClassHandler, public Singleton::Singleton<TaskScheduler> {
	public:
		TaskScheduler() : ClassHandler() { this->Initialse(); }
		class Job : public ClassHandler {
		public:
			Job(uintptr_t p) : ClassHandler(p) { vtable = Hooks::HookVTable::hook(this->ptr(), 10); }
			std::string Name() {
				return read_string(this->ptr() + 0x10);
			}
			RBX::Instance GetDataModel() {
				return readloc(this->ptr() + 0x28) + 0x4;
			}

			Hooks::HookVTable::hook_data vtable;
		};

		std::vector<Job> GetJobs() {
			std::vector<Job> Jobs;

			for (uintptr_t p = readloc(this->ptr() + 0x12C); p != readloc(this->ptr() + 0x130); p += 8)
				Jobs.emplace_back(readloc(p));

			return Jobs;
		}

		Job FindJobByName(std::string name) {
			for (Job job : this->GetJobs())
				if (!job.Name().compare(name)) return job;
		}

		void Initialse() {
			if (this->ptr()) return;
			int link_func = memscan::scan("535657E8????????8BD8E8????????")[0]; // this is the function directly after GetTaskScheduler !

			// link_func contains 2 consecutive calls to TaskScheduler::singleton()
			for (int i = 0; i < 50; i++) {
				if (*(BYTE*)(link_func + i) == 0xE8) {
					uintptr_t ptr = link_func + i + 1;
					uintptr_t val = readloc(ptr);

					uintptr_t TaskScheduler_Singleton = ptr + val + sizeof uintptr_t;
					//printf("%p\n", TaskScheduler_Singleton);
					this->set(reinterpret_cast<int(__cdecl*)()>(TaskScheduler_Singleton)());

					printf("TaskScheduler: %p\n", this->ptr());
					break;
				}
			}
		}
	};
}