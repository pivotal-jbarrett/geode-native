
#include "ccache.h"

#include <geode/CacheFactory.hpp>

using apache::geode::client::CacheFactory;

void* createCacheFactory() { return new CacheFactory(); }

void destroyCacheFactory(void* cacheFactory) {
  delete static_cast<CacheFactory*>(cacheFactory);
}
