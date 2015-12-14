#ifndef _SYNTAXANALYZE_H_
#define _SYNTAXANALYZE_H_

#include<string>
#include<vector>
#include<iostream>
#include<list>
#include <fstream>
#include <map>
#include <stack>

namespace MyPL0Compiler{

	using namespace std;
	class Production;
	typedef struct{
		string name;
		int kind;//0 for CONSTANT  1 for VARIABLE  2 for PROCEDURE
		int val;//常量的值
		int level;//层
		int adr;//层中相对位置
	}characterRecord;

	extern characterRecord characterTable[50];//字符表   默认最多50个字符
	extern int currentIndex;//characterTable表的当前索引
	extern int LEVEL;//当前层数
	extern int DXTable[3];//PL0默认最多3层，记录每层DX值，每层DX初始化默认为3
	void enter(characterRecord & cr);//加入一个字符记录入字符表
	class WordAnalyze;
	void enterCharacterTable(Production * production, WordAnalyze & wa);

	//目标代码结构
	typedef struct
	{
		string f; // 功能码
		int l; // 层差
		int a; // 位移量
	} instruction;
	extern instruction code[50];//默认不超过50条指令代码
	extern int codeIndex;//code代码表的当前索引
	class State{


	public:
		int type_;//0 终结符  1非终结符
		string name_;
		int pos_;
		vector<State*> first_relate;//这个状态的FIRST集属于first_relate里面的每一个状态的FIRST集
		vector<State*> follow_relate;//这个状态的FOLLOW集属于follow_relate里面的每一个状态的FOLLOW集。
		vector<State*> myFIRST;//FIRST集记录
		vector<State*> myFOLLOW;//FOLLOW集记录
		int myFOLLOW_size = 0;
		//若该State是标识符或者无符号整数，则记录下他们的值，以便语义分析生成目标代码使用..
		string id_ = "";
		int val_ = 0;
		int nextCode = -1;//下一条指令代码位置
		int backpatchIndex = -1;//判断是否需要回填，若需要，则backpatchIndex表示需要回填的指令代码的位置。
		State(){}
		State(string name, int type, int pos) :type_(type), name_(name), pos_(pos){}
		~State(){}
		bool equalsTo(State *state){
			return (name_ == state->name_);
		}
		bool existInFIRST(State * s){
			for (int i = 0; i < myFIRST.size(); i++){
				if (myFIRST[i]->equalsTo(s)){
					return true;
				}
			}
			return false;
		}
		bool existInFOLLOW(State * s){
			for (int i = 0; i < myFOLLOW.size(); i++){
				if (myFOLLOW[i]->equalsTo(s)){
					return true;
				}
			}
			return false;
		}

		void addToFIRST(State * state){
			if (!existInFIRST(state)){
				myFIRST.push_back(state);
				for (int i = 0; i < first_relate.size(); i++){
					first_relate[i]->addToFIRST(state);
				}
			}
		}

		void addToFOLLOW(State * state){
			if (!existInFOLLOW(state)){
				myFOLLOW.push_back(state);
				for (int i = 0; i < follow_relate.size(); i++){
					follow_relate[i]->addToFOLLOW(state);
				}
			}
		}
		//求FIRST集
		vector<State*> getFIRST(){
			return myFIRST;
		}

		//求FOLLOW集   a[]用于判断求FOLLOW集时非终结符的使用状态   b[]用于判断求FIRST集时使用状态
		vector<State*> getFOLLOW(){
			return myFOLLOW;
		}
	};



	//产生式
	class Production{


	public:
		Production(){}
		Production(State *start, vector<State*> end, int pos) :start_(start), end_(end), pos_(pos){}
		~Production(){}
		State *start_; //产生式左部
		vector<State*> end_;//产生式右部
		int pos_;//产生式索引
		bool equalsTo(Production *production){
			if (end_.size() == production->end_.size() && start_->equalsTo(production->start_)){
				for (int i = 0; i < end_.size(); i++){
					if (!(end_[i])->equalsTo((production->end_[i]))){
						return false;
					}
				}
				return true;
			}
			return false;
		}
	};

	//项目
	class Item{

	public:
		Production *production_; //项目所属产生式
		int pointIndex_; //点的位置
		int type_ = 0;//0移进项目 1规约项目
		Item(Production *production, int pointIndex) :production_(production), pointIndex_(pointIndex){
			if (pointIndex_ == production_->end_.size()){ type_ = 1; }
		}
		bool equalsTo(Item *item){
			if (pointIndex_ == item->pointIndex_&&production_->equalsTo(item->production_)){
				return true;
			}
			return false;
		}
		~Item(){}
	};


	//项目集
	class ItemSet{

	public:
		int id_;
		list<Item*> items_;//项目集
		ItemSet(int id, list<Item*> item) :id_(id), items_(item){}
		~ItemSet(){}
	};
	extern int  table[150][60];

	class SynataxAnalyze{
	public:
		//S'->E
		Production *production1;
		//文法G'字符集 
		vector<State*> g_;
		//记录文法G' 文法符号与其对象的映射关系，方便查找 第一个参数为名字 第二个参数为其对应对象指针
		map<string, State*> nameMap;
		//所有产生式集合
		vector<Production*> allProductions;
		//所有的项目集
		list<ItemSet*> c;
		//map记录目前确定加入c中的项目集，key取为项目集id，value取为该项目集未进行closure之前的集合
		//用于判断新形成的项目集是否已经存在
		map<int, list<Item*>> existItems;
		//map记录文法符号对应这表的位置，即表中文法符号的索引顺序
		map<string, int> g_index;
		
		//项目状态集索引对应的栈
		stack<int> itemSetId_stack;
		//符号状态字符串对应的栈
		stack<State> state_stack;

		int judge = 0;//判断循环停止

		SynataxAnalyze(){}
		~SynataxAnalyze(){}


		//项目集里是否已经存在项目
		bool existInItems(list<Item*> items, Item* item);
		//从状态集中找出同名状态
		State * findState(string name);
		//根据产生式索引查找产生式
		Production * findProduction(int pos);
		//求项目集 items 的closure
		list<Item*> closure(list<Item*> items);
		//判断大小相等的两个list元素是否完全相等
		bool ifTwoListEquals(list<Item*> list1, list<Item*> list2);
		//判断items里面的项目是否已经存在于existItems,若存在，则返回存在的项目集的id，若不存在，则返回-1
		int isExist(list<Item*> items);
		//把规约的部分填表
		int fillMergedPart(ItemSet* itemSet);
		//构建项目集规范族和建表
		void writeTable();
		//初始化FIRST集合FOLLOW集
		void initFirstAndFollow();

		void init();
		//释放内存
		void free();
		//测试
		void printTable(){
			for (int i = 0; i < codeIndex; i++){
				instruction in = code[i];
				cout << i << "    " << in.f << "    " << in.l << "     " << in.a << endl;
			}
		}

		
	};


	//加入字符表
	void enter(characterRecord & cr);
	void enterCharacterTable(Production * production, WordAnalyze & wa);
	//从字母表找到相关字符
	characterRecord * findCharacterFromTable(string id);
	//插入代码表
	void gen(string f, int l, int a);
	//生成目标代码  >=0的数代表生成代码成功，并返回代码入口  -1不需要生成代码  -2符号表找不到符号(即语法出错)
	int generateCode(State & startState, Production * production, SynataxAnalyze & sa);
	//处理操作符产生式
	void processOPR(State & start, Production * production, SynataxAnalyze & sa);
	//回填
	void backpatch(int index, int backpatchVal);
	//每次进行规约的时候，扫描规约产生式，若需要回填，且可以回填，则回填，否则传递给上一级
	void ifBackpatch(State & start, Production * production, SynataxAnalyze & sa);
	void noNeedToGenerateCode(State & startState, Production * production, SynataxAnalyze & sa);
	int process(State * s, SynataxAnalyze & sa);
	//分析
	void analyze();


}


#endif