/* Premier test de l'implémentation d'un controller pour l'agreggation*/

#ifndef CAGGREGATION_CONTROLLER_H
#define CAGGREGATION_CONTROLLER_H

#include <argos3/core/control_interface/ci_controller.h>
#include <argos3/core/utility/math/vector2.h>
#include <argos3/core/utility/logging/argos_log.h>
#include <argos3/plugins/robots/kilobot/control_interface/kilolib.h>

using namespace argos;

enum MovementState {KILOBOT_STATE_STOP, KILOBOT_STATE_TURNING, KILOBOT_STATE_MOVING};

enum SurrondingRobots {KILOBOT_ALONE, KILOBOT_NEXT_TO_F, KILOBOT_NEXT_TO_E, KILOBOT_NEXT_TO_FE};

struct teamMessage {UInt32 team_kilo;};

/*
 * implémentation de la classe CCI_Controller
 */
class CAggregation_controller : public CCI_Controller {

public:

    /* Class constructeur */
    CAggregation_controller();

    /* Class destructeur*/
    virtual ~CAggregation_controller() {}

    /* Init */
    virtual void Init(TConfigurationNode& t_node);

    /* Loop fonction de controle */
    virtual void ControlStep();

    /* Reset */
    virtual void Reset();

    /* Destroye */
    virtual void Destroy() {}

    /* Récupération des états */
    inline const MovementState GetCurrentMovementState() const {return m_tCurrentMovementState;};
    inline const MovementState GetPreviousMovementState() const {return mt_PreviousMovementState;};

    inline const SurrondingRobots GetCurrentSurrondingRobots() const {return m_tCurrentSurrondingRobots;};
    inline const SurrondingRobots GetPreviousSurrondingRobots() const {return m_tPreviousSurrondingRobots;};

private:

    /*Variable et fonction spécifique*/
    MovementState m_tCurrentMovementState;
    MovementState mt_PreviousMovementState;
    
    SurrondingRobots m_tCurrentSurrondingRobots;
    SurrondingRobots m_tPreviousSurrondingRobots;

    /* Data from XML file*/
    UInt32 team_kilo;
    UInt32 rcv_timing;

    /* Fonction de message*/
    void SendMessage();
    void ReceiveMessages();

    /* change le state en fct du message reçut */
    void SetStateSurrondingRobots();

    /* Random walk */
    void RW_Alone();
    void RW_NF();
    void RW_NE();
    void RW_NFE();
};

#endif // CAGGREGATION_CONTROLLER_H