#include "gui.h"
#include "imgui.h"
#include "ogl.h"
#include "mb.h"
#include "implot.h"
#include <math.h>

#include <string>


bool save_window = false;
char save_input_text[255] = "";

int selected_tab = 0;

bool graf_window = false;

void init_gui(GLFWwindow *window) {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImPlot::CreateContext();
    ImGuiIO& io = ImGui::GetIO();

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init();
    ImGui::StyleColorsDark();
}


void gui_graf_view() {
    if(graf_window) {
        MB *selected = g_get_active_mb_obj();
        if(selected->get_type() == 1) {
            const char*  glabels[]   = {"1","2","4","8","16","32","64","128"};
            const double positions[] = {0,1,2,3,4,5,6,7,8};
            ImGui::Begin("OMP");
            double *data = &selected->omp_data[0];
            if (ImPlot::BeginPlot("OMP DATA")) {
                ImPlot::SetupLegend(ImPlotLocation_East, ImPlotLegendFlags_Outside);
                ImPlot::SetupAxisTicks(ImAxis_X1,positions,8, glabels);
                ImPlot::SetupAxes("Vlakna","Cas",ImPlotAxisFlags_AutoFit,ImPlotAxisFlags_AutoFit);
                ImPlot::PlotBars("OMP",data,8,0.4);
                ImPlot::EndPlot();
            }
            if(ImGui::Button("ZAVRIET")) { graf_window = false;}
            ImGui::End();
        }
        else if(selected->get_type() == 0 ) {
            ImGui::Begin("GPU");
            if(selected->gpu_data.size() == 10) {
                double *data = &selected->gpu_data[0];
                if (ImPlot::BeginPlot("GPU DATA")) {
                    ImPlot::SetupLegend(ImPlotLocation_East, ImPlotLegendFlags_Outside);
                    ImPlot::SetupAxes("It","Cas",ImPlotAxisFlags_AutoFit,ImPlotAxisFlags_AutoFit);
                    ImPlot::PlotBars("GPU",data,10,0.4);
                    ImPlot::EndPlot();
                }
            }
            else {
                ImGui::Text("Ziskavam Data! 10s");
            }
            if(ImGui::Button("ZAVRIET")) { graf_window = false;}
            ImGui::End();
        }
        else {
            ImGui::Begin("MPI");
            double *data = &selected->mpi_data[0];
            if (ImPlot::BeginPlot("MPI DATA")) {
                ImPlot::SetupLegend(ImPlotLocation_East, ImPlotLegendFlags_Outside);
                ImPlot::SetupAxes("N Process","Cas",ImPlotAxisFlags_AutoFit,ImPlotAxisFlags_AutoFit);
                ImPlot::PlotBars("MPI",data,3,0.4,1);
                ImPlot::EndPlot();
            }
            if(ImGui::Button("ZAVRIET")) { graf_window = false;}
            ImGui::End();
        }
    }
}

void gui_save_view() {
    if(save_window) {

        float pos_x = ( (float)g_get_screen_w() / 100 ) * 20;
        float pos_y = ( (float)g_get_screen_h() / 100 ) * 35;

        float size_w = ( (float)g_get_screen_w() / 100 ) * 40;
        float size_h = ( (float)g_get_screen_h() / 100 ) * 10;

        ImGui::SetNextWindowPos(ImVec2(pos_x,pos_y));
        ImGui::SetNextWindowSize(ImVec2(size_w,size_h));

        if(ImGui::Begin("Export",0,ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoSavedSettings)) {
            ImGui::InputText("",save_input_text,IM_ARRAYSIZE(save_input_text));
            ImGui::SameLine();
            if(ImGui::Button("ULOZIT"))
            {
                save_window = false;
                export_image(save_input_text);
                return;
            }
            ImGui::SameLine();
            if(ImGui::Button("ZRUSIT"))
            {
                save_window = false;
            }

            ImGui::End();
        }
    }
}

void gui_main_menu() {
    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("Subor"))
        {
            if (ImGui::MenuItem("Export")) 
            {
                save_window = true;
            }
            if (ImGui::MenuItem("Novy Tab")) 
            {
                g_add_mb_obj();
            }

            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }
}

void gui_fbo_view() {
    ImGui::SetNextWindowPos(ImVec2(0,19));

    float size_w = ( (float)g_get_screen_w() / 100 ) * 80;
    ImGui::SetNextWindowSize(ImVec2(size_w,g_get_screen_h() - 19));

    if(ImGui::Begin("FBO_RENDER",0,ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoBringToFrontOnFocus  )) {
        if(ImGui::BeginTabBar("Main"))
        {
            for(int i=0;i<g_get_mb_objs()->size();i++) {
                string id = "Tab "+ to_string(i);
                if(ImGui::BeginTabItem(id.c_str())) {
                    ImGui::BeginChild("OBJ");
                    MB* objekt = g_get_mb_objs()->at(i);
                    g_set_active_mb_obj(objekt);

                    ImVec2 size= ImGui::GetWindowSize();
                    unsigned int t_id=objekt->get_buffer()->texture;
                    ImGui::Image((ImTextureID)t_id,size,ImVec2(0,1),ImVec2(1,0));
                    ImGui::EndChild();

                    if (i!=selected_tab)
                        graf_window = false;
                    selected_tab = i;


                    ImGui::EndTabItem();
                } 
            }
            ImGui::EndTabBar();
        }
        ImGui::End();
    }

}

void gui_side_panel() {

    float size_w = ( (float)g_get_screen_w() / 100) * 20 ;
    float pos_w = ( (float)g_get_screen_w() / 100) * 80;

    ImGui::SetNextWindowPos(ImVec2(pos_w,19));
    ImGui::SetNextWindowSize(ImVec2(size_w,g_get_screen_h() - 19));


    if(ImGui::Begin("BRUH",0,ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoBringToFrontOnFocus  )) {
        MB *selected = g_get_active_mb_obj();

        int r = selected->get_r();
        int g = selected->get_g();
        int b = selected->get_b();

        int iter = selected->get_iter();
        float offset_x = selected->get_offset_x();
        float offset_y = selected->get_offset_y();
        float zoom = selected->get_zoom();


        int omp_n_threads = selected->get_omp_threads();

        const char* items[] = { "GPU", "OMP", "MPI"};
        int item_current_idx = selected->get_type();
        const char* combo_preview_value = items[item_current_idx];

    


        ImGui::Spacing();
        ImGui::Text("Farba");
        ImGui::DragInt("R", &r);
        ImGui::DragInt("G", &g);
        ImGui::DragInt("B", &b);
        ImGui::Separator();
        ImGui::Text("Nastavenie");
        if (ImGui::BeginCombo("", combo_preview_value))
        {
            for (int n = 0; n < IM_ARRAYSIZE(items); n++)
            {
                const bool is_selected = (item_current_idx == n);
                if (ImGui::Selectable(items[n], is_selected)) {
                    item_current_idx = n;
                    selected->set_type(item_current_idx);
                    graf_window = false;
                }

                if (is_selected)
                    ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }
        ImGui::DragFloat("OFFSET X", &offset_x);
        ImGui::DragFloat("OFFSET Y", &offset_y);
        ImGui::DragInt("Iteracie", &iter);
        ImGui::DragFloat("Zoom", &zoom);
        if(item_current_idx != 0 ) {
            ImGui::DragInt("N Vlakna", &omp_n_threads);
        }
        ImGui::Separator();
        if(ImGui::Button("Reset")) {
            selected->reset();
            return;
        }
        ImGui::SameLine();
        if(ImGui::Button("Vymaz")) {
            if(g_get_mb_objs()->size() > 1 ) {
                g_get_mb_objs()->erase(std::remove(g_get_mb_objs()->begin(), g_get_mb_objs()->end(), selected), g_get_mb_objs()->end());
            }
            return;
        }
        ImGui::SameLine();
        if(ImGui::Button("GRAF")) {
            graf_window = true;
            if(selected->get_type() == 0 ) {
                selected->gpu_test();
            }
            else if(selected->get_type() == 1) {
                selected->omp_test();
            }
            else {
                selected->mpi_test();
            }
        }
        string cas_t = "Cas " + to_string(selected->cas) + " ms";
        ImGui::Text(cas_t.c_str());

        selected->set_r(r);
        selected->set_g(g);
        selected->set_b(b);

        selected->set_offset_x(offset_x);
        selected->set_offset_y(offset_y);
        selected->set_iter(iter);
        selected->set_zoom(zoom);
        selected->set_omp_threads(omp_n_threads);


        ImGui::End();
    }
}


void draw_gui() {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    gui_main_menu();
    gui_fbo_view();
    gui_side_panel();
    gui_save_view();
    gui_graf_view();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
