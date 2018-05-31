#pragma once
class ClientVariables {
public://Rifles
	struct LegitbotSniper {
		bool EnableAimbot = false;
		int LegitBotkey = 0;
		float fov = 0;
		float Smooth = 0;
		float RCSy = 0;
		float FakeLaga = 0;
		float RCSx = 0;
		int hitboxtype = 0;
		bool RCS = false;
		bool test = false;
		bool FakeLag = false;
		int FakeLagOnKey = 0;

	} LegitbotSniper;
	struct LegitbotRifles {
		bool EnableAimbot = false;
		int LegitBotkey = 0;
		float fov = 0;
		float Smooth = 0;
		float RCSy = 0;
		float FakeLaga = 0;
		float RCSx = 0;
		int hitboxtype = 0;
		bool RCS = false;
		bool test = false;
		bool FakeLag = false;
		int FakeLagOnKey = 0;

	} LegitbotRifles;
	struct LegitbotPistols {
		bool EnableAimbot = false;
		int LegitBotkey = 0;
		float fov = 0;
		float Smooth = 0;
		float RCSy = 0;
		float FakeLaga = 0;
		float RCSx = 0;
		int hitboxtype = 0;
		bool RCS = false;
		bool test = false;
		bool FakeLag = false;
		int FakeLagOnKey = 0;

	} LegitbotPistols;
	struct Legitbot {
		bool EnableAimbot = false;
		int LegitBotkey = 0;
		float fov = 0;
		float Smooth = 0;
		float RCSy = 0;
		float FakeLaga = 0;
		float RCSx = 0;
		int hitboxtype = 0;
		bool RCS = false;
		bool test = false;
		bool FakeLag = false;
		int FakeLagOnKey = 0;

	} Legitbot;
	struct Ragebot {
		bool EnableAimbot = false;
		int AimbotSelection = 0;
		bool AutomaticFire = false;
		bool AutomaticScope = false;
		bool SilentAimbot = false;
		bool AutoRevolver = false;
		bool EnginePred = false;
		int EnginePred1 = 0;
		bool NoRecoil = false;
		bool NoSpread = false;
		bool BodyFakeWalk = false;
		bool PitchCorrection = false;
		bool PositionAdjustment = false;
		int Preferbodyaim = 0;
		int Hitbox = 0;
		int Hitscan = 0;
		bool AutowallHitscan = false;
		bool Autowall = false;
		int Multipoint = 0.f;
		bool enablescale = false;
		float Headscale = 0.0f;
		bool PreferBody = false;
		bool PreferBodyEnable = false;
		float Bodyscale = 0.0f;
		int Fov = 0.f;
		int Mindamage = 1.f;
		bool enablemindmg = false;
		bool Hitchance = false;
		int Minhitchance = 0.f;
		bool hitchanceenable = false;
		/*bool AutomaticResolver = false;
		bool AutomaticResolver2 = false;*/
		int ResolverMode = 0;
		int ResolverStepAngle = 25;
		bool FriendlyFire = false;
		bool Quickstop = false;
		bool HeadH = false;
		bool NeckH = false;
		bool BodyH = false;
		bool ArmsH = false;
		bool LegsH = false;
		bool FootsH = false;
	} Ragebot;
	struct Antiaim {
		bool AntiaimEnable = false;
		int pitchadd = 0;
		bool Manual = false;
		int Pitch = 0;
		int Yaw = 0;
		int YawAdd = 0;
		int YawRunning = 0;
		int YawRunningAdd = 0;
		int FakeYaw = 0;
		int FakeYawAdd = 0;
		int AntiResolver = 0;
		int Fakewalk = 0;
		int Freestanding = 0;
		int FreestandingDelta = 0; int AtPlayer = 0;
		bool Disableaa = false;
		int Jitterrange = 0.f;
		bool Randomizejitter = false;
		int AntiaimMode = 0;
	}

	//-------------------------------------------------------------------------

	Antiaim;
	struct Visuals {
		float Opacity = 0.0f;
		bool EspEnable = false;
		bool EnemyOnly = false;
		bool BoundingBox = false;
		bool Bones = false;
		bool Health = false;
		bool Armor = false;
		bool Flags = false;
		bool Fake = false;
		bool Dlight = false;
		bool Name = false;
		bool Weapon = false;
		bool Icon = false;
		bool Ammo = false;
		bool AllItems = false;
		bool Rank = false;
		bool Wins = false;
		bool Glow = false;
		bool LineofSight = false;
		bool SnapLines = false;
		bool GrenadePrediction = false;
		int Crosshair = 0;
		bool SpreadCrosshair = false;
		bool RecoilCrosshair = false;
		bool FartherESP = false;
		//Cbase/filers
		int DroppedWeapons = 0;
		bool Hostage = false;
		bool ThrownNades = false;
		bool LocalPlayer = false;
		bool LocalChams = false;
		bool LocalGlow = false;
		bool BulletTracers = false;
		bool Teammates = false;
		bool Bomb = false;
		bool Spectators = false;
		bool DamageIndicators = false;
		//Effects/world
		bool OutOfPOVArrows = false;
		bool nightmode = false;
		int Skybox = 0;
		int FlashbangAlpha = 0;
		bool Nosmoke = false;
		bool Noscope = false;
		bool RemoveParticles = false;
		bool Novisrevoil = false;
		int Hitmarker;
		bool ChamsEnable = false;
		bool removeparticles2 = false;
		int ChamsStyle = 0;
		int FakeAngleGhost = 0;
		bool WeaponMaterials = false;
		bool Chamsenemyonly = false;
		bool ChamsPlayer = false;
		bool ChamsPlayerWall = false;
		bool ChamsHands = false;
		bool ChamsHandsWireframe = false;
		bool WeaponWireframe = false;
		bool WeaponChams = false;
		bool TransparensWallsEnable = false;
		bool WeaponEnableChams = false;
		bool HandEnableChams = false;
		bool test = false;
		bool test1 = false;
		bool test2 = false;
		bool test3 = false;
	} Visuals;
	struct Misc {
		int Rightside = 0;
		int Leftside = 0;
		int Backside = 0;
		bool AntiUT = true;
		bool OBSBypass = true;
		int PlayerFOV = 0.f;
		int PlayerViewmodel = 0.f;
		int TPangles = 0;
		int TPKey = 0;
		int MenuKey = 0x2d;
		int WalkbotSet = 0x2d;
		int WalkbotDelete = 0x2d;
		int WalkbotStart = 0x2d;
		int Clantag;
		bool AutoJump = false;
		bool AutoStrafe = false;
		bool AutoAccept = false;
		bool Prespeed = false;
		int Retrack = 0.f;
		int PrespeedKey = 0;
		bool FakelagEnable = false;
		bool FakelagOnground = false;
		int FakelagMode = 0;
		int FakelagAmount = 0.f;
		bool FakelagShoot = false;
		int ConfigSelection = 0;
		bool Walkbot = false;
		bool WalkbotBunnyhop = false;
		//int WalkbotSetPoint = 0;
		//int WalkbotDeletePoint = 0;
		//int WalkbotStart = 0;
		bool FakewalkEnable = false;
		int FakewalkKey = 0;
		bool enablemanual = false;
		bool buybot = false;
		int buybot2;
		bool buybot3 = false;
		bool clantag = false;
		int clantag_b1gware = 0;
	} Misc;
	struct Skins {
		bool Enabled;
		int Knifes;
	} Skinchanger;
	struct CPlayerlist {
		bool bEnabled;
		int iPlayer;
		char *szPlayers[64] = {
			" ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ",
			" ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ",
			" ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ",
			" ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ",
			" ", " ", " ", " ", " ", " ", " "
		};
	}

	//-------------------------------------------------------------------------

	Playerlist;
	struct NigColors {
		float MenuColor[3] = {
			0.f, 1.f, 0.f
		};
		float BoundingBox[3] = {
			0.f, 1.f, 0.f
		};
		float PlayerChams[3] = {
			0.f, 1.f, 0.f
		};
		float PlayerChamsWall[3] = {
			1.f, 0.f, 0.f
		};
		float LocalChams[3] = {
			0.f, 1.f, 0.f
		};
		float Skeletons[3] = {
			1.f, 1.f, 1.f
		};
		float SelfChams[3] = {
			0.f, 0.f, 1.f
		};
		float Bulletracer[3] = {
			1.f, 0.f, 0.f
		};
		float ArmorColor[3] = {
			0.f, 0.f, 1.f
		};
		float WireframeHand[3] = {
			1.f, 1.f, 1.f
		};
		float ChamsHand[3] = {
			1.f, 1.f, 1.f
		};
		float ChamsWeapon[3] = {
			1.f, 1.f, 1.f
		};
		float WireframeWeapon[3] = {
			1.f, 1.f, 1.f
		};
		float CrystalColor[3] = {
			1.f, 1.f, 1.f
		};
		float Glow[3] = {
			1.f, 0.f, 0.f
		};
		float LocalGlow[3] = {
			1.f, 0.f, 0.f
		};
		float DroppedWeapon[3] = {
			1.f, 1.f, 1.f
		};
		float Bomb[3] = {
			1.f, 1.f, 1.f
		};
		float PlantedBomb[3] = {
			1.f, 1.f, 1.f
		};
		float Hostage[3] = {
			1.f, 1.f, 1.f
		};
		float GrenadePrediction[3] = {
			0.f, 1.f, 0.f
		};
		float FakeAngleGhost[3] = {
			1.f, 1.f, 1.f
		};
		float SpreadCrosshair[3] = {
			0.f, 0.f, 0.f
		};
		float Snaplines[3] = {
			1.f, 1.f, 1.f
		};
		float DamageIndicator[3] = {
			1.f, 0.f, 0.f
		};
		float WeaponsCAT[3] = {
			1.f, 1.f, 1.f
		};
	}

	//-------------------------------------------------------------------------

	Colors;
};
extern ClientVariables *Clientvariables;