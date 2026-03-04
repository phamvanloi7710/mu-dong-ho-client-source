#include "stdafx.h"
#include "CustomMonster.h"
#include "ZzzInfomation.h"

CCustomMonster gCustomMonster;

//--------------------------------------------------------------
// Trả về tên quái dựa theo ID (dùng hàm gốc trong 5.2)
//--------------------------------------------------------------
const char* CCustomMonster::GetNameMonter(int MonsterID)
{
    const char* name = getMonsterName(MonsterID);
    if (name == NULL || strlen(name) == 0)
    {
        return "Monster";
    }
    return name;
}