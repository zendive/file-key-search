/*******
* AUTHOR:   Block Alexander. OBoyurand 2005
* DATE:     Oct 14 2001  -->  [21:31] - 29thu-Sep-2005
* TITLE:    class CPExTA.
* USAGE:    In "drivers" to maintaine Process Execution Time Analyze
*           while various optimization tests (PExTA v2.0)
******************************************************************************/

#pragma once
#ifndef	_765_ANALYZES_H__
#define	_765_ANALYZES_H__

#include    <windows.h>       // for GetTickCount().

class CPExTA
{
private:
  DWORD m_start;             // time of start;
  DWORD m_pause;             // delta for resorection in restart method;
  DWORD m_end;               // time of end;

public:

  // Constructor: set timers to zero;
  CPExTA(void)
  {
    m_start = m_pause = m_end = 0;
  };

  // Initialization of count with reset;
  inline void Start(void)
  {
    m_start = GetTickCount();
    m_pause = m_end = 0;
  };

  // Temporary stop the count;
  inline void PauseON(void)
  {
    m_pause = GetTickCount() - m_start;
  };

  // Restart the count after pause;
  inline void PauseOFF(void)
  {
    m_start = GetTickCount() - m_pause;
  };

  // Reset all data variables to zero;
  inline CPExTA& Reset(void)
  {
    m_start = m_pause = m_end = 0;
    return *this;
  };

  // End count process & return the end_time;
  inline DWORD Stop(void)
  {
    m_end = GetTickCount() - m_start;
    return m_end;
  };

  // Return the m_end time;
  inline DWORD GetStopTime(void) const
  {
    return m_end;
  };

  // Get curent timer value;
  inline DWORD GetCurTime(void) const
  {
    return (GetTickCount() - m_start);
  };

  inline operator DWORD()
  {
    return m_end;
  };

};

#endif	_765_ANALYZES_H__
//_________________________________________________________