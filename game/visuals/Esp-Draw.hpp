#include "../../imgui/imgui.h"

void stroked_text(ImFont* font, float fontSize, ImVec2 position, ImColor color, const char* text)
{
	ImGui::GetForegroundDrawList()->AddText(font, fontSize, ImVec2(position.x - 1, position.y - 1), ImColor(0, 0, 0), text);
	ImGui::GetForegroundDrawList()->AddText(font, fontSize, ImVec2(position.x + 1, position.y - 1), ImColor(0, 0, 0), text);
	ImGui::GetForegroundDrawList()->AddText(font, fontSize, ImVec2(position.x - 1, position.y + 1), ImColor(0, 0, 0), text);
	ImGui::GetForegroundDrawList()->AddText(font, fontSize, ImVec2(position.x + 1, position.y + 1), ImColor(0, 0, 0), text);
	ImGui::GetForegroundDrawList()->AddText(font, fontSize, position, color, text);
}

void draw_box(int x, int y, int w, int h, const ImColor color)
{
	ImGui::GetForegroundDrawList()->AddRectFilled(ImVec2(x, y), ImVec2(x + w, y + h), ImColor(0, 0, 0, 50), 0, 0);
	ImGui::GetForegroundDrawList()->AddRect(ImVec2(x + 1, y + 1), ImVec2(x + w - 1, y + h - 1), ImColor(0, 0, 0), 0, 0, 1.0f);
	ImGui::GetForegroundDrawList()->AddRect(ImVec2(x - 1, y - 1), ImVec2(x + w + 1, y + h + 1), ImColor(0, 0, 0), 0, 0, 1.0f);
	ImGui::GetForegroundDrawList()->AddRect(ImVec2(x + 1, y - 1), ImVec2(x + w - 1, y + h + 1), ImColor(0, 0, 0), 0, 0, 1.0f);
	ImGui::GetForegroundDrawList()->AddRect(ImVec2(x - 1, y + 1), ImVec2(x + w + 1, y + h - 1), ImColor(0, 0, 0), 0, 0, 1.0f);
	ImGui::GetForegroundDrawList()->AddRect(ImVec2(x, y), ImVec2(x + w, y + h), color, 0, 0, 1.0f);
}

static void draw_corner_box(float x, float y, float w, float h, ImColor color)
{
	ImGui::GetForegroundDrawList()->AddRectFilled(ImVec2(x, y), ImVec2(x + w, y + h), ImColor(0, 0, 0, 100), 0, 0);
	ImGui::GetForegroundDrawList()->AddLine(ImVec2(x - 1, y - 1), ImVec2(x - 1, y + (h / 3) - 1), ImColor(0, 0, 0), 1.0f);
	ImGui::GetForegroundDrawList()->AddLine(ImVec2(x - 1, y - 1), ImVec2(x + (w / 3) - 1, y - 1), ImColor(0, 0, 0), 1.0f);
	ImGui::GetForegroundDrawList()->AddLine(ImVec2(x + w - (w / 3) + 1, y - 1), ImVec2(x + w + 1, y - 1), ImColor(0, 0, 0), 1.0f);
	ImGui::GetForegroundDrawList()->AddLine(ImVec2(x + w + 1, y - 1), ImVec2(x + w + 1, y + (h / 3) - 1), ImColor(0, 0, 0), 1.0f);
	ImGui::GetForegroundDrawList()->AddLine(ImVec2(x - 1, y + h - (h / 3) + 1), ImVec2(x - 1, y + h + 1), ImColor(0, 0, 0), 1.0f);
	ImGui::GetForegroundDrawList()->AddLine(ImVec2(x - 1, y + h + 1), ImVec2(x + (w / 3) - 1, y + h + 1), ImColor(0, 0, 0), 1.0f);
	ImGui::GetForegroundDrawList()->AddLine(ImVec2(x + w - (w / 3) + 1, y + h + 1), ImVec2(x + w + 1, y + h + 1), ImColor(0, 0, 0), 1.0f);
	ImGui::GetForegroundDrawList()->AddLine(ImVec2(x + w + 1, y + h - (h / 3) + 1), ImVec2(x + w + 1, y + h + 1), ImColor(0, 0, 0), 1.0f);
	ImGui::GetForegroundDrawList()->AddLine(ImVec2(x + 1, y + 1), ImVec2(x + 1, y + (h / 3) + 1), ImColor(0, 0, 0), 1.0f);
	ImGui::GetForegroundDrawList()->AddLine(ImVec2(x + 1, y + 1), ImVec2(x + (w / 3) + 1, y + 1), ImColor(0, 0, 0), 1.0f);
	ImGui::GetForegroundDrawList()->AddLine(ImVec2(x + w - (w / 3) - 1, y + 1), ImVec2(x + w - 1, y + 1), ImColor(0, 0, 0), 1.0f);
	ImGui::GetForegroundDrawList()->AddLine(ImVec2(x + w - 1, y + 1), ImVec2(x + w - 1, y + (h / 3) + 1), ImColor(0, 0, 0), 1.0f);
	ImGui::GetForegroundDrawList()->AddLine(ImVec2(x + 1, y + h - (h / 3) - 1), ImVec2(x + 1, y + h - 1), ImColor(0, 0, 0), 1.0f);
	ImGui::GetForegroundDrawList()->AddLine(ImVec2(x + 1, y + h - 1), ImVec2(x + (w / 3) + 1, y + h - 1), ImColor(0, 0, 0), 1.0f);
	ImGui::GetForegroundDrawList()->AddLine(ImVec2(x + w - (w / 3) - 1, y + h - 1), ImVec2(x + w - 1, y + h - 1), ImColor(0, 0, 0), 1.0f);
	ImGui::GetForegroundDrawList()->AddLine(ImVec2(x + w - 1, y + h - (h / 3) - 1), ImVec2(x + w - 1, y + h - 1), ImColor(0, 0, 0), 1.0f);
	ImGui::GetForegroundDrawList()->AddLine(ImVec2(x, y), ImVec2(x, y + (h / 3)), color, 1.0f);
	ImGui::GetForegroundDrawList()->AddLine(ImVec2(x, y), ImVec2(x + (w / 3), y), color, 1.0f);
	ImGui::GetForegroundDrawList()->AddLine(ImVec2(x + w - (w / 3), y), ImVec2(x + w, y), color, 1.0f);
	ImGui::GetForegroundDrawList()->AddLine(ImVec2(x + w, y), ImVec2(x + w, y + (h / 3)), color, 1.0f);
	ImGui::GetForegroundDrawList()->AddLine(ImVec2(x, y + h - (h / 3)), ImVec2(x, y + h), color, 1.0f);
	ImGui::GetForegroundDrawList()->AddLine(ImVec2(x, y + h), ImVec2(x + (w / 3), y + h), color, 1.0f);
	ImGui::GetForegroundDrawList()->AddLine(ImVec2(x + w - (w / 3), y + h), ImVec2(x + w, y + h), color, 1.0f);
	ImGui::GetForegroundDrawList()->AddLine(ImVec2(x + w, y + h - (h / 3)), ImVec2(x + w, y + h), color, 1.0f);
}