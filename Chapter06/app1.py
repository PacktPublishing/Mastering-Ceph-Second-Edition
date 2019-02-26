       import rados, sys, argparse 
       from PIL import Image 

       #Argument Parser used to read parameters and generate --help 
       parser = argparse.ArgumentParser(description='Image to RADOS 
       Object Utility') 
       parser.add_argument('--action', dest='action', action='store',
       required=True, help='Either upload or download image to/from 
       Ceph') 
       parser.add_argument('--image-file', dest='imagefile',
       action='store', required=True, help='The image file to
       upload to RADOS') 
       parser.add_argument('--object-name', dest='objectname', 
       action='store', required=True, help='The name of the
       RADOS object') 
       parser.add_argument('--pool', dest='pool', action='store', 
       required=True, help='The name of the RADOS pool to store
       the object') 
       parser.add_argument('--comment', dest='comment', action=
       'store', help='A comment to store with the object') 


       args = parser.parse_args() 

       try: #Read ceph.conf config file to obtain monitors 
         cluster = rados.Rados(conffile='/etc/ceph/ceph.conf') 
       except: 
         print "Error reading Ceph configuration" 
         sys.exit(1) 

       try: #Connect to the Ceph cluster 
         cluster.connect() 
       except: 
         print "Error connecting to Ceph Cluster" 
         sys.exit(1) 

       try: #Open specified RADOS pool 
         ioctx = cluster.open_ioctx(args.pool) 
       except: 
         print "Error opening pool: " + args.pool 
         cluster.shutdown() 
         sys.exit(1) 

       if args.action == 'upload': #If action is to upload 
         try: #Open image file in read binary mode 
           image=open(args.imagefile,'rb') 
           im=Image.open(args.imagefile) 
         except: 
           print "Error opening image file" 
           ioctx.close() 
           cluster.shutdown() 
           sys.exit(1) 
         print "Image size is x=" + str(im.size[0]) + " y=" + 
         str(im.size[1]) 
         try: #Write the contents of image file to object and add 
         attributes 
           ioctx.write_full(args.objectname,image.read()) 
           ioctx.set_xattr(args.objectname,'xres',str(im.size[0]) 
           +"\n") 
           ioctx.set_xattr(args.objectname,'yres',str(im.size[1]) 
           +"\n") 
           im.close() 
           if args.comment: 
             ioctx.set_xattr(args.objectname,'comment',args.comment 
             +"\n") 
         except: 
           print "Error writing object or attributes" 
           ioctx.close() 
           cluster.shutdown() 
           sys.exit(1) 
         image.close() 
       elif args.action == 'download': 
         try: #Open image file in write binary mode 
           image=open(args.imagefile,'wb') 
         except: 
           print "Error opening image file" 
           ioctx.close() 
           cluster.shutdown() 
           sys.exit(1) 
         try: #Write object to image file 
           image.write(ioctx.read(args.objectname)) 
         except: 
           print "Error writing object to image file" 
           ioctx.close() 
           cluster.shutdown() 
           sys.exit(1) 
         image.close() 
       else: 
         print "Please specify --action as either upload or download" 
       ioctx.close() #Close connection to pool 
       cluster.shutdown() #Close connection to Ceph 
       #The End
