#include <iostream>
#include <string>
#include <queue>
#include <vector>
#include <cstdlib>
#include <windows.h>
#include <conio.h>
#include <thread>
#include <iomanip>
#include <cmath>
#include <fstream>
#include <bits/stdc++.h>

#define all(v) v.begin(), v.end()

using namespace std;
/** Global Constants **/
const string TRACE = "trace";
const string SHOW_STATISTICS = "stats";
const string ALGORITHMS[5] = {"", "FCFS", "RR-", "SPN", "SRT"};
string operation;
int last_instant, process_count;
vector<pair<char, int>> algorithms;
vector<tuple<string, int, int>> processes;
vector<vector<char>> timeline;
unordered_map<string, int> processToIndex;

// Results
vector<int> finishTime;
vector<int> turnAroundTime;
vector<float> waitingTime;

bool sortByServiceTime(const tuple<string, int, int> &a, const tuple<string, int, int> &b)
{
    return (get<2>(a) < get<2>(b));
}
bool sortByArrivalTime(const tuple<string, int, int> &a, const tuple<string, int, int> &b)
{
    return (get<1>(a) < get<1>(b));
}

void clear_timeline()
{
    for(int i=0; i<last_instant; i++)
        for(int j=0; j<process_count; j++)
            timeline[i][j] = ' ';
}

string getProcessName(tuple<string, int, int> &a)
{
    return get<0>(a);
}

int getArrivalTime(tuple<string, int, int> &a)
{
    return get<1>(a);
}

int getServiceTime(tuple<string, int, int> &a)
{
    return get<2>(a);
}

void fillInWaitTime() {
    for (int i = 0; i < process_count; i++)
    {
        int arrivalTime = getArrivalTime(processes[i]);
        for (int k = arrivalTime; k < finishTime[i]; k++)
        {
            if (timeline[k][i] != '*')
                timeline[k][i] = '.';
        }
    }
}

void firstComeFirstServe()
{
    int time = getArrivalTime(processes[0]);
    for (int i = 0; i < process_count; i++)
    {
        int processIndex = i;
        int arrivalTime = getArrivalTime(processes[i]);
        int serviceTime = getServiceTime(processes[i]);

        finishTime[processIndex] = (time + serviceTime);
        turnAroundTime[processIndex] = (finishTime[processIndex] - arrivalTime);
        waitingTime[processIndex] = (turnAroundTime[processIndex] - serviceTime);

        for (int j = time; j < finishTime[processIndex]; j++)
            timeline[j][processIndex] = '*';
        for (int j = arrivalTime; j < time; j++)
            timeline[j][processIndex] = '.';
        time += serviceTime;
    }
}

void roundRobin(int originalQuantum)
{
    queue<pair<int,int>>q;
    int j=0;
    
    if(getArrivalTime(processes[j])==0){
        q.push(make_pair(j,getServiceTime(processes[j])));
        j++;
    }
    
    int currentQuantum = originalQuantum;
    for(int time =0;time<last_instant;time++){
        if(!q.empty()){
            int processIndex = q.front().first;
            q.front().second = q.front().second-1;
            int remainingServiceTime = q.front().second;
            int arrivalTime = getArrivalTime(processes[processIndex]);
            int serviceTime = getServiceTime(processes[processIndex]);
            currentQuantum--;
            timeline[time][processIndex]='*';
            while(j<process_count && getArrivalTime(processes[j])==time+1){
                q.push(make_pair(j,getServiceTime(processes[j])));
                j++;
            }

            if(currentQuantum==0 && remainingServiceTime==0){
                finishTime[processIndex]=time+1;
                turnAroundTime[processIndex] = (finishTime[processIndex] - arrivalTime);
                waitingTime[processIndex] = (turnAroundTime[processIndex] - serviceTime);
                currentQuantum=originalQuantum;
                q.pop();
            }else if(currentQuantum==0 && remainingServiceTime!=0){
                q.pop();
                q.push(make_pair(processIndex,remainingServiceTime));
                currentQuantum=originalQuantum;
            }else if(currentQuantum!=0 && remainingServiceTime==0){
                finishTime[processIndex]=time+1;
                turnAroundTime[processIndex] = (finishTime[processIndex] - arrivalTime);
                waitingTime[processIndex] = (turnAroundTime[processIndex] - serviceTime);
                q.pop();
                currentQuantum=originalQuantum;
            }
        }
        while(j < process_count && getArrivalTime(processes[j])==time+1){
            q.push(make_pair(j,getServiceTime(processes[j])));
            j++;
        }
    }
    fillInWaitTime();
}

void shortestProcessNext()
{
    priority_queue<pair<int, int>, vector<pair<int, int>>, greater<pair<int, int>>> pq; // pair of service time and index
    int j = 0;
    for (int i = 0; i < last_instant; i++)
    {
        while(j<process_count && getArrivalTime(processes[j]) <= i){
            pq.push(make_pair(getServiceTime(processes[j]), j));
            j++;
        }
        if (!pq.empty())
        {
            int processIndex = pq.top().second;
            int arrivalTime = getArrivalTime(processes[processIndex]);
            int serviceTime = getServiceTime(processes[processIndex]);
            pq.pop();

            int temp = arrivalTime;
            for (; temp < i; temp++)
                timeline[temp][processIndex] = '.';

            temp = i;
            for (; temp < i + serviceTime; temp++)
                timeline[temp][processIndex] = '*';

            finishTime[processIndex] = (i + serviceTime);
            turnAroundTime[processIndex] = (finishTime[processIndex] - arrivalTime);
            waitingTime[processIndex] = (turnAroundTime[processIndex] - serviceTime);
            i = temp - 1;
        }
    }
}

void shortestRemainingTime()
{
    priority_queue<pair<int, int>, vector<pair<int, int>>, greater<pair<int, int>>> pq;
    int j = 0;
    for (int i = 0; i < last_instant; i++)
    {
        while(j<process_count &&getArrivalTime(processes[j]) == i){
            pq.push(make_pair(getServiceTime(processes[j]), j));
            j++;
        }
        if (!pq.empty())
        {
            int processIndex = pq.top().second;
            int remainingTime = pq.top().first;
            pq.pop();
            int serviceTime = getServiceTime(processes[processIndex]);
            int arrivalTime = getArrivalTime(processes[processIndex]);
            timeline[i][processIndex] = '*';

            if (remainingTime == 1) // process finished
            {
                finishTime[processIndex] = i + 1;
                turnAroundTime[processIndex] = (finishTime[processIndex] - arrivalTime);
                waitingTime[processIndex] = (turnAroundTime[processIndex] - serviceTime);
            }
            else
            {
                pq.push(make_pair(remainingTime - 1, processIndex));
            }
        }
    }
    fillInWaitTime();
}

void parse_algorithms(string algorithm_chunk) {
    stringstream stream(algorithm_chunk);
    while (stream.good()) {
        string temp_str;
        getline(stream, temp_str, ',');
        stringstream ss(temp_str);
        getline(ss, temp_str, '-');
        char algorithm_id = temp_str[0];
        getline(ss, temp_str, '-');
        int quantum = temp_str.size() >= 1 ? stoi(temp_str) : -1;
        algorithms.push_back(make_pair(algorithm_id, quantum));
    }
}

void parse_processes() {
    string process_chunk, process_name;
    int process_arrival_time, process_service_time;
    for (int i = 0; i < process_count; i++) {
        cin >> process_chunk;

        stringstream stream(process_chunk);
        string temp_str;
        getline(stream, temp_str, ',');
        process_name = temp_str;
        getline(stream, temp_str, ',');
        process_arrival_time = stoi(temp_str);
        getline(stream, temp_str, ',');
        process_service_time = stoi(temp_str);

        processes.push_back(make_tuple(process_name, process_arrival_time, process_service_time));
        processToIndex[process_name] = i;
    }
}

void parse() {
    string algorithm_chunk;
    cin >> operation >> algorithm_chunk >> last_instant >> process_count;
    parse_algorithms(algorithm_chunk);
    parse_processes();
    finishTime.resize(process_count);
    turnAroundTime.resize(process_count);
    waitingTime.resize(process_count);
    timeline.resize(last_instant);
    for (int i = 0; i < last_instant; i++)
        for (int j = 0; j < process_count; j++)
            timeline[i].push_back(' ');
}


void printAlgorithm(int algorithm_index)
{
    int algorithm_id = algorithms[algorithm_index].first - '0';
    if(algorithm_id==2)
        cout << ALGORITHMS[algorithm_id] <<algorithms[algorithm_index].second <<endl;
    else
        cout << ALGORITHMS[algorithm_id] << endl;
}

void printProcesses()
{
    cout << "Process    ";
    for (int i = 0; i < process_count; i++)
        cout << "|  " << getProcessName(processes[i]) << "  ";
    cout << "|\n";
}
void printArrivalTime()
{
    cout << "Arrival    ";
    for (int i = 0; i < process_count; i++)
        printf("|%3d  ",getArrivalTime(processes[i]));
    cout<<"|\n";
}
void printServiceTime()
{
    cout << "Service    |";
    for (int i = 0; i < process_count; i++)
        printf("%3d  |",getServiceTime(processes[i]));
    cout << " Mean|\n";
}
void printFinishTime()
{
    cout << "Finish     ";
    for (int i = 0; i < process_count; i++)
        printf("|%3d  ",finishTime[i]);
    cout << "|-----|\n";
}
void printTurnAroundTime()
{
    cout << "Turnaround |";
    int sum = 0;
    for (int i = 0; i < process_count; i++)
    {
        printf("%3d  |",turnAroundTime[i]);
        sum += turnAroundTime[i];
    }
    if((1.0 * sum / turnAroundTime.size())>=10)
		printf("%2.2f|\n",(1.0 * sum / turnAroundTime.size()));
    else
	 	printf(" %2.2f|\n",(1.0 * sum / turnAroundTime.size()));
}

void printwaitingTime()
{
    cout << "waitingTime|";
    float sum = 0;
    for (int i = 0; i < process_count; i++)
    {
        if( waitingTime[i]>=10 )
            printf("%2.2f|",waitingTime[i]);
        else
            printf(" %2.2f|",waitingTime[i]);
        sum += waitingTime[i];
    }

    if( (1.0 * sum / waitingTime.size()) >=10 )
        printf("%2.2f|\n",(1.0 * sum / waitingTime.size()));
	else
        printf(" %2.2f|\n",(1.0 * sum / waitingTime.size()));
}
void printStats(int algorithm_index)
{
    printAlgorithm(algorithm_index);
    printProcesses();
    printArrivalTime();
    printServiceTime();
    printFinishTime();
    printTurnAroundTime();
    printwaitingTime();
}

void printTimeline(int algorithm_index)
{
    for (int i = 0; i <= last_instant; i++)
        cout << i % 10<<" ";
    cout <<"\n";
    cout << "------------------------------------------------\n";
    for (int i = 0; i < process_count; i++)
    {
        cout << getProcessName(processes[i]) << "     |";
        for (int j = 0; j < last_instant; j++)
        {
            cout << timeline[j][i]<<"|";
        }
        cout << " \n";
    }
    cout << "------------------------------------------------\n";
}

void execute_algorithm(char algorithm_id, int quantum,string operation)
{
    switch (algorithm_id)
    {
    case '1':
        if(operation==TRACE) cout<<"FCFS  ";
        firstComeFirstServe();
        break;
    case '2':
        if(operation==TRACE) cout<<"RR-"<<quantum<<"  ";
        roundRobin(quantum);
        break;
    case '3':
        if(operation==TRACE) cout<<"SPN   ";
        shortestProcessNext();
        break;
    case '4':
        if(operation==TRACE) cout<<"SRT   ";
        shortestRemainingTime();
        break;
    default:
        break;
    }
}

int getInput(string str)
{
	int val;
	do {
		cout << "Enter Your " << str << ": ";
		cin >> val;
		if (val >= 0)
		{
			break;
		}
		else
		{
			cout << "\t INVALID INPUT" << endl;
		}
	} while (1);
	return val;
}

float getTime (string str)
{
	float val;
	do {
		cout << "Enter " << str << ": ";
		cin >> val;
		if (val >= 0)
		{
			break;
		}
		else
		{
			cout << "\t INVALID INPUT" << endl;
		}
	} while (1);
	return val;
}

int getRandomDigit(int digits)
{
	int var = 0;
	for (int j = 0; j < digits; j++)
	{
		int randDigit = rand() % 9 + 1;
		var = (var * 10) + randDigit;
	}
	return var;
}
void entryOrExit(int val)
{
	if (val == 1)
	{
		cout << endl << endl << "\t\t\tPress Any Key To Start...";
		char ch = _getch();
		cout << endl << endl << endl << endl;
	}
	else if (val == 2)
	{
		cout << endl << endl << "\t\t\tPress Any Key To Exit...";
		char ch = _getch();
	}
}


/////////////
class Bank
{
private:
	int numOfProcesses;
	int numOfResources;
	int** max;
	int** allocation;
	int** need;
	int* available;
	void bankMenuDisplay();
	bool safeState(bool);
	bool request();
public:
	Bank();
	void setNumOfProcesses(int);
	void setNumOfResources(int);
	void allocateMatrixes();
	void initializeAvailablity();
	void initializeMatrixes();
	void print();
	void  run();
	~Bank();
};
void Bank::bankMenuDisplay()
{
	cout << endl;
	cout << "\t\t\t**** Deadlock Avoidance Algorithm ****" << endl;
	cout << "\t\t\t             0.Exit" << endl;
	cout << "\t\t\t             1.Evaluate Safe State" << endl;
	cout << "\t\t\t             2.Make A Request" << endl;
	cout << "\t\t\t             3.Print Matrixes" << endl;
	cout << endl;
}
bool Bank::safeState(bool print)
{
	bool isSafe = true;
	bool* processExecuted = new bool[numOfProcesses];
	vector<int> safeState, tempAvailability;
	for (int i = 0; i < numOfProcesses; i++)
	{
		processExecuted[i] = false;
	}

	for (int i = 0; i < numOfResources; i++)
	{
		tempAvailability.push_back(available[i]);
	}

	for (int i = 0, count = 0; safeState.size() < numOfProcesses && isSafe; i = (i + 1) % numOfProcesses, count++)
	{
		if (!processExecuted[i])
		{
			bool isExecutable = true;
			for (int j = 0; j < numOfResources && isExecutable; j++)
			{
				if (need[i][j] > tempAvailability[j])
				{
					isExecutable = false;
				}
			}
			if (isExecutable)
			{
				for (int j = 0; j < numOfResources; j++)
				{
					tempAvailability[j] += allocation[i][j];
				}
				processExecuted[i] = true;
				safeState.push_back(i);
				count = 0;
			}
		}

		if (count == numOfProcesses)
		{
			isSafe = false;
		}
	}
	if (print)
	{
		if (isSafe)
		{
			cout << endl << endl << "\t\t\t Safe State Exists ";
			cout << endl << "\t\t\t< ";
			for (int i = 0; i < safeState.size(); i++)
			{
				cout << "P" << safeState[i] + 1 << " ";
			}
			cout << " >";
			cout << endl;
		}
		else
		{
			cout <<endl<<endl <<"\t\t\tDeadlock Exists ";
			cout << endl << "\t\t\t< ";
			for (int i = 0; i < safeState.size(); i++)
			{
				cout << "P" << safeState[i] + 1 << " ";
			}
			cout << " >";
			cout << endl;
		}
     }
	delete[] processExecuted;
	return isSafe;
}
bool Bank::request()
{
	bool isGranted = true,step1=true,step2=true;
	int processNum, val;
	string letter = "A";
	vector<int> request,tempAvailable,tempAllocation,tempNeed;
		do 
		{
			cout << "Enter Process Number That Wishes To Make Request: ";
			cin >> processNum;
			if (processNum > 0&& processNum<=numOfProcesses)
			{
				break;
			}
			else
			{
				cout << "\t INVALID INPUT" << endl;
			}
		} while (1);

		for (int i = 0; i < numOfResources; i++)
		{
			do
			{
				cout << "Enter Instances For Resource " + letter+":";
				cin >> val;
				if (val >= 0 && val <= max[processNum-1][i])
				{
					break;
				}
				else
				{
					cout << "\t INVALID INPUT" << endl;
				}
			} while (1);
			request.push_back(val);
            letter[0] = char(int(letter[0]) + 1);

			tempAvailable.push_back(available[i]);
			tempAllocation.push_back(allocation[processNum - 1][i]);
			tempNeed.push_back(need[processNum - 1][i]);
		}

		entryOrExit(1);

		for (int i = 0; i < numOfResources && step1; i++)
		{
			if (request[i] > need[processNum - 1][i])
			{
				step1 = false;
			}
		}

		for (int i = 0; i < numOfResources && step2; i++)
		{
			if (request[i] > available[i])
			{
				step2 = false;
			}
		}

		if (step1)//(Request <= Need)
		{
			if (step2)//(Request <= Available)
			{
				for (int i = 0; i < numOfResources; i++)
				{
					available[i] -= request[i];                       //Available = Available  Request
					allocation[processNum - 1][i] += request[i];   //Allocation = Allocation + Request
					need[processNum - 1][i] -= request[i];	       //Need = Need  Request
				}

				cout << endl << endl << "\t\t\tRunning Safety Algorithm ..." << endl;
				Sleep(600);
				cout << "\t\t\t      Please Wait ..." << endl;
				Sleep(1000);
				if (safeState(0))
				{
					cout << endl << endl << "\t\tGranting Request Will NOT Create Deadlock ..." << endl;
					Sleep(900);
					cout << "\t\t\t      Granting Request ..." << endl;
					Sleep(900);
					cout << "\t\t    Matrixes Have Been Updated Accordingly ..." << endl;
				}

				else
				{
				
					cout << endl << endl << "\t\tGranting Request Will Create Deadlock In Future! ..." << endl;
					Sleep(800);
					cout << "\t\t\t      Discarding Request ..." << endl << endl;
					for (int i = 0; i < numOfResources; i++)
					{
						available[i] = tempAvailable[i];
						allocation[processNum - 1][i] = tempAllocation[i];
						need[processNum - 1][i] = tempNeed[i];
					}
					isGranted = false;
				}
			}
			else
			{
				cout << endl << endl << "\t\tRequest Greater Than Availability Detected ..." << endl;
				Sleep(500);
				cout << "\t\t\t          Try Later ..." << endl;
				Sleep(500);
				cout << "\t\t\t      Discarding Request ..." << endl << endl;
				isGranted = false;
			}
		}
		else
		{
			cout << endl << endl << "\t\tRequest Greater Than Need Detected ..." << endl;
			Sleep(500);
			cout << "\t\t\t      Discarding Request ..." << endl << endl;
			isGranted = false;
		}
	return isGranted;
}
Bank::Bank()
{
	numOfProcesses = 0;
    numOfResources = 0;
	max = NULL;
	allocation = NULL;
	need = NULL;
	available = NULL;
}
void Bank::setNumOfProcesses(int val)
{
	numOfProcesses = val;
}
void Bank::setNumOfResources(int val)
{
	numOfResources = val;
}

void Bank::allocateMatrixes()
{
	max = new int* [numOfProcesses];
	allocation = new int* [numOfProcesses];
	need = new int* [numOfProcesses];
	for (int i = 0; i < numOfProcesses; i++)
	{
		max[i] = new int[numOfResources];
		allocation[i] = new int [numOfResources];
		need[i] = new int [numOfResources];
	}
	available = new int [numOfResources];
}

void  Bank::initializeAvailablity()
{
	if (available != NULL)
	{
		cout << endl << "\t\t\tResource Instances Input" << endl << endl;
		string letter = "A";
		for (int i = 0; i < numOfResources; i++)
		{
			available[i] = getInput("Instances For Resource "+ letter);
			letter[0] = char(int(letter[0]) + 1);
		}
		cout << endl << endl;
	}
}
void  Bank::initializeMatrixes()
{
	if (max != NULL && allocation != NULL && available != NULL)
	{
		vector<int> availabilityCount;
		for (int i = 0; i < numOfResources; i++)
		{
			availabilityCount.push_back(available[i]);
		}

		for (int i = 0; i < numOfProcesses; i++)
		{
			string letter = "A";
			for (int j= 0; j < numOfResources; j++)
			{
				int val = 0;
				do
				{
					cout << "Enter    Max     Resource " + letter + " For Process " + to_string(i + 1) << ": ";
					cin >> val;
					if (val >= 0 &&val<= availabilityCount[j])
					{
						break;
					}
					else
					{
						cout << "\t INVALID INPUT" << endl;
					}
				} while (1);

				max[i][j]= val;
				
				do
				{
					cout << "Enter Allocated  Resource " + letter + " For Process " + to_string(i + 1) << ": ";
					cin >> val;
					if (val >= 0 && val <= available[j] && val<=max[i][j])
					{
						break;
					}
					else
					{
						cout << "\t INVALID INPUT" << endl;
					}
				} while (1);
				available[j] -= val;
				allocation[i][j] = val;
				need[i][j] = max[i][j] - allocation[i][j];
				letter[0] = char(int(letter[0]) + 1);
			}
			cout << endl;
		}
	}
}
void  Bank::print()
{
	entryOrExit(1);
	cout << endl << endl << "\t\t\t\t Matrixes View";
	cout << endl << endl << endl;

	cout << "    Process#     " << "\t    MAX  " << "  \t         ALLOCATED "<<"            \t    NEED" << endl;
	Sleep(600);
	for (int i = 0; i < numOfProcesses; i++)
	{
		Sleep(300);
		cout << "       P" << setw(2)<<left << i + 1 << "        \t";
		Sleep(300);
		for (int j = 0; j < numOfResources; j++)
		{
			cout << setw(4)<<left<< max[i][j] << " ";
			Sleep(300);
		}
		cout << "   \t"; 
		for (int j = 0; j < numOfResources; j++)
		{
			cout << setw(4)<< left << allocation[i][j] << " ";
			Sleep(300);
		}
		cout << "     \t";
		for (int j = 0; j < numOfResources; j++)
		{
			cout << setw(4) << left << need[i][j] << " ";
			Sleep(300);
		}

		cout << endl;
		Sleep(600);
	}
	cout << endl<<endl<<"\t\t\t\t AVAILABLE "<<endl<<"\t\t\t\t";
	for (int i = 0; i < numOfResources; i++)
	{
		cout << setw(4) << left << available[i] << " ";
		Sleep(300);
	}
	entryOrExit(1);
}
void  Bank::run()
{
	int option;
	do
	{
		bankMenuDisplay();
		cout << "Enter Option: ";
		cin >> option;
		if (option == 0)
		{
			break;
		}
		else if (option == 1)
		{
			entryOrExit(1);
			cout << endl << endl << "\t\t\tRunning Safety Algorithm ..." << endl;
			Sleep(600);
			cout << "\t\t\t      Please Wait ..." << endl;
			Sleep(1500);
			safeState(1);
			entryOrExit(2);
		}
		else if (option == 2)
		{
			request();
			entryOrExit(2);
		}
		else if (option == 3)
		{
			print();
		}

	} while (1);
}
Bank::~Bank()
{
	for (int i = 0; i < numOfProcesses; i++)
	{
		delete[] max[i];
		delete[] allocation[i];
		delete[] need[i];
	}
	delete[] max;
	delete[] allocation;
	delete[] need;
	delete[] available;
	numOfProcesses = 0;
	numOfResources = 0;
}

void deadlock()
{
	int n1 = getInput("Number Of Processes");
	int n2 = getInput("Number Of Resources");
	cout << endl << endl;
	Bank bank;
	bank.setNumOfProcesses(n1);
	bank.setNumOfResources(n2);
	bank.allocateMatrixes();
	bank.initializeAvailablity();
	bank.initializeMatrixes();
	bank.run();
}

void MainMenuDisplay()
{
	cout << endl;
	cout << "\t\t\t**** OPERATING SYSTEM SIMULATOR ****" << endl;
	cout << "\t\t\t             Deadlock" << endl;
	cout << endl;
}

/////////////
bool checkFileExistence(const string& path) {
    return GetFileAttributes(path.c_str()) != INVALID_FILE_ATTRIBUTES;
}

// Function to detect cycle in the Wait-For Graph
bool cycleDetectionHelper(int v, vector<bool>& visited, vector<bool>& recStack, const vector<vector<int>>& wfg) {
    if (!visited[v]) {
        visited[v] = true;
        recStack[v] = true;

        for (int neighbor : wfg[v]) {
            if (!visited[neighbor] && cycleDetectionHelper(neighbor, visited, recStack, wfg)) {
                return true;
            } else if (recStack[neighbor]) {
                return true;
            }
        }
    }
    recStack[v] = false; // Remove the vertex from recursion stack
    return false;
}

bool cycleDetection(const vector<vector<int>>& wfg, int numOfProcesses) {
    vector<bool> visited(numOfProcesses, false);
    vector<bool> recStack(numOfProcesses, false);

    for (int i = 0; i < numOfProcesses; i++) {
        if (cycleDetectionHelper(i, visited, recStack, wfg)) {
            return true;
        }
    }
    return false;
}

class ResourceHandler {
private:
    int processCount;
    vector<int> allocationData; // The index represents process, value represents resource allocated
    vector<int> requestData;    // The index represents process, value represents requested resource
    string graphvizPath = "C:\\Program Files\\Graphviz\\bin\\dot.exe"; // Change to your Graphviz path

public:
    ResourceHandler(int p) {
        processCount = p;
        allocationData.resize(processCount);
        requestData.resize(processCount);
    }

    void inputResourceData() {
        cout << "Enter the resources allocated to each process (0 for none, 1 for R1, 2 for R2, ...):" << endl;
        for (int i = 0; i < processCount; i++) {
            cout << "Enter resource allocated to Process " << (i + 1) << ": ";
            cin >> allocationData[i];
            cout<< endl;
        }

        cout << "Enter the resources requested by each process:" << endl;
        for (int i = 0; i < processCount; i++) {
            cout << "Request by Process " << (i + 1) << ": ";
            cin >> requestData[i];
            cout<< endl;
        }
    }

    void generateResourceAllocationGraph() {
        ofstream outfile("resource_allocation.dot");
        outfile << "digraph G {" << endl;

        // Adding processes to the RAG
        for (int i = 0; i < processCount; i++) {
            outfile << "P" << (i + 1) << " [label=\"Process " << (i + 1) << "\"];" << endl; // Start numbering from 1
            if (allocationData[i] > 0) { // If resource is allocated to process i
                outfile << "R" << allocationData[i] << " [label=\"Resource R" << allocationData[i] << "\"];" << endl;
                outfile << "R" << allocationData[i] << " -> P" << (i + 1) << " [label=\"Allocates\"];" << endl; // Allocation edge
            }
            if (requestData[i] > 0) { // If process i is requesting a resource
                outfile << "P" << (i + 1) << " -> R" << requestData[i] << " [label=\"Requests\"];" << endl; // Request edge
            }
        }

        outfile << "}" << endl;
        outfile.close();
        renderGraph("resource_allocation.dot", "resource_allocation.png");
    }

    void generateWaitForGraph() {
        ofstream outfile("wait_for.dot");
        outfile << "digraph G {" << endl;

        // Adding processes to the WFG
        for (int i = 0; i < processCount; i++) {
            outfile << "P" << (i + 1) << " [label=\"Process " << (i + 1) << "\"];" << endl; // Start numbering from 1
        }

        // Build the Wait-For Graph
        for (int i = 0; i < processCount; i++) {
            for (int j = 0; j < processCount; j++) {
                if (i != j && requestData[i] == allocationData[j] && allocationData[j] > 0) {
                    outfile << "P" << (i + 1) << " -> P" << (j + 1) << " [label=\"Wait\"];" << endl; // Edge from i to j
                }
            }
        }

        outfile << "}" << endl;
        outfile.close();
        renderGraph("wait_for.dot", "wait_for.png");

        // Check for deadlock
        if (cycleDetection(fetchWaitForGraph(), processCount)) {
            cout << "Deadlock detected in the Wait-For Graph." << endl;
        } else {
            cout << "No deadlock detected." << endl;
            cout << endl;
        }
    }

    vector<vector<int>> fetchWaitForGraph() {
        vector<vector<int>> wfg(processCount);
        for (int i = 0; i < processCount; i++) {
            for (int j = 0; j < processCount; j++) {
                if (i != j && requestData[i] == allocationData[j] && allocationData[j] > 0) { // Process i is waiting for resource held by j
                    wfg[i].push_back(j);
                }
            }
        }
        return wfg;
    }

    void renderGraph(const string& filename, const string& outputName) {
        if (!checkFileExistence(graphvizPath)) {
            cout << "Error: Graphviz is not installed or path is incorrect." << endl;
            return;
        }

        string command = "\"" + graphvizPath + "\" -Tpng " + filename + " -o " + outputName;
        system(command.c_str());
        cout << "Graph generated: " << outputName << endl;
    }
};
//////////////

int main() {
    int choice;
    do {
        cout << "\"Operating System Resource Management and Process Scheduling Simulator: \n";
        cout << "1. CPU Scheduling\n";
        cout << "2. Bankers Algo\n";
        cout << "3. Single Instance Deadlock\n";
        cout << "4. Exit\n";
        cout << "Enter your choice: ";
        cin >> choice;
        cout << endl ;

        switch(choice) {
            case 1:
                parse();
                for (int idx = 0; idx < (int)algorithms.size(); idx++)
                {   
                    clear_timeline();
                    execute_algorithm(algorithms[idx].first, algorithms[idx].second,operation);
                    if (operation == TRACE)
                        printTimeline(idx);
                    else if (operation == SHOW_STATISTICS)
                        printStats(idx);
                    cout << "\n";
                }
                break;
            case 2:
                srand(time(NULL));
	            int option;

	            MainMenuDisplay();
	            deadlock();
                break;
            case 3: {
                cout<<"Single Instance Resource"<<endl<<endl;
                int processCount;
    			cout << "Enter number of processes: ";
    			cin >> processCount;
                cout << endl;

    			ResourceHandler resourceHandler(processCount);
    			resourceHandler.inputResourceData();
                cout << endl;
    			resourceHandler.generateResourceAllocationGraph();
    			resourceHandler.generateWaitForGraph();
                cout << endl;
                break;
			}
            case 4:
                cout << "Exiting the program. Goodbye!\n";
                break;
            default:
                cout << "Invalid choice. Please try again.\n";
        }
    } while (choice != 4);

    return 0;
}
