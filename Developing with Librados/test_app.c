       #include <rados/librados.h> 
       #include <stdio.h> 
       #include <stdlib.h> 

       rados_t rados = NULL; 

       int exit_func(); 


       int main(int argc, const char **argv) 
       { 
         int ret = 0; 
         ret = rados_create(&rados, "admin"); // Use the
         client.admin keyring 
         if (ret < 0) { // Check that the rados object was created 
           printf("couldn't initialize rados! error %d\n", ret); 
           ret = EXIT_FAILURE; 
           exit_func; 
         } 
         else 
           printf("RADOS initialized\n"); 

         ret = rados_conf_read_file(rados, "/etc/ceph/ceph.conf"); 
         if (ret < 0) { //Parse the ceph.conf to obtain cluster details 
           printf("failed to parse config options! error %d\n", ret); 
           ret = EXIT_FAILURE; 
           exit_func(); 
         } 
         else 
           printf("Ceph config parsed\n"); 

         ret = rados_connect(rados); //Initiate connection to the
         Ceph cluster 
         if (ret < 0) { 
           printf("couldn't connect to cluster! error %d\n", ret); 
           ret = EXIT_FAILURE; 
           exit_func; 
         } else { 
           printf("Connected to the rados cluster\n"); 
         } 

         exit_func(); //End of example, call exit_func to clean
         up and finish 

       } 


       int exit_func () 
       { 
         rados_shutdown(rados); //Destroy connection to the
         Ceph cluster 
         printf("RADOS connection destroyed\n"); 
         printf("The END\n"); 
         exit(0); 
       } 
