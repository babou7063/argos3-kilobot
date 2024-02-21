#include <kilolib.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#define TICKS_TO_SEC 32
#define STD_MOTION_STEPS 5*16
#define REFRESH_RATE 10


// Definition des etats
typedef enum {
    ALONE = 0,           // Mode PERSISTENT -> explorer
    FRIEND_MAJORITY = 1, // Mode BROWNIAN -> rester +- au même endroit
    ENNEMY_MAJORITY = 2, // Mode PERSISTENT -> explorer
} state_type;

// Définition des mouvement
typedef enum {
    STOP = 0,
    FORWARD = 1,
    TURN_RIGHT = 2,
    TURN_LEFT = 3,
} motion_type;


// Initalisation des variables globales //

// Message
message_t send_msg;
message_t rcv_msg;
int new_message;
// state
state_type current_state;
state_type previous_state;
//motion
motion_type current_motion;
motion_type  previous_motion;
//team
int kilo_team;
int neighbors_count[2]; // neighbors_count[0] = le nombre d'ami, neighbors_count[1] = le nombre d'ennemi, peu importe son équipe
// random walk
float crw_exponent;
float levy_exponent;
const std_motion_steps = STD_MOTION_STEPS;
// ticks
uint32_t census_ticks;
uint32_t turning_ticks = 0; 
const uint8_t max_turning_ticks = 120;
unsigned int straight_ticks = 0;       
const uint16_t max_straight_ticks = 320;
uint32_t last_motion_ticks = 0;

void setup(){
    // Récupération equipe
    kilo_team = 1;

    // Assignation des état 
    current_state = ALONE;
    previous_state = ALONE;

    // Assignation des motion
    current_motion = STOP;
    previous_motion = STOP;

    // Assigniation de la valeur du message et calcule de son CRC
    send_msg.type = NORMAL;
    send_msg.data[0] = kilo_team; 
    send_msg.crc = message_crc(&send_msg);

    // Création du tableau de nombre de voisin
    neighbors_count[0] = 0;
    neighbors_count[1] = 0;

    // Assignation des exposants de départ
    set_exponent();

    // On met à zéro le compteur de maj du tableau
    census_ticks = kilo_ticks;
}


void flashing_LED(int team){
    set_color(RGB((kilo_team*100)%255, ((kilo_team+1)*100)%255, ((kilo_team+2)*100)%255));
    delay(500);
    set_color(RGB(0, 0, 0));
    delay(500);
}

void SetStateSurrondingRobots(){
    // modification de l'état du kb

    // ALONE
    if (neighbors_count[0] == 0 && neighbors_count[1] == 0) {

        current_state = ALONE;
    }
    // FRIEND
    else if (neighbors_count[0] > neighbors_count[1]) {
        
        current_state = FRIEND_MAJORITY;
    }
    // ENNEMY => choix arbitraire : s'ils sont le même nombre il sera en mode exploration
    else if (neighbors_count[0] <= neighbors_count[1]) {

        current_state = ENNEMY_MAJORITY;
    }

    // mise à jour du tableau
    neighbors_count[0] = 0;
    neighbors_count[1] = 0;
}

void set_motion(motion_type new_motion){

    if(new_motion != current_motion){

        switch(new_motion){

            case FORWARD:
                spinup_motors();
                set_motors(kilo_straight_left, kilo_straight_right);
                break;
            
            case TURN_RIGHT:
                spinup_motors();
                set_motors(0, kilo_turn_right);
                break;

            case TURN_LEFT:
                spinup_motors();
                set_motors(kilo_turn_left, 0);
                break;

            default:
                set_motors(0,0);
        }

        current_motion = new_motion;
    }
}

void set_exponent(){
    switch (current_state)
    {
    case FRIEND_MAJORITY:
        crw_exponent = 0.0;
        levy_exponent = 2.0;
        break;
    
    case ENNEMY_MAJORITY:
        crw_exponent = 0.9;
        levy_exponent = 2.0;

    case ALONE:
        crw_exponent = 0.0;
        levy_exponent = 2.0;
        break;

    default:
        break;
    }
}

void RW_Turn(){

    // si il a tourné assez longtemps il va FORWARD
    if(kilo_ticks > last_motion_ticks + turning_ticks){
        last_motion_ticks = kilo_ticks;
        set_motion(FORWARD);
    }
}

void RW_Forward(){
    
    // si il a tourné assez de temps
    if(kilo_ticks > last_motion_ticks + straight_ticks){

        last_motion_ticks = kilo_ticks;

        // tourne de manière random à gauche ou à droite
        if(rand_soft()%2){
            set_motion(TURN_RIGHT);
        }
        else{
            set_motion(TURN_LEFT);
        }

        // mise à jour des ticks en fct des exposant
        double angle = 0;

        if (crw_exponent == 0){
            angle = (uniform_distribution(0, (M_PI)));
        }
        else{
            angle = fabs(wrapped_cauchy_ppf(crw_exponent));
        }

        turning_ticks = (uint32_t)((angle / M_PI) * max_turning_ticks);
        straight_ticks = (uint32_t)(fabs(levy(std_motion_steps, levy_exponent)));
    }
}

void RW_ENNEMY_MAJORITY(){
    
}

void broadcast() {

}
void loop(){
    
    flashing_LED(kilo_team);
    broadcast();

    // Action de random walk
    switch (current_motion){
            
            case TURN_RIGHT:
                
                RW_Turn();
                break;
            
            case TURN_LEFT:
                
                RW_TURN();
                break;

            case FORWARD:
            
                RW_Forward();
                break;

            default:
                set_motion(STOP);
                break;
        }
    
    // Si le temps s'est écoulé on met à jour le tableau et le timer de la prochaine maj
    if (kilo_ticks > (census_ticks + REFRESH_RATE * TICKS_TO_SEC)){
        census_ticks = kilo_ticks;

        // Changer le current_state en fct des kb croisés et le mettre à zéro
        SetStateSurrondingRobots();
    }
}

message_t *message_tx(){
    return &send_msg;
}

void message_rx (message_t *message, distance_measurement_t *distance){
    // on stocke le message qu'on recoit
    rcv_msg = *message;

    // mise à jour du tableau de compte
    if (rcv_msg.data == kilo_team) {
        neighbors_count[0] ++; // on ajoute un ami
    }
    else {
        neighbors_count[1] ++; // on ajoute un ennemi
    }
    
    // on met a jour le bool
    new_message = 1;
}

/*---------------------------------------------------------------------*/
/*     Fonction d'implémentation de distribution de Cauchy et Levy     */
/*                       @author cdimidov                              */
/*---------------------------------------------------------------------*/

double uniform_distribution(double a, double b)
{

  return (rand() * (b - a) + a) / ((double)RAND_MAX + 1);
}

double wrapped_cauchy_ppf(const double c)
{
  double val, theta, u, q;
  q = 0.5;
  u = uniform_distribution(0.0, 1.0);
  val = (1.0 - c) / (1.0 + c);
  theta = 2 * atan(val * tan(M_PI * (u - q)));
  return theta;
}

double exponential_distribution(double lambda)
{
  double u, x;
  u = uniform_distribution(0.0, 1.0);
  x = (-lambda) * log(1 - u);
  return (x);
}

/* The stable Levy probability distributions have the form

   p(x) dx = (1/(2 pi)) \int dt exp(- it x - |c t|^alpha)

   with 0 < alpha <= 2. 

   For alpha = 1, we get the Cauchy distribution
   For alpha = 2, we get the Gaussian distribution with sigma = sqrt(2) c.

   */

int levy(const double c, const double alpha)
{
  double u, v, t, s;

  u = M_PI * (uniform_distribution(0.0, 1.0) - 0.5); /*vedi uniform distribution */

  if (alpha == 1) /* cauchy case */
  {
    t = tan(u);
    return (int)(c * t);
  }

  do
  {
    v = exponential_distribution(1.0); /*vedi esponential distribution */
  } while (v == 0);

  if (alpha == 2) /* gaussian case */
  {
    t = 2 * sin(u) * sqrt(v);
    return (int)(c * t);
  }

  /* general case */

  t = sin(alpha * u) / pow(cos(u), 1 / alpha);
  s = pow(cos((1 - alpha) * u) / v, (1 - alpha) / alpha);

  return (int)(c * t * s);
}

/*----------------------------------------------------------------------*/
/*                          Main function                               */
/*                        @author babou7063                             */
/*----------------------------------------------------------------------*/
int main(){
    kilo_init();

    kilo_message_tx = message_tx;
    kilo_message_rx = message_rx;

    kilo_start(setup, loop);

    return 0;
}