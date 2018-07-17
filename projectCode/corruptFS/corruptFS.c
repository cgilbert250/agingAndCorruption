/*
NOTE: Compile as follows: gcc -D_FILE_OFFSET_BITS=64 -l c corruptFS.c
*/
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<math.h>
#define _FILE_OFFSET_BITS 64
#define EXT4_SUPERBLOCK_LOCATION 1024
#define EXT4_PADDING_OFFSET 1023
#define BLOCK_COUNT_ROW 13
#define BLOCK_SIZE_ROW 18
#define BLOCKS_PER_GROUP_ROW 21
#include<stdint.h>
void ext4Attack(unsigned long long int grpNum, FILE* fpTmp);
void xfsAttack();
void byteAttack(off_t byteNum);
unsigned long long int getNumOfBlockGroups(FILE* fpTmp);

char path[200];

int main(int argc, char *argv[])
{
   int a = 0;
   int i;
   int j;
   int pathFlag = 0;
   int byteFlag = 0;
   int bitFlag = 0;
   off_t byteNumber;
   int bitNumber;
   int FSIndicator = 0;
   int corruptIndicator = 0;
   unsigned long long int groupIndicator = 0;
   off_t groupIndicatorOff = 0;
   off_t lsize;


   if (argc != 2)
   {
      printf("\nUsage: ./corruptFS [device]\n");
      printf("\nWhere [device] is the path to the ");
      printf("desired file system\n\n");
      exit(0);
   }

   strcpy(path,argv[1]);
   
   bitNumber = 1;

   printf("\nPlease enter the number corresponding to the given filesystem:\n");
   printf("1. EXT4      2. XFS\n");
   int* ind = &FSIndicator;
   scanf("%d",ind);
   
   if((FSIndicator != 1) && (FSIndicator != 2))
   {
      printf("ERROR: Accepted values are 1 or 2. You entered %d. Exiting...\n",FSIndicator);
      exit(0);
   }
   
   unsigned long long int* gInd = &groupIndicator;
   if(FSIndicator == 1) //ext4 case
   {
      char p1[100] = "dumpe2fs ";
      char p3[100] = " > DUMPE2FSOUT 2>/dev/null";
      strcat(p1,path);
      strcat(p1,p3); 
   
      system(p1);//call the dumpe2fs command

      FILE* eFp;
      eFp = fopen("DUMPE2FSOUT","r");
      unsigned long long int extBGNumber = getNumOfBlockGroups(eFp);
      rewind(eFp);

      printf("\nPlease enter the number of the desired block group (0 .. %llu):\n",(extBGNumber-1));
      scanf("%llu",gInd);

      if((groupIndicator > (extBGNumber-1)) || (groupIndicator < 0))
      {
         printf("ERROR: Accepted values are 0 .. %jd. You entered %llu. Exiting...\n",(intmax_t)(extBGNumber-1),groupIndicator);
         exit(0);
      }

      ext4Attack(groupIndicator,eFp); //call ext4attack on the specified block group
      char p4[100] = "rm DUMPE2FSOUT";
      system(p4);
      fclose(eFp);
   }
 
   else //if FSIndicator == 2
   {
      xfsAttack();
   }

   printf("\nOperation is complete!\n\n");

   return 0;
}

void ext4Attack(unsigned long long int grpNum, FILE* fpTmp)
{ 
   char curStr[200];  
   const char delimeter[2] = " ";
   char* token;
   off_t lineNo = 0;
   char groupCheck[200];
   char groupNumCheck[200];
   char correctGroupNum[200];
   char nextGroupNum[200];
   char groupCase[200];
   sprintf(nextGroupNum,"%llu:",(grpNum+1));
   sprintf(correctGroupNum,"%llu:",grpNum); 
   int grpFlag = 0;   
   
   char grpDescriptorOld[200];
   char grpDescriptor[200];
   char superblock[200];
   char reservedGDTBlocks[200];
   char blockBitmap[200];
   char inodeBitmap[200];
   char inodeTable[200];
   unsigned long long int superblockLocation = 0;
   unsigned long long int grpDescriptorLocation = 0;
   unsigned long long int reservedGDTBlocksLocation = 0;
   unsigned long long int blockBitmapLocation = 0;
   unsigned long long int inodeBitmapLocation = 0;
   unsigned long long int inodeTableLocation = 0;
   unsigned long long int myCounter = 48;


   while(fgets(curStr,200,fpTmp) != NULL) // loop through each line of the dump file
   {
      if(lineNo > 47)
      {

         if(strcmp(curStr,"\n") == 0) //empty case
            continue;

         token = strtok(curStr,delimeter);
         strcpy(groupCheck,token); 
         token = strtok(NULL,delimeter);
         strcpy(groupNumCheck,token);
      
         if((strcmp(groupCheck,"Group") == 0) && (strcmp(groupNumCheck,correctGroupNum) == 0))
         {
            fgets(curStr,200,fpTmp);
            token = strtok(curStr,delimeter);
            strcpy(groupCase,token);          

            while(strcmp(groupCase,"Group") != 0) 
            {
               //Superblock and Group Descriptors Case
               if((strcmp(groupCase,"Primary") == 0) || (strcmp(groupCase,"Backup") == 0))
               {
                  token = strtok(NULL,delimeter);
                  token = strtok(NULL,delimeter);
                  token = strtok(NULL,delimeter); //token should now equal superblock location
                  strcpy(superblock,token);
                  superblock[strlen(superblock)-1] = 0; // trim off the comma
                  char* strtoullHelper;
                  superblockLocation = strtoull(superblock,&strtoullHelper,10);
                  superblockLocation += EXT4_PADDING_OFFSET;

                  token = strtok(NULL,delimeter);
                  token = strtok(NULL,delimeter);
                  token = strtok(NULL,delimeter); 
                  token = strtok(NULL,delimeter);

                  strcpy(grpDescriptorOld,token);             
                  const char descDelimeter[2] = "-";
                  char* descToken;                   
                  descToken = strtok(grpDescriptorOld,descDelimeter);
                  strcpy(grpDescriptor,descToken);
                  char* strtoullHelper2;
                  grpDescriptorLocation = strtoull(grpDescriptor,&strtoullHelper2,10);
                  grpDescriptorLocation += EXT4_PADDING_OFFSET;
               }//end Superblock and Group Descriptors Case 

               if(strcmp(groupCase,"Reserved") == 0) // Reserved GDT Blocks case
               {
                  token = strtok(NULL,delimeter);
                  token = strtok(NULL,delimeter);
                  token = strtok(NULL,delimeter);
                  token = strtok(NULL,delimeter);
                  
                  char oldReserved[200];   
                  strcpy(oldReserved,token);
                  const char descDelimeter[2] = "-";
                  char* descToken;      
                  descToken = strtok(oldReserved,descDelimeter);
                  strcpy(reservedGDTBlocks,descToken);
                  char* strtoullHelper;
                  reservedGDTBlocksLocation = strtoull(reservedGDTBlocks,&strtoullHelper,10);
                  reservedGDTBlocksLocation += EXT4_PADDING_OFFSET;                          
               }

               if(strcmp(groupCase,"Block") == 0) // Block bitmap case
               {
                  token = strtok(NULL,delimeter);
                  token = strtok(NULL,delimeter);
                  token = strtok(NULL,delimeter);
         
                  strcpy(blockBitmap,token);
                  char* strtoullHelper;
                  blockBitmapLocation = strtoull(blockBitmap,&strtoullHelper,10);
                  blockBitmapLocation += EXT4_PADDING_OFFSET;                                             
               }

               if(strcmp(groupCase,"Inode") == 0) // Inode cases
               {
                  char inodeDecision[200];
                  token = strtok(NULL,delimeter);
                  strcpy(inodeDecision,token);
                  
                  if(strcmp(inodeDecision,"bitmap") == 0) //inode bitmap case
                  {
                     token = strtok(NULL,delimeter);
                     token = strtok(NULL,delimeter);
                     
                     strcpy(inodeBitmap,token);
                     char* strtoullHelper;
                     inodeBitmapLocation = strtoull(inodeBitmap,&strtoullHelper,10);
                     inodeBitmapLocation += EXT4_PADDING_OFFSET;          
                  }

                  else if(strcmp(inodeDecision,"table") == 0) //inode table case
                  {
                     token = strtok(NULL,delimeter);
                     token = strtok(NULL,delimeter);

                     char oldTable[200];
                     strcpy(oldTable,token);
                     const char descDelimeter[2] = "-";
                     char* descToken;
                     descToken = strtok(oldTable,descDelimeter);
                     strcpy(inodeTable,descToken);
                     char* strtoullHelper;
                     inodeTableLocation = strtoull(inodeTable,&strtoullHelper,10);
                     inodeTableLocation += EXT4_PADDING_OFFSET;                     
                  }                   
               }

               if(fgets(curStr,200,fpTmp) == NULL)
                  break;
               token = strtok(curStr,delimeter);
               strcpy(groupCase,token); 
            }//end group while

            grpFlag = 1;
         }//end group if
         
         if(grpFlag == 1)
            break;
      }//end line # if
      lineNo++;
   }//end main ext4 attack while 

   printf("\nCorruption options for the selected group are as follows:\n");

   int choice[10]; // 1 = Superblock, 2 = grpdescriptors, 3 = gdt blocks, 4 = block bitmap, 5 = inode bitmap, 6 = inode table
   int qfiterator;
   for(qfiterator = 0; qfiterator < 10; qfiterator++)
   {
      choice[qfiterator] = 0;
   }

   int whileIterator = 0;

   if(superblockLocation != 0)
   {
      printf("%d. Superblock\n",whileIterator+1);
      whileIterator++;
      choice[whileIterator] = 1;
   }
   if(grpDescriptorLocation != 0)
   {
      printf("%d. Group Descriptors\n",whileIterator+1);
      whileIterator++;
      choice[whileIterator] = 2; 
   } 
   if(reservedGDTBlocksLocation != 0)
   {
      printf("%d. Reserved GDT Blocks\n",whileIterator+1);
      whileIterator++;
      choice[whileIterator] = 3;
   } 
   if(blockBitmapLocation != 0)
   {
      printf("%d. Block Bitmap\n",whileIterator+1);
      whileIterator++;
      choice[whileIterator] = 4;
   }
   if(inodeBitmapLocation != 0)
   {
      printf("%d. Inode Bitmap\n",whileIterator+1);
      whileIterator++;
      choice[whileIterator] = 5;
   }
   if(inodeTableLocation != 0)
   {
      printf("%d. Inode Table\n",whileIterator+1);
      whileIterator++;
      choice[whileIterator] = 6;
   }

   printf("\nPlease select the component that you would like to corrupt (1 .. %d)\n",whileIterator);
   int corChoice = 0;
   int* chorChoicePtr = &corChoice;
   scanf("%d",chorChoicePtr); 

   switch(choice[corChoice])
   {
      case 1: //Superblock
         byteAttack((off_t)superblockLocation);
         //printf("Superblock switch case! A %llu\n",superblockLocation);
         break;
      case 2: // group descriptor
         byteAttack((off_t)grpDescriptorLocation);
         //printf("Group desc switch case! At %llu\n",grpDescriptorLocation);
         break;
      case 3: //gdt blocks
         byteAttack((off_t)reservedGDTBlocksLocation);
         //printf("GDT switch case! At %llu\n",reservedGDTBlocksLocation);
         break;   
      case 4: // block bitmap
         byteAttack((off_t)blockBitmapLocation);
         //printf("block bitmap switch case! At %llu\n",blockBitmapLocation);
         break;
      case 5: //inode bitmap
         byteAttack((off_t)inodeBitmapLocation);
         //printf("Inode bitmap switch case! At %llu\n",inodeBitmapLocation);
         break;
      case 6: // inode table
         byteAttack((off_t)inodeTableLocation);
         //printf("inode table switch case! At %llu\n",inodeTableLocation);
         break;
   }

   //printf("End of ext4attack\n"); 
   //exit(-1);

   return;
}//end ext4attack

void xfsAttack()
{
   printf("Please enter the number corresponding to your desired corruption:\n");
   printf("1. Inodes\n");
   printf("2. Superblock\n");
   printf("3. B+ Tree Inodes\n");
   printf("4. B+ Tree Block Number\n");
   printf("5. B+ Tree Count\n");

   int sbf = 0;
   int xCorChoice = 0;
   int* xCorChoicePtr = &xCorChoice;
   scanf("%d",xCorChoicePtr);

   char sp1[120] = "xfs_db -x -c blockget -c \"blocktrash -s 512109 -n 10 -t ";
   char sbc[120] = "xfs_db -x -c blockget -c \"blocktrash -s 512109 -n 3 -t ";
   char sp2[120];
   char sp3[120] = "\" ";
   strcat(sp3,path);
   strcat(sp3," > /dev/null 2>&1");

   switch(xCorChoice)
   {
      case 1:
         strcpy(sp2,"inode");
         break; 
      case 2:
         strcpy(sp2,"sb");
         sbf = 1;
         break;
      case 3:
         strcpy(sp2,"btino");
         break;
      case 4:
         strcpy(sp2,"btbno");
         break;
      case 5:
         strcpy(sp2,"btcnt");
         break;
      default:
         printf("ERROR: %d is not an accepted value. Exiting...\n",xCorChoice);
         exit(-1);
   }

   if(sbf == 1)
   {
      strcat(sbc,sp2);
      strcat(sbc,sp3);
      system(sbc);
   }
   
   else
   {
      strcat(sp1,sp2);
      strcat(sp1,sp3);
      system(sp1);
   }

   return;
}

void byteAttack(off_t byteNum)
{

   int xorValue = 128; 
   FILE* fp;
   char* buff;
   int value;

   fp = fopen(path, "rb");

   fseeko(fp,byteNum,SEEK_SET);

   buff = (char*) malloc(sizeof(char)); 
   fread(buff,1,1,fp);
   fclose(fp);

   value = buff[0];

   value = value & 255;

   //swap the bit
   value = value ^ xorValue; 

   buff[0] = value;

   fp = fopen(path, "rb+");
   fseeko(fp,byteNum,SEEK_SET);
   fwrite(buff,1,1,fp);
   fclose(fp);

   free(buff);

   return;
}

unsigned long long int getNumOfBlockGroups(FILE* fpTmp)
{
        char placeholder[400];
        char placeholder2[400];
        char blockCount[120];
        char blockSize[120];
        char blocksPerGroup[120];
        const char delimeter[2] = " ";
        const char colonDelimeter[2] = ":";
        char* token;
        char* myToken;
        unsigned long long int bc = 0; 
        unsigned long long int bs = 0;
        unsigned long long int bpg = 0;
        unsigned long long int numOfGroups = 0;

        char curStr[400];

        int curLine;
        for(curLine = 0; curLine < 40; curLine++)
        {
                fgets(curStr,400,fpTmp);
                strcpy(placeholder,curStr);
                myToken = strtok(curStr,colonDelimeter);  
                strcpy(curStr,myToken);   
   
                if(strcmp(curStr,"Block count") == 0) 
                {
                        token = strtok(placeholder,delimeter);
                        token = strtok(NULL,delimeter);
                        token = strtok(NULL,delimeter);
                        strcpy(blockCount,token);
                }
                else if(strcmp(curStr,"Block size") == 0)
                {
                        token = strtok(placeholder,delimeter);
                        token = strtok(NULL,delimeter);
                        token = strtok(NULL,delimeter);
                        strcpy(blockSize,token);
                }
                else if(strcmp(curStr,"Blocks per group") == 0)
                {
                        token = strtok(placeholder,delimeter);
                        token = strtok(NULL,delimeter);
                        token = strtok(NULL,delimeter);
                        token = strtok(NULL,delimeter);
                        strcpy(blocksPerGroup,token);
                }
        }

        char* reqChar;

        bc  = strtoull(blockCount,&reqChar,10);
        bs  = strtoull(blockSize,&reqChar,10);
        bpg = strtoull(blocksPerGroup,&reqChar,10);  

        numOfGroups = ((bc * bs) / (bpg * bs));
        
        return numOfGroups;
}


