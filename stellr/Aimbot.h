#pragma once
#include "globals.h"
#include "TaskScheduler.h"
#include "CurrentCamera.h"
#include "Mouse.h"

#include "CheckHelper.h"

namespace Exploit {
	namespace Aimbot {

		const char* aimPart = "Head";
		bool aimbotMethod = true;
		bool WallCheck = true;
		bool Smoothing = true;

		class Aimbot : public Cheat::Cheat, public CheckHelper, public Singleton::Singleton<Aimbot> {
		private:
			struct RaycastResult {
				RBX::Vector3 Position; // The world space point at which the intersection occurred, usually a point directly on the surface of the instance.
				RBX::Vector3 Normal; // The normal vector of the intersected face.
				uintptr_t Material; // The Material at the intersection point. For normal parts this is the BasePart.Material; for Terrain this can vary depending on terrain data.
				float noclue;
				RBX::Instance Instance; // The BasePart or Terrain cell that the ray intersected.
				uintptr_t afk; // thingy of instance
			};

			struct RaycastParams
			{
				char pad_0x0000[0x1]; //0x0000
				bool IgnoreWater = false; //0x0001 
				BYTE dk1 = 0x53; //0x0002 2 n 3 r just random bytes
				BYTE dk2 = 0x12; //0x0003 
				bool WhitelistType = false; //0x0004  Blacklist (0) Whitelist (1)
				char pad_0x0005[0x3]; //0x0005
				DWORD FilterDescendantsInstances; //0x0008 
				char pad_0x000C[0x4]; //0x000C
				std::string CollisionGroup = std::string("Default"); 
			}; //Size=0x0014
		public:
			Aimbot() { initialise(); }

			bool static GetClosestPlayerToMouse(RBX::Instance Players, RBX::Instance& target) {
				static RBX::Instance workspace;  if (!workspace.ptr()) workspace = Players.Parent().GetPropertyValue<int>("Workspace");
				static RBX::Reflection::BoundFuncDescriptor Raycast; if (!Raycast.ptr()) Raycast = workspace.FindBoundFuncDescriptor("Raycast");

				static RBX::Mouse* mouseClass = RBX::Mouse::GetSingleton();
				static RBX::Instance mouse = mouseClass->GetMouse(); if (!mouse) return false;
				static RBX::CurrentCamera* CurrentCamera = RBX::CurrentCamera::GetSingleton();
				static RBX::Instance camera = CurrentCamera->GetCamera(); if (!camera) return false;

				static RBX::Vector2 mousePos{ (float)mouse.GetPropertyValue<int>("X"), (float)mouse.GetPropertyValue<int>("Y") };

				static RBX::Instance LocalPlayer = Players.GetPropertyValue<int>("LocalPlayer"); if (!LocalPlayer.ptr()) return false; 
				RBX::Instance LocalCharacter = LocalPlayer.GetPropertyValue<int>("Character"); if (!LocalCharacter.ptr())return false;
				RBX::Instance LocalOriginPart = LocalCharacter.FindFirstChild(aimPart); if (!LocalOriginPart.ptr()) LocalOriginPart = LocalCharacter.GetPropertyValue<int>("PrimaryPart"); if (!LocalOriginPart)return false;
				static RBX::Vector3  LocalOrigin = LocalOriginPart.GetCustomPropertyValue<RBX::Vector3>("Position");

				std::pair<float, RBX::Instance> best = { 9e9f, RBX::Instance() };
				float closest_mag = 9e9f;

				for (RBX::Instance player : Players.GetChildren()) {
					if (!(player.ptr() && player.GetClassDescriptor().ClassName() == "Player")) continue;
					if (RBX::Instance character = player.GetPropertyValue<int>("Character")) {
						if (RBX::Instance target = character.FindFirstChild("Head")) {
							if (!DoChecks(player)) continue;
							RBX::Vector3 position = target.GetCustomPropertyValue<RBX::Vector3>("Position");

							if (Exploit::Aimbot::WallCheck) {
								RBX::Vector3 vec = position - LocalOrigin;
								RBX::Vector3 direction = vec.unit() * vec.magnitude();
								std::vector<uintptr_t> FilterDescendantsInstances = { LocalCharacter.ptr(), LocalCharacter.Ref(), character.ptr(), character.Ref() };

								RaycastParams* raycastParams = new RaycastParams();
								raycastParams->FilterDescendantsInstances = (uintptr_t)&FilterDescendantsInstances;

								static RaycastResult ret;
								reinterpret_cast<int(__thiscall*)(int, RaycastResult*, RBX::Vector3*, RBX::Vector3*, RaycastParams*)>(Raycast.Func())(workspace.ptr(), &ret, &LocalOrigin, &direction, raycastParams);
								if (ret.Instance) continue;
							}

							std::pair<RBX::Vector2, float> pair = CurrentCamera->WorldToViewportPoint(position);

							if (pair.second > closest_mag) continue;

							if (((pair.first - mousePos).length() < best.first)) {
								best = { (pair.first - mousePos).length(), player };
								closest_mag = pair.second;
							}
						}
					}
				}

				target = best.second;
				return !!best.second.ptr();
			}

			bool isActive() {
				return this->Enabled && (bool)GetAsyncKeyState(aimbotKey);
			}

			void OnStep(float dT = 1.f / 60.f) { // need to implement proper dt
				if (!this->Enabled) return;
				RBX::TaskScheduler* taskScheduler = RBX::TaskScheduler::GetSingleton(); // in future allow singleton to rescan if rejoin game
				if (RBX::Instance dataModel = taskScheduler->FindJobByName("Render").GetDataModel()) {
					if (RBX::Instance players = dataModel.FindFirstChildOfClass("Players")) {
						RBX::Instance target;
						if (this->isActive() && this->GetClosestPlayerToMouse(players, target)) {
							if (RBX::Instance character = target.GetPropertyValue<int>("Character"))
								if (RBX::Instance targetPart = character.FindFirstChild(Exploit::Aimbot::aimPart)) {
									RBX::CurrentCamera* CurrentCamera = RBX::CurrentCamera::GetSingleton();
									RBX::Instance camera = CurrentCamera->GetCamera();

									if (Exploit::Aimbot::aimbotMethod) { // camera - WORK ON MOVING CAM POS TO WHERE MOUSE IS POINTING
										RBX::CoordinateFrame cframe = camera.GetCustomPropertyValue<RBX::CoordinateFrame>("CFrame");
										if (Exploit::Aimbot::Smoothing) {
											if (!prev_target) prev_target = target;
											if (lerpAlpha > 1.f) lerpAlpha = 1.f;

										//	RBX::CoordinateFrame targetcframe = dataModel.FindFirstChildOfClass("Workspace").FindFirstChild("Baseplate").GetCustomPropertyValue<RBX::CoordinateFrame>("CFrame");
											RBX::CoordinateFrame targetcframe = cframe;
											targetcframe.lookAt(dataModel.FindFirstChildOfClass("Workspace").FindFirstChild("Baseplate").GetCustomPropertyValue<RBX::Vector3>("Position"));
											RBX::CoordinateFrame lrp = cframe.lerp(targetcframe, lerpAlpha);

											camera.SetCustomPropertyValue<RBX::CoordinateFrame>("CFrame", lrp);

											lerpAlpha += 0.1f * dT;
										}
										else {
											cframe.lookAt(targetPart.GetCustomPropertyValue<RBX::Vector3>("Position"));
											camera.SetCustomPropertyValue<RBX::CoordinateFrame>("CFrame", cframe);
										}
									}
									else { // mouse

									}
								}
						}
						else {
							prev_target = target;
							lerpAlpha = 0.f;
						}
					}
				}
			}
		private:
			RBX::Instance prev_target;
			float lerpAlpha = 0.f;

			BYTE aimbotKey = VK_MENU;
			// implement bezier curve?
			void initialise() {

			}
		};
	}
}