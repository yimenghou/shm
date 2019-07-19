
#include <shm/shm.h>

int main(int argc, char *argv[])
{
  shm::ShareMemoryHandler shm_recv("dummy", 10000);

  while(true) {
    auto data = shm_recv.read<std::vector<int>>();

    for(const auto& d:data) {
      std::cout << d << std::endl;
    }
    shm_recv.spinOnce();
  }

  return 0;
}