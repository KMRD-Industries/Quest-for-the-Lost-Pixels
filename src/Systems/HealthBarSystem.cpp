#include "HealthBarSystem.h"

#include <imgui.h>

#include "AnimationSystem.h"
#include "CharacterComponent.h"
#include "Config.h"
#include "Helpers.h"

void HealthBarSystem::drawHealthBar()
{
    if (m_entities.empty())
        return;

    const auto& entity = *m_entities.begin();

    auto& characterComponent = gCoordinator.getComponent<CharacterComponent>(entity);

    const auto progressValue{characterComponent.hp / config::maxCharacterHP};

    const auto interpolatedColor{interpolateColor(config::lowHPColor, config::fullHPColor, progressValue)};

    ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(250, 0), ImGuiCond_Always);
    ImGui::Begin("Player HP", nullptr,
                 ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse |
                 ImGuiWindowFlags_NoTitleBar);

    ImGui::PushStyleColor(ImGuiCol_PlotHistogram, interpolatedColor);

    const ImVec2 available_size = ImGui::GetContentRegionAvail();

    ImGui::ProgressBar(progressValue, ImVec2(available_size.x, 0.0f), "");

    ImGui::PopStyleColor();

    ImGui::End();
}