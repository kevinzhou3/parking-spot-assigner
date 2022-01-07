#include <iostream>
#include <cstdlib>
#include <cmath>
#include <fstream>
#include <iomanip>

using namespace std;

const int NUM_SPOTS = 50;
const int NUM_INFO = 2;
const int NAME_COL = 1;
const int STATUS_COL = 0;
const int MAX_PEOPLE = 25;
const string STU_STATUS = "0";
const int HALF_LOT = 25;
const int FULL_LOT = 0;

void readCurrentPark(ifstream & fin, string lot[NUM_SPOTS][NUM_INFO])
{
	string name = "", status = "";
	int spot = 0;
	while (fin >> status >> name >> spot)
	{
		lot[spot-1][NAME_COL] += name;
		lot[spot-1][STATUS_COL] += status;
	}	
}

void readChanges(ifstream & fin, string change[NUM_SPOTS][NUM_INFO])
{
	string name = "", status = "";
	int spot = 0;
	while (fin >> status >> name)
	{
		change[spot][NAME_COL] += name;
		change[spot][STATUS_COL] += status;
		spot++;
	}
}

void freeSpot(string lot[NUM_SPOTS][NUM_INFO], string name)
{
	bool found = 0;
	int count = 0;
	while(!found)
	{
		if(lot[count][NAME_COL] == name)
		{
			found = 1;
			lot[count][NAME_COL] = "";
			lot[count][STATUS_COL] = "";
		}
		count++;
	}
}

int findLowestSpace(string lot[NUM_SPOTS][NUM_INFO], string status)
{
	int count = 0;
	bool found = 0;
	if(status == STU_STATUS)
	{
		count = HALF_LOT;
	}
	
	while(!found && count < NUM_SPOTS)
	{
		if(lot[count][NAME_COL] == "")
		{
			found = 1;
		}
		count++;
	}
	
	if(!found)
		count = FULL_LOT;
	
	return count-1;
}

bool addPerson(string lot[NUM_SPOTS][NUM_INFO], string name, string status)
{
	int space = 0;
	bool foundSpace = 0;
	space = findLowestSpace(lot, status);
	
	if(space >= 0)
	{
		lot[space][NAME_COL] = name;
		lot[space][STATUS_COL] = status;
		foundSpace = 1;
	}
	return foundSpace;
}

void moveExisting(string lot[NUM_SPOTS][NUM_INFO])
{
	for(int count = HALF_LOT; count < NUM_SPOTS; count++)
	{
		if(lot[count][STATUS_COL] == "1")
		{
			string name = "";
			string status = "";
			name = lot[count][NAME_COL];
			status = lot[count][STATUS_COL];
			
			freeSpot(lot, name);
			bool foundSpace = 0;
			foundSpace = addPerson(lot, name, status);
			
			if(!foundSpace)
			{
				lot[count][NAME_COL] = name;
				lot[count][STATUS_COL] = status;
			}
		}
	}
}

void writeParkingData(ofstream & fout, string lot[NUM_SPOTS][NUM_INFO])
{
	for(int count = 0; count < NUM_SPOTS; count++)
	{
		fout << lot[count][STATUS_COL] << " " << lot[count][NAME_COL]
			<< " " << count+1 << endl;
	}
}

int main()
{
	ifstream parking_current("parking_current.txt");
	ifstream parking_add("parking_add.txt");
	ifstream parking_remove("parking_remove.txt");
	ofstream fout("parking_update.txt");
	if(!parking_current || !parking_add || !parking_remove)
	{
		cout << "File not opened";
		return EXIT_FAILURE;
	}
	
	string lot[NUM_SPOTS][NUM_INFO] = {""};
	string add[MAX_PEOPLE][NUM_INFO] = {""};
	string remove[MAX_PEOPLE][NUM_INFO] = {""};
	
	readCurrentPark(parking_current, lot);
	readChanges(parking_add, add);
	readChanges(parking_remove, remove);
	
	for(int count = 0; count < MAX_PEOPLE; count++)
	{
		freeSpot(lot, remove[count][NAME_COL]);
	}
	
	moveExisting(lot);
	
	for(int count = 0; count < MAX_PEOPLE; count++)
	{
		bool foundSpace = 0;
		foundSpace = addPerson(lot, add[count][NAME_COL], 
					add[count][STATUS_COL]);
		if(!foundSpace)
		{
			cout << "The lot is full, " << add[count][NAME_COL] 
				<< " does not have a space." << endl;
		}
	}
	
	writeParkingData(fout, lot);
	parking_current.close();
	parking_add.close();
	parking_remove.close();
	fout.close();
	return EXIT_SUCCESS;
}
