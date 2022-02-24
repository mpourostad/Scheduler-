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
int ofs;
int time_stamp;
vector <string> randvals;

int myrandom(int burst) {
    ofs++;
    return 1 + (stoi(randvals[ofs]) % burst); 
}
class Process{
    public:
    // PRIO must be added after IO
    int AT, TC, CB, IO, PRIO, FT, TT, IT, CW;
    int state;
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
    int get_state(){
        return state;
    }
};
Process::Process(vector <string> proc, int prio){
    AT = stoi(proc.at(0));
    TC = stoi(proc.at(1));
    CB = stoi(proc.at(2));
    IO = stoi(proc.at(3)); 
    PRIO = myrandom(prio);
}
// Process create(vector< vector <string> > event, int index){
//    Process pcb (event.at(index));
//    return pcb;
   
// }

// void put_event(Process *arrived_event){
//     int flag = 0; 
//     for (int i = 0; i < event_Q.size(); i++){
//         if (time_stamp - event_Q.at(i) -> FT > time_stamp - arrived_event -> FT ){
//             event_Q.insert(event_Q.begin() + i , arrived_event);
//             flag = 1;
            
//         }
//     }
//     if (flag == 0){
//         event_Q.push_back(arrived_event);
//     }
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
    int old_state;
    int new_state;
    int current_time;
    Process* p;
    Event(/* args */);
    ~Event();
};

Event::Event(/* args */)
{
}

Event::~Event()
{
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
    vector <Process> event_Q; 
   
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
        create.push_back(pcb);
    }

    typedef enum { READY, RUNNING , BLOCKED } process_state;
    while (!create.empty()){
        if (time_stamp >= create.at(0).AT){
           event_Q.push_back(create.at(0));
           create.erase(create.begin());
           time_stamp = 500; 
        }
    }
    for(int i = 0; i < event_Q.size(); i++){
        cout<< event_Q.at(i).AT<< endl;
    }
   

    
    return 0;
}