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
		int val;//������ֵ
		int level;//��
		int adr;//�������λ��
	}characterRecord;

	extern characterRecord characterTable[50];//�ַ���   Ĭ�����50���ַ�
	extern int currentIndex;//characterTable��ĵ�ǰ����
	extern int LEVEL;//��ǰ����
	extern int DXTable[3];//PL0Ĭ�����3�㣬��¼ÿ��DXֵ��ÿ��DX��ʼ��Ĭ��Ϊ3
	void enter(characterRecord & cr);//����һ���ַ���¼���ַ���
	class WordAnalyze;
	void enterCharacterTable(Production * production, WordAnalyze & wa);

	//Ŀ�����ṹ
	typedef struct
	{
		string f; // ������
		int l; // ���
		int a; // λ����
	} instruction;
	extern instruction code[50];//Ĭ�ϲ�����50��ָ�����
	extern int codeIndex;//code�����ĵ�ǰ����
	class State{


	public:
		int type_;//0 �ս��  1���ս��
		string name_;
		int pos_;
		vector<State*> first_relate;//���״̬��FIRST������first_relate�����ÿһ��״̬��FIRST��
		vector<State*> follow_relate;//���״̬��FOLLOW������follow_relate�����ÿһ��״̬��FOLLOW����
		vector<State*> myFIRST;//FIRST����¼
		vector<State*> myFOLLOW;//FOLLOW����¼
		int myFOLLOW_size = 0;
		//����State�Ǳ�ʶ�������޷������������¼�����ǵ�ֵ���Ա������������Ŀ�����ʹ��..
		string id_ = "";
		int val_ = 0;
		int nextCode = -1;//��һ��ָ�����λ��
		int backpatchIndex = -1;//�ж��Ƿ���Ҫ�������Ҫ����backpatchIndex��ʾ��Ҫ�����ָ������λ�á�
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
		//��FIRST��
		vector<State*> getFIRST(){
			return myFIRST;
		}

		//��FOLLOW��   a[]�����ж���FOLLOW��ʱ���ս����ʹ��״̬   b[]�����ж���FIRST��ʱʹ��״̬
		vector<State*> getFOLLOW(){
			return myFOLLOW;
		}
	};



	//����ʽ
	class Production{


	public:
		Production(){}
		Production(State *start, vector<State*> end, int pos) :start_(start), end_(end), pos_(pos){}
		~Production(){}
		State *start_; //����ʽ��
		vector<State*> end_;//����ʽ�Ҳ�
		int pos_;//����ʽ����
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

	//��Ŀ
	class Item{

	public:
		Production *production_; //��Ŀ��������ʽ
		int pointIndex_; //���λ��
		int type_ = 0;//0�ƽ���Ŀ 1��Լ��Ŀ
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


	//��Ŀ��
	class ItemSet{

	public:
		int id_;
		list<Item*> items_;//��Ŀ��
		ItemSet(int id, list<Item*> item) :id_(id), items_(item){}
		~ItemSet(){}
	};
	extern int  table[150][60];

	class SynataxAnalyze{
	public:
		//S'->E
		Production *production1;
		//�ķ�G'�ַ��� 
		vector<State*> g_;
		//��¼�ķ�G' �ķ�������������ӳ���ϵ��������� ��һ������Ϊ���� �ڶ�������Ϊ���Ӧ����ָ��
		map<string, State*> nameMap;
		//���в���ʽ����
		vector<Production*> allProductions;
		//���е���Ŀ��
		list<ItemSet*> c;
		//map��¼Ŀǰȷ������c�е���Ŀ����keyȡΪ��Ŀ��id��valueȡΪ����Ŀ��δ����closure֮ǰ�ļ���
		//�����ж����γɵ���Ŀ���Ƿ��Ѿ�����
		map<int, list<Item*>> existItems;
		//map��¼�ķ����Ŷ�Ӧ����λ�ã��������ķ����ŵ�����˳��
		map<string, int> g_index;
		
		//��Ŀ״̬��������Ӧ��ջ
		stack<int> itemSetId_stack;
		//����״̬�ַ�����Ӧ��ջ
		stack<State> state_stack;

		int judge = 0;//�ж�ѭ��ֹͣ

		SynataxAnalyze(){}
		~SynataxAnalyze(){}


		//��Ŀ�����Ƿ��Ѿ�������Ŀ
		bool existInItems(list<Item*> items, Item* item);
		//��״̬�����ҳ�ͬ��״̬
		State * findState(string name);
		//���ݲ���ʽ�������Ҳ���ʽ
		Production * findProduction(int pos);
		//����Ŀ�� items ��closure
		list<Item*> closure(list<Item*> items);
		//�жϴ�С��ȵ�����listԪ���Ƿ���ȫ���
		bool ifTwoListEquals(list<Item*> list1, list<Item*> list2);
		//�ж�items�������Ŀ�Ƿ��Ѿ�������existItems,�����ڣ��򷵻ش��ڵ���Ŀ����id���������ڣ��򷵻�-1
		int isExist(list<Item*> items);
		//�ѹ�Լ�Ĳ������
		int fillMergedPart(ItemSet* itemSet);
		//������Ŀ���淶��ͽ���
		void writeTable();
		//��ʼ��FIRST����FOLLOW��
		void initFirstAndFollow();

		void init();
		//�ͷ��ڴ�
		void free();
		//����
		void printTable(){
			for (int i = 0; i < codeIndex; i++){
				instruction in = code[i];
				cout << i << "    " << in.f << "    " << in.l << "     " << in.a << endl;
			}
		}

		
	};


	//�����ַ���
	void enter(characterRecord & cr);
	void enterCharacterTable(Production * production, WordAnalyze & wa);
	//����ĸ���ҵ�����ַ�
	characterRecord * findCharacterFromTable(string id);
	//��������
	void gen(string f, int l, int a);
	//����Ŀ�����  >=0�����������ɴ���ɹ��������ش������  -1����Ҫ���ɴ���  -2���ű��Ҳ�������(���﷨����)
	int generateCode(State & startState, Production * production, SynataxAnalyze & sa);
	//�������������ʽ
	void processOPR(State & start, Production * production, SynataxAnalyze & sa);
	//����
	void backpatch(int index, int backpatchVal);
	//ÿ�ν��й�Լ��ʱ��ɨ���Լ����ʽ������Ҫ����ҿ��Ի���������򴫵ݸ���һ��
	void ifBackpatch(State & start, Production * production, SynataxAnalyze & sa);
	void noNeedToGenerateCode(State & startState, Production * production, SynataxAnalyze & sa);
	int process(State * s, SynataxAnalyze & sa);
	//����
	void analyze();


}


#endif