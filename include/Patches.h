#pragma once

namespace FEC
{
	namespace PATCH
	{
		void Install();
	}

	namespace POST_LOAD_PATCH
	{
		void Install();
	}

	namespace DISTRIBUTE
	{
		bool CanDeathEffectsBeAdded(RE::TESNPC* a_npc);

	    void Install();
	}
}
