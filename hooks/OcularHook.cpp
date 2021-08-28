#include "OcularHook.h"

std::vector<std::function<void()>> OcularHook::renderFunctions;

void OcularHook::BindRenderFunction(std::function<void()> function) {
	auto func = std::bind(function);
	renderFunctions.push_back(func);
}

void OcularHook::ClearRenderFunctions() {
	renderFunctions.clear();
}

void OcularHook::RenderFrame() {
	ImGui::NewFrame();

	for (auto func : renderFunctions) {
		func();
	}

	ImGui::Render();
}