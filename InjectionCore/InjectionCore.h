#ifndef INJECTIONCORE_H
#define INJECTIONCORE_H
int __declspec(dllimport) GetProgressBarMainValue();
int __declspec(dllimport) GetProgressBarUpdateValue();

char __declspec(dllimport) *GetLabel1MainText();
char __declspec(dllimport) *GetLabel2UpdaterText();

bool __declspec(dllimport) GetFormLoginState();
bool __declspec(dllimport) GetFormSelectGameState();
bool __declspec(dllimport) GetFormUpdateState();
bool __declspec(dllimport) GetFormMainState();

void __declspec(dllimport) SetFormLoginState(bool value);
void __declspec(dllimport) SetFormSelectGameState(bool value);
void __declspec(dllimport) SetFormUpdateState(bool value);
void __declspec(dllimport) SetFormMainState(bool value);


void __declspec(dllimport) SetMemberLoginInformation (char* text);
void __declspec(dllimport) SetLoggedInState(bool value);
void __declspec(dllimport) SetGameSelectedState(bool value);

int __declspec(dllimport) GetNumberOfGames();
char __declspec(dllimport) *GetGameNamebyIdx(int i);
char __declspec(dllimport) *GetGameCodebyIdx(int i);
void __declspec(dllimport) SetGameCode(char *GameCode);


void __declspec(dllimport) InitializeInjectionCore(char* api_key);
#endif  // INJECTIONCORE_H

