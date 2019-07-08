
#include <shm/shm.h>

int main(int argc, char *argv[])
{
  shm::ShareMemoryHandler shm_recv("dummy", 10000);

  while(true) {
    auto data = shm_recv.read<std::vector<double>>();

    std::cout << "recv vector: ";
    for(auto& d:data) {
      std::cout << d << " ";
    }
    std::cout << std::endl;

    // std::cout << "recv: ";
    // for(char* it = data; *it; ++it) {
    //   std::cout << *it;
    // }
    // std::cout << std::endl;

    shm_recv.spinOnce();
  }

  return 0;
}