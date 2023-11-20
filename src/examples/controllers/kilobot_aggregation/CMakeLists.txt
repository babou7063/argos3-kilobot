add_library(kilobot_aggregation SHARED aggregation_controller.h aggregation_controller.cpp)

target_link_libraries(kilobot_aggregation
    argos3core_simulator
    argos3plugin_simulator_kilobot
    argos3plugin_simulator_genericrobot)