#include <kilolib.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#define TICKS_TO_SEC 32
#define STD_MOTION_STEPS 5*16
#define REFRESH_RATE 10
#define NEIGHBOR_FRIEND_INDEX 0
#define NEIGHBOR_ENEMY_INDEX 1
#define KILO_TEAM 1

// States definition
typedef enum {
    ALONE = 0,           // PERSISTENT mode -> Explore
    FRIEND_MAJORITY = 1, // BROWNIAN mode -> Staying in the same zone
    ENNEMY_MAJORITY = 2, // PERSISTENT mode -> Explorer
} state_type;

// Movement definition
typedef enum {
    STOP = 0,
    FORWARD = 1,
    TURN_RIGHT = 2,
    TURN_LEFT = 3,
} motion_type;

// Boolean definition
typedef enum {
    true = 1,
    false = 0,
} bool;

/*-------------------------------------------------------------------------*/
/*                    Defining global variables                            */
/*-------------------------------------------------------------------------*/

// Message variables
message_t send_msg;
message_t rcv_msg;
int sending_mess = 0;

// State variables
state_type current_state;
state_type previous_state;

// Motion variables
motion_type current_motion;
motion_type  previous_motion;

// Team variables
int neighbors_count[2]; // neighbors_count[0] = Number of friends crossed
                        // neighbors_count[1] = Number of enemy crossed, whatever his team

// Random walk variables
float crw_exponent;
float levy_exponent;
const float std_motion_steps = STD_MOTION_STEPS;

// Ticks variables, which represents the kilobot time indicator
uint32_t census_ticks;
uint32_t turning_ticks = 0; 
const uint8_t max_turning_ticks = 120;
unsigned int straight_ticks = 0;       
const uint16_t max_straight_ticks = 320;
uint32_t last_motion_ticks = 0;
uint32_t max_broadcast_ticks = 150;
uint32_t last_broadcast_ticks = 0;

/*-------------------------------------------------------------------------*/
/*         Cauchy and Levy distribution implementation function            */
/*                       @author cdimidov                                  */
/*-------------------------------------------------------------------------*/

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

/*-------------------------------------------------------------------------*/
/*                  Implementing kilobot behavior                          */
/*                       @author babou7063                                 */
/*-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*/
/* Function initializing the exponents of the levy and cauchy distributions*/
/*-------------------------------------------------------------------------*/
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
        break;

    case ALONE:
        crw_exponent = 0.0;
        levy_exponent = 2.0;
        break;

    default:
        break;
    }
}

/*-------------------------------------------------------------------------*/
/*                            Setup fonction                               */
/*-------------------------------------------------------------------------*/
void setup(){
    // States assignment 
    current_state = ALONE;
    previous_state = ALONE;

    // Motion assignment
    current_motion = FORWARD;
    previous_motion = STOP;

    // Message assignment and calculates its CRC
    send_msg.type = NORMAL;
    send_msg.data[0] = KILO_TEAM; 
    send_msg.crc = message_crc(&send_msg);

    // Creating the neighbor number table
    neighbors_count[NEIGHBOR_FRIEND_INDEX] = 0;
    neighbors_count[NEIGHBOR_ENEMY_INDEX] = 0;

    // Assigning starting exponents 
    set_exponent();

    // Census tiks assignment
    census_ticks = kilo_ticks;
}

/*-------------------------------------------------------------------------*/
/*                  Team-based LED lighting function                       */
/*-------------------------------------------------------------------------*/
void flashing_LED(int team){
    // If multiple of 3 -> red = 2, if not it will be 1 or 0
    int red = (team % 3 == 0) * 3 + ((team % 3) - 1);
    // If multiple of 2 -> green = 3
    int green = (team % 2 == 0) * 3;
    //If non multiple of 2 -> blue = 3
    int blue = (team % 2 != 0) * 3;

    set_color(RGB(red, green, blue));
}

/*-------------------------------------------------------------------------*/
/*           Robot status modification function and table update           */
/*-------------------------------------------------------------------------*/
void SetStateSurrondingRobots(){

    // ALONE
    if (neighbors_count[NEIGHBOR_FRIEND_INDEX] == 0 && neighbors_count[NEIGHBOR_ENEMY_INDEX] == 0) {

        current_state = ALONE;
    }
    // FRIEND
    else if (neighbors_count[NEIGHBOR_FRIEND_INDEX] > neighbors_count[NEIGHBOR_ENEMY_INDEX]) {
        
        current_state = FRIEND_MAJORITY;
    }
    // ENNEMY => arbitrary choice: if as many friend as enemy, it will be in exploration mode 
    else if (neighbors_count[NEIGHBOR_FRIEND_INDEX] <= neighbors_count[NEIGHBOR_ENEMY_INDEX]) {

        current_state = ENNEMY_MAJORITY;
    }

    // Table update
    neighbors_count[NEIGHBOR_FRIEND_INDEX] = 0;
    neighbors_count[NEIGHBOR_ENEMY_INDEX] = 0;
}

/*-------------------------------------------------------------------------*/
/*                       Defines robot motion state                        */
/*-------------------------------------------------------------------------*/
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

/*-------------------------------------------------------------------------*/
/*                          Turn function                                  */
/*-------------------------------------------------------------------------*/
void RW_Turn(){

    // If it turned enough, starts to go forward
    if(kilo_ticks > last_motion_ticks + turning_ticks){
        last_motion_ticks = kilo_ticks;
        set_motion(FORWARD);
    }
}

/*-------------------------------------------------------------------------*/
/*                           Forward function                              */
/*-------------------------------------------------------------------------*/
void RW_Forward(){
    
    // If it went forward enough, starts to turn
    if(kilo_ticks > last_motion_ticks + straight_ticks){

        last_motion_ticks = kilo_ticks;

        // Turns randomly left or right
        if(rand_soft()%2){
            set_motion(TURN_RIGHT);
        }
        else{
            set_motion(TURN_LEFT);
        }

        // Calculates movement times (tiks)
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

/*-------------------------------------------------------------------------*/
/*                      Message startup function                           */
/*-------------------------------------------------------------------------*/
void broadcast() {
    // If listening period is over
    if (sending_mess == false && kilo_ticks > last_broadcast_ticks + max_broadcast_ticks){
        last_broadcast_ticks = kilo_ticks;
        sending_mess = true;
    }
}

/*-------------------------------------------------------------------------*/
/*              Callback for message transmission                          */
/*-------------------------------------------------------------------------*/
message_t *message_tx(){
    if (sending_mess == true){
        return &send_msg;
    }
    return 0;
}

/*-------------------------------------------------------------------------*/
/*          Callback for successful message transmission                   */
/*-------------------------------------------------------------------------*/
void message_tx_sucess(){
    sending_mess = false;
}

/*-------------------------------------------------------------------------*/
/*                  Callback for message receiption                        */
/*-------------------------------------------------------------------------*/
void message_rx (message_t *rcv_msg, distance_measurement_t *distance){
    
    // table update according to message
    if (rcv_msg->data[0] == (KILO_TEAM & 0xFF) && rcv_msg->data[1] == ((KILO_TEAM >> 8) & 0xFF)) {
        neighbors_count[NEIGHBOR_FRIEND_INDEX]++; // add a friend

    } else {
        neighbors_count[NEIGHBOR_ENEMY_INDEX]++; // add an enemy
    }
}

/*-------------------------------------------------------------------------*/
/*                           Loop function                                 */
/*-------------------------------------------------------------------------*/
void loop(){
    
    flashing_LED(KILO_TEAM);
    
    if(current_state == ALONE || current_state == ENNEMY_MAJORITY){
        set_color(RGB(0, 0, 0));
    }
    broadcast();

    // Random walk action
    switch (current_motion){
            
            case TURN_RIGHT:
                
                RW_Turn();
                break;
            
            case TURN_LEFT:
                
                RW_Turn();
                break;

            case FORWARD:
            
                RW_Forward();
                break;

            default:
                set_motion(STOP);
                break;
        }
    
    // If time has elapsed, census ticks update
    if (kilo_ticks > (census_ticks + REFRESH_RATE * TICKS_TO_SEC)){
        census_ticks = kilo_ticks;

        // Sates and table update
        SetStateSurrondingRobots();
    }
}

/*----------------------------------------------------------------------*/
/*                          Main function                               */
/*----------------------------------------------------------------------*/
int main(){
    kilo_init();

    kilo_message_tx = message_tx;
    kilo_message_rx = message_rx;

    kilo_start(setup, loop);

    return 0;
}