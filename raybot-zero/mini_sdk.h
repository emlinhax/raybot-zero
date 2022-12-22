#pragma once

namespace g_Offsets
{
	constexpr u32 dwEntityList = 0x4DDB8FC;
	constexpr u32 dwForceAttack = 0x320BDC8;
	constexpr u32 dwForceJump = 0x52858A0;
	constexpr u32 dwLocalPlayer = 0xDBF4BC;
	constexpr u32 dwGlowObjectManager = 0x5324590;

	constexpr u32 m_iCrosshairId = 0x11838;
	constexpr u32 m_bDormant = 0xED;
	constexpr u32 m_fFlags = 0x104;
	constexpr u32 m_iGlowIndex = 0x10488;
	constexpr u32 m_iHealth = 0x100;
	constexpr u32 m_iTeamNum = 0xF4;
}

class client_t
{
public:

	u32 base()
	{
		return reinterpret_cast<u32>(this);
	}

	void write_force_attack(i32 v)
	{
		memory::write<i32>(reinterpret_cast<u32>(this) + g_Offsets::dwForceAttack, v);
	}

	i32 read_force_attack()
	{
		return memory::read<i32>(reinterpret_cast<u32>(this) + g_Offsets::dwForceAttack);
	}
};

class entity_t
{

public:
	u32 base()
	{
		return reinterpret_cast<u32>(this);
	}

	i32 health()
	{
		return memory::read<i32>((reinterpret_cast<u32>(this) + g_Offsets::m_iHealth));
	}

	b8 is_alive()
	{
		return health() > 0;
	}

	i32 is_dormant()
	{
		return memory::read<i32>((reinterpret_cast<u32>(this) + g_Offsets::m_bDormant));
	}

	i32 crosshair_id()
	{
		return memory::read<i32>((reinterpret_cast<u32>(this) + g_Offsets::m_iCrosshairId));
	}

	i32 team_id()
	{
		return memory::read<i32>((reinterpret_cast<u32>(this) + g_Offsets::m_iTeamNum));
	}
};