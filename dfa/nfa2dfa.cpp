#include "dfa.h"
#include "LexicalAnaly.h"
using namespace std;


int findSetNum(int count, int n){
	for(int i = 0; i < count; i++){
		if(s[i].count(n)){						
			return i;
		}
	}
}
DFA minDFA(DFA d){
	int i, j;
	DFA minDfa;
	minDfa.terminator = d.terminator;
	memset(minDfa.trans, -1, sizeof(minDfa.trans));
	bool endFlag = true;
	for(i = 0; i < dfaStateNum; i++){
		if(dfanodes[i].isEnd == false){
			endFlag = false;
			minDfaStateNum = 2;
			s[1].insert(dfanodes[i].index);
		}
		else{
			s[0].insert(dfanodes[i].index);
		}
	}
	
	if(endFlag){
		minDfaStateNum = 1;
	}
	bool cutFlag = true;
	while(cutFlag){
		int cutCount = 0;
		for(i = 0; i < minDfaStateNum; i++){
			set<char>::iterator it;
			for(it = d.terminator.begin(); it != d.terminator.end(); it++){
				int setNum = 0;
				stateSet temp[20];
				set<int>::iterator iter;
				for(iter = s[i].begin(); iter != s[i].end(); iter++){
					bool epFlag = true;
					for(j = 0; j < dfanodes[*iter].edgeNum; j++){
						if(dfanodes[*iter].Edges[j].input == *it){
							epFlag = false;
							int transNum = findSetNum(minDfaStateNum, dfanodes[*iter].Edges[j].Trans);
							int curSetNum = 0;
							while((temp[curSetNum].index != transNum) && (curSetNum < setNum)){
								curSetNum++;
							}
							if(curSetNum == setNum){
								temp[setNum].index = transNum;	
								temp[setNum].s.insert(*iter);
								setNum++;
							}
							else{
								temp[curSetNum].s.insert(*iter);
							}
						}
					}
					if(epFlag){
						int curSetNum = 0;
						while((temp[curSetNum].index != -1) && (curSetNum < setNum)){
							curSetNum++;
						}
						if(curSetNum == setNum){
							temp[setNum].index = -1;
							temp[setNum].s.insert(*iter);
							setNum++;
						}
						else{
							temp[curSetNum].s.insert(*iter);
						}
					}	
				}
				if(setNum > 1){
					cutCount++;
					for(j = 1; j < setNum; j++){
						set<int>::iterator t;
						for(t = temp[j].s.begin(); t != temp[j].s.end(); t++){
							s[i].erase(*t);
							s[minDfaStateNum].insert(*t);
						}
						minDfaStateNum++;
					}
				}
			}	
		}
		if(cutCount == 0){
			cutFlag = false;
		}
	}
	for(i = 0; i < minDfaStateNum; i++){
		set<int>::iterator y;
		for(y = s[i].begin(); y != s[i].end(); y++){
			if(*y == d.startState){
				minDfa.startState = i;
			}
			if(d.endStates.count(*y)){
				mindfanodes[i].isEnd = true;
				minDfa.endStates.insert(i);
			}
			for(j = 0; j < dfanodes[*y].edgeNum; j++){
				for(int t = 0; t < minDfaStateNum; t++){
					if(s[t].count(dfanodes[*y].Edges[j].Trans)){
						bool haveEdge = false;
						for(int l = 0; l < mindfanodes[i].edgeNum; l++){
                            if((mindfanodes[i].Edges[l].input == dfanodes[*y].Edges[j].input) && (mindfanodes[i].Edges[l].Trans == t)){
								haveEdge = true;
							}
						}
						if(!haveEdge){
							mindfanodes[i].Edges[mindfanodes[i].edgeNum].input = dfanodes[*y].Edges[j].input;
							mindfanodes[i].Edges[mindfanodes[i].edgeNum].Trans = t;
							minDfa.trans[i][dfanodes[*y].Edges[j].input - 'a'] = t;
							mindfanodes[i].edgeNum++;
						}
						break;
					}
				}
			}
		}
	}
	return minDfa;
}

void _print_(DFA d)
{
	int i, j;
	cout<<"初态为"<<d.startState<<endl;
	cout<<"有穷字母表为｛ ";
	set<char>::iterator it;
	for(it = d.terminator.begin(); it != d.terminator.end(); it++){
		cout<<*it<<' ';
	}
	cout<<'}'<<endl<<endl;
	cout<<"终态集为｛ "; 
	set<int>::iterator iter;
	for(iter = d.endStates.begin(); iter != d.endStates.end(); iter++){
		cout<<*iter<<' ';
	}
	cout<<'}'<<endl<<endl;
	cout<<"转移函数为："<<endl;
	for(i = 0; i < minDfaStateNum; i++){
		for(j = 0; j < mindfanodes[i].edgeNum; j++){
			if(mindfanodes[mindfanodes[i].Edges[j].Trans].isEnd == true){
				cout<<mindfanodes[i].index<<"->'"<<mindfanodes[i].Edges[j].input;
				cout<<"'-><"<<mindfanodes[i].Edges[j].Trans<<">\t";
			}
			else{
				cout<<mindfanodes[i].index<<"->'"<<mindfanodes[i].Edges[j].input;
				cout<<"'->"<<mindfanodes[i].Edges[j].Trans<<'\t';
			}
		}
		cout<<endl;
	}
	cout<<endl<<"转移矩阵为："<<endl<<"     ";
	set<char>::iterator t;
	for(t = d.terminator.begin(); t != d.terminator.end(); t++)
	{
		cout<<*t<<"   ";
	}
	cout<<endl;
	for(i = 0; i < minDfaStateNum; i++)
	{
		if(d.endStates.count(i)){
			cout<<'('<<i<<")  ";
		}
		else{
			cout<<' '<<i<<"   ";
		}
		for(j = 0; j < 26; j++){
			if(d.terminator.count(j + 'a')){
				if(d.trans[i][j] != -1){
					cout<<d.trans[i][j]<<"   ";
				}
				else{
					cout<<"    "; 
				}
			}
		}
		cout<<endl;
	}
}


int main(){
	string str = "((d|a)(d|a|g)*)|((q)g(g*))|(g(g*))|k|s|o";
	str = mid2after(str);
	int i, j;
	for(i = 0; i < maxsize; i++){
		nfanode[i].index = i;
		nfanode[i].input = '#';
		nfanode[i].char2tran = -1;
	}
	for(i = 0; i < maxsize; i++){
		dfanodes[i].index = i;
		dfanodes[i].isEnd = 0;
		for(j = 0; j < 10; j++){
			dfanodes[i].Edges[j].input = '#';
			dfanodes[i].Edges[j].Trans = -1;
		}
	}
	for(i = 0; i < maxsize; i++){
        mindfanodes[i].index = i;
		mindfanodes[i].isEnd = false;
		for(int j = 0; j < 10; j++){
			mindfanodes[i].Edges[j].input = '#';
			mindfanodes[i].Edges[j].Trans = -1;
		}
	}
	NFA n = str2nfa(str);
	DFA d = nfa2dfa(n, str);
	DFA minDfa = minDFA(d);
	_print_(minDfa);
	return 0;
}

set<int> _E_(set<int> s){
	stack<int> epStack;
	set<int>::iterator it;
	for(it = s.begin(); it != s.end(); it++){
		epStack.push(*it);
	}
	while(!epStack.empty()){
		int temp = epStack.top();
		epStack.pop();
		set<int>::iterator iter;
		for(iter = nfanode[temp].null2tran.begin(); iter != nfanode[temp].null2tran.end(); iter++){
			if(!s.count(*iter)){
				s.insert(*iter);
				epStack.push(*iter);
			}
		}
	}
	return s;
}


void insert(string &s, int n, char ch)
{
	s += '#';
	for(int i = s.size() - 1; i > n; i--)
	{
		s[i] = s[i - 1];
	}
	s[n] = ch;
}

void add_ch(nfa_node *n1, nfa_node *n2, char ch)
{
	n1->input = ch;
	n1->char2tran = n2->index;
}

void add_null(nfa_node *n1, nfa_node *n2)
{
	
	n1->null2tran.insert(n2->index);
}

NFA get_nfa(int all_num)
{
	NFA my_nfa;
	my_nfa.head = &nfanode[all_num];
	my_nfa.tail = &nfanode[all_num + 1];
	return my_nfa;
}   

void _init_(string &s)
{
	int i = 0 , length = s.size();
	while(i < length)
	{
		if((s[i] >= 'a' && s[i] <= 'z') || (s[i] == '*') || (s[i] == ')'))
		{
			if((s[i + 1] >= 'a' && s[i + 1] <= 'z') || s[i + 1] == '(')
			{
				insert(s, i+1 , ':');
				length ++;
			}
		}
		i++;
	}
} 

NFA str2nfa(string s){
	stack<NFA> NfaStack;
	for(int i = 0; i < s.size(); i++){
        if(s[i] >= 'a' && s[i] <= 'z' ){
			NFA n = get_nfa(nfaStateNum);
			nfaStateNum += 2;
			add_ch(n.head, n.tail, s[i]);
			NfaStack.push(n);
		}
		else if(s[i] == '*'){
			NFA n1 = get_nfa(nfaStateNum);
			nfaStateNum += 2;
			NFA n2 = NfaStack.top();
			NfaStack.pop();
			add_null(n2.tail, n1.head);
			add_null(n2.tail, n1.tail);
			add_null(n1.head, n2.head);
			add_null(n1.head, n1.tail);
			NfaStack.push(n1);
		}
		
		else if(s[i] == '|'){
			NFA n1, n2;
			n2 = NfaStack.top();
			NfaStack.pop();
			n1 = NfaStack.top();
			NfaStack.pop();
			NFA n = get_nfa(nfaStateNum);
			nfaStateNum +=2;
			add_null(n.head, n1.head);
			add_null(n.head, n2.head);	
			add_null(n1.tail, n.tail);
			add_null(n2.tail, n.tail);
			NfaStack.push(n);
		}
		else if(s[i] == ':'){
			NFA n1, n2, n;
			n2 = NfaStack.top();
			NfaStack.pop();
			n1 = NfaStack.top();
			NfaStack.pop();
			add_null(n1.tail, n2.head);
			n.head = n1.head;
			n.tail = n2.tail;
			NfaStack.push(n);
		}
	}
	return NfaStack.top();
}

bool is_end(NFA n, set<int> s)
{
	set<int>::iterator it;
	for(it = s.begin(); it != s.end(); it++){
		if(*it == n.tail->index){
			return true;
		}
	}
	return false;
}

DFA nfa2dfa(NFA n, string str){
    int i;
    DFA d;
    set<set<int>> states;
    memset(d.trans, -1, sizeof(d.trans));
    for(i = 0; i < str.size(); i++){
        if(str[i] >= 'a' && str[i] <= 'z'){
            d.terminator.insert(str[i]);
        }
    }
    d.startState = 0;
    set<int> tempSet;
    tempSet.insert(n.head->index);
    dfanodes[0].closure = _E_(tempSet);
    dfanodes[0].isEnd = is_end(n, dfanodes[0].closure);
    dfaStateNum++;
    queue<int> q;
    q.push(d.startState);
    while(!q.empty()){
        int num = q.front();
        q.pop();
        set<char>::iterator it;
        for(it = d.terminator.begin(); it != d.terminator.end(); it++){
            
            set<int> temp = move_E_(dfanodes[num].closure, *it);
            if(!states.count(temp) && !temp.empty()){
                states.insert(temp);
                dfanodes[dfaStateNum].closure = temp;
                dfanodes[num].Edges[dfanodes[num].edgeNum].input = *it;
                dfanodes[num].Edges[dfanodes[num].edgeNum].Trans = dfaStateNum;
                dfanodes[num].edgeNum++;
                d.trans[num][*it - 'a'] = dfaStateNum;
                dfanodes[dfaStateNum].isEnd = is_end(n, dfanodes[dfaStateNum].closure);
                q.push(dfaStateNum);
                dfaStateNum++;
            }
            else if (temp.empty()) {
                // Transition to trap state
                d.trans[num][*it - 'a'] = -1;
            }
            else{
                for(i = 0; i < dfaStateNum; i++){
                    if(temp == dfanodes[i].closure){
                        dfanodes[num].Edges[dfanodes[num].edgeNum].input = *it;
                        dfanodes[num].Edges[dfanodes[num].edgeNum].Trans = i;
                        dfanodes[num].edgeNum++;
                        d.trans[num][*it - 'a'] = i;
                        break;
                    }
                }
            }
        }
    }
    for(i = 0; i < dfaStateNum; i++){
        if(dfanodes[i].isEnd == true){
            d.endStates.insert(i);
        }
    }
    return d;
}