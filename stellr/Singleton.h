#pragma once

namespace Singleton {
	template <class T>
	class Singleton {
	public:
		static T* GetSingleton() {
			static T* self;
			if (!self) self = new T();
			return self;
		}
	};
}