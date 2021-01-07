#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/time.h>
#include <limits.h>
#include <errno.h>


struct timeval stop,start;
int main(int argc,char **argv)
{
	
	if(argc != 2)
	{
		printf("Usage: %s filename\n"
			"where \n"
			"filename: name of input file\n",argv[0]);
		return 1;

	}
	
	/*File reading */
	FILE *fp=fopen(argv[1],"r");

	if(fp==NULL)
	{
		char str[50];
		sprintf(str,"Error, '%s' ",argv[1]);
		perror(str);
		return 2;
	}


	int i,N;

	/*Reading from file */
	int a,b;
	int min=INT_MAX;
	int max=INT_MIN;
	
	while(fscanf(fp,"%d %d\n",&a,&b)!=EOF)//finding the max and min IDs of the graph
	{
		if(a>max) {max=a;}
		if(b>max) {max=b;}
		if(a<min) {min=a;}
		if(b<min) {min=b;}
	}

	N=max-min+1; //Number of nodes
	
	if(N<0) { printf("Error in file reading\n");return 3; }

	//Allocating arrays
	int **A=(int **)malloc(N*sizeof(int *));//A[][] stores for each node,which nodes are connected to him
	int *n_outnodes=(int*)calloc(N,sizeof(int));//n_outnodes[] stores the number of outgoing nodes from each node
	int *n_innodes=(int*)calloc(N,sizeof(int));//n_innodes[] stores the number of incoming nodes to each node
	

	rewind(fp);//Go to the start of the file

	while(fscanf(fp,"%d %d\n",&a,&b)!=EOF)
	{
		a=a-min;//So the lower ID of a node be 0
		b=b-min;
		A[b]=(int *)realloc(A[b],(n_innodes[b]+1)*sizeof(int));
		A[b][n_innodes[b]]=a;//a is an incoming node to b
		n_innodes[b]++;
		n_outnodes[a]++;
	}

	fclose(fp);
	

     int m=0;
	double *P,*P_new,*temp;
	P=(double *)calloc(N,sizeof(double));
	P_new=(double *)malloc(N*sizeof(double));
	
	double d=0.85,tempc;
	int j,k,l,c;
	gettimeofday(&start, NULL);
	for(i=0;i<N;i++) {P[i]=(double)1/N;}//Initiallazing P
	int flag=1;//While condition
	c=0;//itterations counter
	while (flag)
	{
		flag=0;
		


		for(k=0;k<N;k++) {P_new[k]=0;}
		
		for(j=0;j<N;j++)
		{
			double aself=1.0;
			double sum=((1-d)/N);
			for(l=0;l<n_innodes[j];l++)
			{
                m=A[j][l];
				if(j==m){aself +=(-d)/n_outnodes[j];}
				else{
					sum -=((-d/n_outnodes[m])*P[m]);
					//sum -=((-d/n_outnodes[m])*P[m])*(m>j);

				}
			}
            
			P_new[j] =sum/aself;
			}	
				
				
			
		
		for(j=0;j<N;j++)
		{
			
			flag |= fabs(P_new[j] - P[j])>0.0001;//Loop condition
		}
		//Pointer swapping to so P=P_new
		temp=P_new;
		P_new=P;
		P=temp;
		c++;
	}
	gettimeofday(&stop, NULL);
	double sum1=0;
	for(int i=0;i<N;i++){
		sum1 +=P[i];
	}
	for(int i=0;i<N;i++){
		P[i]=P[i]/sum1;
	}
	
	double time=(double)((stop.tv_usec- start.tv_usec)/1.0e6 + stop.tv_sec -start.tv_sec);
	printf("Iterations : %d\n Time : %f",c,time);
	FILE *f=fopen("resultsserial.txt","w");
	

	for(int i=0;i<N;i++){
		fprintf(f,"%f\n",P[i]);
	}
	fclose(f);


	free(P);
	free(P_new);
}
