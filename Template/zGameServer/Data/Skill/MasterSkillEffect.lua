--===================================================================================
-- zGameServer
-- ----------------------------------------------------------------------------------
-- Description: Master skill effect formuls
-- ----------------------------------------------------------------------------------
-- Revision: 	4:34 15.10.2014
--===================================================================================

-- 
function FN_LuaGetPercent(nPoint)
	local nReturn = 1 + ((((((((nPoint) - 30) ^ 3) + 25000) / 499) / 50) * 100) / 12)
	return nReturn
end

-- type 2
function FN_LuaGetPercent2(nPoint)
	local nReturn = 0.8 + (((((((nPoint - 30) ^ 3) + 25000) / 499) / 6)))
	return nReturn
end

-- type 3
function FN_LuaGetSkillDamageMin(nPoint)
	local nReturn = (0.85 + (((((((nPoint - 30) ^ 3) + 25000) / 499) / 6)))) * 6
	return nReturn
end

-- type 4
function FN_LuaGetSkillDamageMiddle(nPoint)
	local nReturn = (0.9 + (((((((nPoint - 30) ^ 3) + 25000) / 499) / 6)))) * 8
	return nReturn
end

-- type 5
function FN_LuaGetSkillDamageMax(nPoint)
	local nReturn = (0.95 + (((((((nPoint - 30) ^ 3) + 25000) / 499) / 6)))) * 10
	return nReturn
end

-- type 6
function FN_LuaGetMonsterKill(nPoint)
	local nReturn =  52 / (1 + (((((((nPoint - 30) ^ 3) + 25000) / 499) / 6))))
	return nReturn
end

-- type 7
function FN_LuaGetPercent3(nPoint)
	local nReturn =  (1 + (((((((nPoint - 30) ^ 3) + 25000) / 499) / 6)))) * 1.5
	return nReturn
end

-- type 8
function FN_LuaGetMaxAG(nPoint)
	local nReturn = (1 + (((((((nPoint - 30) ^ 3) + 25000) / 499) / 50) * 100) / 12)) * 23
	return nReturn
end

-- type 9
function FN_LuaGetMaxLifeMana(nPoint)
	local nReturn = (1 + (((((((nPoint - 30) ^ 3) + 25000) / 499) / 50) * 100) / 12)) * 85
	return nReturn
end

-- type 10
function FN_LuaGetAttack(nPoint)
	local nReturn = (0.8 + (((((((nPoint - 30) ^ 3) + 25000) / 499) / 50) * 100) / 12)) * 8
	return nReturn
end

-- type 11
function FN_LuaGetSDMonsterKill(nPoint)
	local nReturn = 11 / (1 + (((((((nPoint - 30) ^ 3) + 25000) / 499) / 50) * 100) / 12))
	return nReturn
end

-- type 12
function FN_LuaGetBlockingRate(nPoint)
	local nReturn = (1 + (((((((nPoint - 30) ^ 3) + 25000) / 499) / 50) * 100) / 12)) * 85 * 6
	return nReturn
end

-- type 13
function FN_LuaGetAttackSuccessRate(nPoint)
	local nReturn = (1 + (((((((nPoint - 30) ^ 3) + 25000) / 499) / 50) * 100) / 12)) * 85 * 5
	return nReturn
end

-- type 14
function FN_LuaGetManAttackRate(nPoint)
	local nReturn = (1 + (((((((nPoint - 30) ^ 3) + 25000) / 499) / 50) * 100) / 12)) * 85 * 8
	return nReturn
end

-- type 15
function FN_LuaGetPercent4(nPoint)
	local nReturn = (1 + (((((((nPoint - 30) ^ 3) + 25000) / 499) / 6)))) * 15
	return nReturn
end

-- type 16
function FN_LuaGetIncreaseBlocking(nPoint)
	local nReturn = (1 + (((((((nPoint - 30) ^ 3) + 25000) / 499) / 6)))) * 50
	return nReturn
end

-- type 17
function FN_LuaGetIncreaseStat(nPoint)
	local nReturn = (1 + (((((((nPoint - 30) ^ 3) + 25000) / 499) / 6)))) * 10
	return nReturn
end

-- type 18
function FN_LuaGetDecreaseMana(nPoint)
	local nReturn = (1 + (((((((nPoint - 30) ^ 3) + 25000) / 499) / 6)))) * 6
	return nReturn
end

-- type 19
function FN_LuaGetChangeBlocking(nPoint)
	local nReturn = 50 / (1 + (((((((nPoint - 30) ^ 3) + 25000) / 499) / 6)))) + 20
	return nReturn
end

-- type 20
function FN_LuaGetControlAttack(nPoint)
	local nReturn = 40 / (1 + (((((((nPoint - 30) ^ 3) + 25000) / 499) / 6)))) + 5
	return nReturn
end

-- type 21
function FN_LuaGetManaIncRate(nPoint)
	local nReturn = (2 + (((((((nPoint - 30 ) ^ 3) + 25000) / 499 ) / 6)))) * 1.5
	return nReturn
end

-- type 22
function FN_LuaGetSkillDamageMin2(nPoint)
	local nReturn = (0.8 + (((((((nPoint - 30) ^ 3) + 25000) / 499) / 6)))) * 5
	return nReturn
end

-- type 23
function FN_LuaGetAttackSpeed(nPoint)
	local nReturn = nPoint * 1.0
	return nReturn
end

-- type 25
function FN_LuaBloodStorm(nPoint)
	local nReturn = 170
	return nReturn
end

-- type 26 
function FN_LuaEarthPrison(nPoint)
	local nReturn = 150
	return nReturn
end

-- type 27
function FN_LuaPoisonArrow(nPoint)
	local nReturn = 130
	return nReturn
end

-- type 28
function FN_LuaIronDefense(nPoint)
	local nReturn = 800
	return nReturn
end

-- type 29
function FN_LuaGetBlockingRateFighter(nPoint)
	local nReturn = (1 + ( ( ( ( ( ( (nPoint - 30) ^ 3) + 25000) / 499) / 50) * 100) / 12)) * 85 * 2.5
	return nReturn
end

-- type 30
function FN_LuaGetAttackSuccessRateFighter(nPoint)
	local nReturn = (1 + ( ( ( ( ( ( (nPoint - 30) ^ 3) + 25000) / 499) / 50) * 100) / 12) ) * 85 * 2
	return nReturn
end

-- type 31
function FN_LuaGetManAttackRateFighter(nPoint)
	local nReturn = (1 + ( ( ( ( ( ( (nPoint - 30) ^ 3) + 25000) / 499) / 50) * 100) / 12)) * 85 * 3.2
	return nReturn
end

-- type 33
function FN_LuaGetMaxSDFighter(nPoint)
	local nReturn = (1 + ( ( ( ( ( ((nPoint - 30) ^ 3) + 25000) / 499) / 50) * 100) / 12)) * 85 * 5
	return nReturn
end

-- type 34
function FN_LuaGetMaxLifeManaFighter(nPoint)
	local nReturn = (1 + ( ( ( ( ( ( (nPoint - 30) ^ 3) + 25000) / 499) / 50) * 100) / 12)) * 45
	return nReturn
end

--type 35
function FN_LuaGetIncreaseBlockingFighter(nPoint)
	local nReturn = (1 + ( ( ( ( ( ( (nPoint - 30) ^ 3) + 25000) / 499) / 6) ) ) ) * 28
	return nReturn
end

--type 36
function FN_LuaGetIncreaseStatFighter(nPoint)
	local nReturn = (1 + ( ( ( ( ( ( (nPoint - 30) ^ 3) + 25000) / 499) / 6) ) ) ) * 8
	return nReturn
end

--type 37
function FN_LuaGetMaxAGFighter(nPoint)
	local nReturn = (1 + ( ( ( ( ( ( (nPoint - 30) ^ 3) + 25000) / 499) / 50) * 100) / 12) ) * 15
	return nReturn
end

--type 38
function FN_LuaGetPercent5(nPoint)
	local nReturn = (0.95 + ( ( ( ( (((nPoint - 30) ^ 3) + 25000) / 499) / 6)))) * 7.8
	return nReturn
end