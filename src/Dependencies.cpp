#include "Dependencies.h"

namespace Dependencies
{
	std::string CheckErrors()
	{
		const auto papyrusExtenderHandle = GetModuleHandleA(PapyrusExtender.data());

		if (papyrusExtenderHandle == nullptr) {
			logger::error("PapyrusExtender SSE plugin not found | error {}", GetLastError());

			return "[FEC] Papyrus Extender is not installed! Mod will not work correctly!\n";
		} else {
			const auto peGetVersion = reinterpret_cast<_PEGETVERSION>(GetProcAddress(papyrusExtenderHandle, "GetPluginVersion"));
			if (peGetVersion != nullptr) {
				Version currentPE{ peGetVersion() };
				if (currentPE < requiredPE) {
					return fmt::format("[FEC] Papyrus Extender is out of date! FEC requires {} or higher; current PE version is {}\n", requiredPE, currentPE);
				}
			} else {
				logger::error("Failed version check info from PapyrusExtender | error {} ", GetLastError());
			}
		}

		return {};
	}
}
