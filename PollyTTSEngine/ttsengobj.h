/*  Copyright 2017 - 2018 Amazon.com, Inc. or its affiliates.All Rights Reserved.
Licensed under the Amazon Software License(the "License").You may not use
this file except in compliance with the License.A copy of the License is
located at

http://aws.amazon.com/asl/

and in the "LICENSE" file accompanying this file.This file is distributed
on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, express
or implied.See the License for the specific language governing
permissions and limitations under the License. */

/******************************************************************************
* TtsEngObj.h *
*-------------*
*  This is the header file for the sample CTTSEngObj class definition.
*
******************************************************************************/
#ifndef TtsEngObj_h
#define TtsEngObj_h
#define NOMINMAX

//--- Additional includes
#ifndef __PollyTTSEngine_h__
#include "PollyTTSEngine.h"
#endif

#ifndef SPDDKHLP_h
#include <spddkhlp.h>
#endif

#include <minmax.h>
#ifndef SPCollec_h
#include <spcollec.h>
#endif

#include "resource.h"
#include <string>
#include "spdlog/spdlog.h"
#include "spdlog/sinks/msvc_sink.h"
namespace spd = spdlog;

//=== Constants ====================================================

//=== Class, Enum, Struct and Union Declarations ===================

//=== Enumerated Set Definitions ===================================

//=== Function Type Definitions ====================================

//=== Class, Struct and Union Definitions ==========================

/*** CSentItem
*   This object is a helper class
*/
class CSentItem
{
  public:
    CSentItem() { memset( this, 0, sizeof(*this) ); }
    CSentItem( CSentItem& Other ) { memcpy( this, &Other, sizeof( Other ) ); }

  /*--- Data members ---*/
    const SPVSTATE* pXmlState;
    LPCWSTR         pItem;
    ULONG           ulItemLen;
    ULONG           ulItemSrcOffset;        // Original source character position
    ULONG           ulItemSrcLen;           // Length of original source item in characters
};

typedef CSPList<CSentItem,CSentItem&> CItemList;

/*** CTTSEngObj COM object ********************************
*/
class ATL_NO_VTABLE CTTSEngObj : 
	public CComObjectRootEx<CComMultiThreadModel>,
	public CComCoClass<CTTSEngObj, &CLSID_PollyTTSEngine>,
	public ISpTTSEngine,
    public ISpObjectWithToken
{
  /*=== ATL Setup ===*/
  public:
    DECLARE_REGISTRY_RESOURCEID(IDR_POLLYTTSENGINE)
    DECLARE_PROTECT_FINAL_CONSTRUCT()

    BEGIN_COM_MAP(CTTSEngObj)
	    COM_INTERFACE_ENTRY(ISpTTSEngine)
	    COM_INTERFACE_ENTRY(ISpObjectWithToken)
    END_COM_MAP()

  /*=== Methods =======*/
  public:
    /*--- Constructors/Destructors ---*/
    HRESULT FinalConstruct();
    void FinalRelease();
	TCHAR* GetPath();

  /*=== Interfaces ====*/
  public:
    //--- ISpObjectWithToken ----------------------------------
    STDMETHODIMP SetObjectToken( ISpObjectToken * pToken );
    STDMETHODIMP GetObjectToken( ISpObjectToken ** ppToken )
        { return SpGenericGetObjectToken( ppToken, m_cpToken ); }


    //--- ISpTTSEngine --------------------------------------------
    STDMETHOD(Speak)( DWORD dwSpeakFlags,
                      REFGUID rguidFormatId, const WAVEFORMATEX * pWaveFormatEx,
                      const SPVTEXTFRAG* pTextFragList, ISpTTSEngineSite* pOutputSite );
    STDMETHOD(GetOutputFormat)( const GUID * pTargetFormatId, const WAVEFORMATEX * pTargetWaveFormatEx,
                                GUID * pDesiredFormatId, WAVEFORMATEX ** ppCoMemDesiredWaveFormatEx );
    std::wstring ReplaceText(const std::wstring orig, const std::wstring fnd, const std::wstring repl);


  private:
    /*--- Non interface methods ---*/
    HRESULT MapFile(const WCHAR * pszTokenValName, HANDLE * phMapping, void ** ppvData );
    HRESULT GetNextSentence( CItemList& ItemList );
    BOOL    AddNextSentenceItem( CItemList& ItemList );
    HRESULT OutputSentence( CItemList& ItemList, ISpTTSEngineSite* pOutputSite );

  /*=== Member Data ===*/
  private:
    CComPtr<ISpObjectToken> m_cpToken;
    HANDLE                  m_hVoiceData;
    void*                   m_pVoiceData;
	LPWSTR      			m_pPollyVoice;
	bool                    m_isNeural;
	bool                    m_isNews;
	bool                    m_isConversational;
	wchar_t                 m_voiceOveride[100];
	std::shared_ptr<spdlog::logger> m_logger;


    //--- Voice (word/audio data) list
    //  Note: You will probably have something more sophisticated here
    //        we are just trying to keep it simple for the example.
    VOICEITEM*          m_pWordList;
    ULONG               m_ulNumWords;

    //--- Working variables to walk the text fragment list during Speak()
    const SPVTEXTFRAG*  m_pCurrFrag;
    const WCHAR*        m_pNextChar;
    const WCHAR*        m_pEndChar;
    ULONGLONG           m_ullAudioOff;
};

#endif //--- This must be the last line in the file
