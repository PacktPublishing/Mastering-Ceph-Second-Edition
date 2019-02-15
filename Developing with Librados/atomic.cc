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
         std::string object_string("I am an atomic object\n"); 
         std::string attribute_string("I am an atomic attribute\n"); 
         std::string object_name("atomic_object"); 
         librados::IoCtx io_ctx; 

         // Create the Rados object and initialize it 
         { 
           ret = rados.init("admin"); // Use the default client.admin 
           keyring 
           if (ret < 0) { 
             std::cerr << "Failed to initialize rados! error " << ret 
             << std::endl; 
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
           std::cerr << "Failed to connect to cluster! Error " << ret 
           << std::endl; 
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

         librados::bufferlist object_bl; // Initialize a bufferlist 
         object_bl.append(object_string); // Add our object text
         string to the bufferlist 
         librados::ObjectWriteOperation write_op; // Create a write 
         transaction 
         write_op.write_full(object_bl); // Write our bufferlist to the 
         transaction 
         std::cout << "Object: " << object_name << " has been written 
         to transaction" << std::endl; 
         char c; 
         std::cout << "Would you like to abort transaction? (Y/N)? "; 
         std::cin >> c; 
         if (toupper( c ) == 'Y') { 
           std::cout << "Transaction has been aborted, so object will 
           not actually be written" << std::endl; 
           exit_func(99); 
         } 
         librados::bufferlist attr_bl; // Initialize another bufferlist 
         attr_bl.append(attribute_string); // Add our attribute to the 
         bufferlist 
         write_op.setxattr("atomic_attribute", attr_bl); // Write our 
         attribute to our transaction 
         std::cout << "Attribute has been written to transaction" << 
         std::endl; 
         ret = io_ctx.operate(object_name, &write_op); // Commit the
         transaction 
         if (ret < 0) { 
           std::cerr << "failed to do compound write! error " << ret << 
           std::endl; 
           ret = EXIT_FAILURE; 
         } else { 
           std::cout << "We wrote the transaction containing our object
           and attribute" << object_name << std::endl; 
         } 

       } 

       void exit_func(int ret) 
       { 
         // Clean up and exit 
         rados.shutdown(); 
         exit(ret); 
       } 
