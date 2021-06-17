

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 7.00.0500 */
/* at Tue Nov 18 14:42:20 2014
 */
/* Compiler settings for .\SWDOverlays.idl:
    Oicf, W1, Zp8, env=Win32 (32b run)
    protocol : dce , ms_ext, c_ext, robust
    error checks: stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
//@@MIDL_FILE_HEADING(  )

#pragma warning( disable: 4049 )  /* more than 64k source lines */


/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 475
#endif

#include "rpc.h"
#include "rpcndr.h"

#ifndef __RPCNDR_H_VERSION__
#error this stub requires an updated version of <rpcndr.h>
#endif // __RPCNDR_H_VERSION__

#ifndef COM_NO_WINDOWS_H
#include "windows.h"
#include "ole2.h"
#endif /*COM_NO_WINDOWS_H*/

#ifndef __SDOverlays_i_h__
#define __SDOverlays_i_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __IAdded_FWD_DEFINED__
#define __IAdded_FWD_DEFINED__
typedef interface IAdded IAdded;
#endif 	/* __IAdded_FWD_DEFINED__ */


#ifndef __IConflict_FWD_DEFINED__
#define __IConflict_FWD_DEFINED__
typedef interface IConflict IConflict;
#endif 	/* __IConflict_FWD_DEFINED__ */


#ifndef __IDelete_FWD_DEFINED__
#define __IDelete_FWD_DEFINED__
typedef interface IDelete IDelete;
#endif 	/* __IDelete_FWD_DEFINED__ */


#ifndef __IForbid_FWD_DEFINED__
#define __IForbid_FWD_DEFINED__
typedef interface IForbid IForbid;
#endif 	/* __IForbid_FWD_DEFINED__ */


#ifndef __ILocked_FWD_DEFINED__
#define __ILocked_FWD_DEFINED__
typedef interface ILocked ILocked;
#endif 	/* __ILocked_FWD_DEFINED__ */


#ifndef __IModified_FWD_DEFINED__
#define __IModified_FWD_DEFINED__
typedef interface IModified IModified;
#endif 	/* __IModified_FWD_DEFINED__ */


#ifndef __INormal_FWD_DEFINED__
#define __INormal_FWD_DEFINED__
typedef interface INormal INormal;
#endif 	/* __INormal_FWD_DEFINED__ */


#ifndef __IReadOnly_FWD_DEFINED__
#define __IReadOnly_FWD_DEFINED__
typedef interface IReadOnly IReadOnly;
#endif 	/* __IReadOnly_FWD_DEFINED__ */


#ifndef __ISyncing_FWD_DEFINED__
#define __ISyncing_FWD_DEFINED__
typedef interface ISyncing ISyncing;
#endif 	/* __ISyncing_FWD_DEFINED__ */


#ifndef __Added_FWD_DEFINED__
#define __Added_FWD_DEFINED__

#ifdef __cplusplus
typedef class Added Added;
#else
typedef struct Added Added;
#endif /* __cplusplus */

#endif 	/* __Added_FWD_DEFINED__ */


#ifndef __Conflict_FWD_DEFINED__
#define __Conflict_FWD_DEFINED__

#ifdef __cplusplus
typedef class Conflict Conflict;
#else
typedef struct Conflict Conflict;
#endif /* __cplusplus */

#endif 	/* __Conflict_FWD_DEFINED__ */


#ifndef __Delete_FWD_DEFINED__
#define __Delete_FWD_DEFINED__

#ifdef __cplusplus
typedef class Delete Delete;
#else
typedef struct Delete Delete;
#endif /* __cplusplus */

#endif 	/* __Delete_FWD_DEFINED__ */


#ifndef __Forbid_FWD_DEFINED__
#define __Forbid_FWD_DEFINED__

#ifdef __cplusplus
typedef class Forbid Forbid;
#else
typedef struct Forbid Forbid;
#endif /* __cplusplus */

#endif 	/* __Forbid_FWD_DEFINED__ */


#ifndef __Locked_FWD_DEFINED__
#define __Locked_FWD_DEFINED__

#ifdef __cplusplus
typedef class Locked Locked;
#else
typedef struct Locked Locked;
#endif /* __cplusplus */

#endif 	/* __Locked_FWD_DEFINED__ */


#ifndef __Modified_FWD_DEFINED__
#define __Modified_FWD_DEFINED__

#ifdef __cplusplus
typedef class Modified Modified;
#else
typedef struct Modified Modified;
#endif /* __cplusplus */

#endif 	/* __Modified_FWD_DEFINED__ */


#ifndef __Normal_FWD_DEFINED__
#define __Normal_FWD_DEFINED__

#ifdef __cplusplus
typedef class Normal Normal;
#else
typedef struct Normal Normal;
#endif /* __cplusplus */

#endif 	/* __Normal_FWD_DEFINED__ */


#ifndef __ReadOnly_FWD_DEFINED__
#define __ReadOnly_FWD_DEFINED__

#ifdef __cplusplus
typedef class ReadOnly ReadOnly;
#else
typedef struct ReadOnly ReadOnly;
#endif /* __cplusplus */

#endif 	/* __ReadOnly_FWD_DEFINED__ */


#ifndef __Syncing_FWD_DEFINED__
#define __Syncing_FWD_DEFINED__

#ifdef __cplusplus
typedef class Syncing Syncing;
#else
typedef struct Syncing Syncing;
#endif /* __cplusplus */

#endif 	/* __Syncing_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"

#ifdef __cplusplus
extern "C"{
#endif 


#ifndef __IAdded_INTERFACE_DEFINED__
#define __IAdded_INTERFACE_DEFINED__

/* interface IAdded */
/* [unique][helpstring][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_IAdded;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("649C00F1-0378-4B81-8A64-ABEAF2A8FF0C")
    IAdded : public IDispatch
    {
    public:
    };
    
#else 	/* C style interface */

    typedef struct IAddedVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IAdded * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IAdded * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IAdded * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IAdded * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IAdded * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IAdded * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IAdded * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        END_INTERFACE
    } IAddedVtbl;

    interface IAdded
    {
        CONST_VTBL struct IAddedVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IAdded_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IAdded_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IAdded_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IAdded_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IAdded_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IAdded_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IAdded_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IAdded_INTERFACE_DEFINED__ */


#ifndef __IConflict_INTERFACE_DEFINED__
#define __IConflict_INTERFACE_DEFINED__

/* interface IConflict */
/* [unique][helpstring][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_IConflict;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("81F878CE-B076-4E3D-98B0-04C9A8DA664B")
    IConflict : public IDispatch
    {
    public:
    };
    
#else 	/* C style interface */

    typedef struct IConflictVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IConflict * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IConflict * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IConflict * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IConflict * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IConflict * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IConflict * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IConflict * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        END_INTERFACE
    } IConflictVtbl;

    interface IConflict
    {
        CONST_VTBL struct IConflictVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IConflict_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IConflict_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IConflict_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IConflict_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IConflict_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IConflict_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IConflict_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IConflict_INTERFACE_DEFINED__ */


#ifndef __IDelete_INTERFACE_DEFINED__
#define __IDelete_INTERFACE_DEFINED__

/* interface IDelete */
/* [unique][helpstring][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_IDelete;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("2A9268FD-9373-4B2D-8FBC-F03B153359E4")
    IDelete : public IDispatch
    {
    public:
    };
    
#else 	/* C style interface */

    typedef struct IDeleteVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IDelete * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IDelete * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IDelete * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IDelete * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IDelete * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IDelete * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IDelete * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        END_INTERFACE
    } IDeleteVtbl;

    interface IDelete
    {
        CONST_VTBL struct IDeleteVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IDelete_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IDelete_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IDelete_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IDelete_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IDelete_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IDelete_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IDelete_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IDelete_INTERFACE_DEFINED__ */


#ifndef __IForbid_INTERFACE_DEFINED__
#define __IForbid_INTERFACE_DEFINED__

/* interface IForbid */
/* [unique][helpstring][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_IForbid;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("24F30812-C717-4E32-8D3F-85BAC9B141B0")
    IForbid : public IDispatch
    {
    public:
    };
    
#else 	/* C style interface */

    typedef struct IForbidVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IForbid * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IForbid * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IForbid * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IForbid * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IForbid * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IForbid * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IForbid * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        END_INTERFACE
    } IForbidVtbl;

    interface IForbid
    {
        CONST_VTBL struct IForbidVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IForbid_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IForbid_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IForbid_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IForbid_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IForbid_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IForbid_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IForbid_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IForbid_INTERFACE_DEFINED__ */


#ifndef __ILocked_INTERFACE_DEFINED__
#define __ILocked_INTERFACE_DEFINED__

/* interface ILocked */
/* [unique][helpstring][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_ILocked;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("D87636DE-4359-43FE-B547-AA9388138400")
    ILocked : public IDispatch
    {
    public:
    };
    
#else 	/* C style interface */

    typedef struct ILockedVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            ILocked * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            ILocked * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            ILocked * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            ILocked * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            ILocked * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            ILocked * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            ILocked * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        END_INTERFACE
    } ILockedVtbl;

    interface ILocked
    {
        CONST_VTBL struct ILockedVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ILocked_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define ILocked_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define ILocked_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define ILocked_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define ILocked_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define ILocked_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define ILocked_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __ILocked_INTERFACE_DEFINED__ */


#ifndef __IModified_INTERFACE_DEFINED__
#define __IModified_INTERFACE_DEFINED__

/* interface IModified */
/* [unique][helpstring][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_IModified;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("FD6DE6B7-3145-42E0-8FC6-313D8ED9F5D4")
    IModified : public IDispatch
    {
    public:
    };
    
#else 	/* C style interface */

    typedef struct IModifiedVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IModified * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IModified * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IModified * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IModified * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IModified * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IModified * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IModified * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        END_INTERFACE
    } IModifiedVtbl;

    interface IModified
    {
        CONST_VTBL struct IModifiedVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IModified_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IModified_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IModified_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IModified_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IModified_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IModified_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IModified_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IModified_INTERFACE_DEFINED__ */


#ifndef __INormal_INTERFACE_DEFINED__
#define __INormal_INTERFACE_DEFINED__

/* interface INormal */
/* [unique][helpstring][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_INormal;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("F68E9050-9025-4B5C-9D95-EAF4FCF81DB7")
    INormal : public IDispatch
    {
    public:
    };
    
#else 	/* C style interface */

    typedef struct INormalVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            INormal * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            INormal * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            INormal * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            INormal * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            INormal * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            INormal * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            INormal * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        END_INTERFACE
    } INormalVtbl;

    interface INormal
    {
        CONST_VTBL struct INormalVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define INormal_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define INormal_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define INormal_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define INormal_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define INormal_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define INormal_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define INormal_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __INormal_INTERFACE_DEFINED__ */


#ifndef __IReadOnly_INTERFACE_DEFINED__
#define __IReadOnly_INTERFACE_DEFINED__

/* interface IReadOnly */
/* [unique][helpstring][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_IReadOnly;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("856076A6-B919-4754-AB2D-97BA21B1769D")
    IReadOnly : public IDispatch
    {
    public:
    };
    
#else 	/* C style interface */

    typedef struct IReadOnlyVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IReadOnly * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IReadOnly * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IReadOnly * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IReadOnly * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IReadOnly * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IReadOnly * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IReadOnly * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        END_INTERFACE
    } IReadOnlyVtbl;

    interface IReadOnly
    {
        CONST_VTBL struct IReadOnlyVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IReadOnly_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IReadOnly_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IReadOnly_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IReadOnly_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IReadOnly_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IReadOnly_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IReadOnly_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IReadOnly_INTERFACE_DEFINED__ */


#ifndef __ISyncing_INTERFACE_DEFINED__
#define __ISyncing_INTERFACE_DEFINED__

/* interface ISyncing */
/* [unique][helpstring][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_ISyncing;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("F3FF495F-1AB0-4A4B-9480-698542C5BDB2")
    ISyncing : public IDispatch
    {
    public:
    };
    
#else 	/* C style interface */

    typedef struct ISyncingVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            ISyncing * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            ISyncing * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            ISyncing * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            ISyncing * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            ISyncing * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            ISyncing * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            ISyncing * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        END_INTERFACE
    } ISyncingVtbl;

    interface ISyncing
    {
        CONST_VTBL struct ISyncingVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ISyncing_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define ISyncing_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define ISyncing_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define ISyncing_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define ISyncing_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define ISyncing_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define ISyncing_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __ISyncing_INTERFACE_DEFINED__ */



#ifndef __SWDOverlaysLib_LIBRARY_DEFINED__
#define __SWDOverlaysLib_LIBRARY_DEFINED__

/* library SWDOverlaysLib */
/* [helpstring][version][uuid] */ 


EXTERN_C const IID LIBID_SWDOverlaysLib;

EXTERN_C const CLSID CLSID_Added;

#ifdef __cplusplus

class DECLSPEC_UUID("37E3FAA5-C656-471E-B0D9-065D50FD0A10")
Added;
#endif

EXTERN_C const CLSID CLSID_Conflict;

#ifdef __cplusplus

class DECLSPEC_UUID("B0128151-D09A-44D6-9585-E6EC694DFE53")
Conflict;
#endif

EXTERN_C const CLSID CLSID_Delete;

#ifdef __cplusplus

class DECLSPEC_UUID("B2B24E46-DAE4-4379-8B96-910D0134F9E8")
Delete;
#endif

EXTERN_C const CLSID CLSID_Forbid;

#ifdef __cplusplus

class DECLSPEC_UUID("C7F27E92-CB43-4A23-B019-CE13DE2227FC")
Forbid;
#endif

EXTERN_C const CLSID CLSID_Locked;

#ifdef __cplusplus

class DECLSPEC_UUID("69C22DEA-70D5-42BC-8F8F-369F14701891")
Locked;
#endif

EXTERN_C const CLSID CLSID_Modified;

#ifdef __cplusplus

class DECLSPEC_UUID("AD8CC79D-9F8B-478B-8C4D-4332EE64CB46")
Modified;
#endif

EXTERN_C const CLSID CLSID_Normal;

#ifdef __cplusplus

class DECLSPEC_UUID("1A910B09-7B75-4BAD-9600-28649F2CF9F7")
Normal;
#endif

EXTERN_C const CLSID CLSID_ReadOnly;

#ifdef __cplusplus

class DECLSPEC_UUID("0A97FBBB-10B7-4EF9-8E9C-E0076153B76E")
ReadOnly;
#endif

EXTERN_C const CLSID CLSID_Syncing;

#ifdef __cplusplus

class DECLSPEC_UUID("0EF9A413-9E51-47A9-8E37-07221B57EB6B")
Syncing;
#endif
#endif /* __SWDOverlaysLib_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


