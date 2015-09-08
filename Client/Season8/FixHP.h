#pragma once
#if (__FIX_HP__ == 1)
extern int gHP;
extern int gMAXHP;
extern int gMP;
extern int gMAXMP;
extern int gSD;
extern int gMAXSD;
extern int gAttackRate;

void gObjUserSetHP(int hp);
void gObjUserSetMAXHP(int maxhp);
void gObjUserSetMP(int mp);
void gObjUserSetMAXMP(int maxmp);
void gObjUserSetSD(int sd);
void gObjUserSetMAXSD(int maxsd);
void InitHPFix();
void InitMPFix();
void InitSDFix();
void InitDMGFix();

void __FixHP1();
void __FixHP2();
void __FixSD1();
void __FixSD2();
void __fastcall CalculateAttackRate(int This, LPVOID EDX);
void AttackRateFix1();
void AttackRateFix2();
void __fastcall CalculateDamageRate(int This, LPVOID EDX);
void DamageRateFix1();
void DamageRateFix2();
#endif