#include <iostream>
#include <algorithm>
#include <memory>
#include <vector>

class Robot{
public:
    int blood_ =0;
    int heat_ =0;
    int blood_volume_ = 0;
    int heat_volume_ = 0;
    int team_mark_ =0;
    int robot_mark_ =0;
    int type_ = 0;
    int level_ =0;
};

class Infantry : public Robot{
public:

    void GetVolume (std::shared_ptr<Robot> robot) {
        switch (robot->level_) {
            case 1: robot->blood_volume_ = 100; robot->heat_volume_ = 100; break;
            case 2: robot->blood_volume_ = 150; robot->heat_volume_ = 200; break;
            case 3: robot->blood_volume_ = 250; robot->heat_volume_ = 300; break;
            default: break;
        }
    };

};

class Machinery : public Robot{
public:

};

std::vector<std::shared_ptr<Robot>> live_robot;
std::vector<std::shared_ptr<Robot>> destroy_robot;


class Manage{
public:
    //parameters for orders
    int time_ =0;
    char order_ = 0;
    int team_mark_ = 0;
    int robot_mark_ = 0;
    int parameter3_ = 0;

    //distinguish the order which is A,F,H or U
    void DistinguishOrder (std::unique_ptr<Manage> order) {

        if (order->order_ == 'A') {
            OrderA(std::move(order));
        } else if (order->order_ == 'F') {
            OrderF(std::move(order));
        } else if (order->order_ == 'H') {
            OrderH(std::move(order));
        } else if (order->order_ == 'U') {
            OrderU(std::move(order));
        }
    }

private:
    int judge_in_destroy_or_live = 0;
    std::vector<std::shared_ptr<Robot>>::iterator search;

    //search in destroyed robots
    std::unique_ptr<Manage> SearchDestroyRobot (std::unique_ptr<Manage> order) {
        judge_in_destroy_or_live = 0;
        for (std::vector<std::shared_ptr<Robot>>::iterator it = destroy_robot.begin(); it != destroy_robot.end();) {
            if ((*it)->team_mark_ == order->team_mark_) {
                if ((*it)->robot_mark_ == order->robot_mark_) {
                    if ((*it)->type_ == order->parameter3_) {
                        judge_in_destroy_or_live = 1;//1 means in destroyed robots, 0 means not in destroyed robots
                        search = it;
                        break;
                    }
                }
            }
            ++it;
        }
        return order;
    }

    //search in live robots
    std::unique_ptr<Manage> SearchLiveRobot (std::unique_ptr<Manage> order) {
        judge_in_destroy_or_live = 0;
        for (auto it = live_robot.begin(); it != live_robot.end();) {
            if ((*it)->team_mark_ == order->team_mark_) {
                if ((*it)->robot_mark_ == order->robot_mark_) {
                    judge_in_destroy_or_live = 2;//2 means in live robots, 0 means not in live robots
                    search = it;
                    goto break_for;
                }
            }
            it++;
        }
        break_for:
        return order;
    }

    //set a new robot
    void SetNewRobot (std::unique_ptr<Manage> order) {
        std::shared_ptr<Robot> new_robot = std::make_shared<Robot>();
        new_robot->level_ = 1;
        new_robot->team_mark_ = order->team_mark_;
        new_robot->robot_mark_ = order->robot_mark_;
        if(order->parameter3_ == 0) {
            new_robot->type_ = 0;
            Infantry I;
            I.GetVolume(new_robot);
        } else if (order->parameter3_ == 1){
            new_robot->type_ = 1;
            new_robot->blood_volume_ = 300;
            new_robot->heat_volume_ = 0;
        }
        new_robot->heat_ = 0;
        new_robot->blood_ = new_robot->blood_volume_;
        live_robot.push_back (std::move(new_robot));
    }


    //corresponding to order A
    void OrderA (std::unique_ptr<Manage> order) {

        //search in destroyed robots
        std::unique_ptr<Manage> order1 = SearchDestroyRobot(std::move(order));
        if (judge_in_destroy_or_live == 1) {    //1 means in destroyed robots

            (*search)->blood_ = (*search)->blood_volume_;
            (*search)->heat_ = 0;
            live_robot.push_back(*search);
            destroy_robot.erase(search);

        } else {

            //search in live robots
            std::unique_ptr<Manage> order2 = SearchLiveRobot(std::move(order1));
            if (judge_in_destroy_or_live == 2) {
            }    //2 means in live robots

            //neither in destroyed robots nor in live robots
            if (judge_in_destroy_or_live == 0) {
                //create a new robot
                SetNewRobot(std::move(order2));
            }
        }
        judge_in_destroy_or_live = 0;
    }

    //corresponding to order F
    void OrderF (std::unique_ptr<Manage> order) {
        std::unique_ptr<Manage> order1 = SearchLiveRobot(std::move(order));
        if (judge_in_destroy_or_live == 2) {
            if (order1->parameter3_ < (*search)->blood_) {
                (*search)->blood_ -= order1->parameter3_;
            } else {
                std::cout << "D " << (*search)->team_mark_ << " " << (*search)->robot_mark_ << std::endl;
                destroy_robot.push_back ((*search));
                live_robot.erase(search);
            }
        }
    }

    //corresponding to order H
    void OrderH (std::unique_ptr<Manage> order) {
        std::unique_ptr<Manage> order1 = SearchLiveRobot(std::move(order));
        if (judge_in_destroy_or_live == 2) {
            if ((*search)->type_ == 0){
                (*search)->heat_ = order1->parameter3_;
            }
        }
    }

    //corresponding to order U
    void OrderU (std::unique_ptr<Manage> order) {
        std::unique_ptr<Manage> order1 = SearchLiveRobot(std::move(order));
        if (judge_in_destroy_or_live == 2) {
            if((*search)->type_ == 0 && (*search)->level_ < order1->parameter3_) {
                (*search)->level_ = order1->parameter3_;
                Infantry I;
                I.GetVolume((*search));
                (*search)->blood_ = (*search)->blood_volume_;
            }
        }
    }

};


int main() {
    unsigned int num_of_order = 0;
    int time_previous_order = 0;
    std::cin >> num_of_order;
    std::cin.get();
    for(int i=0; i < num_of_order; i++){
        std::unique_ptr<Manage> order = std::make_unique<Manage>();
        std::cin >> order->time_>>order->order_ >> order->team_mark_ >>order->robot_mark_ >> order->parameter3_;

        //first to calculate more heat which caused hurt

        int time_minus = order->time_ - time_previous_order;
        for (std::vector<std::shared_ptr<Robot>>::iterator check_it = live_robot.begin(); check_it != live_robot.end();) {
            int heat_minus = (*check_it)->heat_ - (*check_it)->heat_volume_;
            int judge_erase = 0;
            if (heat_minus > 0) {
                //calculate the blood
                if (time_minus >= heat_minus){
                    (*check_it)->blood_ -= (heat_minus + 1)*heat_minus/2;
                } else {
                    (*check_it)->blood_ -= (time_minus + 1)*time_minus/2;
                }
                if ((*check_it)->blood_ <= 0){
                    std::cout << "D " << (*check_it)->team_mark_ << " " << (*check_it)->robot_mark_ << std::endl;
                    destroy_robot.push_back ((*check_it));  //remove the robot in destroyed robots
                    check_it = live_robot.erase(check_it);  //delete the robot in live robots
                    judge_erase = 1;
                }
            }
            if (judge_erase == 0) {
                ++check_it;
            }
            judge_erase = 0;
        }
        time_previous_order = order->time_;

        //to execute the letter
        Manage M;
        M.DistinguishOrder(std::move(order));
    }
    return 0;
}
