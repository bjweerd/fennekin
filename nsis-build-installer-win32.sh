rm -rf INSTDIR
./configure --prefix=`pwd`/INSTDIR
make clean all install
makensis fennekin.nsi
