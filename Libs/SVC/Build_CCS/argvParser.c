#include "argvParser.h"
#include <stdio.h>

#define FRAMES_TO_SAVE 0
#define DATA_TO_SAVE 50
#define SAVE_DATA_IN_FILE 1


int init_DSP_TI(int *argc, unsigned char*** pArgv,char* outputfilename);


/* Read .txt file content to a buffer
* @*strname: pointer to string buffer
* @filename: name of the file to be opened
*/
int txtreader(unsigned char	**strname, char* filename){

	FILE 	*pFile;   //txt file pointer
	int 	filesize; //size of txt file

	// open file
	if( ( pFile= fopen(filename, "r") )== NULL ) {
		printf("File `%s´ could not be opened\n", filename ); /*E2C*/
		return -1;
	}

	// obtain file size & allocate memory to read the file
	fseek(pFile, 0, SEEK_END);
	filesize= ftell(pFile);
	rewind(pFile);
	*strname= (unsigned char*)calloc( (filesize+1), sizeof(char) );
	if( (*strname)== NULL ){
		return -1;
	}

	// copy file into buffer
	if( fread((void*)(*strname), 1, filesize, pFile)!= filesize){
        printf("Error while reading `%s´\n", filename); /*E2C*/
		return -1;
	}

	fclose (pFile);
	return 0;
}

/* parse arguments allocated in text files
* @
* @
*/
int parseArgs(int *argc, unsigned char*** pArgv){

	unsigned char *argctxt;
	char argcs[]= "../input/argc.txt";
	int i;


	// read "argc.txt" file
	if( txtreader(&argctxt, argcs) ) {
       printf("File `argc.txt´ could not be opened\n"); /*E2C*/
		return -1;
	}

	// update argc integer
	*argc= atoi(argctxt) + 1;

	*pArgv=(unsigned char**)malloc(sizeof(char**)*(*argc));

	for (i=1; i< *argc; i++){

		char num[4] = "\0";
		char argvtxt[200] = "../input/argv";

		sprintf(num, "%d", i-1);
		strcat(argvtxt, num);
		strcat(argvtxt, ".txt");
		if( txtreader(&((*pArgv)[i]), argvtxt) ) {
            printf("File `%s´ could not be opened\n", argvtxt); /*E2C*/
			return -1;
		}
	}

	return 0;
}

unsigned char get_file_name_from_path_v1(char *file_name, const char *path)  //DSM
{
	int i;
	int path_length = strlen(path);
	unsigned char found = 0;
	int trace1;
	int trace2;
	char *ptr;

	/* Find '.' character */
	i = path_length - 1;
	while ( (path[i] != '.') && (!found) )
		i--;

	if(path[i] == '.')
	{
		found = 1;
		trace2 = i-1;
	}
	else
		return found;

	/* Find '/' character or '\' character */
	found = 0;
	while ( (path[i] != '/') && (path[i] != '\\') && (!found) )
		i--;

	if ( (path[i] == '/') || (path[i] == '\\') )
	{
		found = 1;
		trace1 = i+1;
		ptr = (char*)&path[trace1];
	}
	else
		return found;

	/* Get substring in path */
	for (i=0; i < (trace2-trace1+1); i++)
	{
		file_name[i] = *ptr;
		ptr = ptr + 1;
	}
	file_name[i] = '\0';

	return found;
}



int init_DSP_TI(int *argc, unsigned char*** pArgv, char* outfilename){

	char aux_name[100];
    #ifdef VERBOSE //*E2C
    printf("Open SVC Decoder: Video decoding...\n");
    #endif //VERBOSE
    
    if( parseArgs(argc,  pArgv) ){				
		printf("Error while parsing input arguments\n");
		return -1;								
	}  

	strcpy(outfilename, (*pArgv)[4]);
	#ifdef CHIP_DM642
		strcat(outfilename, "OpenSVC_DM642_TIOptim/");
	#elif CHIP_DM6437
		strcat(outfilename, "OpenSVC_DM6437_TIOptim/");
	#elif CHIP_DM648
		strcat(outfilename, "OpenSVC_DM648_TIOptim/");
	#elif CHIP_OMAP3530
		strcat(outfilename, "OpenSVC_OMAP3530_TIOptim/");
	#endif

	get_file_name_from_path_v1(aux_name, (*pArgv)[2]);
	strcat(outfilename, aux_name);

    #ifdef CHIP_DM6437
	    strcat(outfilename, "_d_oSVC_1_04_r183_DM6437_layer");  
	    strcat(outfilename, (*pArgv)[6]);  
	    strcat(outfilename, "_temp_");  
	    strcat(outfilename, (*pArgv)[8]);  
	    strcat(outfilename, ".yuv");    
	#elif CHIP_DM648
	  strcat(outfilename, "_d_oSVC_1_04_r183_DM648_layer");  
	  strcat(outfilename, (*pArgv)[6]);  
	  strcat(outfilename, "_temp_"); 
	  strcat(outfilename, (*pArgv)[8]);  
	  strcat(outfilename, ".yuv");   
    #elif CHIP_DM642  
	  strcat(outfilename, "_d_oSVC_1_04_r183_DM642_layer");  
	  strcat(outfilename, (*pArgv)[6]);  
	  strcat(outfilename, "_temp_");  
	  strcat(outfilename, (*pArgv)[8]);  
	  strcat(outfilename, ".yuv");    
	#elif CHIP_OMAP3530
	  strcat(outfilename, "_d_oSVC_1_04_r183_OMAP3530_layer");  
	  strcat(outfilename, (*pArgv)[6]);  
	  strcat(outfilename, "_temp_");  
	  strcat(outfilename, (*pArgv)[8]);  
	  strcat(outfilename, ".yuv");  
	#endif
    #ifdef VERBOSE 
  	  puts(outfilename);
    #endif 
  	return 0;

}


int save_times_values (char * videofilename, unsigned int value1, unsigned int value2, unsigned int value3)
{
	FILE *pFile; //log file pointers

	// open file
	if( ( pFile= fopen("..//output//Stdout.csv", "a") )== NULL ) {
		printf("File `Stdout-Times.csv´ could not be opened\n");
		return -1;
	}

	fprintf(pFile, "%s, %d, %d, %d \n", videofilename, value1, value2, value3);
	fclose(pFile);	
	return 0;
} //End save_times_values()


void yuv_save(unsigned char **buf, int *wrap, int xsize, int ysize, char *filename)  //DSM
{
	FILE *f;
	int i; 
	static int primera_apertura = 1;

	if(primera_apertura)
	{
		f = fopen(filename, "wb");
		fclose(f);
		primera_apertura = 0;
	}

	f = fopen(filename, "ab");
	if (f == NULL)
	{
		printf("It is not possible to write in the output file\n");  //FIXME DSM
		exit(-1);
	}
	for(i=0; i<ysize; i++)
	{
		fwrite(buf[0] + i * wrap[0], 1, xsize, f);
	}
	for(i=0; i<ysize/2; i++)
	{
		fwrite(buf[1] + i * wrap[1], 1, xsize/2, f);
	}
	for(i=0; i<ysize/2; i++)
	{
		fwrite(buf[2] + i * wrap[2], 1, xsize/2, f);
	}

	fclose(f);

}


void save_data(unsigned char* Y,unsigned char* U,unsigned char* V,int XDIM,int YDIM,char* outfilename,long long acumulado)
{         
	unsigned char *YUV[3];  
	int linesize[3];  
	static int contador=0;

         
         
    YUV[0] = Y;  
    YUV[1] = U;  
	YUV[2] = V;  
	linesize[0] = XDIM + 32;  
	linesize[1] = (XDIM + 32) / 2;  
	linesize[2] = (XDIM + 32) / 2;  


	if (contador<FRAMES_TO_SAVE) 
		 	yuv_save(YUV,
          	linesize,
          	XDIM,
          	YDIM,
          	outfilename);          


    #ifdef VERBOSE
        printf("Saved frame %d.\n", contador);  
	#endif

	contador++;
	
	#ifdef SAVE_DATA_IN_FILE
		if (contador == DATA_TO_SAVE) { 
		   save_times_values (outfilename, (int)((acumulado)/(contador)), 0, contador); 
		   exit(1);     
		}     
	#endif
}
