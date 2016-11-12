/*决策树 每一个DataNode存一组数据，一个property代表一项性质*
*一个二维链表用于存放每个proprty有哪些内容*/

#include <iostream>
#include <cstdio>
#include <string>
#include <cmath>
#include <fstream>
using namespace std;
#define NumofProperty 22
int TotalofAll;//Optimization
int CountFalse, CountTrue;
int totalNode;

struct PropNode {
	string Ans;
	PropNode* next;

	PropNode() {
		Ans = "#";
		next = NULL;
	}

	PropNode(string x) {
		Ans = x;
		next = NULL;
	}

	void InsertAfter(PropNode* x) {
		x->next = next;
		next = x;
	}
};

struct DataNode {
	string Ans[NumofProperty];
	string result;
	DataNode* next;


	DataNode() {	//Important!!!!!!!!!!!
		for (int i = 0; i < NumofProperty; i++) {
			Ans[i] = "#";
		}
		next = NULL;
	}

	void InsertAfter(DataNode* x) {
		x->next = next;
		next = x;
	}
};

struct TreeNode {
	int PrevAns, NextAns;
	string FinalAns;
	string Answer[NumofProperty];
	TreeNode* FirstChild;
	TreeNode* NextBro;

	TreeNode() {
		for (int i = 0; i < NumofProperty; i++) {
			Answer[i] = "#";
		}
		FinalAns = "#";
		FirstChild = NULL;
		NextBro = NULL;
		PrevAns = NextAns = -1;
	}

	void InsertAfter(TreeNode* x) {
		x->NextBro = NextBro;
		NextBro = x;
	}
};

//全局变量
DataNode* HeadofData;
PropNode** Property;

void ReadInput(string InputFile) {
	TotalofAll = 0;
	ifstream IN(InputFile, ios::in);
	string buffer;
	if (!IN.is_open()) {
		exit(1);
	}
	free(HeadofData);
	HeadofData = new DataNode;
	while (!IN.eof()) {
		DataNode* temp = new(DataNode);
		if (!temp) {
			exit(2);
		}
		for (int i = 0; i < NumofProperty; i++) {
			getline(IN, temp->Ans[i], ',');
		}
		getline(IN, temp->result);
		HeadofData->InsertAfter(temp);
		TotalofAll++;
	}
	IN.close();
//	return HeadOfData;
}

void CreateTable(){
//	PropNode* Property[NumofProperty] = *PropertyPTR;
//	Property = (PropNode**)malloc(NumofProperty * sizeof(PropNode*));
	Property = new PropNode*[NumofProperty];
	for (int i = 0; i < NumofProperty; i++) {
		Property[i] = new PropNode;
	}
	DataNode* temp = HeadofData;
	while (temp->next != NULL) {//默认构造函数指向NULL
		temp = temp->next;
		for (int i = 0; i < NumofProperty; i++) {
			string answer = temp->Ans[i];
			bool flag = true;
			PropNode* iterator = Property[i];
			while (iterator->next != NULL) {
				iterator = iterator->next;
				if (answer == iterator->Ans) {
					flag = false;
					break;
				}
			}
			if (flag) {
				PropNode* x = new PropNode(answer);
				Property[i]->InsertAfter(x);
//				cout << Property[i]->next->Ans << endl;
			}
		}
	}
//	return Property;
}

//计算当前选择对应Edible & Unedible个数,返回总数
//O(mn)
int Calculate(string Answer[], int Counter[]) {
//	long double Entropy;
	int Total = 0;
	Counter[0] = Counter[1] = 0;
	DataNode* temp = HeadofData;
	while (temp->next != NULL) {
		temp = temp->next;
		bool flag = true;
		for (int i = 0; i < NumofProperty; i++) {
			if (Answer[i] != "#" && Answer[i] != temp->Ans[i]) {
				flag = false;
				break;
			}
		}
		if (flag) {
//			Total++;
			if (temp->result == "EDIBLE")
				Counter[0]++;
			else Counter[1]++;
		}
	}
	return Counter[0] + Counter[1];
}

long double CalEntropy(int Total, int Counter[]) {
	if (Total == 0)
		exit(2);
	long double Entropy = 0;
	long double Pb[2];
	for (int i = 0; i < 2; i++) {
		Pb[i] = (long double)Counter[i] / (long double)Total;
		if (Pb[i] != 0 && Pb[i] != 1) {
			Entropy += -Pb[i] * log2(Pb[i]);
		}
	}
	return Entropy;
}

void SetFinal(TreeNode* Root, int Edible[]) {
	if (Edible[0] > Edible[1]) {
		Root->FinalAns = "EDIBLE";
	}
	else {
		Root->FinalAns = "POISONOUS";
	}
	if (Edible[0] == 0 && Edible[1] == 0) {
		Root->FinalAns = "UNKNOWN";
	}
}

TreeNode* BuildTree(string Ans[], int PrevAns, int depth) {
	totalNode++;
	TreeNode* Root = new TreeNode;
	Root->PrevAns = PrevAns;
	for (int i = 0; i < NumofProperty; i++) {
		Root->Answer[i] = Ans[i];
	}

	//Determine whether to end the recurssion
	int Edible[2];
	int Total = Calculate(Ans, Edible);
	if (depth == NumofProperty || Total == 0) {
		SetFinal(Root, Edible);
		return Root;
	}
	long double EntropyNow = CalEntropy(Total, Edible);
	if (EntropyNow == 0) {
		SetFinal(Root, Edible);
		return Root;
	}

	//TO DO find the maximum point to determine
	string NowAns[NumofProperty];
	int NextAns;
	long double MinEntropy = 0xfffff;
	for (int i = 0; i < NumofProperty; i++) {
		if (Root->Answer[i] != "#") continue;
		for (int i = 0; i < NumofProperty; i++) {
			NowAns[i] = Root->Answer[i];
		}
		long double TotalofThis = 0;
		long double TotalEntropy = 0;
		PropNode* temp = Property[i];
		while ( temp->next != NULL) {
			int tempTotal;
			temp = temp->next;
			NowAns[i] = temp->Ans;
			tempTotal = Calculate(NowAns, Edible);
			TotalofThis += tempTotal;
			if (tempTotal == 0) continue;
			TotalEntropy += tempTotal * CalEntropy(tempTotal, Edible);
		}
		if (TotalofThis == 0)continue;
		long double EntropyofThis = TotalEntropy / (long double)TotalofThis;
		if (EntropyofThis < MinEntropy) {
			MinEntropy = EntropyofThis;
			NextAns = i;
		}
	}


	//Build the tree based on NextAns
	Root->NextAns = NextAns;
	for (int i = 0; i < NumofProperty; i++) {
		NowAns[i] = Root->Answer[i];
	}
	PropNode* temp = Property[Root->NextAns]->next;
	NowAns[Root->NextAns] = temp->Ans;
	Root->FirstChild = BuildTree(NowAns, Root->NextAns, depth + 1);
	while (temp->next != NULL) {
		temp = temp->next;
		NowAns[Root->NextAns] = temp->Ans;
		Root->FirstChild->InsertAfter(BuildTree(NowAns, Root->NextAns, depth + 1));
	}

	return Root;
}

void Test(DataNode* HeadofTestData, TreeNode* DetermineTree) {
	DataNode* temp = HeadofTestData;
	CountFalse = 0;
	CountTrue = 0;
	while (temp->next != NULL) {
		temp = temp->next;
		TreeNode* Now = DetermineTree;
		string FinalAns;
		string NextAnsNow = temp->Ans[Now->NextAns];
		Now = Now->FirstChild;
		while (true) {
			bool flag = true; // test if it has found consistent TreeNode
			while (Now != NULL) {
				if (Now->Answer[Now->PrevAns] == NextAnsNow) {
					flag = false;
					break;
				}
				Now = Now->NextBro;
			}
			if (flag) {
				FinalAns = "UNKNOWN";
				break;
			}
			if (Now->FinalAns != "#") {
				FinalAns = Now->FinalAns;
				break;
			}
			NextAnsNow = temp->Ans[Now->NextAns];
			Now = Now->FirstChild;
		}
		if (FinalAns == temp->result) {
			CountTrue++;
		}
		else {
			CountFalse++;
		}
	}
}

int main() {
	totalNode = 0;
	ReadInput("expanded.Train.txt");
	//refer to all answers of 22 properties带头结点的单链表
	CreateTable();

	string InitAns[NumofProperty];
	for (int i = 0; i < NumofProperty; i++) {
		InitAns[i] = "#";
	}
	TreeNode* DetermineTree = BuildTree(InitAns, -1, 0);

	ReadInput("expanded.Test.txt");
	Test(HeadofData, DetermineTree);

//	cout << totalNode << endl;

	return 0;
}
