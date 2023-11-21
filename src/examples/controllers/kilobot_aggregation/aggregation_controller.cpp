/* controller definition*/
#include "aggregation_controller.h"
/* XML parsing*/
#include <argos3/core/utility/configuration/argos_configuration.h>
/* verteur 2d*/
#include <argos3/core/utility/math/vector2.h>
#include <vector>
#include <algorithm>

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

    if (team_kilo == "1"){
        set_color(RGB(255, 0, 0));
        printf("team rouge")
    }
    else{
        set_color(RGB(0, 255, 0));
        log("team verte")
    }

    Reset();
}

void CAggregation_controller::Reset(){
    m_tCurrentMovementState = KILOBOT_ALONE;
    mt_PreviousMovementState = KILOBOT_ALONE;
    
    m_tCurrentSurrondingRobots = KILOBOT_STATE_STOP;
    m_tPreviousSurrondingRobots = KILOBOT_STATE_STOP;
}

void CAggregation_controller::ControlStep(){
    
    // Envoie de l'équipe
    message_t msg;
    msg.data[0] = team_kilo;
    msg.type = NORMAL;
    msg.crc = message_crc(&msg);


    kilo_message_tx = msg;
    // register tranmsission success callback
    kilo_message_tx_success = tx_message_success;


    // récupération équipe adverse



    // action
    switch (m_tCurrentSurrondingRobots){

    case KILOBOT_ALONE:
        // random walk Alone
        break;

    case KILOBOT_NEXT_TO_E:
        // random walk next to an enemy
        break;

    case KILOBOT_NEXT_TO_F:
        // random walk next to a friend
        break;

    case KILOBOT_NEXT_TO_FE:
        // random walk next to a friend and an enemy
        break;

    default:
        // gerer les posssibles erreurs ici
        break;
    }
}

REGISTER_CONTROLLER(CAggregation_controller, "kilobot_aggregation_controller")