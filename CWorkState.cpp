#include "StdAfx.h"
#include ".\CWorkState.h"

CWorkState::CWorkState(void)
{
  m_bDenyAnalyze = false;
  m_bDenyExtract = false;
  m_bMakingAnalyze = false;
  m_bMakingExtract = false;
  m_bMakingKeys = false;
}

CWorkState::~CWorkState(void)
{
}

bool CWorkState::IsActive() const
{
  return (m_bMakingAnalyze || m_bMakingExtract || m_bMakingKeys);
}