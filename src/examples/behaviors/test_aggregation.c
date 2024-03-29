#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>

#define NEIGHBOR_FRIEND_INDEX 0
#define NEIGHBOR_ENEMY_INDEX 1

typedef enum {
    ALONE = 0,           // PERSISTENT mode -> Explore
    FRIEND_MAJORITY = 1, // BROWNIAN mode -> Staying in the same zone
    ENNEMY_MAJORITY = 2, // PERSISTENT mode -> Explorer
} state_type;

typedef enum {
    STOP = 0,
    FORWARD = 1,
    TURN_RIGHT = 2,
    TURN_LEFT = 3,
} motion_type;

// variable
state_type current_state;
state_type previous_state;

motion_type current_motion;

float crw_exponent;
float levy_exponent;

int neighbors_count[2];

/*-------------------------------------------------------------------------*/
/*                             Test function                               */
/*-------------------------------------------------------------------------*/

//-----------------------------SET_EXPONENT--------------------------------//

void set_exponentVtest(){
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

void test_set_exponent() {
    // friend majority
    current_state = FRIEND_MAJORITY;
    set_exponentVtest();
    assert(fabs(crw_exponent - 0.0) < 0.001);
    assert(fabs(levy_exponent - 2.0) < 0.001);

    // ennemy mojority
    current_state = ENNEMY_MAJORITY;
    set_exponentVtest();
    assert(fabs(crw_exponent - 0.9) < 0.001);
    assert(fabs(levy_exponent - 2.0) < 0.001);

    // alone
    current_state = ALONE;
    set_exponentVtest();
    assert(fabs(crw_exponent - 0.0) < 0.001);
    assert(fabs(levy_exponent - 2.0) < 0.001);

    printf("test_set_exponent OK\n");
}

//------------------------FLASHING_LED---------------------------//

int* flashing_LED_Vtest(int team){
    int* tab = (int*)malloc(3 * sizeof(int));
    if (tab == NULL) {
        return NULL;
    }

    // If multiple of 3 -> red = 2, if not it will be 1 or 0
    int red = (team % 3 == 0) * 3 + ((team % 3) - 1);
    // If multiple of 2 -> green = 3
    int green = (team % 2 == 0) * 3;
    //If non multiple of 2 -> blue = 3
    int blue = (team % 2 != 0) * 3;
    
    tab[0] = red;
    tab[1] = green;
    tab[2] = blue;
    return tab;
}

void test_flashing_led(){
    int* result;

    result = flashing_LED_Vtest(1);
    assert(result[0] == 0 && result[1] == 0 && result[2] == 3);

    result = flashing_LED_Vtest(2);
    assert(result[0] == 1 && result[1] == 3 && result[2] == 0);

    result = flashing_LED_Vtest(3);
    assert(result[0] == 2 && result[1] == 0 && result[2] == 3);

    result = flashing_LED_Vtest(6);
    assert(result[0] == 2 && result[1] == 3 && result[2] == 0);

    result = flashing_LED_Vtest(9);
    assert(result[0] == 2 && result[1] == 0 && result[2] == 3);

    result = flashing_LED_Vtest(17);
    assert(result[0] == 1 && result[1] == 0 && result[2] == 3);

    result = flashing_LED_Vtest(50);
    assert(result[0] == 1 && result[1] == 3 && result[2] == 0);

    printf("test_flashing_led OK\n");
}

//------------------------SetStateSurrondingRobots---------------------------//

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

void test_SetStateSurrondingRobots() {

    // Case alone
    neighbors_count[NEIGHBOR_FRIEND_INDEX] = 0;
    neighbors_count[NEIGHBOR_ENEMY_INDEX] = 0;
    SetStateSurrondingRobots();
    assert(current_state == ALONE);

    // case friend majority
    neighbors_count[NEIGHBOR_FRIEND_INDEX] = 3;
    neighbors_count[NEIGHBOR_ENEMY_INDEX] = 2; 
    SetStateSurrondingRobots();
    assert(current_state == FRIEND_MAJORITY);

    // Case ennemy mojority
    neighbors_count[NEIGHBOR_FRIEND_INDEX] = 2;
    neighbors_count[NEIGHBOR_ENEMY_INDEX] = 3;
    SetStateSurrondingRobots();
    assert(current_state == ENNEMY_MAJORITY);

    // Equal number of friend and ennemies
    neighbors_count[NEIGHBOR_FRIEND_INDEX] = 3;
    neighbors_count[NEIGHBOR_ENEMY_INDEX] = 3;
    SetStateSurrondingRobots();
    assert(current_state == ENNEMY_MAJORITY);

    printf("test_SetStateSurrondingRobots OK\n");
}

//------------------------SetStateSurrondingRobots---------------------------//
int kilo_straight_left = 0;
int kilo_straight_right = 0;

void set_motion(motion_type new_motion){

    if(new_motion != current_motion){

        switch(new_motion){

            case FORWARD:
                kilo_straight_left = 1;
                kilo_straight_right = 1;
                break;
            
            case TURN_RIGHT:
                kilo_straight_left = 0;
                kilo_straight_right = 1;
                break;

            case TURN_LEFT:
                kilo_straight_left = 1;
                kilo_straight_right = 0;
                break;

            default:
                kilo_straight_left = 0;
                kilo_straight_right = 0;
        }

        current_motion = new_motion;
    }
}

void test_set_motion(){
    // Case forward
    current_motion = TURN_LEFT;
    set_motion(FORWARD);
    assert(kilo_straight_left == 1 && kilo_straight_right == 1);

    // Case turn_right
    current_motion = FORWARD;
    set_motion(TURN_RIGHT);
    assert(kilo_straight_left == 0 && kilo_straight_right == 1);

    // Case turn_left
    current_motion = FORWARD;
    set_motion(TURN_LEFT);
    assert(kilo_straight_left == 1 && kilo_straight_right == 0);

    // Test default motion
    current_motion = TURN_RIGHT;
    set_motion(5);
    assert(kilo_straight_left == 0 && kilo_straight_right == 0);

    printf("test_set_motion OK\n");
}

//------------------------RW_Turn---------------------------//
int testRWTurn = 0;
int kilo_ticks = 100;
int last_motion_ticks = 0;
int turning_ticks = 100;

void RW_Turn(){

    // If it turned enough, starts to go forward
    if(kilo_ticks > last_motion_ticks + turning_ticks){
        last_motion_ticks = kilo_ticks;
        //set_motion(FORWARD);
        testRWTurn = 1;
    }
}

void test_RW_Turn() {

    // Init
    current_motion = TURN_LEFT; // The robot is turning

    // kilo_ticks is < last_motion_ticks + turning_ticks
    kilo_ticks = 50;
    RW_Turn();
    assert(testRWTurn == 0);

    // Reset 
    testRWTurn = 0;

    // kilo_ticks == last_motion_ticks + turning_ticks
    kilo_ticks = 100;
    RW_Turn();
    assert(testRWTurn == 0);

    // Reset 
    testRWTurn = 0;

    // kilo_ticks > last_motion_ticks + turning_ticks
    kilo_ticks = 150;
    RW_Turn();
    assert(testRWTurn == 1);

    printf("test_RW_Turn OK\n");
}

int main()
{
    test_set_exponent();
    test_flashing_led();
    test_SetStateSurrondingRobots();
    test_set_motion();
    test_RW_Turn();

    printf("\nALL TEST OK\n");
    return 0;
}