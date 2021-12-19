#pragma once
#include "TaskScheduler.h"

namespace RBX {
	class CurrentCamera : public Singleton::Singleton<CurrentCamera> {
	public:
		CurrentCamera() {
			TaskScheduler* taskScheduler = TaskScheduler::GetSingleton();
			if (RBX::Instance DataModel = taskScheduler->FindJobByName("Render").GetDataModel())
				if (RBX::Instance workspace = DataModel.GetPropertyValue<int>("Workspace"))
					this->camera = workspace.GetPropertyValue<int>("CurrentCamera");
		}

		std::pair<RBX::Vector2, float> WorldToViewportPoint(RBX::Vector3 worldPos) {
			std::pair<RBX::Vector2, float> pair;

			DWORD ret = camera.CallBoundFunc("WorldToViewportPoint", worldPos.x, worldPos.y, worldPos.z);
			RBX::Vector3 vec3 = *(RBX::Vector3*)(readloc(ret) + 8);
			pair.first = { vec3.x, vec3.y };
			pair.second = vec3.z; // magnitude from camera

			return pair;
		}

		RBX::Instance GetCamera() {
			return this->camera;
		}

	private:
		RBX::Instance camera;
	};
}