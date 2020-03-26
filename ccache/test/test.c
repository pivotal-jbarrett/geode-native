
#include <ccache.h>

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-parameter"
int main(int argc, char* argv[]) {
  void* cacheFactory = createCacheFactory();
  destroyCacheFactory(cacheFactory);
}
#pragma clang diagnostic pop
