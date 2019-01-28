#include "JWBMFontParser.h"

using namespace tinyxml2;
using namespace JWENGINE;

// Static member variable declaration
BMFont JWBMFontParser::m_FontData;

auto WstringToString(WSTRING Source) -> STRING
{
	STRING Result;

	char* temp = nullptr;
	int len = WideCharToMultiByte(CP_ACP, 0, Source.c_str(), -1, temp, 0, nullptr, nullptr);

	temp = new char[len + 1];
	WideCharToMultiByte(CP_ACP, 0, Source.c_str(), -1, temp, len, nullptr, nullptr);

	Result = temp;

	delete[] temp;
	temp = nullptr;
	return Result;
}

auto StringToWstring(STRING Source) -> WSTRING
{
	WSTRING Result;

	wchar_t* temp = nullptr;
	int len = MultiByteToWideChar(CP_ACP, 0, Source.c_str(), -1, temp, 0);

	temp = new wchar_t[len + 1];
	MultiByteToWideChar(CP_ACP, 0, Source.c_str(), -1, temp, len);

	Result = temp;

	delete[] temp;
	temp = nullptr;
	return Result;
}

auto JWBMFontParser::ParseComma(STRING Data, UINT ID)->UINT const
{
	UINT Result = 0;
	STRING tempString = Data;
	tempString += ',';

	int Count = 0;
	size_t FoundPrev = 0;
	size_t Found = 0;
	
	while (true)
	{
		Found = tempString.find_first_of(',', FoundPrev);
		if (ID == Count)
			break;

		FoundPrev = Found + 1;
		Count++;
	}

	tempString = tempString.substr(FoundPrev, Found - FoundPrev);
	Result = atoi(tempString.c_str());

	return Result;
}

auto JWBMFontParser::Parse(WSTRING FileName)->bool
{
	//@warning: Without "tinyxml2::" here, XMLDocument can be ambiguous because of <msxml.h> in Windows Kits
	tinyxml2::XMLDocument tempXMLDoc;

	if (tempXMLDoc.LoadFile(WstringToString(FileName).c_str()) == tinyxml2::XMLEError::XML_SUCCESS)
	{
		const XMLElement* tempElementRoot = tempXMLDoc.FirstChildElement("font");

		const XMLElement* tempElement = nullptr;
		const XMLAttribute* tempAttr = nullptr;

		/** 
		* Parse element <info>
		*/
		tempElement = tempElementRoot->FirstChildElement("info");
		tempAttr = tempElement->FirstAttribute();
		m_FontData.Info.Face = StringToWstring(tempAttr->Value());
		tempAttr = tempAttr->Next();
		m_FontData.Info.Size = tempAttr->UnsignedValue();
		tempAttr = tempAttr->Next();
		m_FontData.Info.IsBold = tempAttr->BoolValue();
		tempAttr = tempAttr->Next();
		m_FontData.Info.IsItalic = tempAttr->BoolValue();
		tempAttr = tempAttr->Next();
		m_FontData.Info.Charset = StringToWstring(tempAttr->Value());
		tempAttr = tempAttr->Next();
		m_FontData.Info.IsUnicode = tempAttr->BoolValue();
		tempAttr = tempAttr->Next();
		m_FontData.Info.StretchH = tempAttr->UnsignedValue();
		tempAttr = tempAttr->Next();
		m_FontData.Info.IsSmooth = tempAttr->BoolValue();
		tempAttr = tempAttr->Next();
		m_FontData.Info.AA = tempAttr->UnsignedValue();
		tempAttr = tempAttr->Next();
		
		m_FontData.Info.Padding.Up = ParseComma(tempAttr->Value(), 0);
		m_FontData.Info.Padding.Right = ParseComma(tempAttr->Value(), 1);
		m_FontData.Info.Padding.Down = ParseComma(tempAttr->Value(), 2);
		m_FontData.Info.Padding.Left = ParseComma(tempAttr->Value(), 3);
		tempAttr = tempAttr->Next();

		m_FontData.Info.Spacing.Horz = ParseComma(tempAttr->Value(), 0);
		m_FontData.Info.Spacing.Vert = ParseComma(tempAttr->Value(), 1);
		tempAttr = tempAttr->Next();

		m_FontData.Info.bOutline = tempAttr->UnsignedValue();

		/**
		* Parse element <common>
		*/
		tempElement = tempElementRoot->FirstChildElement("common");
		tempAttr = tempElement->FirstAttribute();
		m_FontData.Common.LineHeight = tempAttr->UnsignedValue();
		tempAttr = tempAttr->Next();
		m_FontData.Common.Base = tempAttr->IntValue();
		tempAttr = tempAttr->Next();
		m_FontData.Common.ScaleW = tempAttr->UnsignedValue();
		tempAttr = tempAttr->Next();
		m_FontData.Common.ScaleH = tempAttr->UnsignedValue();
		tempAttr = tempAttr->Next();
		m_FontData.Common.Pages = tempAttr->UnsignedValue();
		tempAttr = tempAttr->Next();
		m_FontData.Common.IsPacked = tempAttr->BoolValue();
		tempAttr = tempAttr->Next();
		m_FontData.Common.AlphaChnl = tempAttr->IntValue();
		tempAttr = tempAttr->Next();
		m_FontData.Common.RedChnl = tempAttr->IntValue();
		tempAttr = tempAttr->Next();
		m_FontData.Common.GreenChnl = tempAttr->IntValue();
		tempAttr = tempAttr->Next();
		m_FontData.Common.BlueChnl = tempAttr->IntValue();
		
		/**
		* Parse element <pages>
		*/
		tempElement = tempElementRoot->FirstChildElement("pages");
		tempElement = tempElement->FirstChildElement("page");
		
		for (UINT i = 0; i < m_FontData.Common.Pages; i++)
		{
			BMFont::BMPage tempPage;
			tempAttr = tempElement->FirstAttribute();
			tempPage.ID = tempAttr->UnsignedValue();
			tempPage.File = StringToWstring(tempAttr->Next()->Value());
			m_FontData.Pages.push_back(tempPage);

			tempElement = tempElement->NextSiblingElement();
		}

		/**
		* Parse element <chars>
		*/
		tempElement = tempElementRoot->FirstChildElement("chars");
		UINT tempElementCount = tempElement->FirstAttribute()->IntValue();
		tempElement = tempElement->FirstChildElement("char");

		for (UINT i = 0; i < tempElementCount; i++)
		{
			BMFont::BMChar tempChar;
			tempAttr = tempElement->FirstAttribute();
			tempChar.ID = tempAttr->UnsignedValue();
			tempAttr = tempAttr->Next();
			tempChar.X = tempAttr->UnsignedValue();
			tempAttr = tempAttr->Next();
			tempChar.Y = tempAttr->UnsignedValue();
			tempAttr = tempAttr->Next();
			tempChar.Width = tempAttr->UnsignedValue();
			tempAttr = tempAttr->Next();
			tempChar.Height = tempAttr->UnsignedValue();
			tempAttr = tempAttr->Next();
			tempChar.XOffset = tempAttr->IntValue();
			tempAttr = tempAttr->Next();
			tempChar.YOffset = tempAttr->IntValue();
			tempAttr = tempAttr->Next();
			tempChar.XAdvance = tempAttr->UnsignedValue();
			tempAttr = tempAttr->Next();
			tempChar.Page = tempAttr->UnsignedValue();
			tempAttr = tempAttr->Next();
			tempChar.Chnl = tempAttr->UnsignedValue();
			m_FontData.Chars.push_back(tempChar);

			// map Chars
			m_FontData.CharMap.insert(std::make_pair(static_cast<wchar_t>(tempChar.ID), m_FontData.Chars.size() - 1));
			
			tempElement = tempElement->NextSiblingElement();
		}

		/**
		* Parse element <kernings>
		*/
		//
		//@warning: It's possible that there isn't any kerning in the file!
		//
		tempElement = tempElementRoot->FirstChildElement("kernings");
		if (tempElement)
		{
			tempElementCount = tempElement->FirstAttribute()->IntValue();
			tempElement = tempElement->FirstChildElement("kerning");

			for (UINT i = 0; i < tempElementCount; i++)
			{
				BMFont::BMKerning tempKerning;
				tempAttr = tempElement->FirstAttribute();
				tempKerning.First = tempAttr->UnsignedValue();
				tempAttr = tempAttr->Next();
				tempKerning.Second = tempAttr->UnsignedValue();
				tempAttr = tempAttr->Next();
				tempKerning.Amount = tempAttr->IntValue();
				m_FontData.Kernings.push_back(tempKerning);

				// Map Kernings
				m_FontData.KerningMap.insert(std::make_pair(std::make_pair(tempKerning.First, tempKerning.Second), tempKerning.Amount));

				tempElement = tempElement->NextSiblingElement();
			}
		}
		
		// The parsing ended successfully
		return true;
	}
	
	//@warning: If failed at LoadFile(), the method directly comes here
	return false;
}

auto JWBMFontParser::GetFontData()->const BMFont* const
{
	return &m_FontData;
}