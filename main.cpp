#include "includes.h"

typedef long(__stdcall* EndScene)(LPDIRECT3DDEVICE9);
LPDIRECT3DDEVICE9 pDevice = nullptr;
static EndScene oEndScene = NULL;
static HWND window = NULL;

WNDPROC oWndProc;

int screen_x = GetSystemMetrics(SM_CXSCREEN);
int screen_y = GetSystemMetrics(SM_CYSCREEN);
bool antialias_all = true;
float boxwidth = 0.5f;
int boxThickness = 2;
uintptr_t baseEngine;
int* iShotFired;
Vec3* aimRecoilPunch;
Vec3* viewAngles;
int fov = 30;

uintptr_t gameModule = (uintptr_t)GetModuleHandle("client.dll");

bool WorldToScreen(Vec3 pos, Vec2& screen, float matrix[16], int width, int height) {
    Vec4 clipCoords;
    clipCoords.x = pos.x * matrix[0] + pos.y * matrix[1] + pos.z * matrix[2] + matrix[3];
    clipCoords.y = pos.x * matrix[4] + pos.y * matrix[5] + pos.z * matrix[6] + matrix[7];
    clipCoords.z = pos.x * matrix[8] + pos.y * matrix[9] + pos.z * matrix[10] + matrix[11];
    clipCoords.w = pos.x * matrix[12] + pos.y * matrix[13] + pos.z * matrix[14] + matrix[15];

    if (clipCoords.w < 0.1f)
    {
        return false;
    }
    Vec3 NDC;
    NDC.x = clipCoords.x / clipCoords.w;
    NDC.y = clipCoords.y / clipCoords.w;
    NDC.z = clipCoords.z / clipCoords.w;

    screen.x = (width / 2 * NDC.x) + (NDC.x + width / 2);
    screen.y = -(height / 2 * NDC.y) + (NDC.y + height / 2);
    return true;
}

Vec2 GetBonePosition(uintptr_t Entity, int bone) {
    uintptr_t BoneMatrix_Base = *(uintptr_t*)(Entity + m_dwBoneMatrix);
    BoneMatrix Bone = *(BoneMatrix*)(BoneMatrix_Base + sizeof(Bone) * bone);
    Vec3 Location = { Bone.x, Bone.y, Bone.z };
    Vec2 ScreenCoords;
    float vMatrix[16];
    memcpy(&vMatrix, (PBYTE*)(gameModule + dwViewMatrix), sizeof(vMatrix));
    if (WorldToScreen(Location, ScreenCoords, vMatrix, screen_x, screen_y)) {
        return ScreenCoords;
    }
    return { 0, 0 };
}


extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

void InitImGui(LPDIRECT3DDEVICE9 pDevice) {
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\Arial.ttf", 25, NULL, io.Fonts->GetGlyphRangesCyrillic());
    io.ConfigFlags = ImGuiConfigFlags_NoMouseCursorChange;
    ImGui_ImplWin32_Init(window);
    ImGui_ImplDX9_Init(pDevice);
}

bool init = false;

bool crosshair = false;
bool openmenu = false;
bool triggerbot = false;
bool bunnyhop = false;
bool antiflash = false;
bool autorecoil = false;
bool radarhack = false;
bool wallhack = false;
bool fovchanger = false;

long __stdcall hkEndScene(LPDIRECT3DDEVICE9 o_pDevice)
{



    if (!pDevice)
    {
       pDevice = o_pDevice;
    }

    if (!init)
    {
        InitImGui(o_pDevice);
        init = true;
    }



    // functions

    if (wallhack) {
        uintptr_t LocalPlayer = *(uintptr_t*)(gameModule + dwLocalPlayer);
        if (LocalPlayer != NULL) {
            for (int x = 1; x < 32; x++) {
                int localTeam = *(int*)(LocalPlayer + m_iTeamNum);
                uintptr_t Entity = *(uintptr_t*)(gameModule + dwEntityList + x * 0x10);
                if (Entity != NULL) {
                    Vec2 ScreenPosition;
                    Vec2 HeadPosition;

                    int entityTeam = *(int*)(Entity + m_iTeamNum);
                    int entityHealth = *(int*)(Entity + m_iHealth);

                    if (entityTeam != localTeam && entityHealth > 0 && entityHealth <= 100) {

                        Vec3 EntityLocation = *(Vec3*)(Entity + m_vecOrigin);
                        float vMatrix[16];
                        memcpy(&vMatrix, (PBYTE*)(gameModule + dwViewMatrix), sizeof(vMatrix));
                        if (WorldToScreen(EntityLocation, ScreenPosition, vMatrix, screen_x, screen_y)) {

                            uintptr_t BoneMatrix_Base = *(uintptr_t*)(Entity + m_dwBoneMatrix);
                            BoneMatrix Bone = *(BoneMatrix*)(BoneMatrix_Base + sizeof(Bone) * 9);
                            Vec3 Location = { Bone.x, Bone.y, Bone.z + 10 };
                            Vec2 ScreenCoords;
                            float vMatrix[16];
                            memcpy(&vMatrix, (PBYTE*)(gameModule + dwViewMatrix), sizeof(vMatrix));
                            if (WorldToScreen(Location, ScreenCoords, vMatrix, screen_x, screen_y))
                            {

                                HeadPosition = ScreenCoords;
                                DrawBox(
                                    ScreenPosition.x - (((ScreenPosition.y - HeadPosition.y) * boxwidth) / 2),
                                    HeadPosition.y,
                                    (ScreenPosition.y - HeadPosition.y) * boxwidth,
                                    ScreenPosition.y - HeadPosition.y,
                                    boxThickness, antialias_all, D3DCOLOR_ARGB(255, 255, 255, 255)
                                );
                            }
                        }
                    }
                }
            }

        }
    }


    if (radarhack)
    {
        DWORD localPlayer = *(DWORD*)(gameModule + dwLocalPlayer);
        if (localPlayer != NULL) {
            int32_t myTeam = *(int32_t*)(localPlayer + m_iTeamNum);


            for (int i = 0; i < 32; ++i)
            {
                int32_t Entity = *(int32_t*)(gameModule + dwEntityList + i * 0x10);
                if (Entity == NULL) continue;
                int32_t entityTeam = *(int32_t*)(Entity + m_iTeamNum);
                int32_t entityDormant = *(int32_t*)(Entity + m_bDormant);
                if (entityTeam != myTeam && !entityDormant)
                {
                    *(bool*)(Entity + m_bSpotted) = true;
                }
            }
        }
    }

    if (bunnyhop)
    {
        DWORD localPlayer = *(DWORD*)(gameModule + dwLocalPlayer);

        if (localPlayer != NULL) {

            int32_t flag = *(int32_t*)(localPlayer + m_fFlags);

            if (GetAsyncKeyState(VK_SPACE) && flag & (1 << 0))
            {
                *(DWORD*)(gameModule + dwForceJump) = 6;
            }
        }
    }

    if (crosshair)
    {
        DrawFilledRect(screen_x / 2 - 2, screen_y / 2 - 2, 4, 4, D3DCOLOR_ARGB(255, 255, 255, 255));
    }

    if (antiflash)
    {
        uintptr_t localPlayer = *(uintptr_t*)(gameModule + dwLocalPlayer);
        if (localPlayer != NULL)
        {
            uintptr_t flashDuration = *(uintptr_t*)(localPlayer + m_flFlashDuration);
            if (flashDuration > 0)
            {
                *(uintptr_t*)(localPlayer + m_flFlashDuration) = 0;
            }
        }
    }

    if (autorecoil)
    {
        uintptr_t localPlayer = *(uintptr_t*)(gameModule + dwLocalPlayer);
        if (localPlayer != NULL)
        {
            iShotFired = (int*)(localPlayer + m_iShotsFired);
            aimRecoilPunch = (Vec3*)(localPlayer + m_aimPunchAngle);
            //viewAngle = (Vec3*)(*(uintptr_t*)baseEngine)
        }
    }

    if (triggerbot)
    {
        uintptr_t localPlayer = *(uintptr_t*)(gameModule + dwLocalPlayer);
        if (localPlayer != NULL) {

            uint32_t localTeam = *(int*)(localPlayer + m_iTeamNum);
            uint32_t CrosshairId = *(int*)(localPlayer + m_iCrosshairId);

            if (CrosshairId > 0 && CrosshairId < 32) {
                uintptr_t enemy = *(uintptr_t*)(gameModule + dwEntityList + (CrosshairId - 1) * 0x10);
                if (enemy != NULL) {
                    uint32_t enemyHealth = *(uintptr_t*)(enemy + m_iHealth);
                    uint32_t EnemyTeam = *(uintptr_t*)(enemy + m_iTeamNum);
                    
                    if (EnemyTeam != localTeam && enemyHealth > 0 && enemyHealth <= 100) {
                        mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
                        mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
                        Sleep(30);
                    }
                }   
            }
        }
    }

    if (fovchanger) {
        uintptr_t localPlayer = *(uintptr_t*)(gameModule + dwLocalPlayer);
        *(int*)(localPlayer + m_iDefaultFOV) = fov;

    }

    // panic
    if (GetAsyncKeyState(VK_END))
    {
        kiero::shutdown();
        return 0;
    }

    // open
    if (GetAsyncKeyState(VK_NUMPAD2) & 1)
    {
        openmenu = !openmenu;
    }

    if (openmenu)
    {
        ImGui_ImplDX9_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();
        auto flags = ImGuiConfigFlags_NoMouseCursorChange | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize;
        static int ttab;

        ImGui::Begin("CS:GO", nullptr, flags);
        if (ImGui::Button(xorstr(u8"Visual"), ImVec2(100.f, 0.f))) {
            ttab = 1;
        }
        ImGui::SameLine(0.f, 2.f);
       // if (ImGui::Button(xorstr(u8"Config"), ImVec2(100.f, 0.f))) {
        //    ttab = 2;
        //}
        ImGui::SameLine(0.f, 2.f);
        if (ImGui::Button(xorstr(u8"Combat"), ImVec2(100.f, 0.f))) {
            ttab = 3;
        }
        ImGui::SameLine(0.f, 2.f);
        if (ImGui::Button(xorstr(u8"Misc"), ImVec2(100.f, 0.f))) {
            ttab = 4;
        }

        if (ttab == 1)
        {
            ImGui::Checkbox(u8"Боксы", &wallhack);
            ImGui::Checkbox(u8"Нарисовать прицел", &crosshair);
        }
        else if (ttab == 3)
        {
            ImGui::Checkbox(u8"Триггер бот", &triggerbot);
        }
        else if (ttab == 4) {
            ImGui::Checkbox(u8"Банни-хоп", &bunnyhop);
            ImGui::Checkbox(u8"Радар-хак", &radarhack);
            ImGui::Checkbox(u8"Анти флеш", &antiflash);
            ImGui::Checkbox(u8"Анти отдача", &autorecoil);
            ImGui::Checkbox(u8"Fov changer", &fovchanger);
            if (fovchanger) {
                ImGui::SliderInt(xorstr(u8"FOV"), &fov, -180, 180);
            }
        }
        //else if (ttab == 2) {
            //ImGui::ColorEdit3(xorstr(u8"Цвет бокса"), Colors::boxColor);
            //ImGui::ColorEdit3(xorstr(u8"Цвет прицела"), Colors::crosshairColor);
       // }
        
        ImGui::End();

        ImGui::EndFrame();
        ImGui::Render();
        ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
    }



    return oEndScene(pDevice);
}

LRESULT _stdcall WndProc(const HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    if (true && ImGui_ImplWin32_WndProcHandler(hwnd, uMsg, wParam, lParam))
    {
        return true;
    }

    

    return CallWindowProc(oWndProc, hwnd, uMsg, wParam, lParam);
}

BOOL CALLBACK EnumWindowsCallBack(HWND handle, LPARAM lParam) {
    DWORD procID;
    GetWindowThreadProcessId(handle, &procID);

    if (GetCurrentProcessId() != procID)
        return TRUE;

    window = handle;
    return false;
}

HWND GetProcessWindow() {
    window = NULL;
    EnumWindows(EnumWindowsCallBack, NULL);
    return window;
}

int mainThread()
{
    if (kiero::init(kiero::RenderType::D3D9) == kiero::Status::Success) {
        kiero::bind(42, (void**)&oEndScene, hkEndScene);
        window = GetProcessWindow();
        oWndProc = (WNDPROC)SetWindowLongPtr(window, GWL_WNDPROC, (LONG_PTR)WndProc);
    }
    return 0;
}

BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD fdwReason, LPVOID)
{
    DisableThreadLibraryCalls(hInstance);

    if (fdwReason == DLL_PROCESS_ATTACH)
    {
        CloseHandle(CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)mainThread, NULL, 0, NULL));
    }

    return TRUE;
}