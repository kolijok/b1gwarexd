#pragma once

#include "Hooks.h"

class Spammers
{
public:
	void run(CUserCmd* m_pcmd);
	void clan_changer();
};

extern Spammers spammers;