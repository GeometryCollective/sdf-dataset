// SDF Viewer - Visualize SDFs using Polyscope volume grids
//
// Usage:
//   sdf_viewer <sdf_name> [--resolution N] [--time T] [--seed S] [--list]
//
// Examples:
//   sdf_viewer Sphere
//   sdf_viewer Mandelbulb --resolution 64
//   sdf_viewer Fish --time 1.5
//   sdf_viewer --list

#include <iostream>
#include <string>
#include <vector>
#include <cstdlib>

#include "polyscope/polyscope.h"
#include "polyscope/slice_plane.h"
#include "polyscope/volume_grid.h"
#include "polyscope/implicit_helpers.h"

#include "sdf/sdf.hpp"

void printUsage(const char* progName) {
    std::cout << "Usage: " << progName << " <sdf_name> [options]\n"
              << "\n"
              << "Options:\n"
              << "  --resolution N, -r N   Grid resolution (default: 32)\n"
              << "  --time T, -t T         Time parameter for animated SDFs (default: 0.0)\n"
              << "  --seed S, -s S         Random seed for procedural SDFs (default: 12345)\n"
              << "  --list, -l             List all available SDFs\n"
              << "  --help, -h             Show this help message\n"
              << "\n"
              << "Examples:\n"
              << "  " << progName << " Sphere\n"
              << "  " << progName << " Mandelbulb --resolution 64\n"
              << "  " << progName << " Fish --time 1.5\n";
}

void listSDFs() {
    std::cout << "Available SDFs:\n";
    for (const auto& name : sdf::getAvailableSDFs()) {
        std::cout << "  " << name << "\n";
    }
}

int main(int argc, char* argv[]) {
    // Parse command line arguments
    std::string sdfName;
    uint32_t resolution = 32;
    float time = 0.0f;
    uint32_t seed = 12345;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "--help" || arg == "-h") {
            printUsage(argv[0]);
            return 0;
        }
        else if (arg == "--list" || arg == "-l") {
            listSDFs();
            return 0;
        }
        else if ((arg == "--resolution" || arg == "-r") && i + 1 < argc) {
            resolution = static_cast<uint32_t>(std::atoi(argv[++i]));
        }
        else if ((arg == "--time" || arg == "-t") && i + 1 < argc) {
            time = static_cast<float>(std::atof(argv[++i]));
        }
        else if ((arg == "--seed" || arg == "-s") && i + 1 < argc) {
            seed = static_cast<uint32_t>(std::atoi(argv[++i]));
        }
        else if (arg[0] != '-' && sdfName.empty()) {
            sdfName = arg;
        }
        else {
            std::cerr << "Unknown argument: " << arg << "\n";
            printUsage(argv[0]);
            return 1;
        }
    }
    
    if (sdfName.empty()) {
        std::cerr << "Error: No SDF name specified.\n\n";
        printUsage(argv[0]);
        return 1;
    }
    
    // Validate SDF name
    auto availableSDFs = sdf::getAvailableSDFs();
    bool found = false;
    for (const auto& name : availableSDFs) {
        if (name == sdfName) {
            found = true;
            break;
        }
    }
    
    if (!found) {
        std::cerr << "Error: Unknown SDF '" << sdfName << "'.\n";
        std::cerr << "Use --list to see available SDFs.\n";
        return 1;
    }

    std::cout << "Evaluating SDF '" << sdfName << "' on " 
              << resolution << "x" << resolution << "x" << resolution << " grid...\n";
    
    // Generate grid points in [0,1]^3 (node locations)
    std::vector<glm::vec3> points;
    points.reserve(resolution * resolution * resolution);
    
    // The grid spans [-1, 1]^3 to match typical SDF bounds
    const float minBound = -1.0f;
    const float maxBound = 1.0f;
    const float step = (maxBound - minBound) / static_cast<float>(resolution - 1);
    
    for (uint32_t z = 0; z < resolution; ++z) {
        for (uint32_t y = 0; y < resolution; ++y) {
            for (uint32_t x = 0; x < resolution; ++x) {
                float px = minBound + x * step;
                float py = minBound + y * step;
                float pz = minBound + z * step;
                points.emplace_back(px, py, pz);
            }
        }
    }
    
    // Evaluate SDF at all points
    std::vector<float> sdfValues;
    try {
        sdfValues = sdf::evaluate(sdfName, points, time, seed);
    } catch (const std::exception& e) {
        std::cerr << "Error evaluating SDF: " << e.what() << "\n";
        return 1;
    }
    
    std::cout << "SDF evaluation complete. Launching Polyscope...\n";
    
    // Initialize Polyscope
    polyscope::init();
    
    // Use shadow-only mode instead of the ground plane
    polyscope::options::groundPlaneMode = polyscope::GroundPlaneMode::ShadowOnly;
    
    // Register volume grid
    glm::vec3 boundLow(minBound, minBound, minBound);
    glm::vec3 boundHigh(maxBound, maxBound, maxBound);
    glm::uvec3 gridDim(resolution, resolution, resolution);
    
    polyscope::VolumeGrid* grid = polyscope::registerVolumeGrid(
        sdfName, gridDim, boundLow, boundHigh
    );
    
    // Add SDF values as a scalar quantity at nodes
    auto* scalarQ = grid->addNodeScalarQuantity("distance", sdfValues, polyscope::DataType::SYMMETRIC);
    scalarQ->setEnabled(true);
    scalarQ->setColorMap("coolwarm");
    scalarQ->setIsolinesEnabled(true);
    scalarQ->setIsolinePeriod(0.1f, false);
    
    // Enable isosurface extraction at distance = 0
    scalarQ->setIsosurfaceLevel(0.0f);
    scalarQ->setIsosurfaceVizEnabled(true);

    // Slice plane by default
    polyscope::SlicePlane* slicePlane = polyscope::addSlicePlane("SDF Slice Plane");
    slicePlane->setEnabled(true);
    slicePlane->setDrawPlane(false);
    slicePlane->setDrawWidget(true);
    slicePlane->setPose(glm::vec3(0.0f, 0.0f, 1.5f), glm::vec3(1.0f, 0.0f, 0.0f)); // move the widget off-center

    polyscope::DepthRenderImageQuantity* renderImg = nullptr;
   
    // wrapper to evaluate the SDF in Polyscope's batch format
    auto bactchEvalSDF = [&](float* inPos, float* outResult, size_t N) {
        std::vector<glm::vec3> pBatch(N);
        for (size_t i = 0; i < N; i++) {
            pBatch[i] = glm::vec3(inPos[3*i], inPos[3*i+1], inPos[3*i+2]);
        }
        std::vector<float> out = sdf::evaluate(sdfName, pBatch);
        for (size_t i = 0; i < N; i++) {
            outResult[i] = out[i];
        }
    };

    std::vector<std::string> uiModes = {"Isosurface Mesh", "Slice Volume", "Sphere March Render"};
    int currUIModeInd = -1;

    bool continuouslyRender = false;
    polyscope::ImplicitRenderOpts opts;
    opts.subsampleFactor = 2; // downsample the rendering for performance reasons

    auto callback = [&]() {

        // UI mode selector combobox
        bool modeChanged = false;
        if(currUIModeInd == -1) {
            currUIModeInd = 1; // first time setup
            modeChanged = true;
        }
        if (ImGui::BeginCombo("UI Mode", uiModes[currUIModeInd].c_str())) {
            for (int i = 0; i < uiModes.size(); i++) {
                bool isSelected = (currUIModeInd == i);
                if (ImGui::Selectable(uiModes[i].c_str(), isSelected)) {
                    currUIModeInd = i;
                    modeChanged = true;
                }
                if (isSelected) {
                    ImGui::SetItemDefaultFocus();
                }
            }
            ImGui::EndCombo();
        }

        if (modeChanged) {
            if (uiModes[currUIModeInd] == "Isosurface Mesh") {
                grid->setEnabled(true);
                scalarQ->setEnabled(true);
                scalarQ->setIsosurfaceVizEnabled(true);
                scalarQ->setGridcubeVizEnabled(false);
                slicePlane->setEnabled(false);
            }
            else if (uiModes[currUIModeInd] == "Slice Volume") {
                grid->setEnabled(true);
                scalarQ->setEnabled(true);
                scalarQ->setIsosurfaceVizEnabled(true);
                scalarQ->setGridcubeVizEnabled(true);
                slicePlane->setEnabled(true);
            }
            else if (uiModes[currUIModeInd] == "Sphere March Render") {
                grid->setEnabled(false);
                scalarQ->setEnabled(false);
                slicePlane->setEnabled(false);
            }
        }
        
        if (uiModes[currUIModeInd] == "Sphere March Render") {
            // setup options
            polyscope::ImplicitRenderMode mode = polyscope::ImplicitRenderMode::SphereMarch;

            // render the implicit isosurfaces from the current viewport
            if(ImGui::Button("Render Implicit Surface") || renderImg == nullptr) {
                renderImg = 
                    polyscope::renderImplicitSurfaceBatch("rendered", bactchEvalSDF, mode, opts);
                renderImg->setEnabled(true);
                grid->setEnabled(false);
            }
            ImGui::SameLine();
            bool changed = ImGui::Checkbox("Continuous Render", &continuouslyRender);
            if(continuouslyRender) {
                if(changed) {
                    grid->setEnabled(false);
                }
                renderImg = 
                    polyscope::renderImplicitSurfaceBatch("rendered", bactchEvalSDF, mode, opts);
                renderImg->setEnabled(true);
            }
            ImGui::InputInt("Render Subsample Factor", &opts.subsampleFactor);
            opts.subsampleFactor = std::max(1, opts.subsampleFactor);

        }

    };
    polyscope::state::userCallback = callback;
    
    // Enable isolines on the volume grid
    scalarQ->setIsolinesEnabled(true);
    
    // Show the visualization
    polyscope::show();
    
    return 0;
}


