#include <stdio.h>
#include <stdlib.h>

int main(void)
{
	//varaible declaration
	FILE* ASM_pFile = NULL;

	//code
	if( fopen_s(&ASM_pFile, "ASMLog.txt", "w") != 0)
	{
		printf(" File Creation Failed!!!!!!");
		exit(0);
	}

	fprintf(ASM_pFile,"\n--------- # Log File#-------by------# Akshay Mane #\n\n");
	fprintf(ASM_pFile , "India Is My Country.\n");

	fclose(ASM_pFile);

	ASM_pFile = NULL;

	return(0);
}