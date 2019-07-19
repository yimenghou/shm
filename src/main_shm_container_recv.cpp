
#include <shm/shm_container.h>

int main(int argc, char *argv[])
{
  ShmVectorHelper<double> shm_vector("example");
  auto vec = shm_vector.read();
  for(const auto& v:vec) {
    std::cout << "recv: " << v << std::endl;
  }
  // ShmListHelper<double> shm_list("example");
  return 0;
}