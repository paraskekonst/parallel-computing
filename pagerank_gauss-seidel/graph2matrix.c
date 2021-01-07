#include <stdio.h>
#include <stdlib.h>

#include <limits.h>






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
	int **conn=(int **)malloc(N*sizeof(int *));//conn[][] stores for each node,which nodes are connected to him
       printf("%i \n",N);
	for (int i=0;i<N;i++) {conn[i]=(int*)malloc(N*sizeof(int));}
	printf("%i \n",N);

	rewind(fp);//Go to the start of the file

	while(fscanf(fp,"%d %d\n",&a,&b)!=EOF)
	{       a=a-min;
                b=b-min;
		conn[a][b]=1;
	}

	fclose(fp);
          for(int i=0;i<N;i++){
		for(int j=0;j<N;j++){if (conn[i][j]!=1) conn[i][j]=0;}}
        FILE *f=fopen("set2.txt","w");
	for(int i=0;i<N;i++){
		for(int j=0;j<N;j++){
			fprintf(f,"%i ",conn[i][j]);
		} fprintf(f,"\n");
	}
/*FILE *f=fopen("m" , "wt");
	for(int i=0;i<N;i++){
		for(int j=0;j<N;j++){
			fprintf(f, "%i ,",conn[i][j]);
		} 
	}
fwrite(conn,sizeof(conn),1,f);*/

fclose(f);

}
