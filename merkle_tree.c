/*All the software related files will reside in a folder name, "bitcoin", in root folder.*/
/*MUST READ ABOUT MERKLE TREE AND HOW IT WORKS (duplication of records when no of records are less than number of leaves etc) BEFORE READING THE CODE*/

#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<string.h>
#include<unistd.h>
#include<dirent.h>

#define MAX_SIZE 201                                                  //maximum length of filename

#ifdef __linux__
 char folder[MAX_SIZE] = "/home/";

#elif __APPLE__
 char folder[MAX_SIZE] = "/Users/";

#else
 printf("\n\n\n\t\tOS Not Found.\n\n");

#endif

/*------------------------------------------------------------------------------------*/
// structure for transaction record.

struct transaction
{
 long double amount;                                                  //amount to be debited from account
 unsigned char transaction_fee;                                       //char can be used a integer with range 0-255. Transaction fee will not be greater than that.
 unsigned long timestamp;                                             // number of seconds passed since 1970-01-01 00:00:00 UTC

 /* data types for 'public key' of reciever,
    'digital signature' of sender and          */
 }; 

/*------------------------------------------------------------------------------------*/
//structure for hash node;

struct merkle
{
 unsigned char copy;                             //The need of this memeber is explained in the code of binary_correct() and delete_tree() function.
 char hash[65];                                  //hash is 64 characters long. 1 byte for NULL charactere.
 struct transaction *data;
 struct merkle *left;
 struct merkle *right;
 } *root;


/*------------------------------------------------------------------------------------*/
unsigned short height;                                                           //stores the height of the binary tree
unsigned short count;          //counts the total number of transactions. The ceil() of log of count, with base 2, will give the height of binary tree.
unsigned short c;

char filename[MAX_SIZE];                                                   //will save the name of transaction file, which is read from "a.txt", which is to be opened.
char tempfile[MAX_SIZE];
char miner[] = "/miner/";
char sha[] = "/sha/";

FILE *fpg, *fp;                                                                 //"fpg" to open list.txt, "fp" to open transaction files.

struct merkle* create_merkle_node();                                            //function to dynamically allocate memory for data type merkle.
struct transaction* create_transaction_node();                                  //function to dynamically allocate memory for data type transaction.

struct merkle* binary_make(struct merkle*, unsigned short, unsigned short);     //forms a binary tree out of transaction records.
void merkle_hash(struct merkle*, unsigned short, unsigned short);
void binary_correct(struct merkle*, unsigned short, unsigned short);            //in case if no of transactions is less than no of leaves.

void binary_traverse(struct merkle*, unsigned short, unsigned short);
void delete_tree(struct merkle*,unsigned short,unsigned short);
void prerun_setup();                                                            //initializes some important values, like path to the bitcoin folder.

void char_refresh(char[],unsigned short);
void full_path(char[],char[]);                                                  //gives the path to a file from the root location.
/*------------------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------------*/


int main()
{
 unsigned short i;

 prerun_setup();               //to assign folder path, count no of files. 
 height = ceil(log2(count));  
 

/*
- The height of a complete-balanced binary tree is always x = log2(count), where count is no of leaves. thus count is always 2^x;
- Since we are not sure that no of transaction record will be in form of 2^x, we take [log2(count)], where [.] is GIF function.
- The the leaves which will be left, will contain repeated data.
*/


// printf("\n%hu\n%hu\n%s\n\n\n\n",height,count,folder);

 strcpy(filename,"list.txt");
 full_path(miner,filename);

 root = NULL;
 c=0;

 fpg = fopen(filename,"r");
 root = binary_make(root,height,0);
 fclose(fpg);

 binary_correct(root,height,0);
// printf("\n\n The output is in level order\n");
// binary_traverse(root,height,0);

 merkle_hash(root,height,0);
 binary_traverse(root,height,0);
 strcpy(filename,"merkle_sha.txt");
 full_path(miner,filename);
 fpg = fopen(filename,"w");
 fprintf(fpg,"%s",root->hash);
 fclose(fp);

 delete_tree(root,height,0);
 
 printf("\nSuccessful\n Check ~/betacoin/miner/merkle_hash.txt\n");
 return 0;
 }




/*------------------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------------*/
struct merkle* create_merkle_node()
{
 short i;
 struct merkle *ptr;
 ptr = NULL;
 ptr = (struct merkle*)malloc(sizeof(struct merkle));

 if(ptr == NULL)
  return ptr;

 for(i=0; i<66; i++, ptr->hash[i]='\0');

 ptr->copy = 0;
 ptr->data = NULL;
 ptr->left = NULL;
 ptr->right = NULL;

 return ptr;
 }
/*------------------------------------------------------------------------------------*/

struct transaction* create_transaction_node()
{
 struct transaction *ptr;
 ptr = NULL;
 ptr = (struct transaction*)malloc(sizeof(struct transaction));

 if(ptr == NULL)
  return ptr;

 ptr->amount = 0;
 ptr-> transaction_fee = 0;

 return ptr;
 }
/*------------------------------------------------------------------------------------*/

struct merkle* binary_make(struct merkle *head, unsigned short height, unsigned short h)
{
 if(c == count)  //'c' > 'count' means all transactions are listed in binary tree.
  return head;

 if(h>height)   //if height is getting larger tha the height decided, then it will stop making new nodes and return.
  return head;

 head = create_merkle_node();

 if(head == NULL)
 {
  printf("\n\n\tERROR: WHILE CREATING MERKLE TREE, AT HEIGHT %d",height);
  exit(1);
  }

// printf("\nCreation height: %hu",h);

 if(h == height)
 {
  head->data = create_transaction_node();
  if(head->data == NULL)
  {
   printf("\n\n\tERROR: WHILE CREATING TRANSACTION DATA, AT HEIGHT %d",height);
   exit(1);
   }
  
  fscanf(fpg,"%s",filename);
  full_path(miner,filename);

  fp = fopen(filename,"rb");
  fread(head->data, sizeof(struct transaction),1, fp);
//  printf("\nAmount inserted: %Lf",head->data->amount);
  fclose(fp);
  c++;
 
  }


 head->left = binary_make(head->left, height, h+1);
 head->right = binary_make(head->right, height, h+1);

 return head;
 }



/*------------------------------------------------------------------------------------*/

void char_refresh(char a[], unsigned short n)
{
 unsigned short i;
 for(i=0; i<n; i++)
  a[i]='\0';                 //initializes each element of string with NULL character, so as no garbage value is there.
 }
/*------------------------------------------------------------------------------------*/

void full_path(char subd[],char a[])
{
 unsigned short i;
 char temp[MAX_SIZE];
 for(i=0; ;i++)
 {
  if(a[i]=='\0')
  {
   if(a[i-1] == '\n')
    a[i-1] = '\0';
   break;
   }
  }

 strcpy(temp,folder);
 strcat(temp,subd);
 strcat(temp,a);
 strcpy(a,temp);
 }

/*------------------------------------------------------------------------------------*/

void prerun_setup()
{
 char uname[20];
 char temp[MAX_SIZE];
 DIR *dr;                         //DIR pointer to open "/bitcoin/miner/" directory so that we can list out names of files inside it.
 FILE *fp;                        //FILE pointr to open list.txt which will save names of transaction files.
 struct dirent *de;              
 unsigned short len;
 char **name, *tempo;
 unsigned short i,j;

 root = NULL;
 count = 0;
 c = 0;
 height = 0;

 
 getlogin_r(uname,20);                            //saves the current logged in user_name in "folder"
 strcat(folder,uname);
 strcat(folder,"/betacoin");                    //finally folder contains: "/home/<user_name>/betacoin"
 
 strcpy(temp,folder);
 strcat(temp,"/miner/");

 dr = opendir(temp);

 if(dr == NULL)
 {
  printf("ERROR: CANNOT OPEN DIRECTORY IN prerun_setup.");
  exit(0);
  }


 strcpy(filename,"list.txt");
 full_path(miner,filename);
 fp = fopen(filename,"w");
 
 while((de = readdir(dr)) != NULL)
 {
  if(strcmp(de->d_name , ".") == 0 || strcmp(de->d_name , "..") == 0)                  //not to select "current directory" and "previous directory"
  continue;

 len = strlen(de->d_name);

 if(strncmp(de->d_name + len - 12, ".transaction",12) == 0)                        //length of string ".transaction" is 12.
 {
  fprintf(fp,"%s\n",de->d_name);
  count++;
  }
 }
 
 fclose(fp);
 closedir(dr);

 name = (char**)malloc(sizeof(char*)*count);
 for(c = 0; c < count; c++)
  name[c] = (char*)malloc(sizeof(char*)*50);

 fp = fopen(filename,"r");
 for(i = 0 ; i< count; i++)
  fscanf(fp,"%s",name[i]);
 fclose(fp); 

 for(i=0; i<count; i++)
 {
  for(j=0; j<count-i-1; j++)
  {
   if(strcmp(name[j] , name[j+1])>0)
   {
    tempo = name[j];
    name[j] = name[j+1];
    name[j+1] = tempo;
    }
   }
  }


  strcpy(tempfile,"temp.txt");
  full_path(miner,tempfile);
  fp = fopen(tempfile,"w");
   for(i=0;i<count;i++)
    fprintf(fp,"%s\n",name[i]);
   fclose(fp);


remove(filename);
rename(tempfile,filename);


 }
/*------------------------------------------------------------------------------------*/

void binary_traverse(struct merkle *head, unsigned short height, unsigned short h)
{
 if(head == NULL)
  return;

 if(h > height)
  return;


//printf("\nheight traverde: %hu",h);
 if(h == height)
//  printf("\nAmount: %Lf", head->data->amount);
//  printf("\nhash: %s\n",head->hash);
 binary_traverse(head->left, height, h+1);
 binary_traverse(head->right, height, h+1);

 }

/*------------------------------------------------------------------------------------*/
void binary_correct(struct merkle *head, unsigned short height, unsigned short h)
{
 if(h > height)
  return;

 if(h <= height && head->right == NULL)
 {
  head->right = head->left;
  head->copy = 1;                              
  }

 binary_correct(head->left, height, h+1);
 binary_correct(head->right, height, h+1); 
 }
/*
- Since we are traversing in Inorder sequence, only the right leg of any of the node will have to contain repeated data, since left leg will be filled first.
- Now, copy = 1, means the right leg of current node, is pointing to same chain, pointed by its left leg.
- We could have duplicated the data, but to make it memory efficient, we pointed it.
 */



/*------------------------------------------------------------------------------------*/
void delete_tree(struct merkle *head, unsigned short height, unsigned short h)
{ 
 if(h > height)
 return;
 
 if(head == NULL)
 return;

 if(h == height)
  free(head->data);

 delete_tree(head->left, height, h+1);
 free(head->left);
 head->left = NULL;
 
 if(head->copy != 1)
 {
  delete_tree(head->right, height, h+1);
  free(head->right);
  head->right = NULL;
  }
 }

/*
- Here the need of 'copy' member, is explained.
- When copy = 1, means left leg and right leg are pointing to same data.
- We are traversing in Inorder sequence, means left leg is freed, and right leg was pointing to same data.
- If we again try to free same memory, it will cause segmentation error. 
- Thus we check if copy = 1, it should not try to free the left leg.
 */


/*------------------------------------------------------------------------------------*/

void merkle_hash(struct merkle *head, unsigned short height, unsigned short h)
{ 
 if(h == height)
 {
  strcpy(filename,"input.txt");
  full_path(sha,filename);
  fp = fopen(filename,"w");
  fprintf(fp,"%Lf",head->data->amount);
  fprintf(fp,"%c",head->data->transaction_fee);
  fprintf(fp,"%ld",head->data->timestamp);
  fclose(fp);

  system("python3 ~/betacoin/miner/SHA_function.py");

  strcpy(filename,"output.txt");
  full_path(sha,filename);
  fp = fopen(filename,"r");
  fscanf(fp,"%s",head->hash);
  fclose(fp);
  return;
  }  

  merkle_hash(head->left, height, h+1);
  
  if(head->copy != 1)
   merkle_hash(head->right, height, h+1);

  strcpy(filename,"input.txt");
  full_path(sha,filename);
  fp = fopen(filename,"w");
  fprintf(fp,"%s%s",head->left->hash,head->right->hash);
  fclose(fp);

  system("python3 ~/betacoin/miner/SHA_function.py");

  strcpy(filename,"output.txt");
  full_path(sha,filename);
  fp = fopen(filename,"r");
  fscanf(fp,"%s",head->hash);
  fclose(fp);
  return;
  
 }
/*------------------------------------------------------------------------------------*/
