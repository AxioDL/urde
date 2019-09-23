#pragma once

namespace urde {

class CWordBreakTables {
public:
  static int GetBeginRank(wchar_t ch);
  static int GetEndRank(wchar_t ch);
};

} // namespace urde
