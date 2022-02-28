#include <cstdlib>
#include<sstream>
#include <string>
#include <cstring>
#include <iostream>
#include <fstream>
#include <vector>
#include<algorithm>
#include<tuple>
#include <iterator>
#include <typeinfo>
#include <queue>
#include <unistd.h>

using namespace std;
//Global Variables
int ofs;
int current_time;
vector <string> randvals;
typedef enum { READY, RUNNING , BLOCK, DONE, PREEMPT  } process_state;


int myrandom(int burst) {
    ofs++;
    return 1 + (stoi(randvals[ofs]) % burst); 
}
class Process{
    public:
    // PRIO must be added after IO
    int AT, TC, CB, IO, PRIO, FT, TT, IT, CW;
    int state;
    int pid;
    int proc_time_stamp;
    int dynamic_priority;
    int quantum;
    Process(vector <string>, int);
    void set_FT(){
        FT = TC + CW + IT;
    }
    void set_TT(){
        TT = FT - AT;
    }
    void set_IT(int pre){
        IT = pre + IT; 
    }
    void set_CW(int pre){
        CW = CW + pre;
    }
    void set_state(int process_state){
        state = process_state;
    }
    void set_dynamic_prority(){
        dynamic_priority = PRIO - 1;
    }
    void set_quantum(int q){
        quantum = q;
    }

};
Process::Process(vector <string> proc, int prio){
    quantum = 10000;
    AT = stoi(proc.at(0));
    TC = stoi(proc.at(1));
    CB = stoi(proc.at(2));
    IO = stoi(proc.at(3)); 
    PRIO = myrandom(prio);
}

// void put_event(Process *processed_evnt){
//     int flag = 0; 
//     for (int i = 0; i < event_Q.size(); i++){
//         if (time_stamp - event_Q.at(i).FT > time_stamp - processed_evnt ->FT ){
//             event_Q.insert(event_Q.begin() + i , *processed_evnt);
//             flag = 1;
//             break;
            
//         }
//     }
//     if (flag == 0){
//         event_Q.push_back(*processed_evnt);
//     }
// }
// void put_event(Process *processed_evnt){
//     event_Q.push_back(*processed_evnt);
// }
char* getCmdOption(char ** begin, char ** end, const std::string & option)
{
    char ** itr = std::find(begin, end, option);
    if (itr != end && ++itr != end)
    {
        return *itr;
    }
    return 0;
}

bool cmdOptionExists(char** begin, char** end, const std::string& option)
{
    return std::find(begin, end, option) != end;
}
class Event
{
private:
    /* data */
public:
    int transition;
    int event_time_stamp;
    Process* evtproc;
    Event(Process*);
};

Event::Event(Process* p)
{
    transition = READY;
    event_time_stamp = current_time;
    evtproc = p;
    
}
class Scheduler
{

public:
    queue<Process*> run_Q;
    virtual void add_process(Process *p);
    Process* get_next_process();
    bool test_preempt(Process *p, int curtime ){
        return curtime == p -> quantum; 
    }
};
class FCFS: public Scheduler
{
public:
    void add_process(Process *p){
        run_Q.push(p);
    }
    Process* get_next_process(){
        Process *p = run_Q.front();
        run_Q.pop();
        return p;
    }

};
vector <Event> event_Q;

bool contains(int i){
    for (int j = 0; j < event_Q.size(); j++){
        if (event_Q.at(j).evtproc -> pid == i){
            return true;
        }
    }
    return false;
}
void create_to_ready( vector <Process> create){
    for (int i = 0; i < create.size();  i++){
        //if (current_time >= create.at(i).AT && !contains(create.at(i).pid) ){
        //event_Q.push_back(create.at(0));
            Event eve(&create.at(i));
            //create.erase(create.begin()); 
            event_Q.push_back(eve);
        //}
    }
}
Event get_event(){
    Event evnt = event_Q.at(0);
    event_Q.erase(event_Q.begin());
    return evnt;
}
void simulation(){
    Event evt = get_event();
    bool CALL_SCHEDULER = false;
    //cout <<"pid: " <<evt.evtproc ->pid;
    while(event_Q.size() >= 0){
        cout <<"pid: " <<evt.evtproc ->pid;
        Process *proc = evt.evtproc;
        current_time = evt.event_time_stamp;
        int transition = evt.transition;
        int timeInPrevState = current_time - proc -> proc_time_stamp;
        // switch (evt.transition)
        // {
        // case READY:
        //     // must come from BLOCKED or from PREEMPTION
        //     // must add to run queue
        //     CALL_SCHEDULER = true; // conditional on whether something is run                       
        //     break;
        // case RUNNING:
        //     // create event for either preemption or blocking
        //     break;
        // case BLOCK:
        //     //create an event for when process becomes READY again
        //     CALL_SCHEDULER = true;
        //     break;
        // case PREEMPT:
        // // add to runqueue (no event is generated)
        //     CALL_SCHEDULER = true;
        //     break;
        // default:
        //     break;
        // }
        // if (CALL_SCHEDULER){
        //     if (event_Q.at(0).event_time_stamp == current_time){
        //         evt = get_event();
        //         continue;
        //     }
        //     CALL_SCHEDULER = false;
        // }

        //  evt = get_event() this should be at the  end of the while loop to fetch the next event.
        if (!event_Q.empty()){
            evt = get_event();
        }
        else if (event_Q.empty()){
            break;
        }
        
    }
}
int main(int argc, char** argv){
    //reading input file into a vector
    queue<Process*> process_Q; 
    ifstream f;
    vector<string>  str; 
    string filename_input = argv[1];
    f.open(filename_input);
    copy(istream_iterator<string>(f),
    istream_iterator<string>(),
    back_inserter(str));
    int len = str.size();
    vector <string> temp;
    vector < vector <string > > event;
    vector <Process> create;
    
   
    int count = 0;
    char * priority;
    int max_prio;

    ifstream r;
    string filename_rand = argv[2];
    // cout<< "argv[2] " << argv[2]<< endl;
    r.open(filename_rand);
    copy(istream_iterator<string>(r),
    istream_iterator<string>(),
    back_inserter(randvals));

    f.close();
    r.close();
    
    ofs = 0;
    //determining priority
    if (cmdOptionExists(argv, argv+argc, "-p")){
        priority = getCmdOption(argv, argv + argc, "-p");
        max_prio = strtol(priority, NULL, 10);
        //cout << "max_prio" << max_prio << endl;
    }
    else{
        max_prio = 4;
    }

    // building a 2d vector, each vector in event is a line in the input file. 
    for(int i = 0; i < str.size(); i++){
        for(int j = 0; j < 4; j++){
            if (i % 4 == 0){
               temp.push_back(str[count]);
               count++;
            }
        }
        if (!temp.empty()){
            event.push_back(temp);
            temp.clear();
        }
        
        
    }
    for (int i = 0; i < event.size(); i++){
        Process pcb (event.at(i), max_prio);
        pcb.FT = 0; 
        pcb.CW = 0;
        pcb.IT = 0;
        pcb.TT = 0; 
        pcb.pid = i;
        pcb.proc_time_stamp = pcb.AT;
        create.push_back(pcb);
    }
    create_to_ready(create);
    // current_time = 500;
    // create_to_ready(create);
    for(int i = 0; i < event_Q.size(); i++){
        cout<< event_Q.at(i).evtproc -> pid<< endl;
    }
    simulation();
    
    
    return 0;
}