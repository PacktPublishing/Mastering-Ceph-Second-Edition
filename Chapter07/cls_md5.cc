       #include "objclass/objclass.h"
       #include <openssl/md5.h>

       CLS_VER(1,0)
       CLS_NAME(md5)

       cls_handle_t h_class;
       cls_method_handle_t h_calc_md5;

       static int calc_md5(cls_method_context_t hctx, bufferlist *in,
       bufferlist *out)
       {
         char md5string[33];

         for(int i = 0; i < 1000; ++i)
         {
           size_t size;
           int ret = cls_cxx_stat(hctx, &size, NULL);
           if (ret < 0)
             return ret;

           bufferlist data;
           ret = cls_cxx_read(hctx, 0, size, &data);
           if (ret < 0)
             return ret;
           unsigned char md5out[16];
           MD5((unsigned char*)data.c_str(), data.length(), md5out);
           for(int i = 0; i < 16; ++i)
             sprintf(&md5string[i*2], "%02x", (unsigned int)md5out[i]);
           CLS_LOG(0,"Loop:%d - %s",i,md5string);
           bufferlist attrbl;
           attrbl.append(md5string);
           ret = cls_cxx_setxattr(hctx, "MD5", &attrbl);
           if (ret < 0)
           {
             CLS_LOG(0, "Error setting attribute");
             return ret;
           }
         }
         out->append((const char*)md5string, sizeof(md5string));
         return 0;
       }

       void __cls_init()
       {
         CLS_LOG(0, "loading cls_md5");
         cls_register("md5", &h_class);
         cls_register_cxx_method(h_class, "calc_md5", CLS_METHOD_RD | 
         CLS_METHOD_WR, calc_md5, &h_calc_md5)
       }
