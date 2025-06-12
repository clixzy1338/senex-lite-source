#include "../driver/comm.hpp"
#include "../sdk-and-offsets/sdk.hpp"
#include "../sdk-and-offsets/offsets.hpp"
#include "../imgui/imgui.h"
#include "../settings/settings.hpp"
#include "visuals/Esp-Draw.hpp"

void actorLoop()
{
    cache::uworld = read<uintptr_t>(sigmabase + UWORLD);
 //   std::cout << "uworld: 0x" << std::hex << cache::uworld << std::endl;  
    cache::game_instance = read<uintptr_t>(cache::uworld + GAME_INSTANCE);
    cache::local_players = read<uintptr_t>(read<uintptr_t>(cache::game_instance + LOCAL_PLAYERS));
    cache::player_controller = read<uintptr_t>(cache::local_players + PLAYER_CONTROLLER);
    cache::local_pawn = read<uintptr_t>(cache::player_controller + LOCAL_PAWN);
    cache::root_component = read<uintptr_t>(cache::local_pawn + ROOT_COMPONENT);
    cache::player_state = read<uintptr_t>(cache::local_pawn + PLAYER_STATE);
    cache::current_weapon = read<uintptr_t>(cache::local_pawn + CURRENT_WEAPON);
    cache::localRelativeLocation = read<Vector3>(cache::root_component + RELATIVE_LOCATION);
    if (cache::local_pawn != 0)
    {
    cache::my_team_id = read<int>(cache::player_state + TEAM_INDEX);
    }
    cache::closest_distance = FLT_MAX;
    cache::closest_mesh = NULL;

    uintptr_t game_state = read<uintptr_t>(cache::uworld + GAME_STATE);
    if (!game_state) return;
    uintptr_t player_array = read<uintptr_t>(game_state + PLAYER_ARRAY);
    int player_count = read<int>(game_state + (PLAYER_ARRAY + sizeof(uintptr_t)));

    ImGuiIO& io = ImGui::GetIO();
    const float screenWidth = io.DisplaySize.x;
    const float screenHeight = io.DisplaySize.y;
    auto draw_list = ImGui::GetForegroundDrawList();

    if (settings::RenderCount)
    {
        char rendercount[64];
        sprintf_s(rendercount, "Render Count: %d", player_count);
        ImVec2 text_size = ImGui::CalcTextSize(rendercount);
        draw_list->AddText(ImGui::GetFont(), 15.0f, ImVec2((screenWidth - text_size.x) / 2.0f, 80.0f), ImColor(255, 0, 0, 255), rendercount);
    }

    if (settings::rFovCircle)
        draw_list->AddCircle(ImVec2(screenWidth / 2, screenHeight / 2), settings::rFovSize, ImColor(255, 255, 255), 64);

    for (int i = 0; i < player_count; i++)
    {
        uintptr_t player_state = read<uintptr_t>(player_array + (i * sizeof(uintptr_t)));
        if (!player_state) continue;

        int player_team_id = read<int>(player_state + TEAM_INDEX);

        if (settings::visuals::ignore_teamates && cache::local_pawn) {
            if (cache::my_team_id == player_team_id)
                continue;
        }

        uintptr_t pawn_private = read<uintptr_t>(player_state + PAWN_PRIVATE);
        if (!pawn_private || pawn_private == cache::local_pawn) continue;

       // if (is_dead(pawn_private)) continue;

        uintptr_t mesh = read<uintptr_t>(pawn_private + MESH);
        if (!mesh) continue;

        uintptr_t actorRootComponent = read<uintptr_t>(pawn_private + ROOT_COMPONENT);
        Vector3 actorRelativeLocation = read<Vector3>(actorRootComponent + RELATIVE_LOCATION);
        float distance = cache::localRelativeLocation.distance(actorRelativeLocation) / 100.0f;


        if (settings::visuals::enable && settings::visuals::radar && distance <= 100.0f)
        {
            Vector2 radarPos = worldToRadar(actorRelativeLocation, cache::localRelativeLocation, cache::local_camera.rotation, 200);
            cache::radarPoints.push_back(ImVec2(radarPos.x, radarPos.y));
        }


        if (!settings::visuals::enable || distance > settings::visuals::renderDistance) continue;


        Vector3 head3d = get_entity_bone(mesh, 110);
        Vector2 head2d = project_world_to_screen(head3d);
        Vector3 bottom3d = get_entity_bone(mesh, 0);
        Vector2 bottom2d = project_world_to_screen(bottom3d);
        Vector2 boxHead = project_world_to_screen(Vector3(head3d.x, head3d.y, head3d.z + 20.0f));
        float box_height = abs(boxHead.y - bottom2d.y);
        float box_width = box_height * 0.50f;
        bool visible = is_visible(mesh);


        if (settings::visuals::box)
        {
            auto color = visible ? settings::colors::icBoxColorVisible : settings::colors::icBoxColorInvisible;
            if (settings::visuals::boxType == boxType::normal)
                draw_box(boxHead.x - box_width / 2, boxHead.y, box_width, box_height, color);
            else if (settings::visuals::boxType == boxType::corner)
                draw_corner_box(boxHead.x - box_width / 2, boxHead.y, box_width, box_height, color);
        }


        if (settings::visuals::skeleton)
        {
            ImU32 skeleton_color = visible ? settings::colors::icSkeletonColorVisible : settings::colors::icSkeletonColorInvisible;
            for (const auto& bonePair : boneConnections)
            {
                Vector2 wts1 = project_world_to_screen(get_entity_bone(mesh, bonePair.bone1));
                Vector2 wts2 = project_world_to_screen(get_entity_bone(mesh, bonePair.bone2));

                draw_list->AddLine(ImVec2(wts1.x, wts1.y), ImVec2(wts2.x, wts2.y), ImColor(0, 0, 0), 2.0f);
                draw_list->AddLine(ImVec2(wts1.x, wts1.y), ImVec2(wts2.x, wts2.y), skeleton_color, 1);
            }
        }


        if (settings::visuals::line)
        {
            ImU32 tracer_color = visible ? settings::colors::icTracerColorVisible : settings::colors::icTracerColorInvisible;
            draw_list->AddLine(ImVec2(settings::screen_center_x, settings::height), ImVec2(bottom2d.x, bottom2d.y), tracer_color, 1.0f);
        }


        if (settings::visuals::name)
        {
            std::string playerUsername = GetPlayerName(player_state);
            ImVec2 textSize = ImGui::CalcTextSize(playerUsername.c_str());
            stroked_text(ImGui::GetFont(), 13.0f, ImVec2(boxHead.x - textSize.x / 2, boxHead.y - 10 - textSize.y / 2), ImColor(255, 255, 255), playerUsername.c_str());
        }


        if (settings::visuals::platform)
        {
            uintptr_t platformPtr = read<uintptr_t>(player_state + PLATFORM);
            wchar_t platformChar[64] = { 0 };
            Driver::read_physical(reinterpret_cast<PVOID>(platformPtr), reinterpret_cast<uint8_t*>(platformChar), sizeof(platformChar));
            std::wstring platformWstr(platformChar);
            std::string platformStr(platformWstr.begin(), platformWstr.end());

            ImVec2 textSize = ImGui::CalcTextSize(platformStr.c_str());
            float offsetY = settings::visuals::name ? 25.0f : 10.0f;
            stroked_text(ImGui::GetFont(), 13.0f, ImVec2(boxHead.x - textSize.x / 2, boxHead.y - offsetY - textSize.y / 2), ImColor(255, 255, 255), platformStr.c_str());
        }


        if (settings::visuals::distance)
        {
            char dist[64];
            sprintf_s(dist, "%.fm", distance);
            ImVec2 textSize = ImGui::CalcTextSize(dist);
            stroked_text(ImGui::GetFont(), 13.0f, ImVec2(bottom2d.x - textSize.x / 2, bottom2d.y + 10 - textSize.y / 2), ImColor(255, 255, 255), dist);
        }


        if (settings::visuals::rank)
        {
            uintptr_t habaneroComponent = read<uintptr_t>(player_state + HABANERO_COMPONENT);
            uint32_t rank = read<uint32_t>(habaneroComponent + RANKED_PROGRESS + 0x10);
            std::string rankName = getRank(rank);
            ImVec2 textSize = ImGui::CalcTextSize(rankName.c_str());
            float offsetY = settings::visuals::distance ? 25.0f : 10.0f;
            stroked_text(ImGui::GetFont(), 13.0f, ImVec2(bottom2d.x - textSize.x / 2, bottom2d.y + offsetY - textSize.y / 2), ImColor(255, 255, 255), rankName.c_str());
        }

        auto distToCross = getCrossDistance(head2d.x, head2d.y, settings::width / 2, settings::height / 2);
        if (distToCross <= settings::aimbot::fov && distToCross < cache::closest_distance)
        {
            cache::closest_distance = distToCross;
            cache::closest_mesh = mesh;
        }
    }
}
