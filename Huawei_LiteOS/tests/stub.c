#include <sys/mman.h>  
#include <unistd.h>  
#include <string.h>  
#include <stdio.h>  
#include "stub.h"
  
#define JMP_CMD 0xe9    //�����תָ�� jmp xxx (�ܹ���5���ֽڣ����ĸ��ֽ�Ϊ��ַ)  
                        //0xff ������תָ�FF   
#if 0
typedef struct stubInfo {  
    void *funcAddr; //�����ַ  
    unsigned char byteCode[5]; //�����ַ��Ӧ��ָ��  
} stubInfo;  
  #endif
static void setJumpCode(void *codeAddr, char jumpCode[5])  
{  
   int pagesize = sysconf(_SC_PAGE_SIZE);//4096=0x1000  
   if (mprotect((void*) ((unsigned long) codeAddr & (~(pagesize - 1))), pagesize, PROT_READ | PROT_WRITE | PROT_EXEC) != 0) {   //�޸�ҳ��Ϊ��д  
      return;  
   }  
   memcpy(codeAddr, jumpCode, 5);  
}  
  
void setStub(void *funcAddr, void *stubAddr, stubInfo *si)  
{  
    char jumpCode[5] = {0xe9}; //��תָ��  
    int  dist = stubAddr - funcAddr - 5;    //���ƫ��  
  
    memcpy((void *)&jumpCode[1], (void *)&dist, sizeof(void *));  
    si->funcAddr = funcAddr; //����ԭ�����ĵ�ַ  
    memcpy((void *)&si->byteCode[0], (void *)funcAddr, 5);   //����ԭ��ַ����ָ��  
      
    setJumpCode(funcAddr, jumpCode); //����תָ���滻 ԭ��ַ����ָ��  
    //�˺����൱�ڣ�  
    //*(char*)funcAddr=0xe9;  
    //*(int*)((char*)funcAddr+1)=stubAddr-funcAddr-5;  
}  

void setIntStub(int *funcAddr, int *stubAddr, stubInfo *si)  
{  
    char jumpCode[5] = {0xe9}; //��תָ��  
    int  dist = stubAddr - funcAddr - 5;    //���ƫ��  
  
    memcpy((void *)&jumpCode[1], (void *)&dist, sizeof(void *));  
    si->funcAddr = funcAddr; //����ԭ�����ĵ�ַ  
    memcpy((void *)&si->byteCode[0], (void *)funcAddr, 5);   //����ԭ��ַ����ָ��  
      
    setJumpCode(funcAddr, jumpCode); //����תָ���滻 ԭ��ַ����ָ��  
    //�˺����൱�ڣ�  
    //*(char*)funcAddr=0xe9;  
    //*(int*)((char*)funcAddr+1)=stubAddr-funcAddr-5;  
}  

  
void cleanStub(stubInfo *si)  
{  
    char   jumpCode[5]; //�������ԭ�ش���ָ��  
    memcpy((void *)&jumpCode, (void *)&si->byteCode[0], 5);  
    setJumpCode(si->funcAddr, jumpCode); //�ָ�ԭ��ַ����ָ��  
    //(ʡ���˻�ԭԭ��ַ����ҳ�ı�������)  
}  
 #if 0
int stub(){ //׮����  
        printf("this is stub func !\n");  
        return 0;  
}  
  
int a(){  //���Ժ�������׮��  
        printf("this is a func !\n");  
        return 0;  
}  
  
int main(){  
        stubInfo si;  
        setStub(a,stub,&si);  
        a(); //��ʱ�����׮����      
    cleanStub(&si);  
        a();    
  
} 
#endif 
