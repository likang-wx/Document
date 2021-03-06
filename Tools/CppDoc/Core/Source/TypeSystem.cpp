#include "TypeSystem.h"

class TsysAlloc;

#define DEFINE_TSYS_TYPE(NAME) class ITsys_##NAME;
TSYS_TYPE_LIST(DEFINE_TSYS_TYPE)
#undef DEFINE_TSYS_TYPE

template<typename T, vint BlockSize>
class ITsys_Allocator;

/***********************************************************************
TsysBase
***********************************************************************/

template<typename TType, typename TData>
struct WithParams
{
	IEnumerable<ITsys*>*	params;
	TType*					itsys;
	TData					data;

	static vint Compare(WithParams<TType, TData> a, WithParams<TType, TData> b)
	{
		vint result = TData::Compare(a.data, b.data);
		if (result != 0) return result;
		return CompareEnumerable(*a.params, *b.params);
	}
};
#define OPERATOR_COMPARE(OP)\
	template<typename TType, typename TData>\
	bool operator OP(WithParams<TType, TData> a, WithParams<TType, TData> b)\
	{\
		return WithParams<TType, TData>::Compare(a, b) OP 0;\
	}\

OPERATOR_COMPARE(>)
OPERATOR_COMPARE(>=)
OPERATOR_COMPARE(<)
OPERATOR_COMPARE(<=)
OPERATOR_COMPARE(==)
OPERATOR_COMPARE(!=)
#undef OPERATOR_COMPARE

namespace vl
{
	template<typename TType, typename TData>
	struct POD<WithParams<TType, TData>>
	{
		static const bool Result = true;
	};
}

class TsysBase : public ITsys
{
	template<typename TType, typename TData>
	using WithParamsList = SortedList<WithParams<TType, TData>>;

#define DEFINE_TSYS_TYPE(NAME) friend class ITsys_##NAME;
	TSYS_TYPE_LIST(DEFINE_TSYS_TYPE)
#undef DEFINE_TSYS_TYPE
protected:
	TsysAlloc*										tsys;
	ITsys_LRef*										lrefOf = nullptr;
	ITsys_RRef*										rrefOf = nullptr;
	ITsys_Ptr*										ptrOf = nullptr;
	Dictionary<vint, ITsys_Array*>					arrayOf;
	Dictionary<ITsys*, ITsys_Member*>				memberOf;
	ITsys_CV*										cvOf[3] = { 0 };
	WithParamsList<ITsys_Function, TsysFunc>		functionOf;
	WithParamsList<ITsys_Generic, TsysGeneric>		genericOf;

	template<typename TType, typename TData, vint BlockSize>
	ITsys* ParamsOf(IEnumerable<ITsys*>& params, const TData& data, WithParamsList<TType, TData>& paramsOf, ITsys_Allocator<TType, BlockSize> TsysAlloc::* alloc);

	virtual ITsys* GetEntityInternal(TsysCV& cv, TsysRefType& refType)
	{
		return this;
	}
public:
	TsysBase(TsysAlloc* _tsys) :tsys(_tsys) {}

	TsysPrimitive		GetPrimitive()								{ throw "Not Implemented!"; }
	TsysCV				GetCV()										{ throw "Not Implemented!"; }
	ITsys*				GetElement()								{ throw "Not Implemented!"; }
	ITsys*				GetClass()									{ throw "Not Implemented!"; }
	ITsys*				GetParam(vint index)						{ throw "Not Implemented!"; }
	vint				GetParamCount()								{ throw "Not Implemented!"; }
	TsysFunc			GetFunc()									{ throw "Not Implemented!"; }
	TsysGeneric			GetGeneric()								{ throw "Not Implemented!"; }
	Symbol*				GetDecl()									{ throw "Not Implemented!"; }

	ITsys* LRefOf()													override;
	ITsys* RRefOf()													override;
	ITsys* PtrOf()													override;
	ITsys* ArrayOf(vint dimensions)									override;
	ITsys* FunctionOf(IEnumerable<ITsys*>& params, TsysFunc func)	override;
	ITsys* MemberOf(ITsys* classType)								override;
	ITsys* CVOf(TsysCV cv)											override;
	ITsys* GenericOf(IEnumerable<ITsys*>& params)					override;

	ITsys* GetEntity(TsysCV& cv, TsysRefType& refType)override
	{
		cv = { false,false };
		refType = TsysRefType::None;
		return GetEntityInternal(cv, refType);
	}
};

template<TsysType Type>
class TsysBase_ : public TsysBase
{
public:
	TsysBase_(TsysAlloc* _tsys) :TsysBase(_tsys) {}

	TsysType GetType()override { return Type; }
};

/***********************************************************************
Concrete Tsys
***********************************************************************/

#define ITSYS_CLASS(TYPE) ITsys_##TYPE : public TsysBase_<TsysType::TYPE>

#define ITSYS_MEMBERS_MINIMIZED(TYPE)																\
	public:																							\
		ITsys_##TYPE(TsysAlloc* _tsys) :TsysBase_(_tsys) {}											\

#define ITSYS_MEMBERS_DATA(TYPE, DATA, NAME)														\
	protected:																						\
		DATA				data;																	\
	public:																							\
		ITsys_##TYPE(TsysAlloc* _tsys, DATA _data) :TsysBase_(_tsys), data(_data) {}				\
		DATA Get##NAME()override { return data; }													\

#define ITSYS_MEMBERS_REF(TYPE)																		\
	protected:																						\
		TsysBase*			element;																\
	public:																							\
		ITsys_##TYPE(TsysAlloc* _tsys, TsysBase* _element) :TsysBase_(_tsys), element(_element) {}	\
		ITsys* GetElement()override { return element; }												\

#define ITSYS_MEMBERS_DECORATE(TYPE, DATA, NAME)													\
	protected:																						\
		TsysBase*			element;																\
		DATA				data;																	\
	public:																							\
		ITsys_##TYPE(TsysAlloc* _tsys, TsysBase* _element, DATA _data)								\
			:TsysBase_(_tsys), element(_element), data(_data) {}									\
		ITsys* GetElement()override { return element; }												\
		DATA Get##NAME()override { return data; }													\

#define ITSYS_MEMBERS_WITHPARAMS(TYPE, DATA, NAME)													\
	protected:																						\
		TsysBase*			element;																\
		List<ITsys*>		params;																	\
		DATA				data;																	\
	public:																							\
		ITsys_##TYPE(TsysAlloc* _tsys, TsysBase* _element, DATA _data)								\
			:TsysBase_(_tsys), element(_element), data(_data) {}									\
		List<ITsys*>& GetParams() { return params; }												\
		ITsys* GetElement()override { return element; }												\
		ITsys* GetParam(vint index)override { return params.Get(index); }							\
		vint GetParamCount()override { return params.Count(); }										\
		DATA Get##NAME()override { return data; }													\

class ITSYS_CLASS(Zero)
{
	ITSYS_MEMBERS_MINIMIZED(Zero)

	ITsys* LRefOf()override
	{
		return this;
	}

	ITsys* RRefOf()override
	{
		return this;
	}

	ITsys* CVOf(TsysCV cv)override
	{
		return this;
	}
};

class ITSYS_CLASS(Nullptr)
{
	ITSYS_MEMBERS_MINIMIZED(Nullptr)

	ITsys* LRefOf()override
	{
		return this;
	}

	ITsys* RRefOf()override
	{
		return this;
	}

	ITsys* CVOf(TsysCV cv)override
	{
		return this;
	}
};

class ITSYS_CLASS(Primitive)
{
	ITSYS_MEMBERS_DATA(Primitive, TsysPrimitive, Primitive)
};

class ITSYS_CLASS(Decl)
{
	ITSYS_MEMBERS_DATA(Decl, Symbol*, Decl)
};

class ITSYS_CLASS(GenericArg)
{
	ITSYS_MEMBERS_DATA(GenericArg, Symbol*, Decl)
};

class ITSYS_CLASS(LRef)
{
	ITSYS_MEMBERS_REF(LRef)

	ITsys* LRefOf()override
	{
		return this;
	}

	ITsys* RRefOf()override
	{
		return this;
	}

	ITsys* CVOf(TsysCV cv)override
	{
		return this;
	}
protected:
	ITsys* GetEntityInternal(TsysCV& cv, TsysRefType& refType)override
	{
		refType = TsysRefType::LRef;
		return element->GetEntityInternal(cv, refType);
	}
};

class ITSYS_CLASS(RRef)
{
	ITSYS_MEMBERS_REF(RRef)

	ITsys* LRefOf()override
	{
		return element->LRefOf();
	}

	ITsys* RRefOf()override
	{
		return this;
	}

	ITsys* CVOf(TsysCV cv)override
	{
		return this;
	}
protected:
	ITsys* GetEntityInternal(TsysCV& cv, TsysRefType& refType)override
	{
		refType = TsysRefType::RRef;
		return element->GetEntityInternal(cv, refType);
	}
};

class ITSYS_CLASS(Ptr)
{
	ITSYS_MEMBERS_REF(Ptr)
};

class ITSYS_CLASS(Array)
{
	ITSYS_MEMBERS_DECORATE(Array, vint, ParamCount)
};

class ITSYS_CLASS(CV)
{
	ITSYS_MEMBERS_DECORATE(CV, TsysCV, CV)

	ITsys* CVOf(TsysCV cv)override
	{
		cv.isGeneralConst |= data.isGeneralConst;
		cv.isVolatile |= data.isVolatile;
		return element->CVOf(cv);
	}
protected:
	ITsys* GetEntityInternal(TsysCV& cv, TsysRefType& refType)override
	{
		cv = data;
		return element->GetEntityInternal(cv, refType);
	}
};

class ITSYS_CLASS(Member)
{
	ITSYS_MEMBERS_DECORATE(Member, ITsys*, Class)
};

class ITSYS_CLASS(Function)
{
	ITSYS_MEMBERS_WITHPARAMS(Function, TsysFunc, Func)
};

class ITSYS_CLASS(Generic)
{
	ITSYS_MEMBERS_WITHPARAMS(Generic, TsysGeneric, Generic)
};


#undef ITSYS_MEMBERS_DATA
#undef ITSYS_MEMBERS_REF
#undef ITSYS_MEMBERS_DECORATE
#undef ITSYS_MEMBERS_WITHPARAMS
#undef ITSYS_CLASS

class ITsys_Expr : TsysBase_<TsysType::Expr>
{
public:
	ITsys_Expr(TsysAlloc* _tsys)
		:TsysBase_(_tsys)
	{
	}
};

/***********************************************************************
ITsys_Allocator
***********************************************************************/

template<typename T, vint BlockSize>
class ITsys_Allocator : public Object
{
protected:
	struct Node
	{
		char				items[BlockSize * sizeof(T)];
		vint				used = 0;
		Node*				next = nullptr;

		~Node()
		{
			auto itsys = (T*)items;
			for (vint i = 0; i < used; i++)
			{
				itsys[i].~T();
			}
		}
	};

	Node*					firstNode = nullptr;
	Node*					lastNode = nullptr;
public:

	~ITsys_Allocator()
	{
		auto current = firstNode;
		while (current)
		{
			auto next = current->next;
			delete current;
			current = next;
		}
	}

	template<typename ...TArgs>
	T* Alloc(TArgs ...args)
	{
		if (!firstNode)
		{
			firstNode = lastNode = new Node;
		}

		if (lastNode->used == BlockSize)
		{
			lastNode->next = new Node;
			lastNode = lastNode->next;
		}

		auto itsys = &((T*)lastNode->items)[lastNode->used++];
#ifdef VCZH_CHECK_MEMORY_LEAKS_NEW
#undef new
#endif
		return new(itsys)T(args...);
#ifdef VCZH_CHECK_MEMORY_LEAKS_NEW
#define new VCZH_CHECK_MEMORY_LEAKS_NEW
#endif
	}
};

/***********************************************************************
ITsysAlloc
***********************************************************************/

class TsysAlloc : public Object, public ITsysAlloc
{
protected:
	ITsys_Zero										tsysZero;
	ITsys_Nullptr									tsysNullptr;
	ITsys_Primitive*								primitives[(vint)TsysPrimitiveType::_COUNT * (vint)TsysBytes::_COUNT] = { 0 };
	Dictionary<Symbol*, ITsys_Decl*>				decls;
	Dictionary<Symbol*, ITsys_GenericArg*>			genericArgs;

public:
	ITsys_Allocator<ITsys_Primitive,	1024>		_primitive;
	ITsys_Allocator<ITsys_LRef,			1024>		_lref;
	ITsys_Allocator<ITsys_RRef,			1024>		_rref;
	ITsys_Allocator<ITsys_Ptr,			1024>		_ptr;
	ITsys_Allocator<ITsys_Array,		1024>		_array;
	ITsys_Allocator<ITsys_Function,		1024>		_function;
	ITsys_Allocator<ITsys_Member,		1024>		_member;
	ITsys_Allocator<ITsys_CV,			1024>		_cv;
	ITsys_Allocator<ITsys_Decl,			1024>		_decl;
	ITsys_Allocator<ITsys_Generic,		1024>		_generic;
	ITsys_Allocator<ITsys_GenericArg,	1024>		_genericArg;
	ITsys_Allocator<ITsys_Expr,			1024>		_expr;

	TsysAlloc()
		:tsysZero(this)
		, tsysNullptr(this)
	{
	}

	ITsys* Zero()override
	{
		return &tsysZero;
	}

	ITsys* Nullptr()override
	{
		return &tsysNullptr;
	}

	ITsys* Int()override
	{
		// TODO: Platform Specific
		return PrimitiveOf({ TsysPrimitiveType::SInt,TsysBytes::_4 });
	}

	ITsys* Size()override
	{
		// TODO: Platform Specific
		return PrimitiveOf({ TsysPrimitiveType::UInt,TsysBytes::_4 });
	}

	ITsys* IntPtr()override
	{
		// TODO: Platform Specific
		return PrimitiveOf({ TsysPrimitiveType::SInt,TsysBytes::_4 });
	}

	ITsys* PrimitiveOf(TsysPrimitive primitive)override
	{
		vint a = (vint)primitive.type;
		vint b = (vint)primitive.bytes;
		vint index = (vint)TsysBytes::_COUNT * a + b;
		if (index > sizeof(primitives) / sizeof(*primitives)) throw "Not Implemented!";

		auto& itsys = primitives[index];
		if (!itsys)
		{
			itsys = _primitive.Alloc(this, primitive);
		}
		return itsys;
	}

	ITsys* DeclOf(Symbol* decl)override
	{
		vint index = decls.Keys().IndexOf(decl);
		if (index != -1) return decls.Values()[index];
		auto itsys = _decl.Alloc(this, decl);
		decls.Add(decl, itsys);
		return itsys;
	}

	ITsys* GenericArgOf(Symbol* decl)override
	{
		vint index = genericArgs.Keys().IndexOf(decl);
		if (index != -1) return genericArgs.Values()[index];
		auto itsys = _genericArg.Alloc(this, decl);
		genericArgs.Add(decl, itsys);
		return itsys;
	}
};

Ptr<ITsysAlloc> ITsysAlloc::Create()
{
	return new TsysAlloc;
}

/***********************************************************************
TsysBase (Impl)
***********************************************************************/

template<typename TType, typename TData, vint BlockSize>
ITsys* TsysBase::ParamsOf(IEnumerable<ITsys*>& params, const TData& data, WithParamsList<TType, TData>& paramsOf, ITsys_Allocator<TType, BlockSize> TsysAlloc::* alloc)
{
	WithParams<TType, TData> key;
	key.params = &params;
	key.itsys = nullptr;
	key.data = data;
	vint index = paramsOf.IndexOf(key);
	if (index != -1) return paramsOf[index].itsys;

	auto itsys = (tsys->*alloc).Alloc(tsys, this, data);
	CopyFrom(itsys->GetParams(), params);
	key.params = &itsys->GetParams();
	key.itsys = itsys;
	paramsOf.Add(key);
	return itsys;
}

ITsys* TsysBase::LRefOf()
{
	if (!lrefOf) lrefOf = tsys->_lref.Alloc(tsys, this);
	return lrefOf;
}

ITsys* TsysBase::RRefOf()
{
	if (!rrefOf) rrefOf = tsys->_rref.Alloc(tsys, this);
	return rrefOf;
}

ITsys* TsysBase::PtrOf()
{
	if (!ptrOf) ptrOf = tsys->_ptr.Alloc(tsys, this);
	return ptrOf;
}

ITsys* TsysBase::ArrayOf(vint dimensions)
{
	vint index = arrayOf.Keys().IndexOf(dimensions);
	if (index != -1) return arrayOf.Values()[index];
	auto itsys = tsys->_array.Alloc(tsys, this, dimensions);
	arrayOf.Add(dimensions, itsys);
	return itsys;
}

ITsys* TsysBase::FunctionOf(IEnumerable<ITsys*>& params, TsysFunc func)
{
	return ParamsOf(params, func, functionOf, &TsysAlloc::_function);
}

ITsys* TsysBase::MemberOf(ITsys* classType)
{
	vint index = memberOf.Keys().IndexOf(classType);
	if (index != -1) return memberOf.Values()[index];
	auto itsys = tsys->_member.Alloc(tsys, this, classType);
	memberOf.Add(classType, itsys);
	return itsys;
}

ITsys* TsysBase::CVOf(TsysCV cv)
{
	vint index = ((cv.isGeneralConst ? 1 : 0) << 1) + (cv.isVolatile ? 1 : 0);

	if (index == 0)
	{
		return this;
	}
	else
	{
		index--;
	}

	if (index > sizeof(cvOf) / sizeof(*cvOf)) throw "Not Implemented!";
	auto& itsys = cvOf[index];
	if (!itsys) itsys = tsys->_cv.Alloc(tsys, this, cv);
	return itsys;
}

ITsys* TsysBase::GenericOf(IEnumerable<ITsys*>& params)
{
	return ParamsOf(params, TsysGeneric(), genericOf, &TsysAlloc::_generic);
}