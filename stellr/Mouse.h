#pragma once
#include "TaskScheduler.h"

namespace RBX {
	class Mouse : public Singleton::Singleton<Mouse> {
	public: // work-around for rblx crashing over dumb GetMouse calls etc... will have to eventually figure this out
		Mouse() {
			TaskScheduler* taskScheduler = TaskScheduler::GetSingleton();
			if (RBX::Instance DataModel = taskScheduler->FindJobByName("Render").GetDataModel())
				if (RBX::Instance Players = DataModel.FindFirstChildOfClass("Players"))
					if (RBX::Instance LocalPlayer = Players.GetPropertyValue<int>("LocalPlayer"))
						this->mouse = LocalPlayer.CallBoundFunc("GetMouse");
		}

		RBX::Instance GetMouse() {
			return this->mouse;
		}

	private:
		RBX::Instance mouse;
	};
}