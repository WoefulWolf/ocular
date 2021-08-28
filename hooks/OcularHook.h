#pragma once
#include <Windows.h>
#include <vector>
#include <functional>

#include "../imgui/imgui.h"

class OcularHook {
private:
	static std::vector<std::function<void()>> renderFunctions;
public:
	bool hooked = false;
	void BindRenderFunction(std::function<void()> function);
	void ClearRenderFunctions();
protected:
	static void RenderFrame();
};