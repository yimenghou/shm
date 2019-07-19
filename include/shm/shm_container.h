
#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/containers/vector.hpp>
#include <boost/interprocess/allocators/allocator.hpp>
#include <string>
#include <cstdlib> //std::system
#include <iostream>
#include <chrono>
#include <thread>
#include <shm/utils.h>

using namespace boost::interprocess;

template<typename ElementalT>
class ShmVectorHelper {
public:

  //Define an STL compatible allocator of ints that allocates from the managed_shared_memory.
  //This allocator will allow placing containers in the segment
  typedef allocator<ElementalT, managed_shared_memory::segment_manager>  ShmemAllocator;

  //Alias a vector that uses the previous STL-like allocator so that allocates
  //its values from the segment
  typedef std::vector<ElementalT, ShmemAllocator> ContainerT;

  ShmVectorHelper(const std::string& name, const size_t size = 65536):
  name_(name), segment_(open_or_create, name_.c_str(), size) {
    std::cout << "complete" << std::endl;
    //Initialize shared memory STL-compatible allocator
    const ShmemAllocator alloc_inst(segment_.get_segment_manager());
    std::cout << "complete" << std::endl;
    //Construct a vector named "container" in shared memory with argument alloc_inst
    container_ = segment_.construct<ContainerT>(name_.c_str())(alloc_inst);
    std::cout << "complete" << std::endl;

  }

  inline std::vector<ElementalT> read() {
    container_ = segment_.find<ContainerT>(name_.c_str()).first;
    std::vector<ElementalT> temp;
    for(const auto& v:*container_) {temp.push_back(v);}
    return temp;
  }

  inline void write(const std::vector<ElementalT>& vec) {
    container_->clear();
    for(const auto& v:vec) {container_->push_back(v);}
  }

  void destroy() {
    segment_.destroy<ContainerT>(name_);
  }

  void spin() {
    while(true) {
      using namespace std::literals::chrono_literals;
      std::this_thread::sleep_for(100ms);
    }
  }

  void spinOnce() {
    using namespace std::literals::chrono_literals;
    std::this_thread::sleep_for(100ms);
  }

private:
  //Create a new segment with given name and size
  std::string name_;
  managed_shared_memory segment_;
  ContainerT* container_;
};

// template<typename ElementalT>
// class ShmListHelper {
// public:

//   //Define an STL compatible allocator of ints that allocates from the managed_shared_memory.
//   //This allocator will allow placing containers in the segment
//   typedef allocator<ElementalT, managed_shared_memory::segment_manager>  ShmemAllocator;

//   //Alias a vector that uses the previous STL-like allocator so that allocates
//   //its values from the segment
//   typedef std::list<ElementalT, ShmemAllocator> ContainerT;

//   ShmListHelper(const std::string& name, const size_t size = 65536):
//   name_(name), segment_(create_only, name_.c_str(), size) {
//     //Initialize shared memory STL-compatible allocator
//     const ShmemAllocator alloc_inst(segment_.get_segment_manager());
//     //Construct a vector named "container" in shared memory with argument alloc_inst
//     container_ = segment_.construct<ContainerT>(name_.c_str())(alloc_inst);
//   }

//   inline ContainerT* read() const {
//     return segment_.find<ContainerT>(name_).first;
//   }

//   inline void write(const std::vector<ElementalT>& vec) {
//     container_->clear();
//     for(const auto& v:vec) {container_->push_back(v);}
//   }

//   void destroy() {
//     segment_.destroy<ContainerT>(name_);
//   }

//   void spin() {
//     while(true) {
//       using namespace std::literals::chrono_literals;
//       std::this_thread::sleep_for(100ms);
//     }
//   }

//   void spinOnce() {
//     using namespace std::literals::chrono_literals;
//     std::this_thread::sleep_for(100ms);
//   }

// private:
//   //Create a new segment with given name and size
//   std::string name_;
//   managed_shared_memory segment_;
//   ContainerT* container_;
// };

// //Main function. For parent process argc == 1, for child process argc == 2
// int main(int argc, char *argv[])
// {
//    if(argc == 1){ //Parent process

//       //Remove shared memory on construction and destruction
//       struct shm_remove
//       {
//          shm_remove() { shared_memory_object::remove("MySharedMemory"); }
//          ~shm_remove(){ shared_memory_object::remove("MySharedMemory"); }
//       } remover;

//       //Create a new segment with given name and size
//       managed_shared_memory segment(create_only, "MySharedMemory", 65536);

//       //Initialize shared memory STL-compatible allocator
//       const ShmemAllocator alloc_inst (segment.get_segment_manager());

//       //Construct a vector named "ContainerT" in shared memory with argument alloc_inst
//       ContainerT *ContainerT = segment.construct<ContainerT>("ContainerT")(alloc_inst);

//       for(int i = 0; i < 100; ++i)  //Insert data in the vector
//          ContainerT->push_back(i);

//       //Launch child process
//       std::string s(argv[0]); s += " child ";
//       if(0 != std::system(s.c_str()))
//          return 1;

//       //Check child has destroyed the vector
//       if(segment.find<ContainerT>("ContainerT").first)
//          return 1;
//    }
//    else{ //Child process

//       //Open the managed segment
//       managed_shared_memory segment(open_only, "MySharedMemory");

//       //Find the vector using the c-string name
//       ContainerT *ContainerT = segment.find<ContainerT>("ContainerT").first;

//       //Use vector in reverse order
//       std::sort(ContainerT->rbegin(), ContainerT->rend());

//       for(const auto& v:*ContainerT) {
//         std::cout << v << std::endl;
//       }

//       //When done, destroy the vector from the segment
//       segment.destroy<ContainerT>("ContainerT");
//    }

//    return 0;
// }