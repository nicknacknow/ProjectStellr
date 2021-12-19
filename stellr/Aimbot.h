#pragma once
#include "globals.h"
#include "TaskScheduler.h"
#include "CurrentCamera.h"
#include "Mouse.h"

namespace Exploit {
	namespace Aimbot {

		const char* aimPart = "Head";


		class Aimbot : public Cheat::Cheat, public Singleton::Singleton<Aimbot> {
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
				static RBX::Mouse* mouseClass = RBX::Mouse::GetSingleton();
				static RBX::Instance mouse = mouseClass->GetMouse(); if (!mouse) return false;
				static RBX::CurrentCamera* CurrentCamera = RBX::CurrentCamera::GetSingleton();
				static RBX::Instance camera = CurrentCamera->GetCamera(); if (!camera) return false;

				static RBX::Vector2 mousePos{ (float)mouse.GetPropertyValue<int>("X"), (float)mouse.GetPropertyValue<int>("Y") };

				static RBX::Instance LocalPlayer = Players.GetPropertyValue<int>("LocalPlayer"); if (!LocalPlayer.ptr()) return false; 
				RBX::Instance LocalCharacter = LocalPlayer.GetPropertyValue<int>("Character"); printf("localCharacter : %p\n", LocalCharacter.ptr()); if (!LocalCharacter.ptr())return false;
				RBX::Instance LocalOriginPart = LocalCharacter.FindFirstChild(aimPart); if (!LocalOriginPart.ptr()) LocalOriginPart = LocalCharacter.GetPropertyValue<int>("PrimaryPart"); if (!LocalOriginPart)return false;
				static RBX::Vector3  LocalOrigin = LocalOriginPart.GetCustomPropertyValue<RBX::Vector3>("Position");

				std::pair<float, RBX::Instance> best = { 9e9f, RBX::Instance() };
				float closest_mag = 9e9f;

				for (RBX::Instance player : Players.GetChildren()) {
					if (!(player.ptr() && player.GetClassDescriptor().ClassName() == "Player")) continue;
					if (RBX::Instance character = player.GetPropertyValue<int>("Character")) {
						if (RBX::Instance target = character.FindFirstChild("Head")) {
							// do checks l8r
							RBX::Vector3 position = target.GetCustomPropertyValue<RBX::Vector3>("Position");

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

			void OnStep() {
				if (!this->Enabled) return;
				RBX::TaskScheduler* taskScheduler = RBX::TaskScheduler::GetSingleton(); // in future allow singleton to rescan if rejoin game
				if (RBX::Instance dataModel = taskScheduler->FindJobByName("Render").GetDataModel()) {
					if (RBX::Instance players = dataModel.FindFirstChildOfClass("Players")) {
						RBX::Instance target;
						if (this->isActive() && this->GetClosestPlayerToMouse(players, target)) {
							printf("target : %s\n", target.Name().c_str());
						}
					}
				}
			}

			bool aimbotMethod = true;
			bool WallCheck = true;
		private:
			BYTE aimbotKey = VK_MENU;

			void initialise() {

			}
		};
	}
}