/*	Class: 	CS315
 *	Author: Connor Leslie
 * 	Date: 	5/5/2021
 */	

#include <iostream>
#include <string>
#include <vector>
using namespace std;
#define INDEXMAX 100
#define CHIMAX 10

struct per{
	int pFam;//ID of the family that given person is a parent
	int cFam;//ID of the familay That given person is a child
};

struct fam{
	int ID;
	int repID;
	int husID;
	int wifID;
	//partial array for children
	int chiAmo;
	int chiID[CHIMAX];
};

//helps debugg
void printVector(vector<string> cmd){
	cout << "cmd: ";
	for(int i =0; i < (int)cmd.size(); i++){
		cout << cmd.at(i) << " ";
	}
	cout << "\n";
}

//print a Family Command
void printFamCmd(vector<string> cmd){
	string pstr = "Family " + cmd.at(1) + " has husband " + cmd.at(3) + ", wife " + cmd.at(5) + ", and children";
	int chiAmo = (cmd.size()-6)/2;
	for(int i = 0; i< chiAmo; i++){
		pstr += " " + cmd.at((2*i)+7); 
	}
	pstr += ".\n";
	cout << pstr;
}

//modify given vector to contain command in words seperated by ' '
void parse(string input, vector<string> &cmd){
	cmd.clear();
	string tmpString = "";
	int lastIndex = 0;
	int i =0;
	for(; i < (int)input.size(); i++){
		if(input.at(i) == ' '){
			cmd.push_back(input.substr(lastIndex, i - lastIndex));
			lastIndex = i+1;
		}
	}
	cmd.push_back(input.substr(lastIndex, i-lastIndex));
}

//add family Members to nextID and saved relation to given person all if they have not been seen yet
void pushValidFamMem(int relatedTo[], vector<int> &nextID, fam genealogy[], int family, int tmpPerID){
	int inHusID = genealogy[family].husID;
	int inWifID = genealogy[family].wifID;
	if((relatedTo[inHusID] < 0)){
		nextID.push_back(genealogy[family].husID);
		relatedTo[inHusID] = tmpPerID;
	}
	if((relatedTo[inWifID] < 0)){
		nextID.push_back(genealogy[family].wifID);
		relatedTo[inWifID] = tmpPerID;
	}

	//children
	int iChi;
	for(int i = 0; i < genealogy[family].chiAmo; i++){
		iChi = genealogy[family].chiID[i];
		if(relatedTo[iChi] < 0){
			nextID.push_back(iChi);
			relatedTo[iChi] = tmpPerID;
		}
	}
}


//add family Members to nextID if they have not been seen yet as stored in lookedAt
void pushValidFamMem(bool lookedAt[], vector<int> &nextID, fam genealogy[], int family, int tmpPerID){
	int inHusID = genealogy[family].husID;
	int inWifID = genealogy[family].wifID;
	if(lookedAt[inHusID] == false){
		nextID.push_back(genealogy[family].husID);
		lookedAt[inHusID] = true;
	}
	if(lookedAt[inWifID] == false){
		nextID.push_back(genealogy[family].wifID);
		lookedAt[inWifID] = true;
	}
	//children
	int iChi;
	for(int i = 0; i < genealogy[family].chiAmo; i++){
		iChi = genealogy[family].chiID[i];
		if(lookedAt[iChi] == false){
			nextID.push_back(iChi);
			lookedAt[iChi] = true;
		}
	}
}

//returns a shared direct family relation given two people
//returns if they are not direct family members
int sharedFam(int per1ID, int per2ID, per perList[]){
	int p1P = perList[per1ID].pFam;
	int p1C = perList[per1ID].cFam;
	int p2P = perList[per2ID].pFam;
	int p2C = perList[per2ID].cFam;
	if((p1P == p2P) || (p1P == p2C))
		return p1P;
	if((p1C == p2P)||(p1C == p2C))
		return p1C;
	return -1;
}


//set RepIDs for all Families in the genealogy that are related
void updateRepID(fam genealogy[], per perList[], int initFamID){
	vector<int> nextID;
	int tmpPerID;
	bool lookedAt[INDEXMAX] = {false};
	for(int i = 0; i< INDEXMAX; i++){
		lookedAt[i] = false;
	}

	//set inital conditions
	genealogy[initFamID].repID = initFamID;
	//get a valid initial person
	int initPerID;
	if(genealogy[initFamID].husID > 0){
		initPerID = genealogy[initFamID].husID;
	}else if(genealogy[initFamID].wifID > 0){
		initPerID = genealogy[initFamID].wifID;
	}else{
		initPerID = genealogy[initFamID].chiID[0];
	}
	lookedAt[initPerID] = true;
	nextID.push_back(initPerID);
	
	//breadth
	while(!nextID.empty()){
		tmpPerID = nextID.back();
		nextID.pop_back();

		if(perList[tmpPerID].pFam > -1){ //family exists
			genealogy[perList[tmpPerID].pFam].repID = initFamID;
			pushValidFamMem(lookedAt, nextID, genealogy, perList[tmpPerID].pFam, tmpPerID);
		}
		if(perList[tmpPerID].cFam > -1){ //family exists
			genealogy[perList[tmpPerID].cFam].repID = initFamID;
			pushValidFamMem(lookedAt, nextID, genealogy, perList[tmpPerID].cFam, tmpPerID);
		}
	}
	
}

//handle command "RELATE"
//find the relation between to people and print
bool relatePath(fam genealogy[], per perList[], int per1ID, int per2ID){
	
	//UNION Check
	int fam1ID = perList[per1ID].cFam;
	if(perList[per1ID].pFam > -1){
		fam1ID = perList[per1ID].pFam;
	}
	int fam2ID = perList[per2ID].cFam;
	if(perList[per2ID].pFam > -1){
		fam2ID = perList[per2ID].pFam;
	}
	updateRepID(genealogy, perList, fam1ID);
	if(genealogy[fam1ID].repID != genealogy[fam2ID].repID){
		cout << "Person " << per1ID << " and Person " << per2ID << " are not related.\n";
		return false;
	}

	//at this point people are related
	//initalize variables to use
	vector<int> nextID;
	int tmpPerID;
	int relatedTo[INDEXMAX];
	for(int i= 0; i < INDEXMAX; i++){
		relatedTo[i] = -1;
	}

	//set up inital coniditions
	nextID.push_back(per2ID);
	relatedTo[per2ID] = per2ID;
	//breadth
	while(!nextID.empty()){
		//get next person
		tmpPerID = nextID.back();
		nextID.pop_back();

		if(perList[tmpPerID].pFam > -1){ //family exists
			//add family members that exist and are not seen yet
			pushValidFamMem(relatedTo, nextID, genealogy, perList[tmpPerID].pFam, tmpPerID);
		}
		if(perList[tmpPerID].cFam > -1){ //family exists
			//add family members thatexist and are not seen yet
			pushValidFamMem(relatedTo, nextID, genealogy, perList[tmpPerID].cFam, tmpPerID);
		}
	}

	//print out relation Path
	int lastID = per1ID;
	cout << "Relation: ";
	while(lastID != per2ID){
		cout << "Person " << lastID << " -> Family ";
		cout << sharedFam(lastID, relatedTo[lastID], perList);
		cout << " -> ";
		lastID = relatedTo[lastID];//update
	}
	cout << "Person: " << per2ID << endl;
	return true;
}



int main(){
	//parellel and dependant arrays
	fam genealogy[INDEXMAX];//all families
	per perList[INDEXMAX];//all people
	for(int i = 0; i <INDEXMAX; i++){
		genealogy[i].ID = -1;
		perList[i].cFam = -1;
		perList[i].pFam = -1;
	}
	//vars for inputting user commands
	string input;
	vector<string> cmd;
	do{
		//handle user input
		getline(cin, input);
		parse(input, cmd);

		//find cmd type: relate and Family
		if(cmd.at(0).compare("Family") == 0){
			//get inputs and test for Errors
			bool ERROR = false;
			int inFamID = stoi(cmd.at(1));
			int inHusID = stoi(cmd.at(3));
			int inWifID = stoi(cmd.at(5));
			int inChiAmo = (cmd.size()-6)/2;
			int inChiID[CHIMAX] = {-1};
			

			//Error Testing block
			if(genealogy[inFamID].ID > -1){
				cout << "\tFamily " << inFamID << " is already in use\n";
				ERROR = true;
			}

			if((perList[inHusID].pFam > -1)&&(inHusID != 0 )){
				cout << "\tHusband " << inHusID << " is already a parent\n";
				ERROR = true;
			}
			if((perList[inWifID].pFam > -1)&&(inWifID != 0)){
				cout << "\tWife " << inWifID << " is already a parent\n";
				ERROR = true;
			}
			if((inWifID != 0)&&(inWifID == inHusID)){
				cout << "\tHusband and Wife Cannot be the same person " << inWifID << ".\n";
				ERROR = true;
			}
			//input and test Children
			int iChi;
			for(int i = 0; i < inChiAmo; i++){
				iChi = stoi(cmd.at((2*i)+7));
				if(perList[iChi].cFam > -1){//Error Case
					cout << "\tChild "<< iChi << " is already a child\n";
					ERROR = true;
					break;
				}else{//Input Case
					inChiID[i] = iChi;
				}
			}

			//end loop iteration if an Error occurred
			if(ERROR){
				continue;
			}
			
			//set up Values for inHus and inWif
			if(inHusID != 0)
				perList[inHusID].pFam = inFamID;
			if(inWifID != 0)
				perList[inWifID].pFam = inFamID;
			//update Family and Children
			genealogy[inFamID].ID = inFamID;
			genealogy[inFamID].husID = inHusID;
			genealogy[inFamID].wifID = inWifID;
			genealogy[inFamID].chiAmo = inChiAmo;
			for(int i = 0; i < inChiAmo; i++){
				perList[inChiID[i]].cFam = inFamID;//set Child
				genealogy[inFamID].chiID[i] = inChiID[i];//add ChildID to family
			}
			
			printFamCmd(cmd);
		}else if (cmd.at(0).compare("Relate")== 0){
			int per1ID = stoi(cmd.at(1));
			int per2ID = stoi(cmd.at(2));
			relatePath(genealogy, perList, per1ID, per2ID);	

		}
		
	}while(!cin.eof());
	return 0;
}
