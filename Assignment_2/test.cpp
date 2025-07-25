#include<iostream>
#include "Sorts.h"
/*
Consider Employee database of PCCOE (at least 20 records). Database contains different fields
of every employee like EMP-ID, EMP-Name and EMP-Salary.
a. Arrange list of employees according to EMP-ID in ascending order using Quick Sort
b. Arrange list of Employee alphabetically using Merge Sort
*/




using namespace std;

int main(int argc, char* argv[]){

	MergeSort(argv[1], atoi(argv[2]));
	QuickSort(argv[1],atoi(argv[2]));
	return 0;

}
