import rados, json, sys

try: #Read ceph.conf config file to obtain monitors
  cluster = rados.Rados(conffile='/etc/ceph/ceph.conf')
except:
  print "Error reading Ceph configuration"
  exit(1)

try: #Connect to the Ceph cluster
  cluster.connect()
except:
  print "Error connecting to Ceph Cluster"
  exit(1)

try: #Open specified RADOS pool
  ioctx = cluster.open_ioctx("rbd")
except:
  print "Error opening pool"
  cluster.shutdown()
  exit(1)

cmd = {
  "script": """
      function upper(input, output)
        size = objclass.stat()
        data = objclass.read(0, size)
        upper_str = string.upper(data:str())
        output:append(upper_str)
      end
      objclass.register(upper)
  """,
  "handler": "upper",
}

ret, data = ioctx.execute(str(sys.argv[1]), 'lua', 'eval_json', json.dumps(cmd))
print data[:ret]

ioctx.close() #Close connection to pool
cluster.shutdown() #Close connection to Ceph
