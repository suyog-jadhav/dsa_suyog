#include<iostream>
#include<fstream>

using namespace std;

struct student{
	string PRN,name,result;
	float grade;
};

void load_data(string fname,student s[],int *size){
	ifstream f1;
	f1.open(fname,ios::in);
	if(!f1){
	cout<<"file not open"<<endl;
	return;
	}
	int i=0;
	while (f1 >> s[i].PRN >> s[i].name >> s[i].grade >> s[i].result){
        	i++;
    	}
    	*size=i;
	f1.close();
}

void sort_data(struct student s[],int size){
	int j;
	struct student key;
	for(int i=1;i<size;i++){
		j=i-1;
		key=s[i];
		while(j>=0 && s[j].PRN > key.PRN){
			s[j+1]=s[j];
			j--;
		}
		s[j+1]=key;	
	}
}
void display(student s[],int size){
	for(int i=0;i<size;i++)
		cout<<s[i].PRN<<" "<<s[i].name<<" "<<s[i].grade<<" "<<s[i].result<<endl;
}

void store(student s[],int size,string fname){
	ofstream f1;
	f1.open(fname,ios::out);
	if(!f1) return ;
	for(int i=0;i<size;i++){
		f1<<s[i].PRN <<" "<< s[i].name <<" "<< s[i].grade <<" "<< s[i].result<<endl;
	}
	f1.close();
}

void shell_sort_by_name(student s[],int size) {
    for (int gap=size / 2; gap>0; gap/= 2) {
        for (int i=gap; i<size; i++) {
            student temp = s[i];
            int j;
            for (j = i;j >= gap && s[j - gap].name>temp.name; j-= gap) {
                s[j]=s[j - gap];
            }
            s[j]=temp;
        }
    }
}

void counting_sort(student s[],int size,int exp) {
    student output[size];
    int count[10]={0};
    for(int i=0; i<size;i++)
        count[ (int(s[i].grade * 100) / exp) % 10 ]++;
    for(int i=1;i<10;i++)
        count[i]+=count[i - 1];
    for(int i=size - 1;i >= 0;i--) {
        int idx=(int(s[i].grade * 100)/exp) % 10;
        output[count[idx] - 1]=s[i];
        count[idx]--;
    }
    for(int i=0;i<size;i++)
        s[i]=output[i];
}

void radix_sort_toppers(student s[],int size,string fname) {
    float maxGrade=s[0].grade;
    for(int i =1;i < size;i++)
        if(s[i].grade > maxGrade)
            maxGrade=s[i].grade;
    for(int exp = 1;int(maxGrade * 100)/exp>0; exp*= 10)
        counting_sort(s, size, exp);

    int count=0;
    ofstream f1(fname);
    if(!f1) return;
    // Write only PASS students, skip FAIL
    for(int i = size-1;i >= 0 && count < 10;i--) {
        if(s[i].result == "PASS") {
            f1<<s[i].PRN<<" "<< s[i].name<<" "<<s[i].grade<<" "<<s[i].result<<endl;
            count++;
        }
    }
    f1.close();
}


int main(int argc,char* argv[]){
	int size;	
	student s[200];
	load_data(argv[1],s,&size);

    sort_data(s,size);
    store(s,size,argv[2]);

    shell_sort_by_name(s,size);
    store(s,size,argv[3]);

    radix_sort_toppers(s,size,argv[4]);

	return 0;
}

	
