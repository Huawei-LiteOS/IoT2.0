#ifndef _STUB_H_
#define _STUB_H_
#ifdef __cplusplus
extern "C" {
#endif
typedef struct stubInfo {  
    void *funcAddr; //�����ַ  
    unsigned char byteCode[5]; //�����ַ��Ӧ��ָ��  
} stubInfo;  

void setStub(void *funcAddr, void *stubAddr, stubInfo *si);
void cleanStub(stubInfo *si);

#ifdef __cplusplus
}
#endif
#endif