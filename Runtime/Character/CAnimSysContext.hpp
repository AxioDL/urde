#pragma once

#include <memory>

#include "Runtime/CRandom16.hpp"
#include "Runtime/CToken.hpp"
#include "Runtime/GCNTypes.hpp"

namespace urde {
class CSimplePool;
class CTransitionDatabaseGame;

struct CAnimSysContext {
  TToken<CTransitionDatabaseGame> x0_transDB;
  std::shared_ptr<CRandom16> x8_random;
  CSimplePool& xc_store;

  CAnimSysContext(const TToken<CTransitionDatabaseGame>& transDB, u32 randomSeed, CSimplePool& store)
  : x0_transDB(transDB), x8_random(std::make_shared<CRandom16>(randomSeed)), xc_store(store) {}
};

} // namespace urde
