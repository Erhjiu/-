#include <iostream>
#include <string>

using namespace std;


typedef struct{
    string name;
    int id;
    int period;
    int execution;
    int release = (id-1)*period;
    int deadline = id*period;
    int remaining;
}inprocess;

void EDF(inprocess *A,inprocess *B, int *time) {
    if(A->release <= *time && B->release <= *time){
        if(A->deadline<B->deadline){
            cout<<"time:"<<*time<<" 进程"<<A->name<<"_"<<A->id<<"执行"<<endl;
            A->remaining--;
            (*time)++;
            if(A->remaining == 0){
                A->id++;
                A->release = (A->id-1)*A->period;
                A->deadline = A->id*A->period;
                A->remaining = A->execution;
            }
        }
        else{
            cout<<"time:"<<*time<<" 进程"<<B->name<<"_"<<B->id<<"执行"<<endl;
            B->remaining--;
            (*time)++;
            if(B->remaining == 0){
                B->id++;
                B->release = (B->id-1)*B->period;
                B->deadline = B->id*B->period;
                B->remaining = B->execution;
            }
        }
    }
    else if(A->release <= *time){
        cout<<"time:"<<*time<<" 进程"<<A->name<<"_"<<A->id<<"执行"<<endl;
        A->remaining--;
        (*time)++;
        if(A->remaining == 0){
            A->id++;
            A->release = (A->id-1)*A->period;
            A->deadline = A->id*A->period;
            A->remaining = A->execution;
        }
    }
    else if(B->release <= *time){
        cout<<"time:"<<*time<<" 进程"<<B->name<<"_"<<B->id<<"执行"<<endl;
        B->remaining--;
        (*time)++;
        if(B->remaining == 0){
            B->id++;
            B->release = (B->id-1)*B->period;
            B->deadline = B->id*B->period;
            B->remaining = B->execution;
        }
    }
    else{
        cout<<"time:"<<*time<<" 空闲"<<endl;
        *time += 1;
    }

}

void LLF(inprocess *A,inprocess *B, int *time){
    int laxity_A = A->deadline - A->remaining - *time;
    int laxity_B = B->deadline - B->remaining - *time;
    if(A->release <= *time && B->release <= *time){
        if(laxity_A<laxity_B){
            cout<<"time:"<<*time<<" 进程"<<A->name<<"_"<<A->id<<"执行"<<endl;
            A->remaining--;
            (*time)++;
            if(A->remaining == 0){
                A->id++;
                A->release = (A->id-1)*A->period;
                A->deadline = A->id*A->period;
                A->remaining = A->execution;
            }
        }
        else{
            cout<<"time:"<<*time<<" 进程"<<B->name<<"_"<<B->id<<"执行"<<endl;
            B->remaining--;
            (*time)++;
            if(B->remaining == 0){
                B->id++;
                B->release = (B->id-1)*B->period;
                B->deadline = B->id*B->period;
                B->remaining = B->execution;
            }
        }
    }
    else if(A->release <= *time){
        cout<<"time:"<<*time<<" 进程"<<A->name<<"_"<<A->id<<"执行"<<endl;
        A->remaining--;
        (*time)++;
        if(A->remaining == 0){
            A->id++;
            A->release = (A->id-1)*A->period;
            A->deadline = A->id*A->period;
            A->remaining = A->execution;
        }
    }
    else if(B->release <= *time){
        cout<<"time:"<<*time<<" 进程"<<B->name<<"_"<<B->id<<"执行"<<endl;
        B->remaining--;
        (*time)++;
        if(B->remaining == 0){
            B->id++;
            B->release = (B->id-1)*B->period;
            B->deadline = B->id*B->period;
            B->remaining = B->execution;
        }
    }
    else{
        cout<<"time:"<<*time<<" 空闲"<<endl;
        *time += 1;
    }
}

int main(){
    inprocess A{"A", 1, 20, 10, 0, 20, 10};
    inprocess B{"B", 1, 50, 25, 0, 50, 25};
    int time = 0;

    cout << "EDF调度:" << endl;
    while(time < 100){
        EDF(&A, &B, &time);
    }

    A = inprocess{"A", 1, 20, 10, 0, 20, 10};
    B = inprocess{"B", 1, 50, 25, 0, 50, 25};
    time = 0;

    cout << "LLF调度:" << endl;
    while(time < 100){
        LLF(&A, &B, &time);
    }

    return 0;
}
