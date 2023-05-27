#include<iostream>
#include<cstring>
#include<string>
#include<stack>
#include<vector>
#include<set>
#include<queue>

#define maxsize 128

using namespace std;

struct nfa_node
{
	int index;
	char input;
	int char2tran;
	set<int> null2tran;
};

struct NFA
{
	nfa_node *head;
	nfa_node *tail;
};
nfa_node nfanode[maxsize];
int nfaStateNum = 0;


NFA str2nfa(string);
string mid2after(string);
int _getnum_(char);
void _init_(string &s);
void insert(string &,int, char);
NFA get_nfa(int);
void add_ch(nfa_node*, nfa_node*, char);
void add_null(nfa_node*, nfa_node*);
bool is_end(NFA, set<int>);

struct edge
{
	
	char input;
	int Trans;
};

struct dfanode
{
	bool isEnd;
	int index;
	set<int> closure;
	int edgeNum;
	edge Edges[10];
};

dfanode dfanodes[maxsize];
int dfaStateNum = 0;

struct DFA
{
	int startState;
	set<int> endStates;
	set<char> terminator;
	int trans[maxsize][26];
};
DFA nfa2dfa(NFA, string);


set<int> s[maxsize];
dfanode mindfanodes[maxsize];
int minDfaStateNum = 0;

struct stateSet
{
	
	int index;
	set<int> s;
};
set<int> move_E_(set<int>, char);

set<int> _E_(set<int>);

int _getnum_(char ch)
{

	if(ch == '*')
	{
		return 3;
	}
		
	if(ch == ':')
	{
		return 2;
	}
		
	if(ch == '|')
	{
		return 1;
	}
	
	if(ch == '(')
	{
		return 0;
	}
}

string mid2after(string s){
	_init_(s);
	string str;
	stack<char> oper;
	for(int i = 0; i < s.size(); i++){
		if(s[i] >= 'a' && s[i] <= 'z'){
			str += s[i];
		} 
		else{
			if(s[i] == '('){
				oper.push(s[i]);
			}
			else if(s[i] == ')'){
				char ch = oper.top();
				while(ch != '('){
					str += ch;
					oper.pop();
					ch = oper.top();
				}
				oper.pop();
			}
			else{
				if(!oper.empty()){
					char ch = oper.top();
					while(_getnum_(ch) >= _getnum_(s[i])){
						str +=	ch;
						oper.pop();
						if(oper.empty()){
							break;
						} 								
						else ch = oper.top();
					}
					oper.push(s[i]);
				}
				else{
					oper.push(s[i]);
				}
			}
		}
	}
	while(!oper.empty())
	{
		
		char ch = oper.top();
		oper.pop();
		
		str += ch;
	}

	return str;
}

set<int> move_E_(set<int> s, char ch)
{
	
	set<int> temp;
	
	set<int>::iterator it;
	for(it = s.begin(); it != s.end(); it++){
		if(nfanode[*it].input == ch){
			temp.insert(nfanode[*it].char2tran);
		}
	}
	temp = _E_(temp);
	return temp;
}