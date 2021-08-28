#pragma once
#include "kiero/kiero.h"

#include "hooks/OcularHook.h"
#include "hooks/D3D11Hook.h"

namespace Ocular {
	OcularHook Init();
	void Shutdown();
}
