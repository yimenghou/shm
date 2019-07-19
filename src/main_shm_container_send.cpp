
#include <shm/shm_container.h>

int main(int argc, char *argv[])
{
  ShmVectorHelper<double> shm_vector("example");
  std::vector<double> v;
  std::cout << "!" << std::endl;
  for(size_t i=0;i<10;i++) {v.push_back(i);}
  shm_vector.write(v);
  std::cout << "!" << std::endl;
  shm_vector.spin();
  // ShmListHelper<double> shm_list("example");
  return 0;
}