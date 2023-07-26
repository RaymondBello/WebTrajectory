#include "state-core.h"

#include "icons-font-awesome.h"

#include <cmath>

namespace {

using TColor = uint32_t;

}

// utility structure for realtime plot
struct ScrollingBuffer
{
    int MaxSize;
    int Offset;
    ImVector<ImVec2> Data;
    ScrollingBuffer(int max_size = 2000)
    {
        MaxSize = max_size;
        Offset = 0;
        Data.reserve(MaxSize);
    }
    void AddPoint(float x, float y)
    {
        if (Data.size() < MaxSize)
            Data.push_back(ImVec2(x, y));
        else
        {
            Data[Offset] = ImVec2(x, y);
            Offset = (Offset + 1) % MaxSize;
        }
    }
    void Erase()
    {
        if (Data.size() > 0)
        {
            Data.shrink(0);
            Offset = 0;
        }
    }
};

//
// Rendering
//

void Rendering::init() {
    T = ImGui::GetTime();
    isAnimating = false;
    wSize = ImGui::GetContentRegionAvail();
}

void Rendering::animation(float i) {
    isAnimating |= (i != 0.0f && i < 1.0f);
}

//
// StateCore
//

void StateCore::onWindowResize() {
}

void StateCore::updateDataDummy() {
    dataDummy = "foo bar";
}

//
// UI methods
//

void StateCore::init(float fontScale) {
    //const float T = ImGui::GetTime();

    this->fontScale = fontScale;

    printf("Initialized the application state\n");
    isInitialized = true;
}

void StateCore::render() {
    // shortcuts
    const auto & T     = rendering.T;
    const auto & wSize = rendering.wSize;
    ImVec2 pos;
    float radius;

    if (rendering.isFirstFrame)
    {
        ImGui::SetStyle();

        rendering.isFirstFrame = false;
    }

    // full-screen window with the scene
    {
        ImGui::SetNextWindowPos({ 0.0f, 0.0f });
        ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);
        ImGui::BeginBorderless("background", NULL,
                               ImGuiWindowFlags_NoNav |
                               ImGuiWindowFlags_NoNavFocus |
                               ImGuiWindowFlags_NoBringToFrontOnFocus |
                               ImGuiWindowFlags_NoResize |
                               ImGuiWindowFlags_NoDecoration);

        // call after creating the main window
        rendering.init();

        auto drawList = ImGui::GetWindowDrawList();

        // draw a moving circle
        if (showCircle) {
            // pos.x = (0.5f + 0.25f * std::sin(2.0f * T)) * wSize.x;
            // pos.y = (0.5f + 0.35f * std::sin(3.0f * T + 1.0f)) * wSize.y;

            pos = ImVec2{
                (0.5f + 0.25f * std::sin(2.0f * T)) * wSize.x,
                (0.5f + 0.35f * std::sin(3.0f * T + 1.0f)) * wSize.y,
            };

            radius = 4.0f + 16.0f*std::fabs(std::sin(T));
            const TColor color = ImGui::ColorConvertFloat4ToU32({ 0.0f, 1.0f, 0.1f, 0.8f, });

            drawList->AddCircleFilled(pos, radius, color);

            rendering.isAnimating = true;
        }

        // animation indicator in the lower-left corner of the screen
        if (rendering.isAnimating) {
            drawList->AddRectFilled({ 0.0f, wSize.y - 6.0f, }, { 6.0f, wSize.y, }, ImGui::ColorConvertFloat4ToU32({ 1.0f, 1.0f, 0.0f, 1.0f, }));
        }
    }

    // layer with controls
    {
        ImGui::SetNextWindowPos({ 0.0f, 0.0f });
        ImGui::SetNextWindowSize({ImGui::GetIO().DisplaySize[0] /4, ImGui::GetIO().DisplaySize[1]});
        ImGui::Begin("main", NULL,
                     ImGuiWindowFlags_NoNav |
                     ImGuiWindowFlags_NoResize |
                     ImGuiWindowFlags_NoDecoration |
                     ImGuiWindowFlags_NoBackground);

        {
            // select font
            ImGui::FontSentry sentry(0, 1.0f/fontScale);

            ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_None;
            if (ImGui::BeginTabBar("TrajectoryTabs", tab_bar_flags))
            {
                if (ImGui::BeginTabItem(ICON_FA_SLIDERS_H " Parameters"))
                {
                    ImGui::Text("Window size: %6.3f %6.3f\n", wSize.x, wSize.y);
                    ImGui::Text("Mouse down duration: %g\n", ImGui::GetIO().MouseDownDuration[0]);

                    ImGui::Checkbox("Show circle", &showCircle);

                    ImGui::Text("Time: %5.3f", T);
                    ImGui::Text("Circle Position: %5.3f, %5.3f", pos.x, pos.y);

                    // Setup Scrolling buffer
                    static ScrollingBuffer sdata1, sdata2, sdata3;
                    static float dt = 0;
                    dt += ImGui::GetIO().DeltaTime;

                    sdata1.AddPoint(dt, pos.x);
                    sdata2.AddPoint(dt, pos.y);
                    sdata3.AddPoint(dt, radius);

                    static float history = 10.0f;
                    ImGui::SliderFloat("History", &history, 1, 30, "%.1f s");

                    static ImPlotAxisFlags flags = ImPlotAxisFlags_None;

                    if (ImPlot::BeginPlot("Circle Position", ImVec2(-1, 150)))
                    {
                        ImPlot::SetupAxes(nullptr, nullptr, flags, flags);
                        ImPlot::SetupAxisLimits(ImAxis_X1, dt - history, dt, ImGuiCond_Always);
                        // ImPlot::SetupAxisLimits(ImAxis_Y1, 0, 1);
                        // ImPlot::SetNextFillStyle(IMPLOT_AUTO_COL, 0.5f);
                        ImPlot::PlotLine("Circle X", &sdata1.Data[0].x, &sdata1.Data[0].y, sdata1.Data.size(), 0, sdata1.Offset, 2 * sizeof(float));
                        ImPlot::PlotLine("Circle Y", &sdata2.Data[0].x, &sdata2.Data[0].y, sdata2.Data.size(), 0, sdata2.Offset, 2 * sizeof(float));
                        ImPlot::PlotLine("Circle Radius", &sdata3.Data[0].x, &sdata3.Data[0].y, sdata3.Data.size(), 0, sdata3.Offset, 2 * sizeof(float));

                        ImPlot::EndPlot();
                    }

                    ImGui::EndTabItem();
                }
                if (ImGui::BeginTabItem(ICON_FA_CHART_LINE " Charts"))
                {
                    ImGui::Text("This is the Charts tab!\n");

                    ImGui::EndTabItem();
                }
                if (ImGui::BeginTabItem(ICON_FA_COG " Settings"))
                {
                    
                    ImGui::Text("FA ICON COG: " ICON_FA_COG);

                    ImGui::Button("Push data to JS", {200.0f, 24.0f});
                    if (ImGui::IsItemHovered(ImGuiHoveredFlags_None) && ImGui::IsMouseJustPressed(0))
                    {
                        updateDataDummy();
                    }

                    ImGui::Button("Copy to clipboard", {200.0f, 24.0f});
                    if (ImGui::IsItemHovered(ImGuiHoveredFlags_None) && ImGui::IsMouseJustPressed(0))
                    {
                        dataClipboard = "Some clipboard data from the native app";
                    }

                    ImGui::Button("Open https://google.com", {200.0f, 24.0f});
                    if (ImGui::IsItemHovered(ImGuiHoveredFlags_None) && ImGui::IsMouseJustPressed(0))
                    {
                        dataURL = "https://google.com";
                    }
                    ImGui::Text("This is the Settings tab!\n");
                    ImGui::EndTabItem();
                }
                ImGui::EndTabBar();
            }
        }

        ImGui::End();
    }
    // ImGui::ShowDemoWindow();
    // ImPlot::ShowDemoWindow();

    ImGui::End();
}

bool StateCore::updatePre() {
    //const float T = ImGui::GetTime();

#ifndef EMSCRIPTEN
    if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Escape))) {
        printf("Escape pressed - exiting\n");
        return false;
    }
#endif

    return true;
}

bool StateCore::updatePost() {
    return true;
}

void StateCore::deinitMain() {
}
