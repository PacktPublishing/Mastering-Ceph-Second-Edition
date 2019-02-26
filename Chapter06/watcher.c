       #include <stdio.h> 
       #include <stdlib.h> 
       #include <string.h> 
       #include <syslog.h> 

       #include <rados/librados.h> 
       #include <rados/rados_types.h> 

       uint64_t cookie; 
       rados_ioctx_t io; 
       rados_t cluster; 
       char cluster_name[] = "ceph"; 
       char user_name[] = "client.admin"; 
       char object[] = "my_object"; 
       char pool[] = "rbd"; 

       /* Watcher callback function - called when watcher receives a
       notification */ 
       void watch_notify2_cb(void *arg, uint64_t notify_id, uint64_t
       cookie, uint64_t notifier_gid, void *data, size_t data_len) 
       { 
         const char *notify_oid = 0; 
         char *temp = (char*)data+4; 
         int ret; 
         printf("Message from Notifier: %s\n",temp); 
         rados_notify_ack(io, object, notify_id, cookie, "Received", 8); 
       } 

       /* Watcher error callback function - called if watcher encounters
       an error */ 
       void watch_notify2_errcb(void *arg, uint64_t cookie, int err) 
       { 
         printf("Removing Watcher on object %s\n",object); 
         err = rados_unwatch2(io,cookie); 
         printf("Creating Watcher on object %s\n",object); 
         err = rados_watch2(io,object,&cookie,watch_notify2_cb,
         watch_notify2_errcb,NULL); 
         if (err < 0) { 
           fprintf(stderr, "Cannot create watcher on %s/%s: %s\n", object,
           pool, strerror(-err)); 
           rados_ioctx_destroy(io); 
           rados_shutdown(cluster); 
           exit(1); 
         } 
       } 

       int main (int argc, char **argv) 
       { 
         int err; 
         uint64_t flags; 

         /* Create Rados object */ 
         err = rados_create2(&cluster, cluster_name, user_name, flags); 
         if (err < 0) { 
           fprintf(stderr, "Couldn't create the cluster object!: %s\n",
           strerror(-err)); 
           exit(EXIT_FAILURE); 
         } else { 
           printf("Created the rados object.\n"); 
         } 


         /* Read a Ceph configuration file to configure the cluster
         handle. */ 
         err = rados_conf_read_file(cluster, "/etc/ceph/ceph.conf"); 
         if (err < 0) { 
           fprintf(stderr, "Cannot read config file: %s\n",
           strerror(-err)); 
           exit(EXIT_FAILURE); 
         } else { 
           printf("Read the config file.\n"); 
         } 
         /* Connect to the cluster */ 
         err = rados_connect(cluster); 
         if (err < 0) { 
           fprintf(stderr, "Cannot connect to cluster: %s\n",
           strerror(-err)); 
           exit(EXIT_FAILURE); 
         } else { 
           printf("\n Connected to the cluster.\n"); 
         } 

         /* Create connection to the Rados pool */ 
         err = rados_ioctx_create(cluster, pool, &io); 
         if (err < 0) { 
           fprintf(stderr, "Cannot open rados pool %s: %s\n", pool,
           strerror(-err)); 
           rados_shutdown(cluster); 
           exit(1); 
         } 

         /* Create the Rados Watcher */ 
         printf("Creating Watcher on object %s/%s\n",pool,object); 
         err = rados_watch2(io,object,&cookie,watch_notify2_cb, 
         watch_notify2_errcb,NULL); 
         if (err < 0) { 
           fprintf(stderr, "Cannot create watcher on object %s/%s: %s\n",
           pool, object, strerror(-err)); 
           rados_ioctx_destroy(io); 
           rados_shutdown(cluster); 
           exit(1); 
         } 

         /* Loop whilst waiting for notifier */ 
         while(1){ 
           sleep(1); 
         } 
         /* Clean up */ 
         rados_ioctx_destroy(io); 
         rados_shutdown(cluster); 
       } 
