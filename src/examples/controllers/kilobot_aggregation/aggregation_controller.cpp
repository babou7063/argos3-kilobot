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

}

void CAggregation_controller::Reset(){

}

void CAggregation_controller::ControlStep(){
    
    // collect des données 


    // traitement des données 


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