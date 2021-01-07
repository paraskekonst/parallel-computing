/*
  Knn-MPI 
 Πρόγραμμα υπολογισμού k κοντινότερων σημείων:
 Το πρόγραμμα διαβάζει ένα αρχείο data.bin που περιέχει 60000 σημεία σε 30 διαστάσεις 
 και υπολογίζει τους k κοντινότερους γείτονες .Το k ορίζεται ως 30  λόγω ελέγχου των αποτελεσμάτων .
 Το πρόγραμμα ξεκινάει N διεργασίες , κάθε διεργασία διαβάζει ένα block από τα δεδομένα και τα αποθηκεύει στον πίνακα localdata.
 Στην συνεχεία κάθε διεργασία υπολογίζει τους k κοντινότερους γείτονες για το  block που διάβασε και αποθηκεύει σε ένα πίνακα distance 
 τις αποστάσεις από αυτά τα σημεία και σε ένα πίνακα index το id του σημείου . Στην συνέχεια οι διεργασίες ανταλλάσουν τα δεδομένα 
 και ανανεωνόσουν τους πίνακες με τα νέα δεδομένα . Τέλος γίνεται έλεγχος των αποτελεσμάτων με το αρχείο test.bin που περιέχει τα αποτελέσματα από το πρόγραμμα που δόθηκε στο matlab.
 non-Blocinkg υλοποίηση
*/
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "mpi.h"
#include <sys/time.h>


#define Knn 30
#define dimensions 30
int rows;
void knn (double **localdata,double ** values1,double **arrayofdistance,double **index,int currentrank , int rank);
void compere(double **arrayofdistance, double distance,double **index ,  int i ,double id);
double dist(double **A , double **B , int i ,int i1 );
void fill_dist(double ** dist);
void readdata(double **localdata , int pid);
void switcharrays(double **send , double ** rec);
void test(double **index , int pid);


int main(int argc,char **argv){
  

 int  nproc, pid, rc ,rank  ;
 double **localdata , **distance , **rec , **send,**index, time;
 struct timeval startwtime, endwtime;
  
 MPI_Request reqs[2];
 MPI_Status status[2];
 rc = MPI_Init(&argc,&argv);
 if (rc != MPI_SUCCESS) {
     printf ("Error starting MPI program. Terminating.\n");
     MPI_Abort(MPI_COMM_WORLD, rc);
  }
 MPI_Comm_size(MPI_COMM_WORLD, &nproc);
 MPI_Comm_rank(MPI_COMM_WORLD, &pid);
 rows=60000/nproc+60000%nproc;

//δυναμική δέσμευση μνήμης για τους πίνακες 
 localdata=(double**)malloc(rows*(sizeof(double)));
 for(int i=0; i<rows;i++){
    localdata[i]=(double*)malloc(dimensions*sizeof(double));

    }

 distance=(double**)malloc(rows*(sizeof(double)));

 for(int i=0; i<rows;i++){
    distance[i]=(double*)malloc(Knn*sizeof(double));

    }

 index=(double**)malloc(rows*(sizeof( double)));
 for(int i=0; i<rows;i++){
       index[i]=(double*)malloc(Knn*sizeof( double));

    }
 rec=(double**)malloc(rows*(sizeof(double)));
 for(int i=0; i<rows;i++){
         rec[i]=(double*)malloc(dimensions*sizeof(double));

    }
 send=(double**)malloc(rows*(sizeof(double)));
 for(int i=0; i<rows;i++){
    send[i]=(double*)malloc(Knn*sizeof(double));

    }

    

    
    
 fill_dist(distance);
//διάβασμα δεδομένων από το αρχείο
 readdata(localdata , pid);
			
 MPI_Barrier(MPI_COMM_WORLD);			
 if(rank==0)gettimeofday (&startwtime, NULL); 			
//υπολογισμός κ κοντινότερων σημείων για τα τοπικά δεδομένα	
 knn(localdata ,localdata,distance,index,pid,pid);


 int to = (pid + 1) % nproc;
 int from = (pid + nproc - 1) % nproc;	

//επικοινωνία σε τοπολογία δακτυλίου μεταξύ των διεργασιών ώστε να ανταλλάξουν τα δεδομένα  και να ανανεώσουν τους πίνακες
 for (int k=0; k<nproc-1; k++){
   for(int i =0; i<rows; i++){
    for (int j=0; j<dimensions; j++){
       if (k==0){
        MPI_Isend( &(localdata[i][j]),1,  MPI_DOUBLE, to, 0, MPI_COMM_WORLD,&reqs[0]);
        MPI_Irecv(&(rec[i][j]),1 , MPI_DOUBLE,from ,0 ,   MPI_COMM_WORLD,&reqs[1]);
        MPI_Waitall(2,reqs,status);
      }else{
        MPI_Isend( &(send[i][j]),1,  MPI_DOUBLE, to, 0, MPI_COMM_WORLD,&reqs[0]);
        MPI_Irecv(&(rec[i][j]),1 , MPI_DOUBLE,from ,0 ,   MPI_COMM_WORLD, &reqs[1]);
        MPI_Waitall(2,reqs,status);
        }
     }
    }
   rank=(pid-1-k)%nproc;
   if(rank<0)rank=rank+nproc;
   knn(localdata,rec, distance,index ,pid ,rank	);
   switcharrays(send, rec);
} 
 
 MPI_Barrier(MPI_COMM_WORLD);
 if(rank==0){
	gettimeofday (&endwtime, NULL);
	time = (double)((endwtime.tv_usec - startwtime.tv_usec)/1.0e6
				  + endwtime.tv_sec - startwtime.tv_sec);
	
	printf("time :%f s \n ",time);
	
		
 }
 //έλεγχος αποτελεσμάτων
 test(index , pid);

 for(int i = 0; i<rows; i++){
  free((void*)localdata[i]);
  }
 free((void*)localdata);
 for(int i = 0; i<rows; i++){
  free((void*)distance[i]);
  }
 free((void*)distance);
 for(int i = 0; i<rows; i++){
  free((void*)index[i]);
  }
 free((void*)index);
 for(int i = 0; i<rows; i++){
   free((void*)send[i]);
  }
 free((void*)send); 
 for(int i = 0; i<rows; i++){
  free((void*)rec[i]);
  }
 free((void*)rec);
 MPI_Finalize();
   
 return 0;
}




 void readdata(double **localdata  ,int pid)
{ 
  
  char buff[512],*d,*c;
  FILE *file;
  file = fopen( "data.bin", "rb");  //se kathe thread???
   if(file!=NULL){
     for (int k=0; k<rows*pid; k++){c=fgets(buff, 512, (FILE*)file);}
			
     for (int i=0; i<rows; i++){
	c=fgets(buff, 512, file);
	int j=0;
        for (d = strtok(buff, " ,"); d; d = strtok(NULL, " ,")){
				localdata[i][j]= atof(d);
				
				
						
				j++;
			}
	
		}
    
    fclose(file);
    }else {
      printf("openfile failed , exit");
      exit(1);
      }
}


void fill_dist(double ** dist)
{
    for(int i=0; i<rows;i++){
      for(int j=0; j<Knn; j++){
         dist[i][j]=100;
      }
    }
}


//υπολογισμός κ κοντινότερων γειτόνων . Η συνάρτηση υπολογίζει τις αποστάσεις όλων των σημείων που δέχτηκε σαν όρισμα και βρίσκει και αποθηκεύει τις αποστάσεις και τα id των κ κοντινότερων σημείων
void knn (double **localdata,double ** values1,double **arrayofdistance,double **index ,int currentrank ,int rank )
{
  double id;
 for(int i =0; i<rows; i++){
  for (int i1 = 0 ; i1<rows; i1 ++)
  {
   double distance = dist(localdata,values1, i  , i1 );
   
  
   if(i==i1 && rank==currentrank)
    continue;

    else{ 
   id=i1+rows*rank;
  compere (arrayofdistance , distance,index, i , id );
                 } }
     }

}





//ευκλείδεια απόσταση 
double dist(double **A , double **B , int i  ,int i1 )
{
	
	double tmp=0, sum=0; 
    for (int j=0; j<dimensions; j++){
		 tmp=(A[i][j]-B[i1][j])*(A[i][j]-B[i1][j]);
         sum+=tmp;
	}		 
    sum=sqrt(sum);
	return sum;
}



void compere(double **arrayofdistance, double distance,double**index ,  int i ,double id)
{ 
   
   double temp ; 
   double tempindex;
   for(int j =0; j<Knn; j++)
     { 
       if (distance<arrayofdistance[i][j])
         { temp=arrayofdistance[i][j];
           arrayofdistance[i][j]=distance;
           distance=temp;
           tempindex=index[i][j];
           index[i][j]=id;
           id=tempindex;
          
          
          }
     }
}


void switcharrays(double **send , double ** rec)
{
  for(int i=0; i<rows; i++ ){
   for(int j=0; j<dimensions;j++){
    send[i][j]=rec[i][j];
    }}
   
} 



void test(double **index , int pid)
{
    char buff[512],*d,*c;
     double **testarray;
   
   testarray=(double**)malloc(rows*(sizeof( double)));
     for(int i=0; i<rows;i++){
       testarray[i]=(double*)malloc(dimensions*sizeof(double));
   }
  FILE *file;
  file = fopen( "test.bin", "rb");
   if(file!=NULL ){
     for (int k=0; k<rows*pid; k++){c=fgets(buff, 512, (FILE*)file);}
			
        for (int i=0; i<rows; i++){
	c=fgets(buff, 512, file);
	int j=0;
	
	for (d = strtok(buff, " ,"); d; d = strtok(NULL, " ,")){
				testarray[i][j]= atof(d);
				
				
						
				j++;
			}
	
		}
      }else {
    printf("openfile failed");
    exit(1);
     }
      fclose(file);
      MPI_Barrier(MPI_COMM_WORLD);
     int count=0;
     for(int i =0; i<rows; i ++){
       for(int j=0; j<Knn; j++){
         if(index[i][j]==(testarray[i][j]-1))continue;
         else count++;
       }}
    if (count==0)printf("rank %i passed the test",pid);
    else{
           printf("rank %i found %i mistakes \n",pid,count);
            
   }
     for(int i = 0; i<rows; i++){
      free((void*)testarray[i]);
       }
       free((void*)testarray);
}





