/* controller definition*/
#include "aggregation_controller.h"
/* XML parsing*/
#include <argos3/core/utility/configuration/argos_configuration.h>
/* verteur 2d*/
#include <argos3/core/utility/math/vector2.h>
#include <vector>
#include <algorithm>
/*Log pour debug*/
#include <argos3/core/utility/logging/argos_log.h>
/*Kilolib*/
#include <argos3/plugins/robots/kilobot/control_interface/kilolib.h>

CAggregation_controller::CAggregation_controller():
    
    m_tCurrentSurrondingRobots(KILOBOT_ALONE),
    m_tPreviousSurrondingRobots(KILOBOT_ALONE),
    m_tCurrentMovementState(KILOBOT_STATE_STOP),
    mt_PreviousMovementState(KILOBOT_STATE_STOP)
{
    // init du constructeur
}

void CAggregation_controller::Init(TConfigurationNode& t_node){
    GetNodeAttributeOrDefault(t_node, "team_kilo", team_kilo, team_kilo);
    GetNodeAttributeOrDefault(t_node, "rcv_timing", rcv_timing, rcv_timing);

    SetLinearVelocity(100,100);

    //delay(2000);

    if (team_kilo == 1){
        //set_color(RGB(255,0,0));
    }
    else{
        //set_color(RGB(0,255,0));
    }

    Reset();
}

void CAggregation_controller::Reset(){
    m_tCurrentSurrondingRobots = KILOBOT_ALONE;
    m_tPreviousSurrondingRobots = KILOBOT_ALONE;

    m_tCurrentMovementState = KILOBOT_STATE_STOP;
    mt_PreviousMovementState = KILOBOT_STATE_STOP;
}

void CAggregation_controller::SendMessage(){
    LOG << "[INFO] SendMessage" << std::endl;
}   

void CAggregation_controller::ReceiveMessages(){
    LOG << "[INFO] ReceiveMessages" << std::endl;
}

void CAggregation_controller::SetStateSurrondingRobots(){
    LOG << "[INFO] SetStateSurrondingRobots" << std::endl;
}

void CAggregation_controller::RW_Alone(){
    LOG << "[INFO] RW_Alone" << std::endl;
}

void CAggregation_controller::RW_NF(){
    LOG << "[INFO] RW_NF" << std::endl;
}

void CAggregation_controller::RW_NE(){
    LOG << "[INFO] RW_NE" << std::endl;
}

void CAggregation_controller::RW_NFE(){
    LOG << "[INFO] RW_NFE" << std::endl;
}

void CAggregation_controller::ControlStep(){
    
    // Envoie du message durant rcv_timing ms
    SendMessage();

    // Reception des messages
    ReceiveMessages();

    // Change l'état en fct des mess reçut
    SetStateSurrondingRobots();

    // Action de random walk
    switch (m_tCurrentSurrondingRobots){

    case KILOBOT_ALONE:
        // random walk Alone
        RW_Alone();
        break;

    case KILOBOT_NEXT_TO_E:
        // random walk next to an enemy
        RW_NE();
        break;

    case KILOBOT_NEXT_TO_F:
        // random walk next to a friend
        RW_NF();
        break;

    case KILOBOT_NEXT_TO_FE:
        // random walk next to a friend and an enemy
        RW_NFE();
        break;
    }
}

REGISTER_CONTROLLER(CAggregation_controller, "kilobot_aggregation_controller")