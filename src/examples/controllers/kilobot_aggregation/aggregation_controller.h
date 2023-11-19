/* Premier test de l'implémentation d'un controller pour l'agreggation*/

#ifndef CAGGREGATION_CONTROLLER_H
#define CAGGREGATION_CONTROLLER_H

#include <argos3/core/control_interface/ci_controller.h>
#include <argos3/core/utility/math/vector2.h>

using namespace argos;

enum MovementState {KILOBOT_STATE_STOP, KILOBOT_STATE_TURNING, KILOBOT_STATE_MOVING};

enum SurrondingRobots {KILOBOT_ALONE, KILOBOT_NEXT_TO_F, KILOBOT_NEXT_TO_E, KILOBOT_NEXT_TO_FE};


/*
 * implémentation de la classe CCI_Controller
 */
class CAggregation_controller : public CCI_Controller {

public:

    /* Class constructeur */
    aggregation_controller();

    /* Class destructeur*/
    virtual ~aggregation_controller() {}

    /* Init */
    virtual void Init(TConfigurationNode& t_node);

    /* Loop fonction de controle */
    virtual void ControlStep();

    /* Reset */
    virtual void reset();

    /* Destroye */
    virtual void Destroy();

    /* Récupération des états */
    const MovementState GetCurrentMovementState();
    const MovementState GetPreviousMovementState();

    const SurrondingRobots GetCurrentSurrondingRobots();
    const SurrondingRobots GetPreviousSurrondingRobots();

private:

    /*Variable et fonction spécifique*/
    MovementState m_tCurrentMovementState;
    MovementState mt_PreviousMovementState;
    
    SurrondingRobots m_tCurrentSurrondingRobots;
    SurrondingRobots m_tPreviousSurrondingRobots;  
};

#endif // CAGGREGATION_CONTROLLER_H