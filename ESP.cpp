#include "sdk.h"
#include "ESP.h"
#include "global.h"
#include "xor.h"
#include "Math.h"
#include "Menu.h"
#include "Misc.h"
#include "BacktrackingHelper.h"
#include "GameUtils.h"
#include "Draw.h"
#include "Autowall.h"

CESP* g_ESP = new CESP;

float flPlayerAlpha[65];

#define TICK_INTERVAL			( g_pGlobals->interval_per_tick )
#define TIME_TO_TICKS( dt )		( (int)( 0.5f + (float)(dt) / TICK_INTERVAL ) )
#define TICKS_TO_TIME( t )		( TICK_INTERVAL *( t ) )

char* GetWeaponName(CBaseCombatWeapon* pWeapon)
{
	int ID = pWeapon->WeaponID();

	switch (ID) {
	case 4:
		return "glock";
	case 2:
		return "duals";
	case 36:
		return "p250";
	case 30:
		return "tec9";
	case 1:
		return "deagle";
	case 32:
		return "p2000";
	case 3:
		return "fiveseven";
	case 64:
		return "revolver";
	case 63:
		return "cz75";
	case 61:
		return "usp";
	case 35:
		return "nova";
	case 25:
		return "xm1014";
	case 29:
		return "sawed off";
	case 14:
		return "m249";
	case 28:
		return "negev";
	case 27:
		return "mag7";
	case 17:
		return "mac10";
	case 33:
		return "mp7";
	case 24:
		return "ump45";
	case 19:
		return "p90";
	case 26:
		return "�bizon";
	case 34:
		return "mp9";
	case 10:
		return "famas";
	case 16:
		return "m4a4";
	case 40:
		return "ssg08";
	case 8:
		return "aug";
	case 9:
		return "awp";
	case 38:
		return "scar20";
	case 13:
		return "galil";
	case 7:
		return "ak47";
	case 39:
		return "sg553";
	case 11:
		return "g3sg1";
	case 60:
		return "m4a1s";
	case 46:
	case 48:
		return "molotov";
	case 44:
		return "grenade";
	case 43:
		return "flashbang";
	case 45:
		return "smoke";
	case 47:
		return "decoy";
	case 31:
		return "taser";
	default:
		return "knife";
	}
}

template <class T, class U>
inline T clamp(T in, U low, U high)
{
	if (in <= low)
		return low;
	else if (in >= high)
		return high;
	else
		return in;
}

bool GetCBaseEntityBox(CBaseEntity* pBaseEntity, Vector& BotCenter, Vector& TopCenter, float& Left, float& Right, bool is_player)
{
	if (!pBaseEntity)
		return false;

	if (!is_player) {
		const VMatrix& trans = pBaseEntity->GetCollisionBoundTrans();
		CollisionProperty* collision = pBaseEntity->GetCollision();

		if (!collision)
			return false;

		Vector min, max;
		min = collision->VecMins();
		max = collision->VecMaxs();

		Vector points[] = { Vector(min.x, min.y, min.z),
			Vector(min.x, max.y, min.z),
			Vector(max.x, max.y, min.z),
			Vector(max.x, min.y, min.z),
			Vector(max.x, max.y, max.z),
			Vector(min.x, max.y, max.z),
			Vector(min.x, min.y, max.z),
			Vector(max.x, min.y, max.z) };

		auto vector_transform = [](const Vector in1, const VMatrix& in2) {
			auto dot_product = [](const Vector& v1, const float* v2) {
				float ret = v1[0] * v2[0] + v1[1] * v2[1] + v1[2] * v2[2];
				return ret;
			};

			auto out = Vector();
			out[0] = dot_product(in1, in2[0]) + in2[0][3];
			out[1] = dot_product(in1, in2[1]) + in2[1][3];
			out[2] = dot_product(in1, in2[2]) + in2[2][3];
			return out;
		};

		Vector pointsTransformed[8];
		for (int i = 0; i < 8; i++) {
			pointsTransformed[i] = vector_transform(points[i], trans);
		}

		Vector pos = pBaseEntity->GetAbsOrigin();
		Vector flb;
		Vector brt;
		Vector blb;
		Vector frt;
		Vector frb;
		Vector brb;
		Vector blt;
		Vector flt;

		if (!GameUtils::WorldToScreen(pointsTransformed[3], flb) || !GameUtils::WorldToScreen(pointsTransformed[5], brt)
			|| !GameUtils::WorldToScreen(pointsTransformed[0], blb) || !GameUtils::WorldToScreen(pointsTransformed[4], frt)
			|| !GameUtils::WorldToScreen(pointsTransformed[2], frb) || !GameUtils::WorldToScreen(pointsTransformed[1], brb)
			|| !GameUtils::WorldToScreen(pointsTransformed[6], blt) || !GameUtils::WorldToScreen(pointsTransformed[7], flt))
			return false;

		Vector arr[] = { flb, brt, blb, frt, frb, brb, blt, flt };
		float left = flb.x;
		float top = flb.y;
		float right = flb.x;
		float bottom = flb.y;

		for (int i = 1; i < 8; i++) {
			if (left > arr[i].x)
				left = arr[i].x;
			if (top < arr[i].y)
				top = arr[i].y;
			if (right < arr[i].x)
				right = arr[i].x;
			if (bottom > arr[i].y)
				bottom = arr[i].y;
		}
		BotCenter = Vector(right - ((right - left) / 2), top, 0);
		TopCenter = Vector(right - ((right - left) / 2), bottom, 0);
		Left = left;
		Right = right;
	}
	else {
		Vector org = pBaseEntity->GetAbsOrigin();
		Vector head;
		if ((*pBaseEntity->GetFlags() & FL_DUCKING))
			head = org + Vector(0.f, 0.f, 52.f);
		else
			head = org + Vector(0.f, 0.f, 72.f);

		Vector org_screen, head_screen;
		if (!GameUtils::WorldToScreen(org, org_screen) || !GameUtils::WorldToScreen(head, head_screen))
			return false;

		int height = int(org_screen.y - head_screen.y);
		int width = int(height / 2);
		Left = int(head_screen.x - width / 2);
		Right = int(head_screen.x + width / 2);
		BotCenter = Vector(head_screen.x, org_screen.y);
		TopCenter = BotCenter;
		TopCenter.y = head_screen.y;
	}
	return true;
}

void CESP::DrawBones(CBaseEntity* pBaseEntity, int r, int g, int b, int a)
{
	float Red = Clientvariables->Colors.Skeletons[0] * 255;
	float Green = Clientvariables->Colors.Skeletons[1] * 255;
	float Blue = Clientvariables->Colors.Skeletons[2] * 255;

	studiohdr_t* pStudioModel = g_pModelInfo->GetStudioModel((model_t*)pBaseEntity->GetModel());

	Vector vParent, vChild, sParent, sChild;

	for (int j = 0; j < pStudioModel->numbones; j++) {
		mstudiobone_t* pBone = pStudioModel->pBone(j);

		if (pBone && (pBone->flags & BONE_USED_BY_HITBOX) && (pBone->parent != -1)) {
			vChild = pBaseEntity->GetBonePos(j);
			vParent = pBaseEntity->GetBonePos(pBone->parent);

			g_pDebugOverlay->ScreenPosition(vParent, sParent);
			g_pDebugOverlay->ScreenPosition(vChild, sChild);

			g_pSurface->SetDrawColor(Red, Green, Blue, flPlayerAlpha[pBaseEntity->GetIndex()]);
			g_pSurface->DrawLine(sParent[0], sParent[1], sChild[0], sChild[1]);
		}
	}
}

RECT GetViewport()
{
	RECT Viewport = { 0, 0, 0, 0 };
	int w, h;
	g_pEngine->GetScreenSize(w, h);
	Viewport.right = w;
	Viewport.bottom = h;
	return Viewport;
}

void DrawTexturedPoly(int n, Vertex_t* vertice, Color col)
{
	static int texture_id = g_pSurface->CreateNewTextureID(true);
	static unsigned char buf[4] = { 255, 255, 255, 255 };
	g_pSurface->DrawSetTextureRGBA(texture_id, buf, 1, 1);
	g_pSurface->SetDrawColor(col);
	g_pSurface->DrawSetTexture(texture_id);
	g_pSurface->DrawTexturedPolygon(n, vertice);
}

void DrawFilledCircle(Vector2D center, Color color, float radius, float points)
{
	std::vector<Vertex_t> vertices;
	float step = (float)M_PI * 2.0f / points;
	for (float a = 0; a < (M_PI * 2.0f); a += step)
		vertices.push_back(Vertex_t(Vector2D(radius * cosf(a) + center.x, radius * sinf(a) + center.y)));
	DrawTexturedPoly((int)points, vertices.data(), color);
}

void CESP::DrawLinesAA()
{

	Vector src3D, dst3D, forward, src, dst;
	trace_t tr;
	Ray_t ray;
	CTraceFilter filter;
	float Red, Green, Blue;
	Red = Clientvariables->Colors.Glow[0] * 255;
	Green = Clientvariables->Colors.Glow[1] * 255;
	Blue = Clientvariables->Colors.Glow[2] * 255;
	float Red1, Green1, Blue1;
	Red1 = Clientvariables->Colors.LocalChams[0] * 255;
	Green1 = Clientvariables->Colors.LocalChams[1] * 255;
	Blue1 = Clientvariables->Colors.LocalChams[2] * 255;

	filter.pSkip = G::LocalPlayer;
	Math::AngleVectors(QAngle(0, G::FakeAngle.y, 0), &forward);
	src3D = G::LocalPlayer->GetOrigin();
	dst3D = src3D + (forward * 50.f);
	ray.Init(src3D, dst3D);
	g_pEngineTrace->TraceRay(ray, 0, &filter, &tr);

	if (!GameUtils::WorldToScreen(src3D, src) || !GameUtils::WorldToScreen(tr.endpos, dst))
		return;

	g_pSurface->SetDrawColor(Color(Red, Green, Blue, 255));
	g_pSurface->DrawLine(src.x, src.y, dst.x, dst.y);

	Math::AngleVectors(QAngle(0, G::RealAngle.y, 0), &forward);
	dst3D = src3D + (forward * 50.f);
	ray.Init(src3D, dst3D);
	g_pEngineTrace->TraceRay(ray, 0, &filter, &tr);

	if (!GameUtils::WorldToScreen(src3D, src) || !GameUtils::WorldToScreen(tr.endpos, dst))
		return;

	g_pSurface->SetDrawColor(Color(Red1, Green1, Blue1, 255));
	g_pSurface->DrawLine(src.x, src.y, dst.x, dst.y);
}

void CESP::DrawPlayer(CBaseEntity* pPlayer, CBaseEntity* pLocalPlayer)
{
	Vector m_position = pPlayer->GetAbsOrigin();
	Vector m_top_position = m_position;
	if (*pPlayer->GetFlags() & IN_DUCK)
		m_top_position += Vector(0, 0, 52);
	else
		m_top_position += Vector(0, 0, 72);

	Vector footpos, headpos;

	if (pPlayer->IsDormant() && flPlayerAlpha[pPlayer->GetIndex()] > 0.f)
		flPlayerAlpha[pPlayer->GetIndex()] -= 0.5f;
	else if (flPlayerAlpha[pPlayer->GetIndex()] < 255.f && !pPlayer->IsDormant())
		flPlayerAlpha[pPlayer->GetIndex()] = 255.f;

	if (pPlayer != G::LocalPlayer && !pPlayer->IsDormant() && Clientvariables->Visuals.SnapLines) {
		RECT View = GetViewport();
		int MidX = View.right / 1.9999999999999;
		int MidY = View.bottom / 2;
		Vector VecOrigin = pPlayer->GetAbsOrigin(), SecondVector;
		float Red = Clientvariables->Colors.Snaplines[0] * 255;
		float Green = Clientvariables->Colors.Snaplines[1] * 255;
		float Blue = Clientvariables->Colors.Snaplines[2] * 255;

		if (GameUtils::WorldToScreen(VecOrigin, SecondVector)) {
			g_Draw.LineRGBA(MidX, MidY, SecondVector.x, SecondVector.y, Red, Green, Blue, flPlayerAlpha[pPlayer->GetIndex()]);
		}
	}

	if (GameUtils::WorldToScreen(m_position, footpos) && GameUtils::WorldToScreen(m_top_position, headpos)) {
		Vector Bot, Top;
		float Left, Right;
		GetCBaseEntityBox(pPlayer, Bot, Top, Left, Right, true);
		int height = Bot.y - Top.y;
		int width = Right - Left;
		int x = Left;
		int y = Top.y;

		CBaseCombatWeapon* pMainWeapon = pPlayer->GetWeapon();

		if (Clientvariables->Visuals.Bones)
			DrawBones(pPlayer, 255, 255, 255, flPlayerAlpha[pPlayer->GetIndex()]);

		if (Clientvariables->Visuals.BoundingBox) {
			float Red = Clientvariables->Colors.BoundingBox[0] * 255;
			float Green = Clientvariables->Colors.BoundingBox[1] * 255;
			float Blue = Clientvariables->Colors.BoundingBox[2] * 255;

			if (pPlayer->IsDormant()) {
				g_Draw.Box(x - 1, y - 1, width + 2, height + 2, 0, 0, 0, flPlayerAlpha[pPlayer->GetIndex()]);
				g_Draw.Box(x, y, width, height, Red, Green, Blue, flPlayerAlpha[pPlayer->GetIndex()]);
				g_Draw.Box(x + 1, y + 1, width - 2, height - 2, 0, 0, 0, flPlayerAlpha[pPlayer->GetIndex()]);
			}
			else {
				g_Draw.Box(x - 1, y - 1, width + 2, height + 2, 0, 0, 0, flPlayerAlpha[pPlayer->GetIndex()]);
				g_Draw.Box(x, y, width, height, Red, Green, Blue, flPlayerAlpha[pPlayer->GetIndex()]);
				g_Draw.Box(x + 1, y + 1, width - 2, height - 2, 0, 0, 0, flPlayerAlpha[pPlayer->GetIndex()]);
			}
		}

		if (Clientvariables->Visuals.Armor) {
			if (pPlayer->GetArmor() > 0) {
				int armor = 100 - pPlayer->GetArmor();
				float Red, Green, Blue;
				Red = Clientvariables->Colors.ArmorColor[0] * 255;
				Green = Clientvariables->Colors.ArmorColor[1] * 255;
				Blue = Clientvariables->Colors.ArmorColor[2] * 255;

				int w = 4;
				if (width < 4)
					w = width;

				int step_height = height;

				g_Draw.FillRGBA(x + width, y + armor * height / 100, w, pPlayer->GetArmor() * height / 100, Red, Green, Blue, flPlayerAlpha[pPlayer->GetIndex()]);

				g_Draw.Box(x + width, y, w, height, 0, 0, 0, flPlayerAlpha[pPlayer->GetIndex()]);

				for (int i = 1; i < 1; i++) {
					g_Draw.LineRGBA(x + width, y + i * step_height, x + width + w - 1, y + i * step_height, 0, 0, 0, flPlayerAlpha[pPlayer->GetIndex()]);
				}
			}
		}

		if (Clientvariables->Visuals.Flags) {
			if (pPlayer->Velocity().Length2D() > 45.f)
				g_Draw.StringA(g_Draw.font_esp, false, x + width + 4, y + 3, 255, 255, 255, flPlayerAlpha[pPlayer->GetIndex()], "R: LBY");
			else if (pPlayer->Velocity().Length2D() < 45.f)
				g_Draw.StringA(g_Draw.font_esp, false, x + width + 4, y + 3, 255, 255, 255, flPlayerAlpha[pPlayer->GetIndex()], "R: LastLBY");
		}

		if (Clientvariables->Visuals.Health) {
			int health = 100 - pPlayer->GetHealth();
			int w = 4;
			if (width < 4)
				w = width;
			int hr, hg, hb;

			hr = 255 - (pPlayer->GetHealth() * 2.55);
			hg = pPlayer->GetHealth() * 2.55;
			hb = 0;

			if (pPlayer->IsDormant()) {
				g_Draw.FillRGBA(x - (5), y, w, height, 0, 0, 0, flPlayerAlpha[pPlayer->GetIndex()]);
				g_Draw.FillRGBA(x - (5), y + health * height / 100, w, pPlayer->GetHealth() * height / 100, hr, hg, hb, flPlayerAlpha[pPlayer->GetIndex()]);
				g_Draw.Box(x - (5), y, w, height, 0, 0, 0, flPlayerAlpha[pPlayer->GetIndex()]);
			}
			else {
				g_Draw.FillRGBA(x - (5), y, w, height, 0, 0, 0, flPlayerAlpha[pPlayer->GetIndex()]);
				g_Draw.FillRGBA(x - (5), y + health * height / 100, w, pPlayer->GetHealth() * height / 100, hr, hg, hb, flPlayerAlpha[pPlayer->GetIndex()]);
				g_Draw.Box(x - (5), y, w, height, 0, 0, 0, flPlayerAlpha[pPlayer->GetIndex()]);
			}
			if (pPlayer->GetHealth() <= 100) {
				char hp[20];
				sprintf_s(hp, sizeof(hp), "%i", pPlayer->GetHealth());

				g_Draw.StringA(g_Draw.font_esp, false, x - g_Draw.getWidht(hp, g_Draw.font_espnum), y + health * height / 100 - 6, 255, 255, 255, flPlayerAlpha[pPlayer->GetIndex()], "%i", pPlayer->GetHealth());
			}
		}

		int bot_add = 0;
		int top_add = 0;

		static class Text {
		public:
			std::string text;
			int side;
			int Font;
			Color color;

			Text(std::string text, int side, int Font, Color color)
				: text(text)
				, side(side)
				, Font(Font)
				, color(color)
			{
			}
		};
		std::vector<Text> texts;

		if (Clientvariables->Visuals.Name) {
			player_info_s info;
			g_pEngine->GetPlayerInfo(pPlayer->Index(), &info);
			g_Draw.StringA(g_Draw.font_esp, true, x + width / 2, y - 12 - top_add, 255, 255, 255, flPlayerAlpha[pPlayer->GetIndex()], "%s", info.m_szPlayerName);
			top_add += 12;
		}


		if (Clientvariables->Visuals.Weapon) {
			UINT* hWeapons = pPlayer->GetWeapons();
			if (hWeapons) {
				if (pMainWeapon) {
					std::string s1 = pMainWeapon->GetGunIcon();
					std::string ammo;
					if (Clientvariables->Visuals.Ammo && pMainWeapon->Clip1() != -1 && pMainWeapon->GetWeaponType() != WEAPONCLASS::WEPCLASS_INVALID) {
						ammo.append(XorStr("("));
						ammo.append(to_string(pMainWeapon->Clip1()));
						ammo.append(XorStr("/"));
						ammo.append(to_string(pMainWeapon->GetMaxAmmoReserve()));
						ammo.append(XorStr(")"));
					}
					g_Draw.StringA(g_Draw.font_icon, true, x + width / 2, y + height + bot_add + 5, 255, 255, 255, flPlayerAlpha[pPlayer->GetIndex()], XorStr("%s"), s1.c_str());
					g_Draw.StringA(g_Draw.font_esp, true, x + width / 2, y + height + bot_add + 15, 255, 255, 255, flPlayerAlpha[pPlayer->GetIndex()], XorStr("%s"), ammo.c_str());
				}
			}
		}
	}
}

void CESP::DrawC4(CBaseEntity* pBomb, bool is_planted, CBaseEntity* pLocalPlayer)
{
	Vector Bot, Top;
	float Left, Right;
	GetCBaseEntityBox(pBomb, Bot, Top, Left, Right, false);
	int height = Bot.y - Top.y;
	int width = Right - Left;
	DWORD col = is_planted ? D3DCOLOR_RGBA(250, 42, 42, 255) : D3DCOLOR_RGBA(255, 255, 0, 255);
	g_Draw.StringA(g_Draw.font_esp, true, Left + width / 2, Bot.y, 255, 0, 0, flPlayerAlpha[pBomb->GetIndex()], "%s", "[C4]");

	if (is_planted) {
		CBomb* bomb = (CBomb*)pBomb;
		float flBlow = bomb->GetC4BlowTime();
		float TimeRemaining = flBlow - (g_pGlobals->interval_per_tick * pLocalPlayer->GetTickBase());
		if (TimeRemaining < 0)
			TimeRemaining = 0;
		char buffer[64];

		if (bomb->IsDefused())
			sprintf_s(buffer, XorStr("Defused"));
		else
			sprintf_s(buffer, XorStr("%.1fs Remaining!"), TimeRemaining);

		g_Draw.StringA(g_Draw.font_esp, true, Left + width / 2, Bot.y + 13, 255, 0, 0, 255, "%s", buffer);

		if (!bomb->IsDefused()) {
			float a = 450.7f;
			float b = 75.68f;
			float c = 789.2f;
			float d = ((pLocalPlayer->GetAbsOrigin().DistTo(pBomb->GetOrigin()) - b) / c);
			float flDamage = a * expf(-d * d);

			auto GetArmourHealth = [](float flDamage, int ArmorValue) {
				float flCurDamage = flDamage;

				if (flCurDamage == 0.0f || ArmorValue == 0)
					return flCurDamage;

				float flArmorRatio = 0.5f;
				float flArmorBonus = 0.5f;
				float flNew = flCurDamage * flArmorRatio;
				float flArmor = (flCurDamage - flNew) * flArmorBonus;

				if (flArmor > ArmorValue) {
					flArmor = ArmorValue * (1.0f / flArmorBonus);
					flNew = flCurDamage - flArmor;
				}

				return flNew;
			};

			float damage = max((int)ceilf(GetArmourHealth(flDamage, pLocalPlayer->GetArmor())), 0);

			g_Draw.StringA(g_Draw.font_esp, true, Left + width / 2, Bot.y + 26, 255, 0, 0, 255, "Damage: %f", damage);
		}
	}
}

void CESP::DrawDroppedWeapon(CBaseCombatWeapon* pWeapon)
{
	CBaseEntity* pCBaseEntity = (CBaseEntity*)pWeapon;
	CBaseCombatWeapon* Weapon = (CBaseCombatWeapon*)pWeapon;

	if (!pCBaseEntity || !g_pModelInfo)
		return;

	std::string sCurWeapon = g_pModelInfo->GetModelName(pCBaseEntity->GetModel());
	if (!sCurWeapon.empty() && sCurWeapon.find(XorStr("w_")) != std::string::npos) {
		std::string name;

		if (sCurWeapon.find(XorStr("defuser")) != std::string::npos /* insert check for defuser here*/)
			name = XorStr("defuser");
		else {
			CBaseCombatWeapon* pWeapon = (CBaseCombatWeapon*)pCBaseEntity;
			if (!pWeapon)
				return;
			name = GetWeaponName(pWeapon); 
		}

		Vector Bot, Top;
		float Left, Right;
		GetCBaseEntityBox(pCBaseEntity, Bot, Top, Left, Right, false);
		int width = Right - Left;
		int height = Bot.y - Top.y;

		float Red = Clientvariables->Colors.DroppedWeapon[0] * 255;
		float Green = Clientvariables->Colors.DroppedWeapon[1] * 255;
		float Blue = Clientvariables->Colors.DroppedWeapon[2] * 255;

		g_Draw.StringA(g_Draw.font_esp, true, Left + width / 2, Bot.y, Red, Green, Blue, 255, name.c_str());
		g_Draw.Box(Left, Top.y, width, height, 0, 0, 0, 255);
		g_Draw.Box(Left + 1, Top.y + 1, width - 2, height - 2, Red, Green, Blue, 255);
		g_Draw.Box(Left + 2, Top.y + 2, width - 4, height - 4, 0, 0, 0, 255);
	}
}

Vector MEME2(Vector EntityPos, Vector LocalPlayerPos, int posX, int posY, int sizeX, int sizeY, float angle, float zoom, bool* viewCheck)
{
	float r_1, r_2;
	float x_1, y_1;
	r_1 = -(EntityPos.y - LocalPlayerPos.y);
	r_2 = EntityPos.x - LocalPlayerPos.x;
	float Yaw = angle - 90.0f;
	float yawToRadian = Yaw * (float)(M_PI / 180.0F);
	x_1 = (float)(r_2 * (float)cos((double)(yawToRadian)) - r_1 * sin((double)(yawToRadian))) / 20;
	y_1 = (float)(r_2 * (float)sin((double)(yawToRadian)) + r_1 * cos((double)(yawToRadian))) / 20;
	*viewCheck = y_1 < 0;
	x_1 *= zoom;
	y_1 *= zoom;
	int sizX = sizeX / 2;
	int sizY = sizeY / 2;
	x_1 += sizX;
	y_1 += sizY;

	if (x_1 < 5)
		x_1 = 5;

	if (x_1 > sizeX - 5)
		x_1 = sizeX - 5;

	if (y_1 < 5)
		y_1 = 5;

	if (y_1 > sizeY - 5)
		y_1 = sizeY - 5;

	x_1 += posX;
	y_1 += posY;

	return Vector(x_1, y_1, 0);
}

FORCEINLINE float DotProduct64(const Vector& a, const Vector& b)
{
	return (a.x * b.x + a.y * b.y + a.z * b.z);
}

void CESP::Loop()
{
	if (!G::LocalPlayer)
		return;

	if (Clientvariables->Visuals.Bones && G::LocalPlayer->isAlive())
		DrawLinesAA();

	for (int i = 1; i <= g_pEntitylist->GetHighestEntityIndex(); i++) {
		CBaseEntity* pBaseEntity = g_pEntitylist->GetClientEntity(i);

		if (!pBaseEntity)
			continue;

		ClientClass* pClass = (ClientClass*)pBaseEntity->GetClientClass();

		if (pClass->m_ClassID == int(EClassIds::CCSPlayer) && Clientvariables->Visuals.EspEnable) {
			if (pBaseEntity->GetHealth() < 1)
				continue;

			bool is_friendly = pBaseEntity->GetTeamNum() == G::LocalPlayer->GetTeamNum();

			if (Clientvariables->Visuals.LocalPlayer && pBaseEntity == G::LocalPlayer)
				g_ESP->DrawPlayer(pBaseEntity, G::LocalPlayer);

			if (!is_friendly || is_friendly && !Clientvariables->Visuals.EnemyOnly)
				g_ESP->DrawPlayer(pBaseEntity, G::LocalPlayer);
		}

		Vector buf, pos = pBaseEntity->GetAbsOrigin();

		if (pos.x == 0 || pos.y == 0 || pos.z == 0 || !GameUtils::WorldToScreen(pos, buf))
			continue;

		if ((pClass->m_ClassID != int(EClassIds::CBaseWeaponWorldModel) && (strstr(pClass->m_pNetworkName, XorStr("Weapon")) || pClass->m_ClassID == int(EClassIds::CDEagle) || pClass->m_ClassID == int(EClassIds::CAK47))) && Clientvariables->Visuals.DroppedWeapons == 1)
			DrawDroppedWeapon((CBaseCombatWeapon*)pBaseEntity);

		if ((pClass->m_ClassID == int(EClassIds::CC4) || pClass->m_ClassID == int(EClassIds::CPlantedC4))) {
			if (Clientvariables->Visuals.Bomb)
				DrawC4(pBaseEntity, pClass->m_ClassID == int(EClassIds::CPlantedC4), G::LocalPlayer);
		}

	}
}

void CESP::DrawScope(CBaseEntity* pLocalPlayer)
{
	CBaseCombatWeapon* pLocalWeapon = pLocalPlayer->GetWeapon();

	if (pLocalWeapon) {
		if (pLocalWeapon->IsSniper()) {
			if (pLocalPlayer->IsScoped()) {
				int width = 0;
				int height = 0;
				g_pEngine->GetScreenSize(width, height);
				int centerX = static_cast<int>(width * 0.5f);
				int centerY = static_cast<int>(height * 0.5f);
				g_pSurface->SetDrawColor(Color(0, 0, 0, 255));
				g_pSurface->DrawLine(0, centerY, width, centerY);
				g_pSurface->DrawLine(centerX, 0, centerX, height);
			}
		}
	}
}
















