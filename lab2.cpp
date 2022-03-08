#include <cstdlib>
#include<sstream>
#include <string>
#include <cstring>
#include <iostream>
#include <fstream>
#include <vector>
#include<algorithm>
#include <iterator>
#include <typeinfo>
#include <queue>
#include <unistd.h>
#include <list>

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
    int AT, TC, CB, IO, PRIO, FT, TT, IT, CW;
    int state;
    int pid;
    int proc_time_stamp;
    int dynamic_priority;
    int quantum;
    int remainder;
    int cpu_burst;
    int duration;
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
    void set_CW(int duration){
        CW = CW + duration;
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
    quantum = 5;
    AT = stoi(proc.at(0));
    TC = stoi(proc.at(1));
    CB = stoi(proc.at(2));
    IO = stoi(proc.at(3));
    PRIO = myrandom(prio);
    dynamic_priority = PRIO - 1;
    remainder = TC;
    cpu_burst = 0; 
}
vector <Process*> processes;

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

public:
    int transition;
    int event_time_stamp;
    int pre_time;

    Process* evtproc;
    Event(Process*, int); 
};


Event::Event(Process* p, int trans)
{
    pre_time = 0;
    transition = trans;
    event_time_stamp = current_time;
    evtproc = p;
    
}
vector <Event> event_Q;
class Scheduler
{

public:
    queue<Process*> run_Q;
    virtual void add_process(Process *p){
        run_Q.push(p);
    }
    virtual Process* get_next_process(){
        if (run_Q.empty()){
            return nullptr;
        }
        Process *p = run_Q.front();
        run_Q.pop();
        return p;
    }
    virtual bool test_preempt(Process *p, int curtime){
        return false;
    }
};
class FCFS: public Scheduler
{
public:
    void add_process(Process *p){
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

};

class LCFS: public Scheduler
{
    list<Process*> run_Q;
    public:
        
        void add_process(Process *p){
            // usleep(500);
            //cout << "this" << endl;
            run_Q.push_front(p);
        }
        Process*  get_next_process(){
            if (run_Q.empty()){
                return nullptr;
            }
            Process *p = run_Q.front();
            run_Q.pop_front();
            return p;
        }
};
class SRTF: public Scheduler
{
    vector<Process*> run_Q;
    public:
        
        void add_process(Process *p){
            int flag = 0;
            for (int i = 0; i < run_Q.size(); i++){
                if (p -> remainder < run_Q.at(i) -> remainder){
                    run_Q.insert (run_Q.begin() + i, p);
                    flag = 1;
                    break;
                }
            }
            if (flag == 0){
                run_Q.push_back(p);
            }
        }
        Process* get_next_process(){
            if (run_Q.empty()){
                return nullptr;
            }
            Process* p = run_Q.at(0);
            run_Q.erase(run_Q.begin());
            return p;
        }

};
class RR: public Scheduler
{
    queue<Process*> run_Q;
    public:
    void add_process(Process *p){
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
};

class PRIO : public Scheduler
{
    vector < queue<Process*> > active_queue;
    vector < queue<Process*> > expired_queue;
    

    public:
    PRIO(int);
    vector<queue <Process*> > run_Q;
    void add_process(Process* p){
        if (p -> dynamic_priority < 0){
            p -> set_dynamic_prority();
            expired_queue.at(p -> dynamic_priority).push(p);
        }
        else{
            active_queue.at(p -> dynamic_priority).push(p);
        }
        
    }
    Process* get_next_process(){
        
        for (int i = active_queue.size() - 1; i >= 0; i--){
            if (!active_queue.at(i).empty()){
                Process *p = active_queue.at(i).front();
                active_queue.at(i).pop();
                return p;
            }
        }
        active_queue.swap(expired_queue);
        for (int i = active_queue.size() - 1; i >= 0; i--){
            if (!active_queue.at(i).empty()){
                Process *p = active_queue.at(i).front();
                active_queue.at(i).pop();
                return p;
            }
        }
        return nullptr;

    }
    

};
PRIO::PRIO(int maxprio){
  
    for (int i = 0; i < maxprio; i ++){
        queue<Process*> q;
        active_queue.push_back(q);
    }
    for (int i = 0; i < maxprio; i ++){
        queue<Process*> q;
        expired_queue.push_back(q);
    }
}
    

class PREemptive_PRIO : public Scheduler
{
    vector < queue<Process*> > active_queue;
    vector < queue<Process*> > expired_queue;
    

    public:
    PREemptive_PRIO(int);
    vector<queue <Process*> > run_Q;
    void add_process(Process* p){
        if (p -> dynamic_priority < 0){
            p -> set_dynamic_prority();
            expired_queue.at(p -> dynamic_priority).push(p);
        }
        else{
            active_queue.at(p -> dynamic_priority).push(p);
            for (int i = 0; i < expired_queue.size(); i++){
                
            }
        }
        
    }
    Process* get_next_process(){
        
        for (int i = active_queue.size() - 1; i >= 0; i--){
            if (!active_queue.at(i).empty()){
                Process *p = active_queue.at(i).front();
                active_queue.at(i).pop();
                return p;
            }
        }
        active_queue.swap(expired_queue);
        for (int i = active_queue.size() - 1; i >= 0; i--){
            if (!active_queue.at(i).empty()){
                Process *p = active_queue.at(i).front();
                active_queue.at(i).pop();
                return p;
            }
        }
        return nullptr;

    }
    bool test_preempt(Process *p, int curtime ){
        for (int i = 0; i < event_Q.size(); i++){
            if (event_Q.at(i).event_time_stamp ==  curtime &&  event_Q.at(i).evtproc == p){
                
                return false;
            }
        }
        
        return true;
    }
    

};
PREemptive_PRIO::PREemptive_PRIO(int maxprio){
  
    for (int i = 0; i < maxprio; i ++){
        queue<Process*> q;
        active_queue.push_back(q);
    }
    for (int i = 0; i < maxprio; i ++){
        queue<Process*> q;
        expired_queue.push_back(q);
    }
}



bool contains(int i){
    for (int j = 0; j < event_Q.size(); j++){
        if (event_Q.at(j).evtproc -> pid == i){
            return true;
        }
    }
    return false;
}
// unused function
void create_to_ready( vector <Process> create){
    for (int i = 0; i < create.size();  i++){
            Event eve(&create.at(i), READY);
            eve.event_time_stamp = create.at(i).AT;
            event_Q.push_back(eve);
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
void update_cpu_burst(int num){
    for (int i = 0; i < processes.size(); i ++){
        processes.at(i) -> cpu_burst = num;
    }
}

void print_doneQ(vector <Process*> doneQ){
    for(int i = 0; i < doneQ.size(); i++){
        printf("%04d: %4d %4d %4d %4d %1d | %5d %5d %5d %5d\n", doneQ.at(i) -> pid, doneQ.at(i) -> AT,  doneQ.at(i) -> TC, doneQ.at(i) -> CB, doneQ.at(i) -> IO, 
                      doneQ.at(i) -> PRIO, doneQ.at(i) -> FT, doneQ.at(i) -> TT, doneQ.at(i) -> IT,  doneQ.at(i) -> CW);
    }
}
void print_stats(vector<Process*> done, int last_event, int io_util){
    //int cpu_sum;
    double it_sum;
    double turnaround_sum;
    double cpu_waiting_sum;

    for (int i = 0; i < done.size(); i++){
        it_sum += (double)done.at(i) -> IT;
        turnaround_sum += (double)done.at(i) -> TT;
        cpu_waiting_sum += (double) done.at(i) -> CW;
    }
    double cpu_util = (turnaround_sum - cpu_waiting_sum - it_sum) / (double)last_event;
    double io_util1 = (double) io_util;
    double turnaround_mean = turnaround_sum / (double)done.size();
    double cpu_waiting_mean = cpu_waiting_sum / (double)done.size();
    double throughput = (double)done.size()/ (double)last_event;

     printf("SUM: %d %.2lf %.2lf %.2lf %.2lf %.3lf\n", last_event, 100.0 * cpu_util, (100.0 * io_util1) / last_event, turnaround_mean, cpu_waiting_mean, 100.0 * throughput); 
}
void simulation(Scheduler *scheduler, int process_number){
    vector <Process*> doneQ;
    Event evt = get_event();
    bool CALL_SCHEDULER = false;
    Process* current_running_process = nullptr;
    Process* blocked_proc = nullptr;
    int io_util = 0;
    int last_event = 0;
    int io_finish_time = 0;
    int prev_time;
   
    
    while(event_Q.size() >= 0){
        //cout <<"pid: " <<evt.evtproc ->pid;
        Process *proc = evt.evtproc;
        current_time = evt.event_time_stamp;
        //cout<< "current_time " << current_time<< endl;
        int transition = evt.transition;
        int timeInPrevState = current_time - proc -> proc_time_stamp;
        int cpu_burst = proc -> cpu_burst;
       
        switch (transition)
        {
        case READY:
        {
            // must come from BLOCKED or from PREEMPTION
            // must add to run queue
            // conditional on whether something is run
            proc -> dynamic_priority = proc -> PRIO  - 1;
            if (blocked_proc == proc){
                blocked_proc = nullptr;
            }
            
            scheduler -> add_process(proc);
            proc -> proc_time_stamp = current_time; 
           
            // cout << "time: "<< current_time << " pcb: " << proc -> pid << " block -> Ready " << endl;
    
            if (current_running_process == nullptr){ 
                CALL_SCHEDULER = true;
                
            }
            else{
            
                if (proc -> dynamic_priority > current_running_process -> dynamic_priority){
                     
                    if (scheduler -> test_preempt(current_running_process, current_time)){
                        for (int i = 0; i < event_Q.size(); i++){
                            if (event_Q.at(i).evtproc == current_running_process){
                                event_Q.erase(event_Q.begin() + i);
                                break;
                            }
                        }
                        int spent_cpu_burst = current_time - prev_time;
                        current_running_process -> remainder += (current_running_process -> duration - spent_cpu_burst);
                        current_running_process -> cpu_burst = current_running_process -> cpu_burst + current_running_process -> duration - spent_cpu_burst;
                        Event new_event(current_running_process, PREEMPT);
                        new_event.event_time_stamp = current_time;
                        
                        put_event(new_event);
                        current_running_process = nullptr;
                        CALL_SCHEDULER = true;
                    }
                }
                
                
            }
             
                                          
            break;
            
        }
        case RUNNING:
            // create event for either preemption or blocking
        {    
            prev_time = current_time;
            if (cpu_burst == 0){
                
                cpu_burst = myrandom(proc -> CB);
            }
            if (cpu_burst > proc -> remainder){
                cpu_burst = proc -> remainder;
            }
            
            proc -> set_CW(timeInPrevState);
           
            // cout << "time: "<< current_time << " pcb: " << proc -> pid << " Ready -> Running cb = " << cpu_burst << " rem = " << proc -> remainder <<" prio = "<< proc -> dynamic_priority<< endl;
                if (proc -> quantum >= cpu_burst){
                
                proc -> proc_time_stamp = current_time + cpu_burst;
                proc -> duration = cpu_burst;
                proc -> set_remainder(cpu_burst);
                
                if (proc ->  remainder ==  0){
                    
                    proc -> set_state(DONE);
                    proc -> set_FT();
                    proc -> set_TT();
                    Event new_event(proc, DONE);
                    new_event.event_time_stamp = current_time + cpu_burst;
                    put_event(new_event);
                    //insert_doneQ(proc);
                }  
                
                else{                  
                    Event new_event(proc, BLOCK);
                    new_event.event_time_stamp = current_time + cpu_burst;
                    put_event(new_event);
                    
                }
                
                proc -> cpu_burst = 0; 
                
            }
            else{
                cpu_burst -= proc -> quantum;               
                proc -> proc_time_stamp = current_time + proc -> quantum;
                proc -> duration = proc -> quantum;
                proc -> cpu_burst = cpu_burst;
                proc -> set_remainder(proc -> quantum);               
                Event new_event(proc, PREEMPT);
                new_event.event_time_stamp = current_time + proc -> quantum;
                
                put_event(new_event);
            
            }          
            break;
        }
        case BLOCK:
        {
            //create an event for when process becomes READY again
        
            current_running_process = nullptr;
            
            int IO_burst = myrandom(proc -> IO);
            proc -> set_IT(IO_burst);
            proc -> proc_time_stamp = current_time;

            if (blocked_proc != nullptr){
                if (current_time + IO_burst > io_finish_time){                
                    io_util += IO_burst + current_time - io_finish_time; 
                    io_finish_time = IO_burst + current_time;
                    blocked_proc = proc;                               
                }
               
            }
            else{
                io_finish_time = IO_burst + current_time;
                blocked_proc = proc;
                io_util += IO_burst;
            }

            
            Event new_event(proc, READY);
            new_event.event_time_stamp = current_time + IO_burst;
            put_event(new_event);
            
            // cout << "time: "<< current_time << " pcb: " << proc -> pid<<  " Running ->  Block ib " << IO_burst << " rem " << proc->remainder<< " "<< endl;
            CALL_SCHEDULER = true;
            break;
        }
        case PREEMPT:
        {
        // add to runqueue (no event is generated)
            
            current_running_process = nullptr;
            proc -> dynamic_priority--;
            scheduler -> add_process(proc);
             
            // cout << "time: "<< current_time << " pcb: " << proc -> pid << " Running -> Ready cb = " << proc -> cpu_burst << " rem = " << proc -> remainder << " prio = "<< proc -> dynamic_priority << endl;
            
            proc -> proc_time_stamp = current_time;
            CALL_SCHEDULER = true;
            break;
        }
        case DONE:
        {
            int flag = 0;
            if (doneQ.size() == process_number - 1){
                last_event = proc-> FT;
            }
            for (int i = 0; i < doneQ.size(); i++){
                if (doneQ.at(i) -> pid > proc -> pid ){
                    doneQ.insert(doneQ.begin() + i, proc);
                    flag = 1;                    
                    break;
                }
            }
            if (flag == 0){
                doneQ.push_back(proc);
            }
            current_running_process = nullptr;
            CALL_SCHEDULER = true;
            break;
        }
        default:{
            break;
        }
        }
        if (CALL_SCHEDULER){
            if (!event_Q.empty() && event_Q.at(0).event_time_stamp == current_time){
                // cout << "I'm here"<< endl;
                evt = get_event();
                continue;  
            }
            CALL_SCHEDULER = false;
            if (current_running_process == nullptr) {
                current_running_process = scheduler -> get_next_process();
                if (current_running_process == nullptr){
                    if (event_Q.empty()){
                        break;
                    }
                    if (!event_Q.empty()){
                        evt = get_event();
                    }
                    continue;
                }
                Event new_event(current_running_process, RUNNING);
                new_event.event_time_stamp = current_time;
                put_event(new_event);

            }
        }
        if (event_Q.empty()){
            break;
        }
        if (!event_Q.empty()){
            evt = get_event();
        }
    }
    print_doneQ(doneQ);
    print_stats(doneQ, last_event, io_util);
}
int main(int argc, char** argv){
 
    queue<Process*> process_Q; 
    ifstream f;
    vector<string>  str; 
    string filename_input = argv[2];
    f.open(filename_input);
    copy(istream_iterator<string>(f), istream_iterator<string>(), back_inserter(str));
    int len = str.size();
    vector <string> temp;
    vector < vector <string > > event;
    Scheduler *scheduler;
    int max_prio = 4;
    int quantum = 10000;
   

    
   
    int count = 0;
    char * priority;

    ifstream r;
    string filename_rand = argv[3];
    r.open(filename_rand);
    copy(istream_iterator<string>(r),
    istream_iterator<string>(),
    back_inserter(randvals));

    f.close();
    r.close();
    
    ofs = 0;
        char *get_scheduler = argv[1];
        string schedule(get_scheduler);
       
        if (schedule[2] == 'F'){
            cout << "FCFS" << endl;
            scheduler = new FCFS();
        }
        else if (schedule[2] == 'L'){
            cout << "LCFS" << endl;
            scheduler = new LCFS();
        }
        else if (schedule[2] == 'S'){
            cout << "SRTF" << endl;
            scheduler = new SRTF();
        }
        else if (schedule[2] == 'R'){
            
            string q;
            int i = 3;
            while (i < schedule.size() && schedule[i] != ':'){

                q.push_back(schedule[i]);
                
                i++;
            }
            quantum = stoi(q);
            cout << "RR " << quantum << endl;
            scheduler = new RR();
        }
        else if (schedule[2] == 'P'){
            
            string q;
            int i = 3;
            while (i < schedule.size() && schedule[i] != ':'){

                q.push_back(schedule[i]);
                
                i++;
            }
            quantum = stoi(q);
            if (schedule[i] == ':'){
                i++;
                string get_maxprio;
                while (i < schedule.size()){

                    get_maxprio.push_back(schedule[i]);
                    
                    i++;
                }
                max_prio = stoi(get_maxprio);
                
            }
            cout << "PRIO " << quantum << endl;
            scheduler = new PRIO(max_prio);
        }
        else if (schedule[2] == 'E'){
            
            string q;
            int i = 3;
            while (i < schedule.size() && schedule[i] != ':'){

                q.push_back(schedule[i]);
                
                i++;
            }
            quantum = stoi(q);
            if (schedule[i] == ':'){
                i++;
                string get_maxprio;
                while (i < schedule.size()){

                    get_maxprio.push_back(schedule[i]);
                    
                    i++;
                }
                max_prio = stoi(get_maxprio);
                
            }
            cout << "PREPRIO " << quantum << endl;
            scheduler = new PREemptive_PRIO(max_prio);
        }
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
        Process *pcb;
        pcb =  new Process(event.at(i), max_prio);
        pcb -> FT = 0; 
        pcb -> CW = 0;
        pcb -> IT = 0;
        pcb -> TT = 0; 
        pcb -> set_quantum(quantum);
        pcb -> pid = i;
        pcb -> proc_time_stamp = pcb -> AT;
        Event eve(pcb, READY);
        eve.event_time_stamp = pcb -> AT;
        event_Q.push_back(eve);
    }
    current_time = 0;

    simulation(scheduler, event.size());
    
    
    return 0;
}