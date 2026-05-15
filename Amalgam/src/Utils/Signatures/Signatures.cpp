#include "Signatures.h"

#include "../Memory/Memory.h"
#include "../../Core/Core.h"
#include <string>
#include <format>

CSignature::CSignature(const char* sDLLName, const char* sSignature, int8_t nOffset, const char* sName)
{
	m_dwVal = 0x0;
	m_sDLLName = sDLLName;
	m_sSignature = sSignature;
	m_nOffset = nOffset;
	m_sName = sName;

	U::Signatures.AddSignature(this);
}

bool CSignature::Initialize()
{
	m_dwVal = U::Memory.FindSignature(m_sDLLName, m_sSignature);
	if (!m_dwVal)
	{
#ifdef __linux__
		OutputDebugStringA(std::format("CSignature::Initialize() unresolved on Linux:\n  {}\n  {}\n  {}\n", m_sName, m_sDLLName, m_sSignature).c_str());
#else
		U::Core.AppendFailText(std::format("CSignature::Initialize() failed to initialize:\n  {}\n  {}\n  {}", m_sName, m_sDLLName, m_sSignature).c_str());
#endif
		return false;
	}

	m_dwVal += m_nOffset;
	return true;
}

bool CSignatures::Initialize()
{
#ifdef __linux__
	size_t nMissing = 0;
#endif
	for (auto pSignature : m_vSignatures)
	{
		if (!pSignature)
			continue;

		if (!pSignature->Initialize())
#ifdef __linux__
			nMissing++;
#else
			m_bFailed = true;
#endif
	}

#ifdef __linux__
	if (nMissing)
		OutputDebugStringA(std::format("CSignatures::Initialize() continuing with {} unresolved Linux signatures\n", nMissing).c_str());
	return true;
#else
	return !m_bFailed;
#endif
}
