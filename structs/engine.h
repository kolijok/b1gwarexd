enum FlowType_t
{
	FLOW_OUTGOING = 0,
	FLOW_INCOMING = 1,
	FLOW_MAX = 2
};
enum Typetype_t
{
	TYPE_GENERIC = 0,
	TYPE_LOCALPLAYER = 1
};
class NET_SetConVar
{
public:
	NET_SetConVar(const char* name, const char* value);
	~NET_SetConVar();
private:
	DWORD pad[13];
};
class INetChannelInfo
{
public:
	float GetLatency(int flow)
	{
		typedef float(__thiscall* Fn)(void*, int);
		 return CallVFunction<Fn>(this, 9)(this, flow);
	}
	float GetAvgLatency(int type)
	{
		typedef float(__thiscall* Fn)(void*, int);
		return CallVFunction<Fn>(this, 10)(this, type);
	}
	void SendNetMsg(NET_SetConVar& convar)
	{
		typedef void(__thiscall* oSendNetMsg)(PVOID, NET_SetConVar&, int, int);
		CallVFunction<oSendNetMsg>(this, 42)(this, convar, 0, 0);
	}

	virtual float GetAvgLatency(int flow) const = 0; // average packet latency in seconds

};

class CClientState
{
public:
	void ForceFullUpdate()
	{
		m_nDeltaTick = -1;
	}
	void ForceFullUpdate_test()
	{
		static bool aa;
		static int ticks = 0;
		if (ticks >= 14)
		{
			ticks = 0;
			aa = true;
		}
		else
		{
			aa = false;
			ticks++;
		}
		//	static auto CL_FullUpdatae = reinterpret_cast<CL_FullUpdatea_t>(FindPatatearn("engine.dll", reinterpret_cast<PBYTE>("\xA1\x00\x00\x00\x00\xB9\x00\x00\x00\x00\x56\xFF\x50\x14\x8B\x34\x85"), "x????x????xxxxxxx"));

		//	if (GetKeyState('C') & 0x1) {
		if (*(uint32_t*)((uintptr_t)this + 0x174) != -1 && aa) {
			*(uint32_t*)((uintptr_t)this + 0x174) = -1;
			//CL_FullUpdatae();
		}

		//}


	}
	char pad_0000[156]; //0x0000
	class INetChannel* m_pNetChannel; //0x0094
	char pad_0098[16]; //0x0098
	unsigned int m_nRetryNumber; //0x00A8
	char pad_00AC[84]; //0x00AC
	int m_nSignonState; //0x0100
	char pad_0104[8]; //0x0104
	float m_flNextCmdTime; //0x010C
	int m_nServerCount; //0x0110
	int m_nCurrentSequence; //0x0114
	char pad_0118[75]; //0x0118
	int m_nServerTick; //0x0163
	int m_nClientTick; //0x0167
	int m_nDeltaTick; //0x016B
	char pad_016F[4]; //0x016F
	int m_nViewEntity; //0x0173
	char pad_0177[8]; //0x0177
	char m_szLevelName[260]; //0x017F
	char m_szLevelNameShort[40]; //0x0283
	char pad_02AB[18940]; //0x02AB
	char pad_0x2DA0; //0x4CA7
	int lastoutgoingcommand; //0x4CA8
	int chokedcommands; //0x4CAC
	int last_command_ack; //0x4CB0
	int command_ack; //0x4CB4
	int m_nSoundSequence; //0x4CB8
	char pad_4CBC[8]; //0x4CBC
	bool ishltv; //0x4CC4
	bool aaa;
	//	CBaseHudChat			*m_pChatElement;
};
class CEngine
{
public:
	void SetName(std::string name);
	INetChannelInfo* GetNetChannelInfo()
	{
		typedef INetChannelInfo* (__thiscall* Fn)(void*);
		return CallVFunction<Fn>(this, 78)(this);
	}
	void GetScreenSize( int& Width, int& Height )
	{
		typedef void( __thiscall* Fn )(void*, int&, int&);
		return CallVFunction<Fn>(this, 5)(this, Width, Height);
	}

	bool GetPlayerInfo( int Index, player_info_t* PlayerInfo )	
	{
		typedef bool(__thiscall* Fn)(void*, int, player_info_t*);
		return CallVFunction<Fn>(this, 8)(this, Index, PlayerInfo);
	}

	int GetLocalPlayer()
	{
		typedef int(__thiscall* Fn)(void*);
		return CallVFunction<Fn>(this, 12)(this);
	}
	int GetPlayerForUserID(int userid)
	{
		typedef int(__thiscall* Fn)(void*, int);
		return CallVFunction<Fn>(this, 9)(this, userid);
	}
	void ClientCmd( const char* Command )
	{
		typedef void(__thiscall* Fn)(void*, const char*);
		return CallVFunction<Fn>(this, 108)(this, Command);
		}

	void SetViewAngles(QAngle& Angles)
	{
		typedef void(__thiscall* Fn)(void*, QAngle&);
		return CallVFunction<Fn>(this, 19)(this, Angles);
	}

	void GetViewAngles(QAngle& angle)
	{
		typedef void(__thiscall* Fn)(void*, QAngle&);
		return CallVFunction<Fn>(this, 18)(this,angle);
	}

	matrix3x4_t& WorldToScreenMatrix()
	{
		typedef matrix3x4_t& (__thiscall* Fn)(void*);
		return CallVFunction<Fn>(this, 37)(this);
		
	
	}
	bool IsConnected()
	{
		typedef bool(__thiscall* Fn)(PVOID);
		return CallVFunction<Fn>(this, 27)(this);
	}

	bool IsInGame()
	{
		typedef bool(__thiscall* Fn)(PVOID);
		return CallVFunction<Fn>(this, 26)(this);
	}

	int GetMaxClients()
	{
		typedef bool(__thiscall* Fn)(PVOID);
		return CallVFunction<Fn>(this, 20)(this);
	}

	float Time()
	{
		typedef float(__thiscall* Fn)(PVOID);
		return CallVFunction<Fn>(this, 14)(this);
	}


	void ClientCmd_Unrestricted(char  const* cmd)
	{
		typedef void(__thiscall* oClientCmdUnres)(PVOID, const char*, char);
		return CallVFunction<oClientCmdUnres>(this, 114)(this, cmd, 0);
	}
	//52
	char const    GetLevelName()
	{
		typedef char const(__thiscall* oClientCmdUnres)(PVOID);
		return CallVFunction<oClientCmdUnres>(this, 52)(this);
	}
	unsigned int GetEngineBuildNumber()
	{
		typedef unsigned int(__thiscall* OriginalFn)(PVOID);
		return CallVFunction< OriginalFn >(this, 104)(this);
	}

};
