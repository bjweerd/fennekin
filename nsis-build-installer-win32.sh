rm -rf INSTDIR
./configure --prefix=`pwd`/INSTDIR --without-webkit1
make clean all install
makensis fennekin.nsi
