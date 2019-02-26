#include <cctype>
#include <rados/librados.hpp>
#include <iostream>
#include <string>

void exit_func(int ret);

librados::Rados rados;

int main(int argc, const char **argv)
{
  int ret = 0;

  // Define variables
  const char *pool_name = "rbd";
  std::string object_name("LowerObject");
  librados::IoCtx io_ctx;
  // Create the Rados object and initialize it
  {
    ret = rados.init("admin"); // Use the default client.admin keyring
    if (ret < 0) {
      std::cerr << "Failed to initialize rados! error " << ret <<
      std::endl;
      ret = EXIT_FAILURE;
    }
  }

  // Read the ceph config file in its default location
  ret = rados.conf_read_file("/etc/ceph/ceph.conf");
  if (ret < 0) {
    std::cerr << "Failed to parse config file "
              << "! Error" << ret << std::endl;
    ret = EXIT_FAILURE;
  }

  // Connect to the Ceph cluster
  ret = rados.connect();
  if (ret < 0) {
    std::cerr << "Failed to connect to cluster! Error " << ret <<
    std::endl;
    ret = EXIT_FAILURE;
  } else {
    std::cout << "Connected to the Ceph cluster" << std::endl;
  }

  // Create connection to the Rados pool
  ret = rados.ioctx_create(pool_name, io_ctx);
  if (ret < 0) {
    std::cerr << "Failed to connect to pool! Error: " << ret <<
    std::endl;
    ret = EXIT_FAILURE;
  } else {
    std::cout << "Connected to pool: " << pool_name <<
    std::endl;
  }
  librados::bufferlist in, out;
  io_ctx.exec(object_name, "md5", "calc_md5", in, out);
  exit_func(0);

}
void exit_func(int ret)
{
  // Clean up and exit
  rados.shutdown();
  exit(ret);
}
