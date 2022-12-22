#pragma once
#include "includes.h"

namespace cheat
{
	namespace aim
	{
		inline void run_triggerbot(entity_t* local)
		{
			if (local->is_alive() == FALSE || !g_Input::is_key_down(0x06))
				return;
			
			i32 cId = local->crosshair_id();
			if (cId < 64 && cId > 0)
			{
				entity_t* player = reinterpret_cast<entity_t*>(memory::read<u32>(_::pClient->base() + g_Offsets::dwEntityList + ((cId - 1) * 0x10)));
				if (player->health() > 0)
				{
					if (player->team_id() != local->team_id())
					{
						if (_::pClient->read_force_attack() == 4)
						{
							_::pClient->write_force_attack(6);
						}
					}
				}
			}
		}
	}

	namespace misc
	{
		inline void run_bhop(entity_t* local)
		{
			if (local->is_alive() == FALSE || !g_Input::is_key_down(0x20))
				return;

			u32 flags = memory::read<u32>(local->base() + g_Offsets::m_fFlags);
			if (flags & (1 << 0))
				memory::write(_::pClient->base() + g_Offsets::dwForceJump, 6);
			else
				memory::write(_::pClient->base() + g_Offsets::dwForceJump, 4);
		}
	}

	namespace visuals
	{
		inline void run_glow(entity_t* local)
		{
			u32 pGlowObj = memory::read<u32>(_::pClient->base() + g_Offsets::dwGlowObjectManager);

			for(int i = 0; i < 32; i++)
			{
				entity_t* player = reinterpret_cast<entity_t*>(memory::read<u32>(_::pClient->base() + g_Offsets::dwEntityList + (i * 0x10)));
				if (player == NULL)
					continue;
				
				if (player->health() > 0 && !player->is_dormant())
				{
					if (player->team_id() != local->team_id())
					{
						u32 iGlowIndex = memory::read<u32>(player->base() + g_Offsets::m_iGlowIndex);

						//set player glowing
						memory::write<float>(pGlowObj + (iGlowIndex * 0x38) + 0x8, 1.0f);
						memory::write<float>(pGlowObj + (iGlowIndex * 0x38) + 0xC, 0.f);
						memory::write<float>(pGlowObj + (iGlowIndex * 0x38) + 0x10, 0.f);
						memory::write<float>(pGlowObj + (iGlowIndex * 0x38) + 0x14, 1.0f);
						
						memory::write<u32>(pGlowObj + (iGlowIndex * 0x38) + 0x28, 1);
						memory::write<u32>(pGlowObj + (iGlowIndex * 0x38) + 0x29, 0);
					}
				}
			}
		}
	}
}