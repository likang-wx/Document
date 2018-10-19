#include <Parser.h>
#include "Util.h"

/***********************************************************************
Predefined Types
***********************************************************************/

ITsys* GetTsysFromCppType(Ptr<ITsysAlloc> tsys, const WString& cppType)
{
	ParsingArguments pa(nullptr, tsys, nullptr);
	CppTokenReader reader(GlobalCppLexer(), cppType);
	auto cursor = reader.GetFirstToken();
	auto type = ParseType(pa, cursor);
	TEST_ASSERT(!cursor);

	TypeTsysList types;
	TypeToTsys(pa, type, types);
	TEST_ASSERT(types.Count() == 1);
	return types[0];
}

template<typename> struct TsysInfo;

template<typename T>
struct TsysInfo<T const>
{
	static ITsys* GetTsys(Ptr<ITsysAlloc> tsys)
	{
		return TsysInfo<T>::GetTsys(tsys)->CVOf({ true,false });
	}
};

template<typename T>
struct TsysInfo<T*>
{
	static ITsys* GetTsys(Ptr<ITsysAlloc> tsys)
	{
		return TsysInfo<T>::GetTsys(tsys)->PtrOf();
	}
};

template<typename T>
struct TsysInfo<T&>
{
	static ITsys* GetTsys(Ptr<ITsysAlloc> tsys)
	{
		return TsysInfo<T>::GetTsys(tsys)->LRefOf();
	}
};

template<typename T>
struct TsysInfo<T&&>
{
	static ITsys* GetTsys(Ptr<ITsysAlloc> tsys)
	{
		return TsysInfo<T>::GetTsys(tsys)->RRefOf();
	}
};

#define DEFINE_TSYS(TYPE)\
template<> struct TsysInfo<TYPE>\
{\
	static ITsys* GetTsys(Ptr<ITsysAlloc> tsys)\
	{\
		ITsysAlloc* cachedTsys = nullptr;\
		ITsys* cachedType = nullptr;\
		if (cachedTsys != tsys.Obj())\
		{\
			cachedTsys = tsys.Obj();\
			cachedType = GetTsysFromCppType(tsys, L#TYPE);\
		}\
		return cachedType;\
	}\
}\

DEFINE_TSYS(bool);
DEFINE_TSYS(signed __int8);
DEFINE_TSYS(signed __int16);
DEFINE_TSYS(signed __int32);
DEFINE_TSYS(signed __int64);
DEFINE_TSYS(signed long);
DEFINE_TSYS(unsigned __int8);
DEFINE_TSYS(unsigned __int16);
DEFINE_TSYS(unsigned __int32);
DEFINE_TSYS(unsigned __int64);
DEFINE_TSYS(unsigned long);
DEFINE_TSYS(char);
DEFINE_TSYS(wchar_t);
DEFINE_TSYS(char16_t);
DEFINE_TSYS(char32_t);
DEFINE_TSYS(float);
DEFINE_TSYS(double);
DEFINE_TSYS(long double);

#undef DEFINE_TSYS

/***********************************************************************
Macros
***********************************************************************/

#define TEST_DECL(SOMETHING) SOMETHING auto input = L#SOMETHING

#define TEST_DECL_VARS								\
	TEST_DECL(										\
		bool						b		= 0;	\
		signed __int8				si8		= 0;	\
		signed __int16				si16	= 0;	\
		signed __int32				si32	= 0;	\
		signed __int64				si64	= 0;	\
		unsigned __int8				ui8		= 0;	\
		unsigned __int16			ui16	= 0;	\
		unsigned __int32			ui32	= 0;	\
		unsigned __int64			ui64	= 0;	\
		signed char					sc		= 0;	\
		unsigned char				uc		= 0;	\
		wchar_t						wc		= 0;	\
		char16_t					c16		= 0;	\
		char32_t					c32		= 0;	\
		float						f		= 0;	\
		double						d		= 0;	\
		long double					ld		= 0;	\
		bool				const	cb		= 0;	\
		signed __int8		const	csi8	= 0;	\
		signed __int16		const	csi16	= 0;	\
		signed __int32		const	csi32	= 0;	\
		signed __int64		const	csi64	= 0;	\
		unsigned __int8		const	cui8	= 0;	\
		unsigned __int16	const	cui16	= 0;	\
		unsigned __int32	const	cui32	= 0;	\
		unsigned __int64	const	cui64	= 0;	\
		signed char			const	csc		= 0;	\
		unsigned char		const	cuc		= 0;	\
		wchar_t				const	cwc		= 0;	\
		char16_t			const	cc16	= 0;	\
		char32_t			const	cc32	= 0;	\
		float				const	cf		= 0;	\
		double				const	cd		= 0;	\
		long double			const	cld		= 0;	\
		char*						ps		= 0;	\
		char*				const	cps		= 0;	\
		const char*					pcs		= 0;	\
		const char*			const	cpcs	= 0;	\
		char*						&rps	= ps;	\
		char*				const	&rcps	= cps;	\
		const char*					&rpcs	= pcs;	\
		const char*			const	&rcpcs	= cpcs;	\
	)

#define TEST_EACH_VAR_BOOL(F) F(b)
#define TEST_EACH_VAR_SINT(F) F(si8) F(si16) F(si32) F(si64)
#define TEST_EACH_VAR_UINT(F) F(ui8) F(ui16) F(ui32) F(ui64)
#define TEST_EACH_VAR_INT(F) TEST_EACH_VAR_SINT(F) TEST_EACH_VAR_UINT(F)
#define TEST_EACH_VAR_CHAR(F) F(sc) F(uc) F(wc) F(c16) F(c32)
#define TEST_EACH_VAR_FLOAT(F) F(f) F(d) F(ld)

#define TEST_EACH_VAR(F) TEST_EACH_VAR_BOOL(F) TEST_EACH_VAR_INT(F) TEST_EACH_VAR_CHAR(F) TEST_EACH_VAR_FLOAT(F)
#define TEST_EACH_VAR_NO_BOOL(F) TEST_EACH_VAR_INT(F) TEST_EACH_VAR_CHAR(F) TEST_EACH_VAR_FLOAT(F)
#define TEST_EACH_VAR_NO_BOOL_UNSIGNED(F) TEST_EACH_VAR_SINT(F) TEST_EACH_VAR_FLOAT(F)
#define TEST_EACH_VAR_NO_BOOL_FLOAT(F) TEST_EACH_VAR_INT(F) TEST_EACH_VAR_CHAR(F)
#define TEST_EACH_VAR_NO_FLOAT(F) TEST_EACH_VAR_BOOL(F) TEST_EACH_VAR_INT(F) TEST_EACH_VAR_CHAR(F)

#define TEST_EACH_VAR_CONST_BOOL(F) F(cb)
#define TEST_EACH_VAR_CONST_SINT(F) F(csi8) F(csi16) F(csi32) F(csi64)
#define TEST_EACH_VAR_CONST_UINT(F) F(cui8) F(cui16) F(cui32) F(cui64)
#define TEST_EACH_VAR_CONST_INT(F) TEST_EACH_VAR_CONST_SINT(F) TEST_EACH_VAR_CONST_UINT(F)
#define TEST_EACH_VAR_CONST_CHAR(F) F(csc) F(cuc) F(cwc) F(cc16) F(cc32)
#define TEST_EACH_VAR_CONST_FLOAT(F) F(cf) F(cd) F(cld)

#define TEST_EACH_VAR_CONST(F) TEST_EACH_VAR_CONST_BOOL(F) TEST_EACH_VAR_CONST_INT(F) TEST_EACH_VAR_CONST_CHAR(F) TEST_EACH_VAR_CONST_FLOAT(F)
#define TEST_EACH_VAR_CONST_NO_BOOL(F) TEST_EACH_VAR_CONST_INT(F) TEST_EACH_VAR_CONST_CHAR(F) TEST_EACH_VAR_CONST_FLOAT(F)
#define TEST_EACH_VAR_CONST_NO_BOOL_UNSIGNED(F) TEST_EACH_VAR_CONST_SINT(F) TEST_EACH_VAR_CONST_FLOAT(F)
#define TEST_EACH_VAR_CONST_NO_BOOL_FLOAT(F) TEST_EACH_VAR_CONST_INT(F) TEST_EACH_VAR_CONST_CHAR(F)
#define TEST_EACH_VAR_CONST_NO_FLOAT(F) TEST_EACH_VAR_CONST_BOOL(F) TEST_EACH_VAR_CONST_INT(F) TEST_EACH_VAR_CONST_CHAR(F)

#define TEST_EACH_VAR2_NO_FLOAT_(F, NAME1)\
	F(NAME1, cb)\
	F(NAME1, csi8) F(NAME1, csi16) F(NAME1, csi32) F(NAME1, csi64)\
	F(NAME1, cui8) F(NAME1, cui16) F(NAME1, cui32) F(NAME1, cui64)\
	F(NAME1, csc) F(NAME1, cuc) F(NAME1, cwc) F(NAME1, cc16) F(NAME1, cc32)\

#define TEST_EACH_VAR2_NO_FLOAT(F)\
	TEST_EACH_VAR2_NO_FLOAT_(F, cb)\
	TEST_EACH_VAR2_NO_FLOAT_(F, csi8) TEST_EACH_VAR2_NO_FLOAT_(F, csi16) TEST_EACH_VAR2_NO_FLOAT_(F, csi32) TEST_EACH_VAR2_NO_FLOAT_(F, csi64)\
	TEST_EACH_VAR2_NO_FLOAT_(F, cui8) TEST_EACH_VAR2_NO_FLOAT_(F, cui16) TEST_EACH_VAR2_NO_FLOAT_(F, cui32) TEST_EACH_VAR2_NO_FLOAT_(F, cui64)\
	TEST_EACH_VAR2_NO_FLOAT_(F, csc) TEST_EACH_VAR2_NO_FLOAT_(F, cuc) TEST_EACH_VAR2_NO_FLOAT_(F, cwc) TEST_EACH_VAR2_NO_FLOAT_(F, cc16) TEST_EACH_VAR2_NO_FLOAT_(F, cc32)\

#define TEST_EACH_VAR2_(F, NAME1)\
	F(NAME1, cb)\
	F(NAME1, csi8) F(NAME1, csi16) F(NAME1, csi32) F(NAME1, csi64)\
	F(NAME1, cui8) F(NAME1, cui16) F(NAME1, cui32) F(NAME1, cui64)\
	F(NAME1, csc) F(NAME1, cuc) F(NAME1, cwc) F(NAME1, cc16) F(NAME1, cc32)\
	F(NAME1, cf) F(NAME1, cd) F(NAME1, cld)\

#define TEST_EACH_VAR2(F)\
	TEST_EACH_VAR2_(F, cb)\
	TEST_EACH_VAR2_(F, csi8) TEST_EACH_VAR2_(F, csi16) TEST_EACH_VAR2_(F, csi32) TEST_EACH_VAR2_(F, csi64)\
	TEST_EACH_VAR2_(F, cui8) TEST_EACH_VAR2_(F, cui16) TEST_EACH_VAR2_(F, cui32) TEST_EACH_VAR2_(F, cui64)\
	TEST_EACH_VAR2_(F, csc) TEST_EACH_VAR2_(F, cuc) TEST_EACH_VAR2_(F, cwc) TEST_EACH_VAR2_(F, cc16) TEST_EACH_VAR2_(F, cc32)\
	TEST_EACH_VAR2_(F, f) TEST_EACH_VAR2_(F, d) TEST_EACH_VAR2_(F, ld)\

#define TEST_EACH_VAR_PTR_(F, NAME)\
	F(ps, NAME) F(pcs, NAME) F(cps, NAME) F(cpcs, NAME)\
	F(rps, NAME) F(rpcs, NAME) F(rcps, NAME) F(rcpcs, NAME)\

#define TEST_EACH_VAR_PTR(F)\
	TEST_EACH_VAR_PTR_(F, ps) TEST_EACH_VAR_PTR_(F, pcs) TEST_EACH_VAR_PTR_(F, cps) TEST_EACH_VAR_PTR_(F, cpcs)\
	TEST_EACH_VAR_PTR_(F, rps) TEST_EACH_VAR_PTR_(F, rpcs) TEST_EACH_VAR_PTR_(F, rcps) TEST_EACH_VAR_PTR_(F, rcpcs)\

/***********************************************************************
Test Cases
***********************************************************************/

#pragma warning (push)
#pragma warning (disable: 4101)
#pragma warning (disable: 4804)

WString TsysToString(ITsys* tsys)
{
	auto logTsys = GenerateToStream([&](StreamWriter& writer) { Log(tsys, writer); });

	if (tsys->GetType() == TsysType::LRef)
	{
		logTsys += L" $L";
	}
	else if (tsys->GetType() == TsysType::RRef)
	{
		logTsys += L" $X";
	}
	else
	{
		logTsys += L" $PR";
	}
	return logTsys;
}

template<typename T>
void AssertPostfixUnary(ParsingArguments& pa, const WString& name, const WString& op)
{
	auto input = name + op;
	auto log = L"(" + name + L" " + op + L")";
	auto tsys = TsysInfo<T>::GetTsys(pa.tsys);
	AssertExpr(input, log, TsysToString(tsys), pa);
}

TEST_CASE(TestIntegralPromotion_PostfixUnary)
{
	TEST_DECL_VARS;
	COMPILE_PROGRAM(program, pa, input);

#define TEST_VAR(NAME) AssertPostfixUnary<decltype((NAME++))>(pa, L#NAME, L"++");
	TEST_EACH_VAR(TEST_VAR)
#undef TEST_VAR

#define TEST_VAR(NAME) AssertPostfixUnary<decltype((NAME--))>(pa, L#NAME, L"--");
	TEST_EACH_VAR_NO_BOOL(TEST_VAR)
#undef TEST_VAR
}

template<typename T>
void AssertPrefixUnary(ParsingArguments& pa, const WString& name, const WString& op)
{
	auto input = op + name;
	auto log = L"(" + op + L" " + name + L")";
	auto tsys = TsysInfo<T>::GetTsys(pa.tsys);
	AssertExpr(input, log, TsysToString(tsys), pa);
}

template<typename T>
void AssertDereferenceUnary(ParsingArguments& pa, const WString& name)
{
	auto input = L"*&" + name;
	auto log = L"(* (& " + name + L"))";
	auto tsys = TsysInfo<T>::GetTsys(pa.tsys);
	AssertExpr(input, log, TsysToString(tsys), pa);
}

TEST_CASE(TestIntegralPromotion_PrefixUnary)
{
	TEST_DECL_VARS;
	COMPILE_PROGRAM(program, pa, input);

#define TEST_VAR(NAME) AssertPrefixUnary<decltype((++NAME))>(pa, L#NAME, L"++");
	TEST_EACH_VAR(TEST_VAR)
#undef TEST_VAR

#define TEST_VAR(NAME) AssertPrefixUnary<decltype((--NAME))>(pa, L#NAME, L"--");
	TEST_EACH_VAR_NO_BOOL(TEST_VAR)
#undef TEST_VAR

#define TEST_VAR(NAME) AssertPrefixUnary<decltype((~NAME))>(pa, L#NAME, L"~");
	TEST_EACH_VAR_NO_BOOL_FLOAT(TEST_VAR)
	TEST_EACH_VAR_CONST_NO_BOOL_FLOAT(TEST_VAR)
#undef TEST_VAR

#define TEST_VAR(NAME) AssertPrefixUnary<decltype((!NAME))>(pa, L#NAME, L"!");
	TEST_EACH_VAR(TEST_VAR)
	TEST_EACH_VAR_CONST(TEST_VAR)
#undef TEST_VAR

#define TEST_VAR(NAME) AssertPrefixUnary<decltype((-NAME))>(pa, L#NAME, L"-");
	TEST_EACH_VAR_NO_BOOL_UNSIGNED(TEST_VAR)
	TEST_EACH_VAR_CONST_NO_BOOL_UNSIGNED(TEST_VAR)
#undef TEST_VAR

#define TEST_VAR(NAME) AssertPrefixUnary<decltype((+NAME))>(pa, L#NAME, L"+");
	TEST_EACH_VAR(TEST_VAR)
	TEST_EACH_VAR_CONST(TEST_VAR)
#undef TEST_VAR

#define TEST_VAR(NAME) AssertPrefixUnary<decltype((&NAME))>(pa, L#NAME, L"&");
	TEST_EACH_VAR(TEST_VAR)
	TEST_EACH_VAR_CONST(TEST_VAR)
#undef TEST_VAR

#define TEST_VAR(NAME) AssertDereferenceUnary<decltype((*&NAME))>(pa, L#NAME);
	TEST_EACH_VAR(TEST_VAR)
	TEST_EACH_VAR_CONST(TEST_VAR)
#undef TEST_VAR
}

template<typename T>
void AssertBinary(ParsingArguments& pa, const WString& name1, const WString& name2, const WString& op)
{
	auto input = name1 + op + name2;
	auto log = L"(" + name1 + L" " + op + L" " + name2 + L")";
	auto tsys = TsysInfo<T>::GetTsys(pa.tsys);
	AssertExpr(input, log, TsysToString(tsys), pa);
}

TEST_CASE(TestIntegralPromotion_BinaryIntOp)
{
	TEST_DECL_VARS;
	COMPILE_PROGRAM(program, pa, input);

#define TEST_VAR(NAME1, NAME2) AssertBinary<decltype((NAME1&&NAME2))>(pa, L#NAME1, L#NAME2, L"&&");
	TEST_EACH_VAR2_NO_FLOAT(TEST_VAR)
#undef TEST_VAR

#define TEST_VAR(NAME1, NAME2) AssertBinary<decltype((NAME1||NAME2))>(pa, L#NAME1, L#NAME2, L"||");
	TEST_EACH_VAR2_NO_FLOAT(TEST_VAR)
#undef TEST_VAR

#define TEST_VAR(NAME1, NAME2) AssertBinary<decltype((NAME1&NAME2))>(pa, L#NAME1, L#NAME2, L"&");
	TEST_EACH_VAR2_NO_FLOAT(TEST_VAR)
#undef TEST_VAR

#define TEST_VAR(NAME1, NAME2) AssertBinary<decltype((NAME1|NAME2))>(pa, L#NAME1, L#NAME2, L"|");
	TEST_EACH_VAR2_NO_FLOAT(TEST_VAR)
#undef TEST_VAR

#define TEST_VAR(NAME1, NAME2) AssertBinary<decltype((NAME1^NAME2))>(pa, L#NAME1, L#NAME2, L"^");
	TEST_EACH_VAR2_NO_FLOAT(TEST_VAR)
#undef TEST_VAR

#define TEST_VAR(NAME1, NAME2) AssertBinary<decltype((NAME1<<NAME2))>(pa, L#NAME1, L#NAME2, L"<<");
	TEST_EACH_VAR2_NO_FLOAT(TEST_VAR)
#undef TEST_VAR

#define TEST_VAR(NAME1, NAME2) AssertBinary<decltype((NAME1>>NAME2))>(pa, L#NAME1, L#NAME2, L">>");
	TEST_EACH_VAR2_NO_FLOAT(TEST_VAR)
#undef TEST_VAR

#define TEST_VAR(NAME1, NAME2) AssertBinary<decltype((NAME1%NAME2))>(pa, L#NAME1, L#NAME2, L"%");
	TEST_EACH_VAR2_NO_FLOAT(TEST_VAR)
#undef TEST_VAR
}

TEST_CASE(TestIntegralPromotion_BinaryNumeric)
{
	TEST_DECL_VARS;
	COMPILE_PROGRAM(program, pa, input);

#define TEST_VAR(NAME1, NAME2) AssertBinary<decltype((NAME1+NAME2))>(pa, L#NAME1, L#NAME2, L"+");
	TEST_EACH_VAR2(TEST_VAR)
#undef TEST_VAR

#define TEST_VAR(NAME1, NAME2) AssertBinary<decltype((NAME1-NAME2))>(pa, L#NAME1, L#NAME2, L"-");
	TEST_EACH_VAR2(TEST_VAR)
#undef TEST_VAR

#define TEST_VAR(NAME1, NAME2) AssertBinary<decltype((NAME1*NAME2))>(pa, L#NAME1, L#NAME2, L"*");
	TEST_EACH_VAR2(TEST_VAR)
#undef TEST_VAR

#define TEST_VAR(NAME1, NAME2) AssertBinary<decltype((NAME1/NAME2))>(pa, L#NAME1, L#NAME2, L"/");
	TEST_EACH_VAR2(TEST_VAR)
#undef TEST_VAR
}

TEST_CASE(TestIntegralPromotion_Comparison)
{
	TEST_DECL_VARS;
	COMPILE_PROGRAM(program, pa, input);

#define TEST_VAR(NAME) AssertBinary<decltype((NAME==NAME))>(pa, L#NAME, L#NAME, L"==");
	TEST_EACH_VAR(TEST_VAR)
	TEST_EACH_VAR_CONST(TEST_VAR)
#undef TEST_VAR

#define TEST_VAR(NAME) AssertBinary<decltype((NAME!=NAME))>(pa, L#NAME, L#NAME, L"!=");
	TEST_EACH_VAR(TEST_VAR)
	TEST_EACH_VAR_CONST(TEST_VAR)
#undef TEST_VAR

#define TEST_VAR(NAME) AssertBinary<decltype((NAME<NAME))>(pa, L#NAME, L#NAME, L"<");
	TEST_EACH_VAR(TEST_VAR)
	TEST_EACH_VAR_CONST(TEST_VAR)
#undef TEST_VAR

#define TEST_VAR(NAME) AssertBinary<decltype((NAME<=NAME))>(pa, L#NAME, L#NAME, L"<=");
	TEST_EACH_VAR(TEST_VAR)
	TEST_EACH_VAR_CONST(TEST_VAR)
#undef TEST_VAR

#define TEST_VAR(NAME) AssertBinary<decltype((NAME>NAME))>(pa, L#NAME, L#NAME, L">");
	TEST_EACH_VAR(TEST_VAR)
	TEST_EACH_VAR_CONST(TEST_VAR)
#undef TEST_VAR

#define TEST_VAR(NAME) AssertBinary<decltype((NAME>=NAME))>(pa, L#NAME, L#NAME, L">=");
	TEST_EACH_VAR(TEST_VAR)
	TEST_EACH_VAR_CONST(TEST_VAR)
#undef TEST_VAR
}

TEST_CASE(TestIntegralPromotion_Assignment)
{
	TEST_DECL_VARS;
	COMPILE_PROGRAM(program, pa, input);

#define TEST_VAR(NAME) AssertBinary<decltype((NAME=NAME))>(pa, L#NAME, L#NAME, L"=");
	TEST_EACH_VAR(TEST_VAR)
#undef TEST_VAR

#define TEST_VAR(NAME) AssertBinary<decltype((NAME*=NAME))>(pa, L#NAME, L#NAME, L"*=");
	TEST_EACH_VAR_NO_BOOL(TEST_VAR)
#undef TEST_VAR

#define TEST_VAR(NAME) AssertBinary<decltype((NAME/=NAME))>(pa, L#NAME, L#NAME, L"/=");
	TEST_EACH_VAR_NO_BOOL(TEST_VAR)
#undef TEST_VAR

#define TEST_VAR(NAME) AssertBinary<decltype((NAME%=NAME))>(pa, L#NAME, L#NAME, L"%=");
	TEST_EACH_VAR_NO_BOOL_FLOAT(TEST_VAR)
#undef TEST_VAR

#define TEST_VAR(NAME) AssertBinary<decltype((NAME+=NAME))>(pa, L#NAME, L#NAME, L"+=");
	TEST_EACH_VAR_NO_BOOL(TEST_VAR)
#undef TEST_VAR

#define TEST_VAR(NAME) AssertBinary<decltype((NAME-=NAME))>(pa, L#NAME, L#NAME, L"-=");
	TEST_EACH_VAR_NO_BOOL(TEST_VAR)
#undef TEST_VAR

#define TEST_VAR(NAME) AssertBinary<decltype((NAME<<=NAME))>(pa, L#NAME, L#NAME, L"<<=");
	TEST_EACH_VAR_NO_BOOL_FLOAT(TEST_VAR)
#undef TEST_VAR

#define TEST_VAR(NAME) AssertBinary<decltype((NAME>>=NAME))>(pa, L#NAME, L#NAME, L">>=");
	TEST_EACH_VAR_NO_BOOL_FLOAT(TEST_VAR)
#undef TEST_VAR

#define TEST_VAR(NAME) AssertBinary<decltype((NAME&=NAME))>(pa, L#NAME, L#NAME, L"&=");
	TEST_EACH_VAR_NO_FLOAT(TEST_VAR)
#undef TEST_VAR

#define TEST_VAR(NAME) AssertBinary<decltype((NAME|=NAME))>(pa, L#NAME, L#NAME, L"|=");
	TEST_EACH_VAR_NO_FLOAT(TEST_VAR)
#undef TEST_VAR

#define TEST_VAR(NAME) AssertBinary<decltype((NAME^=NAME))>(pa, L#NAME, L#NAME, L"^=");
	TEST_EACH_VAR_NO_FLOAT(TEST_VAR)
#undef TEST_VAR
}

TEST_CASE(TestPointerArithmetic_Postfix)
{
	TEST_DECL_VARS;
	COMPILE_PROGRAM(program, pa, input);

#define TEST_VAR(NAME)\
	AssertPostfixUnary<decltype((ps++))>(pa, L"ps", L"++");\
	AssertPostfixUnary<decltype((pcs++))>(pa, L"pcs", L"++");\
	AssertPostfixUnary<decltype((rps++))>(pa, L"rps", L"++");\
	AssertPostfixUnary<decltype((rpcs++))>(pa, L"rpcs", L"++");\

	TEST_EACH_VAR_NO_FLOAT(TEST_VAR)
#undef TEST_VAR

#define TEST_VAR(NAME)\
	AssertPostfixUnary<decltype((ps--))>(pa, L"ps", L"--");\
	AssertPostfixUnary<decltype((pcs--))>(pa, L"pcs", L"--");\
	AssertPostfixUnary<decltype((rps--))>(pa, L"rps", L"--");\
	AssertPostfixUnary<decltype((rpcs--))>(pa, L"rpcs", L"--");\

	TEST_EACH_VAR_NO_FLOAT(TEST_VAR)
#undef TEST_VAR
}

TEST_CASE(TestPointerArithmetic_Prefix)
{
	TEST_DECL_VARS;
	COMPILE_PROGRAM(program, pa, input);

#define TEST_VAR(NAME)\
	AssertPrefixUnary<decltype((++ps))>(pa, L"ps", L"++");\
	AssertPrefixUnary<decltype((++pcs))>(pa, L"pcs", L"++");\
	AssertPrefixUnary<decltype((++rps))>(pa, L"rps", L"++");\
	AssertPrefixUnary<decltype((++rpcs))>(pa, L"rpcs", L"++");\

	TEST_EACH_VAR_NO_FLOAT(TEST_VAR)
#undef TEST_VAR

#define TEST_VAR(NAME)\
	AssertPrefixUnary<decltype((--ps))>(pa, L"ps", L"--");\
	AssertPrefixUnary<decltype((--pcs))>(pa, L"pcs", L"--");\
	AssertPrefixUnary<decltype((--rps))>(pa, L"rps", L"--");\
	AssertPrefixUnary<decltype((--rpcs))>(pa, L"rpcs", L"--");\

	TEST_EACH_VAR_NO_FLOAT(TEST_VAR)
#undef TEST_VAR
}

TEST_CASE(TestPointerArithmetic_PI)
{
	TEST_DECL_VARS;
	COMPILE_PROGRAM(program, pa, input);

#define TEST_VAR(NAME)\
	AssertBinary<decltype((ps+NAME))>(pa, L"ps", L#NAME, L"+");\
	AssertBinary<decltype((pcs+NAME))>(pa, L"pcs", L#NAME, L"+");\
	AssertBinary<decltype((cps+NAME))>(pa, L"cps", L#NAME, L"+");\
	AssertBinary<decltype((cpcs+NAME))>(pa, L"cpcs", L#NAME, L"+");\
	AssertBinary<decltype((rps+NAME))>(pa, L"rps", L#NAME, L"+");\
	AssertBinary<decltype((rpcs+NAME))>(pa, L"rpcs", L#NAME, L"+");\
	AssertBinary<decltype((rcps+NAME))>(pa, L"rcps", L#NAME, L"+");\
	AssertBinary<decltype((rcpcs+NAME))>(pa, L"rcpcs", L#NAME, L"+");\

	TEST_EACH_VAR_NO_FLOAT(TEST_VAR)
#undef TEST_VAR

#define TEST_VAR(NAME)\
	AssertBinary<decltype((ps-NAME))>(pa, L"ps", L#NAME, L"-");\
	AssertBinary<decltype((pcs-NAME))>(pa, L"pcs", L#NAME, L"-");\
	AssertBinary<decltype((cps-NAME))>(pa, L"cps", L#NAME, L"-");\
	AssertBinary<decltype((cpcs-NAME))>(pa, L"cpcs", L#NAME, L"-");\
	AssertBinary<decltype((rps-NAME))>(pa, L"rps", L#NAME, L"-");\
	AssertBinary<decltype((rpcs-NAME))>(pa, L"rpcs", L#NAME, L"-");\
	AssertBinary<decltype((rcps-NAME))>(pa, L"rcps", L#NAME, L"-");\
	AssertBinary<decltype((rcpcs-NAME))>(pa, L"rcpcs", L#NAME, L"-");\

	TEST_EACH_VAR_NO_FLOAT(TEST_VAR)
#undef TEST_VAR
}

TEST_CASE(TestPointerArithmetic_IP)
{
	TEST_DECL_VARS;
	COMPILE_PROGRAM(program, pa, input);

#define TEST_VAR(NAME)\
	AssertBinary<decltype((NAME+ps))>(pa, L#NAME, L"ps", L"+");\
	AssertBinary<decltype((NAME+pcs))>(pa, L#NAME, L"pcs", L"+");\
	AssertBinary<decltype((NAME+cps))>(pa, L#NAME, L"cps", L"+");\
	AssertBinary<decltype((NAME+cpcs))>(pa, L#NAME, L"cpcs", L"+");\
	AssertBinary<decltype((NAME+rps))>(pa, L#NAME, L"rps", L"+");\
	AssertBinary<decltype((NAME+rpcs))>(pa, L#NAME, L"rpcs", L"+");\
	AssertBinary<decltype((NAME+rcps))>(pa, L#NAME, L"rcps", L"+");\
	AssertBinary<decltype((NAME+rcpcs))>(pa, L#NAME, L"rcpcs", L"+");\

	TEST_EACH_VAR_NO_FLOAT(TEST_VAR)
#undef TEST_VAR
}

TEST_CASE(TestPointerArithmetic_PP)
{
	TEST_DECL_VARS;
	COMPILE_PROGRAM(program, pa, input);

#define TEST_VAR(NAME1, NAME2)\
	AssertBinary<decltype((NAME1-NAME2))>(pa, L#NAME1, L#NAME2, L"-");\

	TEST_EACH_VAR_PTR(TEST_VAR)
#undef TEST_VAR
}

#undef TEST_EACH_VAR_BOOL
#undef TEST_EACH_VAR_SINT
#undef TEST_EACH_VAR_UINT
#undef TEST_EACH_VAR_INT
#undef TEST_EACH_VAR_CHAR
#undef TEST_EACH_VAR_FLOAT
#undef TEST_EACH_VAR_NO_BOOL
#undef TEST_EACH_VAR_NO_BOOL_UNSIGNED
#undef TEST_EACH_VAR_NO_BOOL_FLOAT
#undef TEST_EACH_VAR_NO_FLOAT
#undef TEST_EACH_VAR

#undef TEST_EACH_VAR_CONST_BOOL
#undef TEST_EACH_VAR_CONST_SINT
#undef TEST_EACH_VAR_CONST_UINT
#undef TEST_EACH_VAR_CONST_INT
#undef TEST_EACH_VAR_CONST_CHAR
#undef TEST_EACH_VAR_CONST_FLOAT
#undef TEST_EACH_VAR_CONST_NO_BOOL
#undef TEST_EACH_VAR_CONST_NO_BOOL_UNSIGNED
#undef TEST_EACH_VAR_CONST_NO_BOOL_FLOAT
#undef TEST_EACH_VAR_CONST_NO_FLOAT
#undef TEST_EACH_VAR_CONST

#undef TEST_EACH_VAR2_NO_FLOAT_
#undef TEST_EACH_VAR2_NO_FLOAT
#undef TEST_EACH_VAR2_
#undef TEST_EACH_VAR2
#undef TEST_EACH_VAR_PTR_
#undef TEST_EACH_VAR_PTR

#undef TEST_DECL_VARS
#undef TEST_DECL

#pragma warning (push)