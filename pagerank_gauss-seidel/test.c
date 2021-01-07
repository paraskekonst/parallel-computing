#include <stdio.h>
#include <math.h>
#include <stdlib.h>

int main(int argc,char **argv){
	if(argc != 2)
	{
		printf("give 1 or 2\n"
			"1 for set1 with 643 nodes \n"
			"2 for set2 with 2426 nodes\n",argv[0]);
		return 1;

	}
	int fl=atoi(argv[1]);
	int N=0;
	FILE *fp,*f;
	if (fl==1){
	 N=643;
	 fp=fopen("results1.txt","r");
		if(fp==NULL)
	{
		printf("Error,can't open file  \n");
		exit (1);
	}
	f=fopen("matlab1.txt","r");
	    if(f==NULL)
	    {
	    	printf("Error,can't open matlabresults1 file  \n");
	    	exit (1);
	    }
	}
	else if (fl==2){
	N=2426;
	fp=fopen("results2.txt","r");
		if(fp==NULL)
	{
		printf("Error,can't open file  \n");
		exit (1);
	}
	f=fopen("matlab2.txt","r");
	    if(f==NULL)
	    {
	    	printf("Error,can't open matlabresults2 file  \n");
	    	exit (1);
	    }
	}
	else{
		printf("wrong input please insert 1 for set1 or 2 for set 2\n");
		exit(1);
	}

    double *a=(double *)malloc(N*sizeof(double));
    double *b=(double *)malloc(N*sizeof(double));
    double a1,b1;
    for(int i=0;i<N;i++){
 	   fscanf(fp,"%lf \n",&a1);
 	   fscanf(f,"%lf \n",&b1);
 	   a[i]=a1;
 	   b[i]=b1;
 }
    int c=1;
    for(int i=0;i<N;i++)
	{
		c &= fabs(a[i]-b[i])<=0.0001; 
		//printf("%f , %f \n",a[i],b[i] );
	}
    if (c)printf("success" );
    else printf("fail \n");	
}

