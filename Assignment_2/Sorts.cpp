
#include<iostream>
#include<fstream>
#include<string>
#include "Sorts.h"

using namespace std;

struct employee{
    int id;
    string name;
    float salary;
};

void store(employee A[],int size,string name){
    ofstream f1;
    f1.open(name,ios::out);
    if(!f1) return ;
    for(int i=0;i<size;i++)
        f1<<A[i].id<<" "<<A[i].name<<" "<<A[i].salary<<"\n";
    f1.close();
    return;
}

void swap(employee *x,employee *y){
    employee temp =*x;
    *x=*y;
    *y=temp;
}

void display(employee A[],int s){
for(int i =0 ;i<s;i++)
    cout<<A[i].id<<" "<<A[i].name<<" "<<endl;
cout<<endl;
return;
}


void Merge(employee A[], int s, int m, int e){
	employee tempA[e - s + 1];
	int i = s, j = m + 1, k = 0;
	while(i <= m && j <= e){
		if(A[i].name < A[j].name){
			tempA[k] = A[i];
			i++;		
		}
		else {
			tempA[k] = A[j];
			j++;
		}
		k++;
	
	}
	while(i <= m)
		tempA[k++] = A[i++];
		
	while(j <= e)
		tempA[k++] = A[j++];
		
	for(int i = s, k = 0; i <= e; i++, k++){
		A[i] = tempA[k];
	
	}
	return;	

}

void Msort(employee A[], int s, int e){
	if(s == e) return;
	int m = (s + e) /2;
	Msort(A, s, m);
	Msort(A, m + 1, e);
	Merge(A, s, m, e);
	return;
}

void MergeSort(char* file, int s){
	ifstream f1; 
	f1.open(file, ios::in);
	if(!f1) return;
	employee A[s];
	for(int i=0;i<s;i++){
	    f1>>A[i].id>>A[i].name>>A[i].salary;
	}
	Msort(A, 0, s-1);
	//display(A, s);
	cout<<"\n";
	store(A,s,"by_name");
	f1.close();
	return;
}

int partition(employee A[], int s, int e) {
    int pivot = A[s].id;
    int i = s;
    int j = e ;  

    while (true) {
        do { i++; } while (i <= e && A[i].id <= pivot);
        do { j--; } while (j >= s && A[j].id > pivot);

        if (i >= j) break;

        swap(&A[i], &A[j]);
    }
    swap(&A[s], &A[j]);
    return j;
}


void Qsort(employee A[],int s, int e){
    
    if(s < e){
        int part = partition (A,s,e);
        Qsort(A,s,part);
        Qsort(A,part+1,e);
    }
    
    return ;    
}

void QuickSort(char* file, int s){
    ifstream f1; 
	f1.open(file, ios::in);
	if(!f1) return;
	employee A[s];
	for(int i=0;i<s;i++){
	    f1>>A[i].id>>A[i].name>>A[i].salary;
	}
	//display(A,s);
    Qsort(A,0,s);
    
    //display(A,s);
    store(A,s,"by_id");
    f1.close();
}


/*int main(int argc, char* argv[]){

	QuickSort(argv[1], atoi(argv[2]));
	MergeSort(argv[1],atoi(argv[2]));
	return 0;

}*/




