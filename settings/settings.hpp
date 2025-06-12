#pragma once
#include "../imgui/imgui.h"

struct bone
{
    int bone1;
    int bone2;
    bone(int b1, int b2) : bone1(b1), bone2(b2) {}
};
inline bone boneConnections[] = {
    bone(110, 67), // head to neck
    bone(67, 7), // neck to chest
    bone(7, 2), // chest to pelvis
    bone(7, 9), // chest to right shoulder
    bone(9, 35), // right shoulder to right elbow
    bone(35, 11), // right elbow to right wrist
    bone(7, 38), // chest to left shoulder
    bone(38, 39), // left shoulder to left elbow
    bone(39, 40), // left elbow to left wrist
    bone(2, 71), // pelvis to right hip
    bone(71, 72), // right hip to right knee
    bone(72, 73), // right knee to right shin
    bone(73, 75), // right shin to right ankle
    bone(2, 78), // pelvis to left hip
    bone(78, 79), // left hip to left knee
    bone(79, 80), // left knee to left shin
    bone(80, 82), // left shin to left ankle
};

enum hitboxType : int
{
    head = 110,
    neck = 67,
    chest = 7,
    ass = 69,
};
inline int hitboxValues[] = {
    hitboxType::head,
    hitboxType::neck,
    hitboxType::chest,
    hitboxType::ass,
};
inline const char* getHitboxTypeName(int type)
{
    switch (type)
    {
        case hitboxType::head: return "Head";
        case hitboxType::neck: return "Neck";
        case hitboxType::chest: return "Chest";
        case hitboxType::ass: return "Ass";
        default: return "Unknown";
    }
}

enum boxType : int
{
    normal = 0,
    corner = 1,
};
inline int boxValues[] = {
    boxType::normal,
    boxType::corner,
};
inline const char* getBoxTypeName(int type)
{
    switch (type)
    {
    case boxType::normal: return "Normal";
    case boxType::corner: return "Corner";
    default: return "Unknown";
    }
}

namespace settings
{
    inline int width = GetSystemMetrics(SM_CXSCREEN);
    inline int height = GetSystemMetrics(SM_CYSCREEN);
    inline int screen_center_x = width / 2;
    inline int screen_center_y = height / 2;
    inline bool debug = false;
    inline bool show_menu = true;
    inline int tab = 0;

    namespace aimbot
    {
        inline bool enable = false;
        inline bool mouseAim = false;
        inline bool silentAim = false;
        inline bool bulletPrediction = false;
        inline bool crosshair = false;
        inline bool show_fov = false;
        inline float fov = 135;
        inline float smoothness = 3.7;
        inline static const char* aimkey[] = { "Left Mouse Button", "Right Mouse Button", "Mouse Button 4", "Mouse Button 5", "Shift", "Ctrl", "ALT", "Caps Lock" };
        inline static int current_aimkey = 1; // default to Right Mouse Button
        inline static int current_key;
        inline static int current_hitbox = hitboxType::head;
    }
    namespace visuals
    {
        inline bool enable = false;
        inline bool radar = false;
        inline bool ignore_teamates = true;
        inline bool box = false;
        inline bool skeleton = false;
        inline bool line = false;
        inline bool name = false;
        inline bool platform = false;
        inline bool distance = false;
        inline bool rank = false;
        inline float renderDistance = 300.0f;
        inline int boxType = boxType::normal;
    }
    namespace colors
    {
        inline ImColor icCrosshairColor = ImColor(255, 0, 255);
        inline ImColor icFovColor = ImColor(255, 0, 255);
        inline ImColor icBoxColorVisible = ImColor(255, 0, 0);
        inline ImColor icBoxColorInvisible = ImColor(255, 0, 255);
        inline ImColor icSkeletonColorVisible = ImColor(255, 0, 0);
        inline ImColor icSkeletonColorInvisible = ImColor(255, 0, 255);
        inline ImColor icTracerColorVisible = ImColor(255, 0, 0);
        inline ImColor icTracerColorInvisible = ImColor(255, 0, 255);
    }

    inline bool RenderCount;
    inline int rFovSize = 200;
    inline bool rFovCircle;

}