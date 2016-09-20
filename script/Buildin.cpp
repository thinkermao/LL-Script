﻿#include "Buildin.h"

#include <cstring>

#include "VM.h"
#include "GC.h"

static inline bool IsCalable(Object self) 
{
	return IsFixnum(self);
}

Object Add(Object LHS, Object RHS)
{
	if (IsCalable(LHS) && IsCalable(RHS)) {
		return CreateFixnum(GetFixnum(LHS) + GetFixnum(RHS));
	}
	return CreateNil();
}

Object Sub(Object LHS, Object RHS)
{
	if (IsCalable(LHS) && IsCalable(RHS)) {
		return CreateFixnum(GetFixnum(LHS) - GetFixnum(RHS));
	}
	return CreateNil();
}

Object Mul(Object LHS, Object RHS)
{
	if (IsCalable(LHS) && IsCalable(RHS)) {
		return CreateFixnum(GetFixnum(LHS) * GetFixnum(RHS));
	}
	return CreateNil();
}

Object Div(Object LHS, Object RHS)
{
	if (IsCalable(LHS) && IsCalable(RHS)) {
		int right = GetFixnum(RHS);
		if (right == 0)
			return CreateNil();
		return CreateFixnum(GetFixnum(LHS) / GetFixnum(RHS));
	}
	return CreateNil();
}

Object Great(Object LHS, Object RHS)
{
	return CreateFixnum(ToFixnum(LHS) > ToFixnum(RHS));
}

Object Less(Object LHS, Object RHS)
{
	return CreateFixnum(ToFixnum(LHS) < ToFixnum(RHS));
}

Object NotGreat(Object LHS, Object RHS)
{
	return CreateFixnum(ToFixnum(LHS) <= ToFixnum(RHS));
}

Object NotLess(Object LHS, Object RHS)
{
	return CreateFixnum(ToFixnum(LHS) >= ToFixnum(RHS));
}

Object Equal(Object LHS, Object RHS)
{
	if (IsString(LHS) && IsString(RHS)) {
		return CreateFixnum(strcmp(StringGet(LHS), StringGet(RHS)) == 0);
	}
	return CreateFixnum(ToFixnum(LHS) == ToFixnum(RHS));
}

Object NotEqual(Object LHS, Object RHS)
{
	if (IsString(LHS) && IsString(RHS)) {
		return CreateFixnum(strcmp(StringGet(LHS), StringGet(RHS)) != 0);
	}
	return CreateFixnum(ToFixnum(LHS) == ToFixnum(RHS));
}

Object Not(Object LHS)
{
	return CreateFixnum(!ToLogicValue(LHS));
}

void ProcessGlobals(void *scene)
{
	using script::VMFrame;
	using script::VMScene;
	using script::GarbageCollector;

	VMScene *vmscene = static_cast<VMScene*>(scene);
	GarbageCollector *GC = &vmscene->GC;
	for (auto *frame : vmscene->frames) {
		for (size_t i = 0; i < frame->paramsNums; ++i)
			GC->processReference(&(frame->params[i]));
		for (size_t i = 0; i < frame->regNums; ++i)
			if (frame->registers[i])
				GC->processReference(&(frame->registers[i]));
	}
	for (auto &object : vmscene->paramsStack) 
		GC->processReference(&object);
}

void ProcessVariableReference(void *scene, Object *object)
{
	using script::VMScene;
	using script::GarbageCollector;

	VMScene *vmscene = static_cast<VMScene*>(scene);
	GarbageCollector *GC = &vmscene->GC;
	if (IsClosure(*object)) {
		size_t hold = ClosureHold(*object);
		Object *params = ClosureParams(*object);
		for (size_t idx = 0; idx < hold; ++idx)
			GC->processReference(&(params[idx]));
	}
}
