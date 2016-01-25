#ifndef INC_SYSTEM_CONFIGURE_NATIVE_INTERFACE_H_
#define INC_SYSTEM_CONFIGURE_NATIVE_INTERFACE_H_

#include <wchar.h>

typedef int (*pf_Validation)(struct NativeProperty*, struct NativePropertyArray*);

#define SHORTLENGTH 40
#define MIDDLELENGTH 100
#define LONGLENGTH 200

typedef struct NativeProperty
{
  wchar_t key[SHORTLENGTH];     //the property's name
  wchar_t value[MIDDLELENGTH];  //the property's value
  wchar_t group[SHORTLENGTH];   //shows what group does the property belong to
  wchar_t options[LONGLENGTH];  //combine all the option strings into one. 
  // And split the options with semicolon(";")
  // E.g. LNB type's options is "Standard;User;Universal"
  pf_Validation m_validation;   //this a function pointer points to validation function
} NativeProperty;

typedef struct NativePropertyArray
{
  unsigned int size;
  NativeProperty* first;
} NativePropertyArray;


// interface the project should implement
__declspec(dllexport) int ProjectInitial(wchar_t* initial_parameter);
__declspec(dllexport) int Load(wchar_t* file_path, struct NativePropertyArray* all_propertys);
__declspec(dllexport) int Save(wchar_t* file_path, struct NativePropertyArray* all_propertys);
__declspec(dllexport) int Clean();
__declspec(dllexport) void GetLastError(wchar_t* info, size_t info_length);


#endif  // INC_SYSTEM_CONFIGURE_NATIVE_INTERFACE_H_