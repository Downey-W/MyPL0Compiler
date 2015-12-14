#include "SyntaxAnalyze.h"
#include "WordAnalyze.h"
namespace MyPL0Compiler{

	int  table[150][60] = {};

	int LEVEL = 0;//��ʼ��Ϊ0
	characterRecord characterTable[50] = {};//Ĭ�����50���ַ�
	int currentIndex = 0;
	int DXTable[3] = { 3, 3, 3 };//��ʼ��Ϊ3


	int codeIndex = 0;
	instruction code[50] = {};//Ĭ�����50��ָ�����
	//
	//��Ŀ�����Ƿ��Ѿ�������Ŀ
	bool SynataxAnalyze::existInItems(list<Item*> items, Item* item){
		list<Item*>::iterator it;
		for (it = items.begin(); it != items.end(); it++){
			if ((*it)->equalsTo(item)){
				return true;
			}
		}
		return false;
	}
	//��״̬�����ҳ�ͬ��״̬
	State * SynataxAnalyze::findState(string name){
		return nameMap[name];
	}
	//����Ŀ�� items ��closure
	list<Item*> SynataxAnalyze::closure(list<Item*> items){

		//bool a = true;
		list<Item*>::iterator iterator;
		iterator = items.begin();
		//	while (a){
		//		a = false;
		for (; iterator != items.end(); iterator++){
			Item * i = *iterator;
			if ((i->pointIndex_ + 1) <= i->production_->end_.size() && (i->production_->end_.at(i->pointIndex_)->type_ == 1)){
				for (int k = 0; k < allProductions.size(); k++){
					if (allProductions[k]->start_->name_ == i->production_->end_.at(i->pointIndex_)->name_){
						Item *xx = new Item(allProductions[k], 0);
						if (!existInItems(items, xx)){
							items.push_back(new Item(allProductions[k], 0));
						}
						//a = true;
					}
				}
			}
		}
		//	}
		return items;
	}
	//�жϴ�С��ȵ�����listԪ���Ƿ���ȫ���
	bool SynataxAnalyze::ifTwoListEquals(list<Item*> list1, list<Item*> list2){
		int matchNum = 0;
		for (list<Item*>::iterator ii = list1.begin(); ii != list1.end(); ii++){
			Item * item_ii = *ii;
			for (list<Item*>::iterator jj = list2.begin(); jj != list2.end(); jj++){
				Item * item_jj = *jj;
				if (item_ii->equalsTo(item_jj)){
					matchNum++;
					break;
				}
			}
		}

		if (matchNum == list1.size()){
			return true;
		}
		return false;
	}
	//�ж�items�������Ŀ�Ƿ��Ѿ�������existItems,�����ڣ��򷵻ش��ڵ���Ŀ����id���������ڣ��򷵻�-1
	int SynataxAnalyze::isExist(list<Item*> items){
		for (map<int, list<Item*>>::iterator ii = existItems.begin(); ii != existItems.end(); ii++){
			list<Item*> itemList = ii->second;
			if (itemList.size() == items.size()){
				if (ifTwoListEquals(itemList, items)){
					return ii->first;
				}
			}
		}
		return -1;
	}
	//�ѹ�Լ�Ĳ������
	int SynataxAnalyze::fillMergedPart(ItemSet* itemSet){
		for (list<Item*>::iterator ii = itemSet->items_.begin(); ii != itemSet->items_.end(); ii++){
			Item * item = *ii;

			if (item->type_ == 1){
				
				//	vector<State*> follow = getFOLLOW(item->production_->start_);
				//cout << "��һ��Լ��Ŀ: start:" << item->production_->start_->name_ << "   end[0]:" << item->production_->end_[0]->name_ << endl;
				//vector<State*> follow = item->production_->start_->getFOLLOW();
				vector<State*> follow = item->production_->start_->getFOLLOW();

				//cout << "���follow����С:" << follow.size() << endl;
				
				for (int i = 0; i < follow.size(); i++){
					//	cout << "��Լ��Ŀ�󲿵�FOLLOW��һ��Ԫ��Ϊ��" << follow[i]->name_ << "   str Ϊ" << str << endl;
					table[itemSet->id_][follow[i]->pos_] = 2000 + item->production_->pos_;
				}
			}
		}

		return -1;
	}
	//������Ŀ���淶��ͽ���
	void SynataxAnalyze::writeTable(){
		int currentItemSetIndex = 0;

		//����һ����Ŀ����ӽ� c
		Item *i_ = new Item(production1, 0);
		list<Item*> first_items;
		first_items.push_back(i_);
		ItemSet *first_itemSet = new ItemSet(currentItemSetIndex, first_items);
		//������Ŀ��δ����closure֮ǰ�ļ��ϼ���existItems��..
		existItems[currentItemSetIndex] = first_items;
		//��closure
		first_itemSet->items_ = closure(first_itemSet->items_);
		//����c��
		c.push_back(first_itemSet);


		for (list<ItemSet*>::iterator iterator = c.begin(); iterator != c.end(); iterator++){
			ItemSet * itemSet = *iterator;

			fillMergedPart(itemSet);
			//��¼ͬһ����Ŀ���ڣ�������ͬ��string֮����ܻ���ת���µ�ͬһ����Ŀ������Ŀ��
			//��ͬһ����Ŀ���� A->a.B �� C->b.B
			map<string, list<Item*>> tempMap;
			for (list<Item*>::iterator ii = itemSet->items_.begin(); ii != itemSet->items_.end(); ii++){
				Item * item = *ii;
				//�����A->B. .���ڱ����������  Ĭ��4000Ϊ����
				if (item->type_ == 1 && item->production_->start_->name_ == "A"){
					table[itemSet->id_][findState("#")->pos_] = 4000;
				}
				if (item->pointIndex_ < item->production_->end_.size()){
					list<Item*> tempMapItemList = tempMap[item->production_->end_[item->pointIndex_]->name_];
					tempMapItemList.push_back(new Item(item->production_, item->pointIndex_ + 1));
					tempMap[item->production_->end_[item->pointIndex_]->name_] = tempMapItemList;
				}
			}

			//��tempMap����Ķ�Ӧ�ַ�����Ӧת������Ŀ��ȡ�������ж��Ƿ��Ѿ����ڣ��������ڣ������closure�γ��µ���Ŀ��
			//������Ŀ���� c
			for (map<string, list<Item*>>::iterator ii = tempMap.begin(); ii != tempMap.end(); ii++){
				string key = ii->first;
				list<Item*> value = ii->second;
				int code = isExist(value);
				if (code == -1){
					//������
					//����existItems����
					currentItemSetIndex++;
					existItems[currentItemSetIndex] = value;
					//�γ��µ���Ŀ��
					value = closure(value);
					c.push_back(new ItemSet(currentItemSetIndex, value));
					//���    
					//���ս��
					State * state = findState(key);
					if (state->type_ == 1){
						//�������ս����ת�Ƶ���һ��״̬  ��3000-4000�������ĺ���λ������Ҫ��ת����״̬��
						if (table[itemSet->id_][state->pos_] != 0){
							cout << "�ظ��޸�" << endl;
						}
						table[itemSet->id_][state->pos_] = 3000 + currentItemSetIndex;
					}
					else{
						//cout << "*****��������Ŀ��  ������ isNeedToBeMerged begin key = " << key << "  itemSet size :" << itemSet->items_.size() << endl;
						//int isNeedTobeMerged_ = isNeedToBeMerged(itemSet, key);
						//cout << "��������Ŀ�� isNeedToBeMerged_ = " << isNeedTobeMerged_ << endl;
						//cout << "*****��������Ŀ��  ������ isNeedToBeMerged done key = " << key << "  itemSet size :" << itemSet->items_.size() << endl;
						//if (isNeedTobeMerged_ == -1){
						//�ƽ� ��1000-2000�������ĺ���λ����Ҫ�ƽ���״̬��
						table[itemSet->id_][state->pos_] = 1000 + currentItemSetIndex;
						//}
						//else{
						//��Լ ��2000-3000�������ĺ���λ������ݵ�XXX������ʽ��Լ
						//	table[itemSet->id_][state->pos_] = 2000 + isNeedTobeMerged_;
						//}
					}

				}
				else{
					//����
					//���    
					//���ս��
					State * state = findState(key);
					if (state->type_ == 1){
						//�������ս����ת�Ƶ���һ��״̬  ��3000-4000�������ĺ���λ������Ҫ��ת����״̬����
						table[itemSet->id_][state->pos_] = 3000 + code;
					}
					else{

						//int isNeedTobeMerged_ = isNeedToBeMerged(itemSet, key);
						//cout << "ת��������Ŀ�� isNeedToBeMerged_ = " << isNeedTobeMerged_ << endl;
						//if (isNeedTobeMerged_ == -1){
						//�ƽ� ��1000-2000�������ĺ���λ����Ҫ�ƽ���״̬��
						table[itemSet->id_][state->pos_] = 1000 + code;
						//}
						//else{
						//��Լ ��2000-3000�������ĺ���λ������ݵ�XXX������ʽ��Լ
						//	table[itemSet->id_][state->pos_] = 2000 + isNeedTobeMerged_;
						//}
					}
				}
			}


		}
	}
	//��ʼ��FIRST����FOLLOW��
	void SynataxAnalyze::initFirstAndFollow(){
		//ȷ��FIRST������ϵ
		for (int i = 0; i < allProductions.size(); i++){
			Production * p = allProductions[i];
			State * start = p->start_;
			vector<State*> end = p->end_;
			if (!start->equalsTo(end[0]) && end[0]->type_ == 1){
				end[0]->first_relate.push_back(start);
			}
		}
		//��FIRST��
		for (int i = 0; i < allProductions.size(); i++){
			Production * p = allProductions[i];
			State * start = p->start_;
			vector<State*> end = p->end_;
			if (end[0]->type_ == 0){
				start->addToFIRST(end[0]);
			}
		}

		//ȷ��FOLLOW������ϵ
		for (int i = 0; i < allProductions.size(); i++){
			Production * p = allProductions[i];
			State * start = p->start_;
			vector<State*> end = p->end_;
			if (end[end.size() - 1]->type_ == 1 && !end[end.size() - 1]->equalsTo(start)){
				start->follow_relate.push_back(end[end.size() - 1]);
			}
		}

		//��FOLLOW
		for (int i = 0; i < allProductions.size(); i++){
			Production * p = allProductions[i];
			State * start = p->start_;
			if (start->name_ == "A"){
				start->addToFOLLOW(findState("#"));
			}
			vector<State*> end = p->end_;
			for (int j = 0; j < end.size(); j++){
				State * temp = end[j];
				if (j < (end.size() - 1) && end[j]->type_ == 1){
					if (end[j + 1]->type_ == 0){
						temp->addToFOLLOW(end[j + 1]);
					}
					else{
						vector<State*> tempVector = end[j + 1]->getFIRST();
						for (int z = 0; z < tempVector.size(); z++){
							temp->addToFOLLOW(tempVector[z]);
						}
					}
				}
			}
		}
		cout << "init follow first done" << endl;
		////���ж�����ǵ�һ��������
		//State * start = p->start_;
		////�������ʽ���ǿ�ʼ���ţ�����"#"��FOLLOW(A)�С���AΪ��ʼ����
		//if (start->name_ == "A"){
		//	start->myFOLLOW.push_back("#");
		//	start->myFOLLOW_size++;
		//}
		//vector<State*> end = p->end_;
		//for (int i = 0; i < end.size(); i++){
		//	State * state = end[i];
		//	if (i == 0&&!start->equalsTo(state)&&!start->existInFIRST(state)){
		//		//����ʽ�Ҳ���һ�� Ϊ�ս�� �����FIRST��start��
		//		start->myFIRST.push_back(state->name_);
		//	}
		//	if (state->type_ == 1){
		//		if (i != end.size() - 1 && !state->existInFOLLOW(end[i + 1])){
		//			state->myFOLLOW.push_back(end[i + 1]->name_);
		//			//action��¼��ĳһλ���ϵķ��ս��Ӧ�ý�����FIRST ������FOLLOW
		//			state->action[state->myFOLLOW_size++] = 0;//��FIRST
		//		}
		//		else if(i == end.size() - 1&& !state->existInFOLLOW(start)){
		//			state->myFOLLOW.push_back(start->name_);
		//			state->action[state->myFOLLOW_size++] = 1;//��FOLLOW
		//		}
		//	}
		//	
		//}

	}

	void SynataxAnalyze::init(){

		//��ȡ״̬��
		char buffer1[256];
		ifstream myfile3("d:\\a.txt");

		if (!myfile3){
			cout << "Unable to open myfile";
			exit(1); // terminate with error

		}
		int index = 0;
		while (!myfile3.eof())
		{
			myfile3.getline(buffer1, 10);
			string str(buffer1);
			char c1[] = { str[0], 0 };
			//map��¼�ķ����Ŷ�Ӧ����λ�ã��������ķ����ŵ�����˳��
			g_index[string(c1)] = index;
			g_.push_back(nameMap[string(c1)] = new State(string(c1), str[1] - 48, index++));
		}
		myfile3.close();

		//��ȡ����ʽ
		char buffer2[256];
		ifstream myfile2("d:\\b.txt");

		if (!myfile2){
			cout << "Unable to open myfile";
			exit(1); // terminate with error

		}
		index = 0;
		while (!myfile2.eof())
		{

			myfile2.getline(buffer2, 10);
			string str(buffer2);//�����ʽ��
			char c1[] = { str[0], 0 };
			State * start = findState(string(c1));
			string str3(buffer2 + 2);//�����ʽ�Ҳ�
			vector<State*> end(str3.size());

			for (int i = 0; i < str3.size(); i++){
				char cx[] = { str3[i], 0 };
				end[i] = findState(string(cx));
			}
			Production *p = new Production(start, end, index);
			allProductions.push_back(p);
			if (index == 0){
				production1 = p;//��һ������ʽ
			}
			index++;
		}
		myfile2.close();

		itemSetId_stack.push(0);//��0״̬����ջ
		state_stack.push(*findState("#"));//��#�ַ�������ջ
	}
	//�ͷ��ڴ�
	void SynataxAnalyze::free(){
		for (int x = 0; x < g_.size(); x++){
			delete g_[x];
		}
		for (int x = 0; x < allProductions.size(); x++){
			delete allProductions[x];
		}

		list<ItemSet*>::iterator it;
		for (it = c.begin(); it != c.end(); it++){
			ItemSet * is = *it;
			list<Item*> items = is->items_;
			list<Item*>::iterator ii;
			for (ii = items.begin(); ii != items.end(); ii++){
				delete *ii;
			}
			delete is;
		}
	}

	Production * SynataxAnalyze::findProduction(int pos){
		for (int i = 0; i < allProductions.size(); i++){
			if (allProductions[i]->pos_ == pos){
				return allProductions[i];
			}
		}
	}

	string procedureId;//������
	//�����ַ���
	void enter(characterRecord & cr){
		for (int i = 0; i < currentIndex; i++){
			if (characterTable[i].kind == cr.kind && characterTable[i].name == cr.name){
				return;
			}
		}
		characterTable[currentIndex++] = cr;
	}

	void enterCharacterTable(Production * production, WordAnalyze & wa){
		//˵�����ֽ������ͳ���������ű�
		if (production->pos_ == 10){
			//D->b=a ��������
			//cout << "string:" << wa.id << "   num:" << wa.num << endl;
			characterRecord cr = { wa.id, 0, wa.num, LEVEL, -1 };
			enter(cr);
		}
		else if (production->pos_ == 52 || production->pos_ == 51){
			// X->b  X->X,b ��������
			characterRecord cr = { wa.id, 1, -1, LEVEL, DXTable[LEVEL]++ };
			enter(cr);
		}
		else if (production->pos_ == 13){
			//G->eb;  ���̶���
			characterRecord cr = { procedureId, 2, codeIndex, LEVEL, DXTable[LEVEL]++ };
			LEVEL++;
			enter(cr);
		}
		else if (production->pos_ == 12){
			//F->GB; ����˵�����ֹ�Լ
			LEVEL--;
		}
	}

	//����ĸ���ҵ�����ַ�
	characterRecord * findCharacterFromTable(string id){
		for (int i = 0; i < currentIndex; i++){
			if (characterTable[i].name == id){
				return &characterTable[i];
			}
		}
		return NULL ;
	}

	//��������
	void gen(string f, int l, int a){
		instruction i = { f, l, a };
		code[codeIndex++] = i;
	}

	//����Ŀ�����  >=0�����������ɴ���ɹ��������ش������  -1����Ҫ���ɴ���  -2���ű��Ҳ�������(���﷨����)
	int generateCode(State & startState,Production * production, SynataxAnalyze & sa){
		int start = -1;
		
		if (production->pos_ == 31){
			//N->b  ����->��ʶ��
			State state = sa.state_stack.top();
			cout << "id:" << state.id_ << endl;
			characterRecord * cr = findCharacterFromTable(state.id_);
			if (cr != NULL){
				start = codeIndex;
				if (cr->kind == 1){
					gen(string("LOD"), LEVEL - (cr->level), cr->adr);
				}
				else{
					gen(string("LIT"), 0, cr->val);
				}
			}
			else{
				return -2;
			}
		}
		else if (production->pos_ == 32){
			//N-> a ����->�޷�������
			State  state = sa.state_stack.top();
			start = codeIndex;
			gen(string("LIT"), 0, state.val_);
		}
		else if (production->pos_ == 29){
			//M->MPN  <��> �� <����><�˳������><����>
			State top = sa.state_stack.top();
			sa.state_stack.pop();
			State state = sa.state_stack.top();
			sa.state_stack.pop();
			State top3 = sa.state_stack.top();
			sa.state_stack.push(state);
			sa.state_stack.push(top);
			if (top3.nextCode != -1){
				start = top3.nextCode;
			}
			else if (state.nextCode != -1){
				start = state.nextCode;
			}
			else if (top.nextCode != -1){
				start = top.nextCode;
			}
			if (state.id_ == "*"){
				gen(string("OPR"), 0, 4);
			}
			else if (state.id_ == "/"){
				gen(string("OPR"), 0, 5);
			}
		}
		else if (production->pos_ == 27){
			//L->-M  <���ʽ> �� -<��>
			start = sa.state_stack.top().nextCode;
			gen(string("OPR"), 0, 3);
		}
		else if (production->pos_ == 28){
			//L->+M  <���ʽ> �� +<��>
			start = sa.state_stack.top().nextCode;
			gen(string("OPR"), 0, 2);
		}
		else if (production->pos_ == 25){
			//L��MOM
			State  top = sa.state_stack.top();
			sa.state_stack.pop();
			State  state = sa.state_stack.top();
			sa.state_stack.pop();
			State top3 = sa.state_stack.top();
			sa.state_stack.push(state);
			sa.state_stack.push(top);

			if (top3.nextCode != -1){
				start = top3.nextCode;
			}
			else if (state.nextCode != -1){
				start = state.nextCode;
			}
			else if (top.nextCode != -1){
				start = top.nextCode;
			}

			if (state.id_ == "+"){
				gen(string("OPR"), 0, 2);
			}
			else if (state.id_ == "-"){
				gen(string("OPR"), 0, 3);
			}
		}
		else if (production->pos_ == 21){
			//I��b: = L
			State top1 = sa.state_stack.top();
			sa.state_stack.pop();
			State  top2 = sa.state_stack.top();
			sa.state_stack.pop();
			State state = sa.state_stack.top();
			sa.state_stack.push(top2);
			sa.state_stack.push(top1);

			if (state.nextCode != -1){
				start = state.nextCode;
			}
			else if (top2.nextCode != -1){
				start = top2.nextCode;
			}
			else if (top1.nextCode != -1){
				start = top1.nextCode;
			}
			characterRecord * cr = findCharacterFromTable(state.id_);
			if (cr != NULL){
				gen(string("STO"), LEVEL - (cr->level), cr->adr);
			}
			else{
				return -2;
			}
		}
		else if (production->pos_ == 23){
			//K->LQL  ��ԼΪ<����> ������תָ��  ��¼����Ҫ����λ��
			
			State  top = sa.state_stack.top();
			sa.state_stack.pop();
			State  state = sa.state_stack.top();
			sa.state_stack.pop();
			State top3 = sa.state_stack.top();
			sa.state_stack.push(state);
			sa.state_stack.push(top);
			if (top3.nextCode != -1){
				start = top3.nextCode;
			}
			else if (state.nextCode != -1){
				start = state.nextCode;
			}
			else if (top.nextCode != -1){
				start = top.nextCode;
			}

			if (state.id_ == "="){
				gen(string("OPR"), 0, 8);
			}
			else if (state.id_ == "#"){
				gen(string("OPR"), 0, 9);
			}
			else if (state.id_ == "r"){
				gen(string("OPR"), 0, 13);
			}
			else if (state.id_ == "s"){
				gen(string("OPR"), 0, 12);
			}
			else if (state.id_ == ">"){
				gen(string("OPR"), 0, 11);
			}
			else if (state.id_ == "<"){
				gen(string("OPR"), 0, 10);
			}
			//����λ��
			startState.backpatchIndex = codeIndex;
			gen(string("JPC"), 0, 0);
		}
		else if (production->pos_ == 24){
			//K->hL  ��ԼΪ<����> ������תָ��  ��¼����Ҫ����λ��
			start = sa.state_stack.top().nextCode;
			gen(string("OPR"), 0, 6);
			startState.backpatchIndex = codeIndex;
			gen(string("JPC"), 0, 0);
		}
		else if (production->pos_ == 46){
			//T��nKoH		<����ѭ�����> �� while<����>do<���>
			State top1 = sa.state_stack.top();
			sa.state_stack.pop();
			State top2 = sa.state_stack.top();
			sa.state_stack.pop();
			State top3 = sa.state_stack.top();

			if (top3.nextCode != -1){
				start = top3.nextCode;
			}
			else if (top2.nextCode != -1){
				start = top2.nextCode;
			}
			else if (top1.nextCode != -1){
				start = top1.nextCode;
			}
			startState.backpatchIndex = top3.backpatchIndex;
			gen(string("JMP"), 0, sa.state_stack.top().nextCode);
			sa.state_stack.push(top2);
			sa.state_stack.push(top1);

		}
		else if (production->pos_ == 44){
			//R��pKqH		<�������> �� if<����>then<���>
			State top1 = sa.state_stack.top();
			sa.state_stack.pop();
			State top2 = sa.state_stack.top();
			sa.state_stack.pop();
			State top3 = sa.state_stack.top();

			if (top3.nextCode != -1){
				start = top3.nextCode;
			}
			else if (top2.nextCode != -1){
				start = top2.nextCode;
			}
			else if (top1.nextCode != -1){
				start = top1.nextCode;
			}
			startState.backpatchIndex = top3.backpatchIndex;
			sa.state_stack.push(top2);
			sa.state_stack.push(top1);
		}
		else if (production->pos_ == 47){
			//U��i(X)	<�����> �� read(<һ��������ʶ��>)
			State top1 = sa.state_stack.top();
			sa.state_stack.pop();
			State top2 = sa.state_stack.top();
			sa.state_stack.push(top1);
			start = top2.nextCode;
			gen(string("OPR"),0,16);
		}
		else if (production->pos_ == 48){
			//V��j(Z) <д���> �� write(<һ���������ʽ>)
			State top1 = sa.state_stack.top();
			sa.state_stack.pop();
			State top2 = sa.state_stack.top();
			sa.state_stack.push(top1);
			start = top2.nextCode;
			gen(string("OPR"), 0, 14);
			gen(string("OPR"), 0, 15);
		}
		else if (production->pos_ == 45){
			//S��mb		<���̵������> �� call<��ʶ��>
			State state = sa.state_stack.top();
			characterRecord * cr = findCharacterFromTable(state.id_);
			if (cr != NULL){
				start = codeIndex;
				gen(string("CAL"), LEVEL - (cr->level), cr->val);
			}
			else{
				return -2;
			}
		}
		if (start == -1){
			return -1;//����Ҫ���ɴ���
		}
		else{
			return start;
		}
	}


	//�������������ʽ
	void processOPR(State & start,Production * production, SynataxAnalyze & sa){
		if (production->pos_ == 34){
			//O��+
			start.id_ = "+";
		}
		else if (production->pos_ == 35){
			//O��-
			start.id_ = "-";
		}
		else if (production->pos_ == 36){
			//P->*
			start.id_ = "*";
		}
		else if (production->pos_ == 37){
			//P->/
			start.id_ = "/";
		}
		else if (production->pos_ == 38){
			//Q->=
			start.id_ = "=";
		}
		else if (production->pos_ == 39){
			//Q->#
			start.id_ = "#";
		}
		else if (production->pos_ == 40){
			//Q-><
			start.id_ = "<";
		}
		else if (production->pos_ == 41){
			//Q->r
			start.id_ = "<=";
		}
		else if (production->pos_ == 42){
			//Q->>
			start.id_ = ">";
		}
		else if (production->pos_ == 43){
			//Q->s
			start.id_ = ">=";
		}
	}

	//����
	void backpatch(int index, int backpatchVal){
		instruction in = code[index];
		in.a = backpatchVal;
		code[index] = in;
	}

	//ÿ�ν��й�Լ��ʱ��ɨ���Լ����ʽ������Ҫ����ҿ��Ի���������򴫵ݸ���һ��
	void ifBackpatch(State & start, Production * production, SynataxAnalyze & sa){
		if (production->pos_ == 44 || production->pos_ == 46){
			return;
		}
		vector<State> states;
		int size = production->end_.size();
		for (int i = 0; i < size; i++){
			State top = sa.state_stack.top();
			states.push_back(top);
			sa.state_stack.pop();
		}
		//�ָ�stack�ṹ
		for (int i = size - 1; i >= 0; i--){
			sa.state_stack.push(states[i]);
		}
		bool backpatchDone = false;
		for (int i = size - 1; i >= 0; i--){
			State state = states[i];
			if (state.backpatchIndex != -1 && i != 0){
				for (int j = i - 1; j >= 0; j--){
					if (states[j].nextCode != -1){
						backpatch(state.backpatchIndex, states[j].nextCode);
						backpatchDone = true;
						break;
					}
				}
				if (!backpatchDone){
					//������һ��
					start.backpatchIndex = state.backpatchIndex;
					break;
				}

			}
			else if (state.backpatchIndex != -1 && i == 0){
				//������һ��
				start.backpatchIndex = state.backpatchIndex;
				break;
			}
		}
	}

	void noNeedToGenerateCode(State & startState, Production * production, SynataxAnalyze & sa){
		vector<State> states;
		int size = production->end_.size();
		for (int i = 0; i < size; i++){
			State top = sa.state_stack.top();
			states.push_back(top);
			sa.state_stack.pop();
		}
		//�ָ�stack�ṹ
		for (int i = size - 1; i >= 0; i--){
			sa.state_stack.push(states[i]);
		}
	
		for (int i = size - 1; i >= 0; i--){
			if (states[i].nextCode != -1){
				startState.nextCode = states[i].nextCode;
				break;
			}
		}

	}

	//0��Ҫ�ı������������  1����Ҫ�ı������������  2�����ɹ�  3����ʧ��
	int process(State & s, SynataxAnalyze & sa, WordAnalyze & wa){
		int pos_ = s.pos_;
		int topItemSetId = sa.itemSetId_stack.top();
		int action = table[topItemSetId][pos_];
		//cout << "***************************" << endl;
		//cout << "���������ַ�:" << s->name_ << "  ״̬ջջ��Ϊ��" << topItemSetId<<"  ����ջջ��Ϊ��"<<sa.state_stack.top() << endl;

		if (action == 0){
			return 3;
		}
		if (action == 4000){
			return 2;
		}
		else if (action >= 2000 && action < 3000){
			//����which������ʽ��Լ
			int which = action - 2000;
		//	cout << "���յ�" << which << "������ʽ��Լ..." << endl;
			Production * production = sa.findProduction(which);
			State start = *(production->start_);
			//�ж��Ƿ���Ҫ����
			ifBackpatch(start, production, sa);
			//�����ַ���
			enterCharacterTable(production, wa);
			//�������������ʽ
			processOPR(start,production, sa);
			//����Ŀ�����
			int success = generateCode(start,production, sa);
			//����nextCode
			if (success != -1){
				start.nextCode = success;
			}
			else{
				noNeedToGenerateCode(start, production, sa);
			}
			
			int size = production->end_.size();
			for (int i = 0; i < size; i++){
				sa.itemSetId_stack.pop();
				sa.state_stack.pop();
			}
			int code = table[sa.itemSetId_stack.top()][production->start_->pos_];
			if (code == 0){
				return 3;
			}
			//cout << "���½�״̬��" << code - 3000 << "����״̬ջ  �ַ�ջѹ��" << production->start_->name_ << endl;
			sa.itemSetId_stack.push( code - 3000);
			sa.state_stack.push(start);
			return 1;
		}
		else if (action >= 1000 && action < 2000){
			//�ƽ���which״̬��
			int which = action - 1000;
			//cout << "��״̬" << which << "����״̬ջ    " << s->name_ << "�������ջ" << endl;
			sa.itemSetId_stack.push(which);
			sa.state_stack.push(s);
			if (s.name_ == "."){
				sa.judge++;
			}
			return 0;
		}

		cout << endl;
	}

	void analyze(){
		WordAnalyze wa;
		SynataxAnalyze sa;
		sa.init();
		sa.initFirstAndFollow();
		sa.writeTable();
		wa.getsym();
		while (true&&(sa.judge != 2)){

			int sym_ = wa.sym;
			char ch_ = wa.ch;
			int num_ = wa.num;
			string id_ = string(wa.id);
			int code = 0;
			switch (sym_)
			{
			case SYM_NULL:{
							//Υ��...ֱ�Ӳ�������
							code = 3;
							break;
			}
			case SYM_IDENTIFIER:{
							//��ʶ��
							State s = (*sa.findState("b"));
							//��¼�±�ʶ��id
							s.id_ = wa.id;
							//code = process(s, sa);
							if (sa.state_stack.top().name_ == "e"){
								//���ջ���Ѿ���procedure  ����ô��¼�������ʶ��ֵ
								procedureId = wa.id;
							}
							code = process(s, sa, wa);

							break;
			}
			case SYM_NUMBER:{
							//����
							State s = (*sa.findState("a"));
							//��¼�����ֵ�ֵ..
							s.val_ = wa.num;
							code = process(s, sa, wa);
							break;
			}
			case SYM_PLUS:{
							 
							State s = (*sa.findState("+"));
							code = process(s, sa, wa);
							break;
			}
			case SYM_MINUS:{
							State s = (*sa.findState("-"));
							code = process(s, sa, wa);
							break;
			}
			case SYM_TIMES:{
							State  s = (*sa.findState("*"));
							code = process(s, sa, wa);
							break;
			}
			case SYM_SLASH:{
							State s = (*sa.findState("/"));
							code = process(s, sa, wa);
							break;
			}
			case SYM_ODD:{
							//odd
							State s = (*sa.findState("h"));
							code = process(s, sa, wa);
							break;
			}
			case SYM_EQU:{
							State s = (*sa.findState("="));
							code = process(s, sa, wa);
							break;
			}
			case SYM_LES:{
							State s = (*sa.findState("<"));
							code = process(s, sa, wa);
							break;
			}
			case SYM_LEQ:{
							State  s = (*sa.findState("r"));
							code = process(s, sa, wa);
							break;
			}
			case SYM_GTR:{
							State s = (*sa.findState(">"));
							code = process(s, sa, wa);
							break;
			}
			case SYM_GEQ:{
							State s = (*sa.findState("s"));
							code = process(s, sa, wa);
							break;
			}
			case SYM_LPAREN:{
							State s = (*sa.findState("("));
							code = process(s, sa, wa);
							break;
			}
			case SYM_RPAREN:{
							State s = (*sa.findState(")"));
							code = process(s, sa, wa);
							break;
			}
			case SYM_COMMA:{
							State s = (*sa.findState(","));
							code = process(s, sa, wa);
							break;
			}
			case SYM_SEMICOLON:{
							State s = (*sa.findState(";"));
							code = process(s, sa, wa);

							break;
			}
			case SYM_PERIOD:{
							State s = (*sa.findState("."));
							if (sa.judge == 0){
								sa.judge++;
							}
							code = process(s, sa, wa);
							break;
			}
			case SYM_BECOMES:{
							//:=
							State  s = (*sa.findState("t"));
							code = process(s, sa, wa);

							break;
			}
			case SYM_BEGIN:{
							//begin
							State  s = (*sa.findState("f"));
							code = process(s, sa, wa);

							break;
			}
			case SYM_END:{
							 //end
							State s = (*sa.findState("g"));
							code = process(s, sa, wa);

					    	break;
			}
			case SYM_IF:{
							//if
							State s = (*sa.findState("p"));
							code = process(s, sa, wa);

							break;
			}
			case SYM_THEN:{
							  //then
							State  s = (*sa.findState("q"));
							code = process(s, sa, wa);

							break;
			}
			case SYM_WHILE:{
							   //while
							State  s = (*sa.findState("n"));
							code = process(s, sa, wa);

							break;
			}
			case SYM_DO:{
							//do
							State  s = (*sa.findState("o"));
							code = process(s, sa, wa);

							break;
			}
			case SYM_CALL:{
							  //call
							State  s = (*sa.findState("m"));
							code = process(s, sa, wa);

							break;
			}
			case SYM_CONST:{
							   //const
							State  s = (*sa.findState("c"));
							code = process(s, sa, wa);

							break;
			}
			case SYM_VAR:{
							 //var
							State s = (*sa.findState("d"));
							code = process(s, sa, wa);

							break;
			}
			case SYM_PROCEDURE:{
								   //procedure
							State  s = (*sa.findState("e"));
							code = process(s, sa, wa);

							break;
			}
			case SYM_READ:{
								   //read
							State s = (*sa.findState("i"));
							code = process(s, sa, wa);

							break;
			}
			case SYM_WRITE:{
								   //write
							State  s = (*sa.findState("j"));
							code = process(s, sa, wa);
							break;
			}
			case SYM_X1:{
							   //#
							State  s = (*sa.findState("#"));
							code = process(s, sa, wa);
							break;
			}

			default:
				cout << "�Ҳ���ƥ����ַ�: sym = " << sym_ << "  num = " << num_ << "  id_ = " << id_ << endl;
				code = 3;
				break;
			}

			if (code == 3){
				cout << "�﷨���ִ���.." << endl;
				return;
			}
			else if (code == 0){
				wa.getsym();
			}
		}

		if (table[sa.itemSetId_stack.top()][sa.findState("#")->pos_] == 4000){
			//��ʱջ������ B.  ��ʱB�������Ҫ�����ֱ�ӻ���������ĩβ
			sa.state_stack.pop();
			if (sa.state_stack.top().backpatchIndex != -1){
				backpatch(sa.state_stack.top().backpatchIndex, codeIndex );
			}
			gen(string("INT"), 0, 0);
			cout << "�������....�﷨��ȷ" << endl;
		}
		else{
			cout << "�������....�﷨����" << endl;
		}
		sa.printTable();
	}
}