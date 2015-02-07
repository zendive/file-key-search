#pragma once

// Working state flags to controll access availability
// while being at "storm of window messages"
class CWorkState
{
public:
  // Deny Analyze when data state is not ready for it or already doing that
  bool m_bDenyAnalyze;
  // Deny Extract when no data for that or already doing that
  bool m_bDenyExtract;
  // Analyze is active
  bool m_bMakingAnalyze;
  // Extract is active
  bool m_bMakingExtract;
  // Key management is active
  bool m_bMakingKeys;

public:
  CWorkState();
  ~CWorkState();
  // Return true if making any kind of activity
  bool IsActive() const;
};
