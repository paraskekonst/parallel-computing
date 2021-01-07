#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/time.h>
#include <limits.h>
#include <pthread.h>

void *pthr_func(void *arg);
int  findNofnodes(FILE *fp , int *max ,int *min);
void allocatearrays(int N);
void createarrays(FILE *fp , int N ,int  max ,int min );
double pagerank_gs(int N , int number_of_threads);
void print_results_to_a_file(int N);

pthread_barrier_t barr1,barr2,barr3,barr4,barr5,barr6,barr7,barr8;

struct argm
{
	int start,end,N,id,nt;
	
};
double *x,*x_new,*temp;
double d=0.85;
int **A;
int *n_innodes,*n_outnodes;
int flag;//loop condition variable
int *flags;
int iter;
double summ_all=0;;
double *thr_sum;
int main(int argc,char **argv)
{
	
	if(argc != 3)
	{
		printf("Give 2 inputs\n"
			"where first input is a graph dataset filename\n"
			"and second input is number of threads \n"
			"Example ./a.out web-Google.txt 4 ",argv[0]);
		return 1;

	}
	/*File reading */
	FILE *fp=fopen(argv[1],"r");

	if(fp==NULL)
	{
		printf("Error,can't open file '%s' \n",argv[1]);
		exit (1);
	}

    int min=INT_MAX;
	int max=INT_MIN;
	int number_of_threads= atoi(argv[2]);;

    int N=findNofnodes(fp,&max ,&min);
    printf("Number of nodes : %i",N);

    double time ;

	allocatearrays(N);//function for memery allocation

    rewind(fp);//Go to the start of the file

    createarrays(fp,N,max,min);//fill the arrays

    fclose(fp);
    

    printf("\nThreads : %d \n",number_of_threads );

    time=pagerank_gs(N,number_of_threads);

	printf("Time  : %f seconds\n",time);
	print_results_to_a_file(N);
	/*free allocated memory*/
	free(x);
	free(x_new);
	free(n_innodes);
	free(n_outnodes);
	for(int i=0;i<N;i++) {free(A[i]);}
	free(A);
}
			
			
	

void print_results_to_a_file(int N){
	

   FILE *f=fopen("results2.txt","w");
	

	for(int i=0;i<N;i++){
		fprintf(f,"%f\n",x[i]);
	}
	fclose(f);
}

	
/*function tha finds the number of nodes */

int  findNofnodes(FILE *fp , int *max ,int *min){
	int N;
	int a,b;
	rewind(fp);
	while(fscanf(fp,"%d %d\n",&a,&b)!=EOF)//finding the max and min IDs of the graph

	{
		
		if(a>(*max)) {*max=a;}
		if(b>(*max)) {*max=b;}
		if(a<(*min)) {*min=a;}
		if(b<(*min)) {*min=b;}
	}

	N=(*max)-(*min)+1; //Number of nodes
	
	if(N<0) { printf("Error in file reading\n");exit(3); }
	return N;

}


void allocatearrays(int N)
{
  	n_outnodes=(int*)calloc(N,sizeof(int));
	n_innodes=(int*)calloc(N,sizeof(int));
    A=(int **)malloc(N*sizeof(int *));
    x=(double *)calloc(N,sizeof(double));
	x_new=(double *)malloc(N*sizeof(double));



}
/*Fill the arrays 
A[i][j]= stores the ids of incoming nodes to node i 
n_innodes[i] stores the number of incoming nodes to each node
n_outnodes[i]  stores the number of outgoing nodes from each node
*/
void createarrays(FILE *fp,int N , int max , int min ){
 int a , b ;
	while(fscanf(fp,"%d %d\n",&a,&b)!=EOF)
	{
		a=a-min;//So the lower ID of a node be 0
		b=b-min;
		A[b]=(int *)realloc(A[b],(n_innodes[b]+1)*sizeof(int));
		A[b][n_innodes[b]]=a;//a is an incoming node to b
		n_innodes[b]++;
		n_outnodes[a]++;
	}

}


	

double pagerank_gs(int N , int number_of_threads){
    struct timeval stop,start;
	flag=1;
    iter=0;
	int nod_thr=(int)N/number_of_threads;//number of nodes given to each thread
	int extras=N%number_of_threads;//extra nodes
	flags=(int*)malloc(number_of_threads*sizeof(int));//loop condition for each thread
	thr_sum=(double*)malloc(number_of_threads*sizeof(double));
	pthread_t *thread=(pthread_t *)malloc(number_of_threads*sizeof(pthread_t));
	struct argm *data=(struct argm *)malloc(number_of_threads*sizeof(struct argm));

    pthread_barrier_init(&barr1, NULL, number_of_threads);
	pthread_barrier_init(&barr2, NULL, number_of_threads);
	pthread_barrier_init(&barr3, NULL, number_of_threads);
	pthread_barrier_init(&barr4, NULL, number_of_threads);
	pthread_barrier_init(&barr5, NULL, number_of_threads);
	pthread_barrier_init(&barr6, NULL, number_of_threads);
	pthread_barrier_init(&barr7, NULL, number_of_threads);
	pthread_barrier_init(&barr8, NULL, number_of_threads);
		
	gettimeofday(&start, NULL);

	for(int i=0;i<number_of_threads;i++)
	{
		data[i].start=(!i)?0:data[i-1].end;//Initializing variables for each thread 
		data[i].end=data[i].start + nod_thr + (int)(extras>0);
		
		
		data[i].N=N;
		data[i].id=i;
		data[i].nt=number_of_threads;
		extras--;

		pthread_create(&thread[i],NULL,pthr_func,(void*) &data[i]);
	}
	
    
	for(int i=0;i<number_of_threads;i++){

		pthread_join(thread[i],NULL);
	}
	;
	gettimeofday(&stop, NULL);
		
	double time_elapsed=(double)((stop.tv_usec- start.tv_usec)/1.0e6 + stop.tv_sec -start.tv_sec);

	printf("Iterations : %d\n",iter );
	free(thread);
	free(data);
	free(flags);
    return time_elapsed ;

}

void *pthr_func(void *arg)
{
	
	struct argm *data=(struct argm *)arg;
	
	int start=data->start;
	int end=data->end;
	

	int N=data->N;
	int id=data->id;
	int nt=data->nt;
	
	
	for(int i=start;i<end;i++)
	{
		x[i]=(double)1/N;
	}
	
	pthread_barrier_wait(&barr1);

	
	double *temp;

	int lock_flag=1;
	while(flag)
	{
		pthread_barrier_wait(&barr2);
		int m;
		lock_flag=0;
		flag=0;
		
		for(int k=start;k<end;k++) {x_new[k]=0;}

		for(int j=start;j<end;j++)
		{
			double sum=(1-d)/N;
			double aself=1.0;
			for(int l=0;l<n_innodes[j];l++)
			{   
				m=A[j][l];

				if(j==m){

					aself +=(-d)/n_outnodes[j];
				}
				else{

					sum -=((-d/n_outnodes[m])*x[m]);
					
					
				}
			}
			
			x_new[j] =sum/aself;
			
		}
		

		pthread_barrier_wait(&barr3);

		for(int j=start;j<end;j++)
		{
			
			lock_flag |= fabs(x_new[j] - x[j])>0.0001;//Loop condition
		}
		flags[id]=lock_flag;

		pthread_barrier_wait(&barr4);
       
		if(id==0)//Thread 0 is the master thread
		{
			temp=x_new;//Pointer swap
			x_new=x;
			x=temp;
			flag=0;
			iter++;
		
			for(int i=0;i<nt;i++)
			{
				flag |= flags[i];//Loop condition
			}
		}
		pthread_barrier_wait(&barr5);

		if(!flag){
            /* x=x/norm(x)   */

			for(int j=start;j<end;j++)
			{
				thr_sum[id]+=x[j];
			}
			pthread_barrier_wait(&barr6);
			if(id==0){
				for(int i=0;i<nt;i++){
					summ_all +=thr_sum[i];
				}
			}
			pthread_barrier_wait(&barr7);
			for(int j=start;j<end;j++)
			{
				x[j]=x[j]/summ_all;
			}
			pthread_barrier_wait(&barr8);

		}





	}	
	pthread_exit(NULL);
}

