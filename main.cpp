#include <malloc.h>
#include <stdlib.h>
#include <conio.h>
#include <windows.h>
#include <stdio.h>
#include <iostream>
#include <string>
#include <fstream>
using namespace std;
typedef struct jcb
{
    int id;  //job_id
    int arrive_time;   //到达时间
    int need_time;  // 作业需要的时间
    int privilege;  // 优先权
    double excellent;  //相应比
    int start_time;  //开始时间
    int wait_time;  //等待时间
    int tr_time;  //周转时间
    double wtr_time; //带权周转时间
    int run_time;  //运行时间
    bool visited = false;   //是否已经运行过
    bool isarrived = false;   // 是否到达
    struct jcb *next;   //下一个jcb的位置
} JCB, *QueuePtr;

//队列指针
typedef struct {
    QueuePtr front;  //队头元素标识队列
    QueuePtr rear;    
} processQueue;


//队列操作方法

//判断队列是否为空
int IsEmpty(processQueue *q){
    if(q->front == q->rear)
        return 1;
    else
        return 0;    
}

//初始化队列
processQueue *InitQueue(){
    processQueue *q = (processQueue *)malloc(sizeof(processQueue));
    if(q!=NULL){
        q->front = q->rear = (QueuePtr)malloc(sizeof(JCB));
        q->front->next = NULL;  //队尾元素没有next元素，设置为空
    }
    else
        exit(1);
    return q;
}

//新元素插入队尾
void EnQueue(processQueue *q, QueuePtr jcb){
    if(jcb != NULL){
        q->rear->next = jcb;
        q->rear = jcb;
        jcb->next = NULL;
    }
    else    cout << "\n异常！" << flush;
}
//将队头元素出队
QueuePtr DeQueue(processQueue *q){
    if(q->front == q->rear){
        cout << "\n队列为空！" << flush;
        system("pause");
        exit(0);
    }
    QueuePtr jcb;
    jcb = q->front->next;
    q->front->next = jcb->next;
    if(q->rear==jcb)
        q->rear = q->front;
    return jcb;
}

//清空队列
void ClearQueue(processQueue *q){
    while(IsEmpty(q) != 1)
        DeQueue(q);
}

//销毁队列
void DestroyQueue(processQueue *q){
    if(IsEmpty(q)!=1)
        ClearQueue(q);
    free(q);
    cout << "\n已销毁！" << flush;
}

//创建JCB
//定义全局变量计算作业数
int job_cnt = 0;

void read_jobdata(processQueue *q)
{
    ifstream outfile("jobdata.txt");
    string s;
    int read_cnt = 0;  //decide how to add a job to job_info
    int opera;
    //add the data in the file to the vector of job
    while (outfile >> s)
    {
        ++read_cnt;
        opera = read_cnt % 4;  //初始化时需要输入4个数据
        switch (opera)
        {
        case 1: //读取第一个数据时，创建新的jcb，赋值给id
            {QueuePtr jcb = (QueuePtr)malloc(sizeof(JCB));
            EnQueue(q, jcb);
            q->rear->id = stoi(s);
            break;}
        case 2: //第二个数据是到达时间
            {q->rear->arrive_time = stoi(s); 
            break;}
        case 3: //第三个数据是运行需要时间
            {q->rear->need_time = stoi(s); 
            break;}
        case 0:     //第四个数据是优先权
            {q->rear->privilege = stoi(s); 
            break;}
        default:
            break;
        }
    }
    outfile.close();
    //遍历队列，输出信息
    cout << "ID" << "\t" << "arrival" << "\t" << "need" << "\t" << "priviledge" << endl;
    QueuePtr jcb = (QueuePtr)malloc(sizeof(JCB));
    jcb = q->front->next;
    do
    {
        cout << jcb->id << '\t' << jcb->arrive_time << '\t' << jcb->need_time << '\t' << jcb->privilege << endl;
        jcb = jcb->next;
    } while (jcb != NULL);

    //读取4个内容，read_cnt除以4就是job_cnt
    job_cnt = read_cnt / 4;
        
}
//找出时间最短
QueuePtr findFastJob(processQueue *q)
{
    QueuePtr fastJob = q->front->next;
    QueuePtr job = q->front->next;
    //遍历队列，比较作业的运行所需时间
    while (job != NULL)
    {
        if (job->need_time < fastJob->need_time)
            fastJob = job;
        job = job->next;
    }
    

    return fastJob;
}
//找出最早到达
QueuePtr findEarlyJob(processQueue *q)
{
    QueuePtr earlyJob = q->front->next;
    QueuePtr job = q->front->next;
    while (job != NULL)
    {
        if (job->arrive_time < earlyJob->arrive_time)
            earlyJob = job;
        job = job->next;
    }

    return earlyJob;
}
//找出最高响应比
QueuePtr findHighJob(processQueue *q, int *time_axis)
{
    QueuePtr highJob = q->front->next;
    QueuePtr job = q->front->next;
    //先计算当前等待队列中各元素的响应比
    while (job!=NULL)
    {
        job->excellent = 1 + (*time_axis - job->arrive_time) * 1.00 / job->need_time;
        job = job->next;
    }
    job = q->front->next;
    //遍历队列并比较
    while (job != NULL)
    {
        if (job->excellent > highJob->excellent)
            highJob = job;
        job = job->next;
    }

    return highJob;
}

//找出最高优先权
QueuePtr findPriorityJob(processQueue *q)
{
    QueuePtr poriorityJob = q->front->next;
    QueuePtr job = q->front->next;
    while (job != NULL)
    {
        if (job->privilege < poriorityJob->privilege)
        {
            poriorityJob = job;
        }
        else if (job->privilege == poriorityJob->privilege)
        {
            if (job->arrive_time < poriorityJob->arrive_time)
                poriorityJob = job;
        }
        job = job->next;
    }

    return poriorityJob;
}


//删除队列中特定元素
void deleteJob(processQueue *q, QueuePtr job_delete)
{
    QueuePtr job = q->front;
    while (job != NULL)
    {
        if(job->next == job_delete)
        {
            if (q->rear == job_delete){
                q->rear = job;
                job->next = NULL;
                break;
            }
            job->next = job_delete->next;
            break;
        }
        job = job->next;
    }
}


//作业调度函数

//到达队列调入等待队列
void arrive2wait(processQueue *aq, processQueue *wq, int* time_axis)
{
    QueuePtr job = aq->front->next;
    QueuePtr job_flag = job;
    QueuePtr jobArrived = NULL;
    while (job_flag != NULL)
    {
        //将到达的作业放入队列
        if (job_flag->arrive_time <= *time_axis)
        {
            job = job_flag;
            job_flag = job->next;
            jobArrived = job;
            deleteJob(aq, jobArrived);
            EnQueue(wq, jobArrived);
            continue;
        }
        job_flag = job_flag->next;
    }
}
//SJF、HRRF、HPF调度方法
//这三种调度方法的不同之处就在于：当作业进入等待队列后，
//从中选取运行作业的标准不同。
//因此只需要更改从等待队列选取的标准即可实现算法的转换
void wait2run(processQueue *rq, processQueue *wq, QueuePtr jcb, int* time_axis)
{
    // jcb = findFastJob(wq);// 找最短，SJF
    jcb = findHighJob(wq, time_axis);// 找相应比最高，HRRF
    // jcb = findPriorityJob(wq); //找优先权最高，HPF
    deleteJob(wq,jcb);
    EnQueue(rq,jcb);
    jcb->visited = true;
    *time_axis += jcb->need_time;
    jcb->run_time = jcb->need_time;
    jcb->wait_time = *time_axis - jcb->arrive_time - jcb->run_time;
    jcb->tr_time = *time_axis - jcb->arrive_time;
    jcb->wtr_time = 1.00 * jcb->tr_time / jcb->run_time;
    jcb->excellent = 1.00 + jcb->wait_time * 1.00 / jcb->need_time;
    cout << "执行完的作业是：" << jcb->id << " 等待时间是：" << jcb->wait_time << 
    " 周转时间是：" << jcb->tr_time << " 带权周转时间是：" << jcb->wtr_time << endl;
}

void run2spare(processQueue *rq, processQueue *sq, QueuePtr jcb){
    jcb = DeQueue(rq);
    EnQueue(sq,jcb);
}

//建立统一的时间轴，监控作业的运行
void schedule(processQueue *wq, processQueue *rq, processQueue *aq)
{
    //建立时间轴
    int *time_axis = (int*)malloc(sizeof(int));
    *time_axis = 0;
    QueuePtr jcb_run;

    while (!IsEmpty(wq) || !IsEmpty(aq))
    {
        if (!IsEmpty(wq))
        {
            wait2run(rq, wq, jcb_run, time_axis);
        }
        //等待队列为空时，找出最早到达的队列
        else if (!IsEmpty(aq))
        {
            jcb_run = findEarlyJob(aq);
            *time_axis = jcb_run->arrive_time;
        }
        if (!IsEmpty(aq)) arrive2wait(aq, wq, time_axis);
    }
    //计算平均值
    QueuePtr job = rq->front->next;
    double aver_wait = 0;
    double aver_tr = 0;
    double aver_wtr = 0;
    while (job != NULL)
    {
        aver_wait += 1.0 * job->wait_time / job_cnt;
        aver_tr += 1.0 * job->tr_time*1.0 / job_cnt;
        aver_wtr += 1.0 * job->wtr_time*1.0 /job_cnt;
        job = job->next;
    }
    cout << "平均等待时间：" << aver_wait << endl;
    cout << "平均周转时间：" << aver_tr << endl;
    cout << "平均带权周转时间：" << aver_wtr << endl;
    
}
//资源分配
void processCreate(){
    processQueue *wq = InitQueue();
    processQueue *rq = InitQueue();
    processQueue *aq = InitQueue();
    //加入就绪队列
    read_jobdata(aq);
    schedule(wq, rq, aq);
}

int main(){
    processCreate();
    return 0;
}








