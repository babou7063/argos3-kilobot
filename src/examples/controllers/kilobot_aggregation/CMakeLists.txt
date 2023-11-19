add_library(aggregation_controller SHARED aggregation_controller.h aggregation_controller.cpp)

target_link_libraries(aggregation_controller
    argos3core_simulator
    argos3plugin_simulator_kilobot
    argos3plugin_simulator_genericrobot)