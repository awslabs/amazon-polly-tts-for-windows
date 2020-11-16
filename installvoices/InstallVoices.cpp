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
* InstallVoices.cpp:
**   Install Amazon Polly Voices
******************************************************************************/
#include "stdafx.h"
#include <iostream>
#include "../PollyTTSEngine/PollyTTSEngine_i.c"
#include <direct.h>
#include "../PollyTTSEngine/PollyTTSEngine.h"
#include <aws/core/Aws.h>
#include <aws/core/utils/Outcome.h>
#include <aws/polly/model/DescribeVoicesRequest.h>
#include <aws/polly/PollyClient.h>
#include "VoiceForSapi.h"
#include <aws/core/auth/AWSCredentialsProvider.h>
#include <locale>
#include <codecvt>


using namespace Aws::Polly;

typedef std::map<std::wstring, VoiceForSAPI> voice_map_t;
typedef std::set<VoiceId> argument_set_t;

voice_map_t SelectedVoicesMap(const std::wstring&);
void PrintHelp(WCHAR*);
int AddVoice(const VoiceForSAPI&);
int RemoveVoice(WCHAR*);
argument_set_t ArgumentSet(const std::wstring&);
Aws::String WStringToAwsString(const std::wstring& s);
std::wstring AwsStringToWString(const Aws::String& s);

int wmain(int argc, __in_ecount(argc) WCHAR* argv[])
{
	HRESULT hr = S_OK;
	if (argc > 3 || argc < 2)
	{
		PrintHelp(argv[0]);
		hr = E_INVALIDARG;
		//return FAILED(hr);
	} else if (argc > 1)
	{
		CoInitialize(nullptr);

		std::wstring voiceList;
		if (argc == 3) {
			voiceList = argv[2];
		}

		voice_map_t voices = SelectedVoicesMap(voiceList);

		if (wcscmp(argv[1], L"install") == 0)
		{			
			
			for (auto& voice : voices)
			{
				std::wcout << L"Installing " << voice.second.tokenKeyName << " - ";
				std::wcout << voice.second.langIndependentName << std::endl;

				AddVoice(voice.second);
			}						
		}
		else if (wcscmp(argv[1], L"uninstall") == 0)
		{			
			for (auto& voice : voices)
			{
				std::wcout << L"Removing " << voice.second.tokenKeyName << " - ";
				std::wcout << voice.second.langIndependentName << std::endl;

				RemoveVoice(const_cast<WCHAR *>(voice.second.tokenKeyName.c_str()));
			}
		}
		else {
			PrintHelp(argv[0]);
			hr = E_INVALIDARG;
		}

		CoUninitialize();
	}

	return FAILED(hr);
}

void PrintHelp(WCHAR* exeName)
{
	printf("Usage to install all voices   : > %ws install \n", exeName);
	printf("Usage to install some voices   : > %ws install Joanna,Filiz\n", exeName);
	printf("Usage to uninstall all voices : > %ws uninstall \n", exeName);
	printf("Usage to uninstall some voices   : > %ws uninstall Joanna,Filiz\n", exeName);
}


voice_map_t SelectedVoicesMap(const std::wstring& voiceList)
{
	Aws::SDKOptions options;
	InitAPI(options);
	PollyClient pc = Aws::MakeShared<Aws::Auth::ProfileConfigFileAWSCredentialsProvider>("InstallVoices", "polly-windows");
	voice_map_t pollyVoices;
	boolean isSelected(false);
	boolean isAllSelected(voiceList.empty());
	DescribeVoicesRequest describeVoices;
	std::wstring name;
	auto voicesOutcome = pc.DescribeVoices(describeVoices);
	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;

	if (voicesOutcome.IsSuccess())
	{
		auto voiceSet = ArgumentSet(voiceList);
		for (auto& voice : voicesOutcome.GetResult().GetVoices())
		{
			if (!isAllSelected && (voiceSet.find(voice.GetId()) != voiceSet.end())) {
				isSelected = true;
			}
			if (isSelected || isAllSelected)
			{
				VoiceForSAPI v4sp(voice, false, false, false);
				name = converter.from_bytes(VoiceIdMapper::GetNameForVoiceId(voice.GetId()).c_str());
				pollyVoices.insert(std::make_pair(name.c_str(), v4sp));
				auto e = voice.GetSupportedEngines();
				if (std::find(e.begin(), e.end(), Engine::neural) != e.end())
				{
					name.append(L"_neural");
					VoiceForSAPI v4sp_neural(voice, true, false, false);
					pollyVoices.insert(std::make_pair(name, v4sp_neural));
				}
				name = converter.from_bytes(VoiceIdMapper::GetNameForVoiceId(voice.GetId()).c_str());

				if (name == L"Lupe" || name == L"Amy") {
					VoiceForSAPI v4sp_newscaster(voice, true, true, false);
					name.append(L"_newscaster");
					pollyVoices.insert(std::make_pair(name, v4sp_newscaster));
				}
				if (name == L"Matthew" || name == L"Joanna")
				{
					std::wcout << "Adding newscaster and conversational\n";
					VoiceForSAPI v4sp_newscaster(voice, true, true, false);
					name.append(L"_newscaster");
					pollyVoices.insert(std::make_pair(name, v4sp_newscaster));
					VoiceForSAPI v4sp_conversational(voice, true, false, true);
					name = converter.from_bytes(VoiceIdMapper::GetNameForVoiceId(voice.GetId()).c_str());
					name.append(L"_conversational");
					pollyVoices.insert(std::make_pair(name, v4sp_conversational));
				}
			}
		}
	}
	else
	{
		std::cout << "Error while getting voices" << std::endl;
		std::cout << voicesOutcome.GetError();
	}
	ShutdownAPI(options);
	return pollyVoices;
}

int AddVoice(const VoiceForSAPI& voiceForSapi)
{
	CComPtr<ISpObjectToken> cpToken;
	CComPtr<ISpDataKey> cpDataKeyAttribs;

	auto hr = SpCreateNewTokenEx(
		SPCAT_VOICES,
		const_cast<WCHAR *>(voiceForSapi.tokenKeyName.c_str()),
		&CLSID_PollyTTSEngine,
		const_cast<WCHAR*>(voiceForSapi.langDependentName.c_str()),
		voiceForSapi.langid,
		const_cast<WCHAR*>(voiceForSapi.langIndependentName.c_str()),
		&cpToken,
		&cpDataKeyAttribs);

	//--- Set additional attributes for searching and the path to the
	//    voice data file we just created.
	if (SUCCEEDED(hr))
	{
		hr = cpDataKeyAttribs->SetStringValue(L"Gender", voiceForSapi.gender.c_str());
		if (SUCCEEDED(hr))
		{
			hr = cpDataKeyAttribs->SetStringValue(L"Name", voiceForSapi.langIndependentName.c_str());
		}
		if (SUCCEEDED(hr))
		{
			hr = cpDataKeyAttribs->SetStringValue(L"VoiceId", voiceForSapi.voiceId.c_str());
		}
		if (SUCCEEDED(hr))
		{
			hr = cpDataKeyAttribs->SetStringValue(L"Language", voiceForSapi.languageText.c_str());
		}
		if (SUCCEEDED(hr))
		{
			hr = cpDataKeyAttribs->SetStringValue(L"Age", voiceForSapi.age.c_str());
		}
		if (SUCCEEDED(hr))
		{
			hr = cpDataKeyAttribs->SetStringValue(L"Vendor", voiceForSapi.vendor.c_str());
		}
		if (SUCCEEDED(hr))
		{
			hr = cpDataKeyAttribs->SetStringValue(L"IsNeural", voiceForSapi.hasNeural ? L"1" : L"0");
		}
		if (SUCCEEDED(hr))
		{
			hr = cpDataKeyAttribs->SetStringValue(L"IsNews", voiceForSapi.hasNewscasterStyle ? L"1" : L"0");
		}
		if (SUCCEEDED(hr))
		{
			hr = cpDataKeyAttribs->SetStringValue(L"IsConversational", voiceForSapi.hasConversationalStyle ? L"1" : L"0");
		}
	}
	return SUCCEEDED(hr);
}

int RemoveVoice(WCHAR* tokenKeyName)
{
	std::wstring subKey;
#if _WIN64	
	subKey = L"SOFTWARE\\Microsoft\\Speech\\Voices\\Tokens\\";
#else
	subKey = L"SOFTWARE\\WOW6432Node\\Microsoft\\Speech\\Voices\\Tokens\\";
#endif
	subKey += tokenKeyName;
	const HRESULT result = SHDeleteKeyW(HKEY_LOCAL_MACHINE, subKey.c_str());
	if (result == ERROR_SUCCESS) {
		return SUCCEEDED(S_OK);
	}
	return FAILED(result);
}

argument_set_t ArgumentSet(const std::wstring& str)
{
	argument_set_t argSet;
	std::wstringstream wss(str);

	if (str.empty()) return argSet;

	while (wss.good())
	{
		std::wstring subStr;
		getline(wss, subStr, L',');
		VoiceId voice = VoiceIdMapper::GetVoiceIdForName(WStringToAwsString(subStr));
		argSet.insert(voice);
	}
	return argSet;
}

Aws::String WStringToAwsString(const std::wstring& s)
{
	Aws::String temp(s.length(), ' ');
	copy(s.begin(), s.end(), temp.begin());
	return temp;
}

std::wstring AwsStringToWString(const Aws::String& s)
{
	std::wstring temp(s.length(), L' ');
	copy(s.begin(), s.end(), temp.begin());
	return temp;
}