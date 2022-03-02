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
typedef enum { READY, RUNNING , BLOCK, PREEMPT, DONE  } process_state;


int myrandom(int burst) {
    if (ofs > stoi(randvals[0]) ){
        //cout<< "I'm here" << endl;
        ofs = 0;
    }
    ofs++;
    //cout << 1 + (stoi(randvals[ofs]) % burst);
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
    int remainder;
    Process(vector <string>, int);
    void set_FT(){
        FT = TC + CW + IT + AT;
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
    void set_remainder(int burst){
        remainder = remainder - burst;
    }

};
Process::Process(vector <string> proc, int prio){
    quantum = 10000;
    AT = stoi(proc.at(0));
    TC = stoi(proc.at(1));
    CB = stoi(proc.at(2));
    IO = stoi(proc.at(3)); 
    PRIO = myrandom(prio);
    remainder = TC;
}

// void put_event(Event *event){
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
    int old_state;
    int event_time_stamp;
    Process* evtproc;
    Event(Process*, int);
};

Event::Event(Process* p, int trans)
{
    transition = trans;
    event_time_stamp = current_time;
    evtproc = p;
    
}



class Scheduler
{

public:
    queue<Process*> run_Q;
    virtual void add_process(Process *p){
        run_Q.push(p);
    }
    Process* get_next_process(){
        if (run_Q.empty()){
            return nullptr;
        }
        Process *p = run_Q.front();
        run_Q.pop();
        return p;
    }
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
            Event eve(&create.at(i), READY);
            eve.event_time_stamp = create.at(i).AT;
            // cout << "create_AT " << i << " : " << create.at(i).AT << " ";
            // cout << "event_time_stamp " << i << " : " << eve.evtproc -> AT << endl;

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
void put_event(Event event){
    int flag = 0; 
    for (int i = 0; i < event_Q.size(); i++){
        if (event_Q.at(i).event_time_stamp > event.event_time_stamp){
            //cout << "event pid: "<< event_Q.at(i).evtproc -> pid<< " time stamp: " << event_Q.at(i).event_time_stamp<< endl;
            event_Q.insert(event_Q.begin() + i , event);
            flag = 1;
            break;
        }
    }
    if (flag == 0){
        event_Q.push_back(event);
    }
}
void simulation(Scheduler scheduler){
    
    Event evt = get_event();
    bool CALL_SCHEDULER = false;
    Process* current_running_process = nullptr;
    Process* process_blocked = nullptr;
    
    while(event_Q.size() >= 0){
        //cout<< "this" << endl;
        //cout <<"pid: " <<evt.evtproc ->pid;
        Process *proc = evt.evtproc;
        current_time = evt.event_time_stamp;
        //cout<< "current_time " << current_time<< endl;
        int transition = evt.transition;
        //int timeInPrevState = current_time - proc -> proc_time_stamp;
        switch (transition)
        {
        case READY:
        {
            // must come from BLOCKED or from PREEMPTION
            // must add to run queue
            // conditional on whether something is run
            //cout <<"pid: " <<evt.evtproc ->pid;
            // if (process_blocked == proc) {
			// 	process_blocked = nullptr;
			// }
            scheduler.add_process(proc);
            //cout <<"runQ: " << scheduler.run_Q.front() ->pid<< endl; 
            if (current_running_process == nullptr){
                //cout<< "this" << endl;
                //cout<< "test" << endl;
                CALL_SCHEDULER = true;
            }
        
                                  
            break;
            
        }
        case RUNNING:
            // create event for either preemption or blocking
        {    
            //cout<< "thiiiiiiis" << endl;
            //current_running_process = proc;
            //cout << (current_running_process == nullptr) << endl;
            //cout <<   proc -> pid;
            int cpu_burst = myrandom(proc -> CB);
            //cout << "cpu_burst " << cpu_burst<< endl;
            if (cpu_burst > proc -> remainder){
                //cout << "that ";
                cpu_burst = proc -> remainder;
            }
            if (proc -> quantum > cpu_burst){
                //cout << "thiiiiiiis";
                proc -> set_remainder(cpu_burst);

                if (proc ->  remainder ==  0){

                    //cout<< "CALL_SCHEDULER " << CALL_SCHEDULER << endl;
                    // cout<< "event_Q.size " <<  event_Q.size() << endl;
                    proc -> set_state(DONE);
                    proc -> set_FT();
                    proc -> set_TT();
                    cout<< proc -> pid << ":\t" << proc -> AT << " " <<  proc -> TC << " "<< proc -> CB << " "<< proc -> IO << " "<< proc -> PRIO<< " | ";
                    cout << proc -> FT<< " " << proc -> TT<< " " <<proc -> IT<< " " << proc -> CW << " "<<endl;
                    current_running_process = nullptr;

                }
                else{
                    
                    //cout << " Ready -> Running cb = " << cpu_burst << " rem = " << proc -> remainder;
                    Event new_event(proc, BLOCK);
                    new_event.event_time_stamp = current_time + cpu_burst;
                    put_event(new_event);
                }
                
            }
           
            break;
        }
        case BLOCK:
        {
            //create an event for when process becomes READY again
            current_running_process = nullptr;
            int IO_burst = myrandom(proc -> IO);
            proc -> set_IT(IO_burst);
            
            Event new_event(proc, READY);
            //cout << "io burst " << IO_burst << endl;
            new_event.event_time_stamp = current_time + IO_burst;
            
            new_event.old_state = BLOCK;
            put_event(new_event);
            
            // cout <<  " Running -> Block ib " << IO_burst << " rem " << proc->remainder<< " ";
            // cout << "that" << endl;
            //cout<< "that " << endl;

            CALL_SCHEDULER = true;
            break;
        }
        case PREEMPT:
        {
        // add to runqueue (no event is generated)
            scheduler.add_process(proc);
            CALL_SCHEDULER = true;
            break;
        }
        default:{
            break;
        }
        }
        if (CALL_SCHEDULER){
            if (!event_Q.empty() && event_Q.at(0).event_time_stamp == current_time){
                //cout << "I'm here"<< endl;
                evt = get_event();
                continue;  
            }
            CALL_SCHEDULER = false;
            if (current_running_process == nullptr) {
                //cout<< "sth" << endl;
                current_running_process = scheduler.get_next_process();
                //cout<< "sth " << endl;
                //cout << "pid :" << current_running_process->pid;
                if (current_running_process == nullptr){
                    if (event_Q.empty()){
                        break;
                    }
                    if (!event_Q.empty()){
                        evt = get_event();
                        //cout <<"next_event " << evt.evtproc -> pid << "transition " << evt.transition << endl;
                    }
                    continue;
                }
                //cout<< "next process: "<<current_running_process -> pid << endl;
                Event new_event(current_running_process, RUNNING);
                //cout << "current time " << current_time<< endl;
                //cout << "event_Q size " << event_Q.size()<< endl;
                new_event.event_time_stamp = current_time;
                put_event(new_event);
                // cout << "event_Q size after put event" << event_Q.size()<< endl;
                // for(int i = 0; i < event_Q.size(); i++){
                //     cout<< "in while loop "<<event_Q.at(i).evtproc -> pid<< endl;
                // }

            }
        }


        //  evt = get_event() this should be at the  end of the while loop to fetch the next event.
        if (event_Q.empty()){
            break;
        }
        if (!event_Q.empty()){
            evt = get_event();
            // cout << "event_size: " << event_Q.size() << endl;
            // cout<< "sth";
            // cout <<"next_event " << evt.evtproc -> pid << " transition " << evt.transition << endl;
        }
       
        
    }
}
int main(int argc, char** argv){
    //reading input file into a vector
    // queue<Process*> process_Q; 
    // ifstream f;
    // vector<string> str; 
    // string filename_input = argv[1];
    // f.open(filename_input);
    // copy(istream_iterator<string>(f),
    // istream_iterator<string>(),
    // back_inserter(str));
    // int len = str.size();
    // vector <string> temp;
    // vector < vector <string > > event;
    // vector <Process> create;


    queue<Process*> process_Q; 
    ifstream f;
    vector<string>  str; 
    string filename_input = argv[1];
    f.open(filename_input);
    copy(istream_iterator<string>(f), istream_iterator<string>(), back_inserter(str));
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
    // for (int i = 0; i < event.size(); i++){
    //     for (int j = 0; j < event.at(i).size(); j++){
    //         cout<< event.at(i).at(j)<< " ";
    //     }
    //     cout<< endl;
    // }
    for (int i = 0; i < event.size(); i++){
        Process pcb (event.at(i), max_prio);
        pcb.FT = 0; 
        pcb.CW = 0;
        pcb.IT = 0;
        pcb.TT = 0; 
        pcb.set_quantum(10000);
        pcb.pid = i;
        //pcb.proc_time_stamp = pcb.AT;
        create.push_back(pcb);
    }
    //create_to_ready(create);
    current_time = 500;
    create_to_ready(create);
    // for(int i = 0; i < create.size(); i++){
    //     cout<< create.at(i).pid<< ": "<<create.at(i).AT<<  endl;
    // }
    // for(int i = 0; i < event_Q.size(); i++){
    //     cout<< event_Q.at(i).evtproc -> pid<< ": "<< event_Q.at(i).evtproc -> AT<< " " << event_Q.at(i).evtproc -> TC << " " <<  event_Q.at(i).evtproc -> CB<< " "<< endl;
    // }
    Scheduler test;
    current_time = 0; 
    simulation(test);
    
    
    return 0;
}