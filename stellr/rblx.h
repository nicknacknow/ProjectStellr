#pragma once
#include "globals.h"

#include "CoordinateFrame.h"
#include "Vector2.h"

namespace RBX {
	typedef g3dimpl::Vector2 Vector2;
	typedef g3dimpl::Vector3 Vector3;
	typedef g3dimpl::Matrix3 Matrix3;
	typedef g3dimpl::CoordinateFrame CoordinateFrame;
}

class ClassHandler {
public:
	ClassHandler() {}
	~ClassHandler() { instance = NULL; }
	ClassHandler(void* p) : instance(reinterpret_cast<uintptr_t>(p)) {}
	ClassHandler(uintptr_t p) : instance(p) {}

	ClassHandler(const ClassHandler& n) : instance(n.instance) {}

	uintptr_t operator+(int a) { return instance + static_cast<uintptr_t>(a); }
	uintptr_t operator-(int a) { return instance - static_cast<uintptr_t>(a); }

	bool operator==(const ClassHandler& n) {
		return n.instance == this->instance;
	}

	operator bool() {
		return !!this->ptr();
	}

	uintptr_t ptr() { return instance; }
	void set(uintptr_t p) { instance = p; }
private:
	uintptr_t instance = NULL;
};

namespace RBX {
	namespace Reflection {
		class GetSet : public ClassHandler {
		public:
			GetSet() : ClassHandler() {}
			GetSet(uintptr_t p) : ClassHandler(readloc(p)) {
				vtable = Hooks::HookVTable::hook(this->ptr(), 6);
				this->GetFunc = vtable.get_original_func(3);
				this->SetFunc = vtable.get_original_func(4);
			}

			uintptr_t GetFunc;
			uintptr_t SetFunc;
		private:
			Hooks::HookVTable::hook_data vtable;
		};

		class Descriptor : public ClassHandler {
		public:
			Descriptor() {}
			Descriptor(uintptr_t p) : ClassHandler(readloc(p)) { vtable = Hooks::HookVTable::hook(this->ptr(), 10); }

			addstr(Name, 0x4);
			addstr(Category, 0x18);

			Hooks::HookVTable::hook_data vtable;
		};

		class PropertyDescriptor : public Descriptor {
		public:
			PropertyDescriptor(uintptr_t p) : Descriptor(p) {
				getset = GetSet(this->ptr() + *(BYTE*)(vtable.get_original_func(2) + 2));
			}
			
			mem_add(IsEnum, bool, 0x20);

			template<class ret, class... T>
			ret callGet(T... arg) {
				return reinterpret_cast<ret(__thiscall*)(uintptr_t, T...)>(this->getGetSet().GetFunc)(this->getGetSet().ptr(), arg...);
			}

			template<class... T>
			void callSet(T... arg) {
				reinterpret_cast<void(__thiscall*)(uintptr_t, T...)>(this->getGetSet().SetFunc)(this->getGetSet().ptr(), arg...);
			}

			GetSet getGetSet() { return this->getset; }

			std::string get_type() {
				uintptr_t type = readloc(this->ptr() + 0x28);
				return read_string(type + 0x4);
			}
		private:
			GetSet getset;
		};

		class BoundFuncDescriptor : public Descriptor {
		public:
			BoundFuncDescriptor() : Descriptor() {}
			BoundFuncDescriptor(uintptr_t p) : Descriptor(p) { CallFunc = readloc(this->ptr() + 0x40); }

			addptr(Func, uintptr_t, 0x40);

			template<class... T>
			uintptr_t Call(uintptr_t obj, T... arg) {
				return reinterpret_cast<uintptr_t(__thiscall*)(uintptr_t, T...)>(CallFunc)(obj, arg...);
			}
		private:
			uintptr_t CallFunc;
		};

		class EventDescDescriptor : public Descriptor {
		public:
			EventDescDescriptor() : Descriptor() {}
			EventDescDescriptor(uintptr_t p) : Descriptor(p) {}
		};

		class ClassDescriptor : public ClassHandler {
		public:
			ClassDescriptor(uintptr_t p) : ClassHandler((p)) {}
			addstr(ClassName, 0x4);

			std::vector<PropertyDescriptor> GetPropertyDescriptors() {
				std::vector<PropertyDescriptor> PropertyDescriptors{};

				for (uintptr_t i = readloc(this->ptr() + 0x18); i != readloc(this->ptr() + 0x1C); i += 4)
					PropertyDescriptors.emplace_back(i);

				return PropertyDescriptors;
			}

			std::vector<BoundFuncDescriptor> GetBoundFuncDescriptors() {
				std::vector<BoundFuncDescriptor> BoundFuncDescriptors{};

				for (uintptr_t i = readloc(this->ptr() + 0xD8); i != readloc(this->ptr() + 0xDC); i += 4)
					BoundFuncDescriptors.emplace_back(i);

				return BoundFuncDescriptors;
			}

			std::vector<EventDescDescriptor> GetEventDescDescriptors() {
				std::vector<EventDescDescriptor> EventDescDescriptors{};

				for (uintptr_t i = readloc(this->ptr() + 0x78); i != readloc(this->ptr() + 0x7C); i += 4)
					EventDescDescriptors.emplace_back(i);

				return EventDescDescriptors;
			}

			PropertyDescriptor FindPropertyDescriptor(std::string str) {
				for (PropertyDescriptor pd : this->GetPropertyDescriptors())
					if (!pd.Name().compare(str)) return pd;
			}

			BoundFuncDescriptor FindBoundFuncDescriptor(std::string str) {
				for (BoundFuncDescriptor bfd : this->GetBoundFuncDescriptors())
					if (!bfd.Name().compare(str)) return bfd;
			}

			EventDescDescriptor FindEventDescDescriptor(std::string str) {
				for (EventDescDescriptor bfd : this->GetEventDescDescriptors())
					if (!bfd.Name().compare(str)) return bfd;
			}
		};
	}

	class Instance : public ClassHandler {
	public:
		Instance() : ClassHandler() {}
		Instance(uintptr_t p) : ClassHandler(p) {}

		mem_add(Archivable, bool, 0x21);
		mem_add(Locked, bool, 0xE5);
		mem_add(CastShadow, bool, 0xCB);
		mem_add(Transparency, float, 0xCC);
		mem_add(Reflectance, float, 0xD0);

		addptr(Parent, Instance, 0x34);
		addptr(Ref, uintptr_t, 0x8);

		addstr(Name, 0x28);

		Reflection::ClassDescriptor GetClassDescriptor() { return Reflection::ClassDescriptor(readloc(this->ptr() + 0xC)); }

		RBX::Reflection::PropertyDescriptor FindPropertyDescriptor(std::string name) {
			return this->GetClassDescriptor().FindPropertyDescriptor(name);
		}

		RBX::Reflection::BoundFuncDescriptor FindBoundFuncDescriptor(std::string name) {
			return this->GetClassDescriptor().FindBoundFuncDescriptor(name);
		}

		RBX::Reflection::EventDescDescriptor FindEventDescDescriptor(std::string name) {
			return this->GetClassDescriptor().FindEventDescDescriptor(name);
		}

		template<class T>
		T GetPropertyValue(std::string name) {
			RBX::Reflection::PropertyDescriptor PropertyDescriptor = this->FindPropertyDescriptor(name);
			return PropertyDescriptor.callGet<T>(this->ptr());
		}

		template<class ret >
		ret GetCustomPropertyValue(std::string name) { // used for custom roblox variables, such as Vector3
			RBX::Reflection::PropertyDescriptor prop_descriptor = this->FindPropertyDescriptor(name);

			static ret val;
			reinterpret_cast<void(__thiscall*)(uintptr_t, ret*, int)>(prop_descriptor.getGetSet().GetFunc)(prop_descriptor.getGetSet().ptr(), &val, this->ptr());
			return val;
		}

		template<class T>
		void SetCustomPropertyValue(std::string name, T val) {
			RBX::Reflection::PropertyDescriptor prop_descriptor = this->FindPropertyDescriptor(name);
			reinterpret_cast<void(__thiscall*)(uintptr_t, uintptr_t, T*)>(prop_descriptor.getGetSet().SetFunc)(prop_descriptor.getGetSet().ptr(), this->ptr(), &val);
		}

		template<class T>
		void SetPropertyValue(std::string name, T val) {
			RBX::Reflection::PropertyDescriptor PropertyDescriptor = this->FindPropertyDescriptor(name);
			PropertyDescriptor.callSet(this->ptr(), &val);
		}

		template <class... T>
		uintptr_t CallBoundFunc(std::string name, T... args) {
			RBX::Reflection::BoundFuncDescriptor Descriptor = this->FindBoundFuncDescriptor(name);

			static int callr;
			//printf("ok\n");
			Descriptor.Call(this->ptr(), &callr, args...);
			return callr;
		}

		std::vector<Instance> GetChildren() {
			std::vector<Instance> Children;

			uintptr_t pChildArray = readloc(this->ptr() + 0x2C);
			uintptr_t pEndArray = readloc(pChildArray + 0x4);

			for (uintptr_t p = readloc(pChildArray); p != pEndArray; p += 8) {
				Children.emplace_back(readloc(p));
			}

			return Children;
		}

		Instance FindFirstChild(std::string childname) {
			for (Instance child : this->GetChildren())
				if (child.Name() == childname)
					return child;
			return NULL;
		}

		Instance FindFirstChildOfClass(std::string classname) {
			for (Instance child : this->GetChildren())
				if (child.GetClassDescriptor().ClassName() == classname)
					return child;
			return NULL;
		}
	};
}