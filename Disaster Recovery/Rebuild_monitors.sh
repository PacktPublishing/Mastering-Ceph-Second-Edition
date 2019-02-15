#!/bin/bash
hosts="osd1 osd2 osd3"
ms=/tmp/mon-store/
mkdir $ms
# collect the cluster map from OSDs
for host in $hosts; do
  echo $host
  rsync -avz $ms root@$host:$ms
  rm -rf $ms
  ssh root@$host <<EOF
    for osd in /var/lib/ceph/osd/ceph-*; do
      ceph-objectstore-tool --data-path \$osd --op update-mon-db --mon-store-path $ms
    done
EOF
  rsync -avz root@$host:$ms $ms
done
